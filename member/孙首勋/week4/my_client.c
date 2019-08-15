#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "my_recv.h"
#include "all.h"
#include <errno.h>
#include <sys/types.h>
#include <stdlib.h>
#include <pthread.h>
#include "list.h"
#include "md5.h"

#define INVALID_USERINFO   'n' //用户信息无效
#define VALID_USERINFO     'y' //用户信息有效

int friendnum = 0;
int groupnum = 0;
int messnum = 0; 
int useruid;
int chatfd;
int deuid;

void judgeaddfri(int mes,int conn_fd);
void judgeaddgro(int mes,int conn_fd);
void watchfrilist(int conn_fd);   //查看好友列表
void watchgrouplist(int conn_fd);  //查看群组列表
int login(int conn_fd);
int UI_zhuce(int conn_fd);
void UI_loginin(int conn_fd);
void UI_user(int conn_fd);
int UI_friendadd(int conn_fd);
int UI_frienddel(int delfd,int conn_fd);
int UI_friendchat(char *firendname,int conn_fd);
int UI_groupchat(char *groupname,int conn_fd);
int UI_groupadd(int conn_fd);
int UI_groupmbkick(char *groupname,int conn_fd);
int UI_groupout(char *groupname,int conn_fd);
int UI_groupdel(char *groupname,int conn_fd);
int UI_groupcreate(int conn_fd);

int get_userinfo(char *mes,int len);
void input_userinfo(int conn_fd,const PACK *senddata);

void *clientrecive(void *conn_fd);
void changepassword(int conn_fd);
void findpassword(int conn_fd);

void watchlistbox(int conn_fd);

char username[20],password[20];
box_list_t head;
box_list_t headgroup;
box_list_t headuser;

//infogroup_list_t headgroup;
//infouser_list_t headuser;

int main(int argc,char **argv){
	
	List_Init(head,box_node_t);
	List_Init(headgroup,box_node_t);
	List_Init(headuser,box_node_t);

	//List_Init(headgroup,infogroup_node_t);
	//List_Init(headuser,infouser_node_t);

	int i;
	int ret;
	int conn_fd;
	int serv_port;
	struct sockaddr_in serv_addr;
	char recv_buf[BUFSIZE];
	memset(recv_buf,0,sizeof(recv_buf));
	//检查参数个数
	if(argc != 5){
		printf("Usage:[-p] [serv_port] [-a] [serv_address]\n");
		exit(1);
	}
	//初始化服务器端地址结构
	memset(&serv_addr,0,sizeof(struct sockaddr));
	serv_addr.sin_family = AF_INET;
	//从命令行获取服务器的端口与地址
	for(i = 1;i < argc;i++){
		if(strcmp("-p",argv[i]) == 0){
			serv_port = atoi(argv[i+1]);
			if(serv_port < 0 || serv_port > 65535){
				printf( "invalid serv_addr.sin_port\n");
				exit(1);
			}
			else{
				serv_addr.sin_port = htons(serv_port);
			}

			continue;
		}

		if(strcmp("-a",argv[i]) == 0){
			if(inet_aton(argv[i+1],&serv_addr.sin_addr) == 0){
				printf( "invalid server ip address\n");
				exit(1);
			}

			continue;
		}
	}

	//检查是否少了某项参数
	if(serv_addr.sin_port == 0 || serv_addr.sin_addr.s_addr == 0){
		printf( "Usage : [-p] [serv_addr,sin_port] [-a] [serv_address] \n");
		exit(1);
	}
	//创建一个TCP套接字
	conn_fd = socket(AF_INET,SOCK_STREAM,0);
	if(conn_fd < 0){
		my_err("clinet socket",__LINE__);
	}

	//向服务器端发送连接请求
	if(connect(conn_fd,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr)) < 0){
		my_err("client connect",__LINE__);
	}

	UI_loginin(conn_fd);
	pthread_t tid1;
    pthread_create(&tid1,NULL,(void *)clientrecive,&conn_fd);
	UI_user(conn_fd);
}

//获取用户输入存入到mes,mes的长度为lenmax，用户输入数据以’\n‘为结束标志
int get_userinfo(char *mes,int len){
	int i;
	int c;
	if(mes ==  NULL)
        return -1;
	i = 0;
	while(((c = getchar()) != '\n') && (c != EOF) && (i < len-2)){
		mes[i++] = c;
	}
	mes[i++] = '\n';
	//printf("%s",mes);
	return 0;
}

//然后用过fd 发送出去
void input_userinfo(int conn_fd,const PACK *senddata){

	if(send(conn_fd,senddata,sizeof(PACK),0) < 0)
		my_err("send",__LINE__);
}



void *clientrecive(void *conn_fd){

	while(1){
		PACK *recvdata = NULL;
		recvdata = (PACK *)malloc(sizeof(PACK));
		int len_remain;
		box_node_t * newmessage = NULL;
		newmessage = (box_list_t)malloc(sizeof(box_node_t));

		//int ret = recv(conn_fd,recvdata,sizeof(PACK),0);
		//printf(" ff\n");
		if((len_remain = recv(*(int*)conn_fd,recvdata,sizeof(PACK),0)) < 0){
            my_err("recv",__LINE__);
        }
		sleep(1);
		//printf(" ff\n");

		if(recvdata->type == FRI_SEE){
			newmessage->data.type = recvdata->type;
			strcpy(newmessage->data.mes,recvdata->data.mes);
			strcpy(newmessage->data.recv_name,recvdata->data.recv_name);
			strcpy(newmessage->data.send_name,recvdata->data.send_name);
			newmessage->data.recv_fd = recvdata->data.recv_fd;
			newmessage->data.send_fd = recvdata->data.send_fd;
			List_AddHead(headuser,newmessage);
			friendnum++;
		}

		if(recvdata->type == GROUP_SEE){
			newmessage->data.type = recvdata->type;
			strcpy(newmessage->data.mes,recvdata->data.mes);
			strcpy(newmessage->data.recv_name,recvdata->data.recv_name);
			strcpy(newmessage->data.send_name,recvdata->data.send_name);
			newmessage->data.recv_fd = recvdata->data.recv_fd;
			newmessage->data.send_fd = recvdata->data.send_fd;
			List_AddHead(headgroup,newmessage);
			groupnum++;
		}
		else{
			newmessage->data.type = recvdata->type;
			strcpy(newmessage->data.mes,recvdata->data.mes);
			strcpy(newmessage->data.recv_name,recvdata->data.recv_name);
			strcpy(newmessage->data.send_name,recvdata->data.send_name);
			newmessage->data.recv_fd = recvdata->data.recv_fd;
			newmessage->data.send_fd = recvdata->data.send_fd;
			List_AddHead(head,newmessage);
			messnum++;
		}
	}
}


void watchfrilist(int conn_fd){

	PACK *senddata = NULL;
	senddata = (PACK *)malloc(sizeof(PACK));
	senddata->type = FRI_SEE;
	if(send(conn_fd,senddata,sizeof(senddata),0) < 0)
		my_err("send",__LINE__);
	printf("--------加载中-------\n");
	sleep(1);


	int i,id;
	int choice;

	char friendname[20];
	memset(friendname,0,sizeof(friendname));

	Pagination_t paging;
	paging.offset = 0;
	paging.pageSize = PAGE_SIZE;
	//infouser_node_t * pos;
	//infouser_node_t * pos1;
	box_node_t * pos;
	box_node_t * pos1;
	paging.totalRecords = friendnum;
	Paging_Locate_FirstPage(headuser,paging);
	do
	{
		printf(
				"\n==========================================\n");
		printf(
				"***************** 好友信息表 *****************\n");

		printf( "%20s %20s\n","uid","在线状态");
		printf( "-----------------------------------------------\n");

		Paging_ViewPage_ForEach(headuser,paging,box_node_t,pos,i){
			printf( "%20d %20d\n",pos->data.recv_fd,pos->data.send_fd);	
		}
	
		printf("--------Total Recoeds: %2d--Page %2d  %2d--------\n",paging.totalRecords,Pageing_CurPage(paging),Pageing_TotalPages(paging));
	
		printf("*************************************************\n");

		printf("[1]上页|[2]下页|[3]添加用户|[4]删除用户|[5]获取更新|[6]选择私聊|[7]返回");
		printf("\n==============================================================\n");
		printf( "your choice: ");
		scanf( "%d",&choice);
		getchar();
		switch(choice)
		{
			case 3:
				UI_friendadd(conn_fd);
				if(UI_friendadd(conn_fd))
				{
					paging.totalRecords = friendnum + 1;
					Paging_Locate_LastPage(headuser,paging,box_node_t);
				}
				break;
			case 4:
				
				printf("请输入你要删除好友的uid：");
				scanf("%d",&deuid);
				getchar();
				List_ForEach(head,pos1){
					if(pos1->data.recv_fd == deuid){
						if(UI_frienddel(deuid,conn_fd))
						{
							paging.totalRecords = friendnum - 1;
							List_Paging(headuser,paging,box_node_t);
						}
					}	
					//遍历没有，不聊天。
				}
				
				break;
			case 5:
				if(1)
				{
					paging.totalRecords = friendnum;
					List_Paging(headuser,paging,box_node_t);
				}
				break;
			case 6:
				
				printf( "请输入你想要私聊的用户uid：");
				scanf("%d",&chatfd);
				getchar();
				List_ForEach(head,pos1){
					if(pos1->data.recv_fd == chatfd){

					}
				}

				if(UI_friendchat(friendname,conn_fd))
				{
					paging.totalRecords = friendnum;
					List_Paging(headuser,paging,box_node_t);
				}
				break;
			case 1:

				if (!Pageing_IsFirstPage(paging)) 
				{
					Paging_Locate_OffsetPage(head, paging, -1,box_node_t);
				}
				break;
			case 2:

				if (!Pageing_IsLastPage(paging)) 
				{
					Paging_Locate_OffsetPage(head, paging, 1,box_node_t);
				}
				break;
			}
	}while(choice != 7);
}


void watchgrouplist(int conn_fd){
	PACK *senddata = NULL;
	senddata = (PACK *)malloc(sizeof(PACK));
	senddata->type = GROUP_SEE;
	senddata->data.send_fd = useruid;
	if(send(conn_fd,senddata,sizeof(senddata),0) < 0)
		my_err("send",__LINE__);
	printf("-----------------------------加载中--------------------------\n");
	sleep(1);

	int i,id;
	int choice;

	char groupname[20];
	memset(groupname,0,sizeof(groupname));



	//infogroup_node_t *pos;
	box_node_t *pos;

	Pagination_t paging;

	paging.offset = 0;
	paging.pageSize = PAGE_SIZE;


	paging.totalRecords = groupnum;
	//返回有多少数量
	Paging_Locate_FirstPage(headgroup,paging);
	do
	{
		printf(
				"\n=============================================\n");
		printf(
				"******************** 群信息 ********************\n");

		printf( "%20s %15s %20s\n","群名","群总人数","群主");
		printf( "---------------------------------------------------\n");

		Paging_ViewPage_ForEach(headgroup,paging,box_node_t,pos,i)
		{
			printf( "%20s %15d %20s\n",/*pos->data.group_name,pos->data.member_num,*/pos->data.mes);	
		}
	

		printf( "---Total Recoeds: %2d----Page %2d   %2d ---\n",paging.totalRecords,Pageing_CurPage(paging),Pageing_TotalPages(paging));
	
		printf("*************************************************\n");

		printf("[1]上页 | [2]下页 | [3]添加群组 | [4]解散群 |[5]建群 | [6]选择群聊 | [7]退群 | [8]踢人 | [9]退出");
		printf( "\n=======================================================================================\n");
		printf( "your choice: ");
		scanf( "%d",&choice);
		getchar();
		switch(choice)
		{
			case 3:
				printf("请输入想要加入群的群名：");
				get_userinfo(groupname,20);
				if(UI_groupadd(conn_fd))
				{
					paging.totalRecords = groupnum + 1;
					Paging_Locate_LastPage(headgroup,paging,box_node_t);
				}
				break;
			case 4:

				printf("请输入解散群的群名：");
				get_userinfo(groupname,20);

				/*List_ForEach(headgroup,pos1){
					//meizai
				}*/
				if(UI_groupdel(groupname,conn_fd)){
					paging.totalRecords = groupnum - 1;
					List_Paging(headgroup,paging,box_node_t);
				}
				break;
			case 5:

				if(UI_groupcreate(conn_fd))
				{
					paging.totalRecords = groupnum;
					List_Paging(headgroup,paging,box_node_t);
				}
				break;
			case 6:
				printf( "请输入选择的群聊：");
				get_userinfo(groupname,20);

				/*while(1){
					//meizai
				}*/

				if(UI_groupchat(groupname,conn_fd))
				{
					paging.totalRecords = groupnum;
					List_Paging(headgroup,paging,box_node_t);
				}
				break;
			case 7:

				printf("请输入退出群的群名：");
				get_userinfo(groupname,20);

				while(1){
					//meizai
				}
				if(UI_groupout(groupname,conn_fd)){
					paging.totalRecords = groupnum - 1;
					List_Paging(headgroup,paging,box_node_t);
				}		

				break;
			case 8:
				printf("请选择你要踢人的群聊：");
				get_userinfo(groupname,20);

				while(1){
					//ruguobushi
				}

				if(UI_groupmbkick(groupname,conn_fd)){
					paging.totalRecords = groupnum;
					List_Paging(headgroup,paging,box_node_t);
				}
				break;
			case 1:
				if (!Pageing_IsFirstPage(paging)){
					Paging_Locate_OffsetPage(headgroup, paging, -1,box_node_t);
				}
				break;
			case 2:
				if (!Pageing_IsLastPage(paging)){
					Paging_Locate_OffsetPage(headgroup, paging, 1,box_node_t);
				}
				break;
			}
	}while(choice != 9);
}



int login(int conn_fd){

	int flag;
	do{//输入用户信息直到正确为止

		flag = 0;
		
		
		printf("usrname:");
		if(get_userinfo(username,20) < 0){
				printf( "error return from get_userinfo\n");
				exit(1);
		}

		printf("password:");
		if(get_userinfo(password,20) < 0){
				printf( "error return from get_userinfo\n");
				exit(1);
		}
		//printf("password:");
		//for(int i = 0;i < strlen(password);i++)
		//	printf("%c",password[i]);
		

		PACK * senddata = NULL;
		
		senddata = (PACK *)malloc(sizeof(PACK));
		senddata->type = LOGIN;
		//senddata->data.recv_fd = ;
		senddata->data.send_fd = conn_fd;
		strcpy(senddata->data.recv_name,"server");
		
		sprintf(senddata->data.mes,"%s%s",username,password);
		//printf("111111111111%s",senddata->data.mes);
		input_userinfo(conn_fd,senddata); //发
		//recive(conn_fd,recvdata);		//收
		login_data logindata;


		PACK * recvdata = NULL;
    	recvdata = (PACK *)malloc(sizeof(PACK));
		int len_remain;

		if((len_remain = recv(conn_fd,recvdata,sizeof(PACK),0)) < 0){
            my_err("recv",__LINE__);
        }
		

		if(recvdata->type == LOGIN){
			if(recvdata->data.mes[0] == 'y'){
				flag = 1;
				useruid = recvdata->data.recv_fd;
				printf("useruid = %d",useruid);
				printf("登录成功\n");
				return 1;
			}
			else{
				printf("你的用户名和密码不匹配，请重新输入，或者选择退出，退出请输【10086】，继续请输【0】\n");
				scanf("%d",&flag);
				getchar();
				if(flag == 10086){
					return 1;
				}
			}
		}
	}while(flag!=1);
}




void UI_loginin(int conn_fd){
	int choice;
	do {
		system("clear");
		printf("\n==============================\n");
		printf("*********** xunchat ************\n");
		printf("[1]  登录\n");
		printf("[2]  注册\n");
		printf("[3]  找回密码\n");
		printf("[4]  退出\n");

		printf("\n==============================\n");
		printf("请输出你的选择：");
		scanf("%d",&choice);
		getchar();
		switch (choice) {
				//输入用户名和密码
		case 1:
			if(login(conn_fd)){
				return ;
			}
			else
				break;
		case 2:
			if(UI_zhuce(conn_fd)){
				return ;
			}
			break;
		case 3:
			findpassword(conn_fd);
			break;
		}
	}while (4 != choice);
}


int UI_zhuce(int conn_fd){
	//getchar();

	int flag = 0;
	do{
		char username2[20],password2[20],mibao[20],sex[3];
		memset(username2,0,sizeof(username2));
		memset(password2,0,sizeof(password2));
		memset(mibao,0,sizeof(mibao));
		memset(sex,0,sizeof(sex));
		printf("usrname:");
		fgets(username2,20,stdin);
		/*if(get_userinfo(username2,20) < 0){
				printf( "error return from get_userinfo\n");
				exit(1);
		}*/
		printf("password:");
		fgets(password2,20,stdin);
		printf("%s",password2);
		/*if(get_userinfo(password2,20) < 0){
				printf( "error return from get_userinfo\n");
				exit(1);
		}*/
		printf("mibao:");
		fgets(mibao,20,stdin);
		/*if(get_userinfo(username2,20) < 0){
				printf( "error return from get_userinfo\n");
				exit(1);
		}*/
		printf("sex:");
		fgets(sex,3,stdin);
		/*if(get_userinfo(sex,3) < 0){
				printf( "error return from get_userinfo\n");
				exit(1);
		}*/


		PACK *senddata = NULL;
		senddata = (PACK *)malloc(sizeof(PACK));
		senddata->type = REGISTER;
		//senddata->data.recv_fd = ;
		senddata->data.send_fd = conn_fd;
		strcmp(senddata->data.recv_name,"server");

		sprintf(senddata->data.mes,"%s%s%s%s",username2,password2,mibao,sex);
		printf("%s",senddata->data.mes);


		input_userinfo(conn_fd,senddata); //发


		PACK * recvdata = NULL;
		int len_remain;
    	recvdata = (PACK *)malloc(sizeof(PACK));

		if((len_remain = recv(conn_fd,recvdata,sizeof(PACK),0)) < 0){
            my_err("recv",__LINE__);
        }
		

		if(recvdata->type == REGISTER){
			if(recvdata->data.mes[0] == 'y'){
				flag = 1;
				//recive(conn_fd,recvdata);
				printf("注册成功\n");
				return 1;
			}
			else{
				printf("你的用户名以使用过，请重新输入，或者选择退出，退出请输【10086】，继续请输【0】\n");
				scanf("%d",&flag);
				getchar();
				if(flag == 10086){
					return 1;
				}
			}
		}

	}while(flag);
}

void UI_user(int conn_fd){
	int choice;
	do{
		system("clear");
		printf("\n=======================\n");
		printf("******** xunchat ********\n");
		printf("【1】查看好友列表\n");
		printf("【2】查看群列表\n");
		printf("【3】修改密码\n");
		printf("【4】查看消息盒子\n");
		printf("【5】退出登录\n");
		printf("\n=======================\n");
		printf("请输出你的选择：");
		scanf("%d",&choice);
		getchar();
		switch (choice){
		case 1:
			watchfrilist(conn_fd);
			break;
		case 2:
			watchgrouplist(conn_fd);
			break;
		case 3:
			changepassword(conn_fd);
			break;
		case 4:
			watchlistbox(conn_fd);
			break;
		}
		
	} while (5 != choice);

	close(conn_fd);
	return ;
}


void changepassword(int conn_fd){
	
	char password1[20],password3[20];
	memset(password3,0,sizeof(password3));
	memset(password1,0,sizeof(password1));
	int cnt = 3;
	
	while(cnt--){	
		printf("你有%d次机会，请输入新密码:",cnt);
		if(get_userinfo(password1,20) < 0){
			printf( "error return from get_userinfo\n");
			exit(1);
		}
		printf("%s%s",password1,password);
		if(strcmp(password1,password) == 0){	
			printf("新密码与旧密码相同，请重新输入\n");
			continue;
		}

		printf("请重新输入新密码:");
		if(get_userinfo(password3,20) < 0){
			printf( "error return from get_userinfo\n");
			exit(1);
		}
		if(strcmp(password1,password3) != 0){
			strcpy(password,password3);

			PACK *senddata = NULL;
			senddata = (PACK *)malloc(sizeof(PACK));
			senddata->type = CHANGE_NUM;
			strcmp(senddata->data.mes,password3);
			senddata->data.send_fd = conn_fd;
			strcmp(senddata->data.recv_name,"server");			
			strcmp(senddata->data.send_name,username);
			input_userinfo(conn_fd,senddata); //发

			/*PACK * recvdata = NULL;
			int len_remain;
    		recvdata = (PACK *)malloc(sizeof(PACK));
		

			if(recvdata->type == CHANGE_NUM){
				if(recvdata->data.mes[0] == 'y'){
					//recive(conn_fd,recvdata);
					printf("修改成功\n");
					break;
				}
				else{
					printf("密码修改错误\n");
				}
			}*/
		}
		else{
			printf("两次密码输入不一致，请重新输入\n");

		}
	}


}

void findpassword(int conn_fd){
	char zhanghu[20],daan[20];
	memset(zhanghu,0,sizeof(zhanghu));
	memset(daan,0,sizeof(daan));

	printf( "****** 找回密码 *******\n");
	printf( "请输入账户:");
	if(get_userinfo(zhanghu,20) < 0){
			printf( "error return from get_userinfo\n");
			exit(1);
	}

	printf( "请输入预留密保答案:");
	if(get_userinfo(daan,20) < 0){
			printf( "error return from get_userinfo\n");
			exit(1);
	}

	PACK *senddata = NULL;
	senddata = (PACK *)malloc(sizeof(PACK));
	senddata->type = FIND_PASSWD;
	strcpy(senddata->data.mes,daan);
	senddata->data.send_fd = conn_fd;
	strcpy(senddata->data.recv_name,"server");			
	strcpy(senddata->data.send_name,zhanghu);
	input_userinfo(conn_fd,senddata); //发

	PACK * recvdata = NULL;
	int len_remain;
	recvdata = (PACK *)malloc(sizeof(PACK));

	if((len_remain = recv(conn_fd,recvdata,sizeof(PACK),0)) < 0){
		my_err("recv",__LINE__);
	}

	if(recvdata->data.mes[0] == 'y'){
		printf("密保问题正确，请输入重置的密码：");
		if(get_userinfo(daan,20) < 0){
			printf( "error return from get_userinfo\n");
			exit(1);
		}
		PACK *senddata = NULL;
		senddata = (PACK *)malloc(sizeof(PACK));
		senddata->type = FIND_PASSWD;
		strcpy(senddata->data.mes,daan);
		senddata->data.send_fd = conn_fd;
		strcpy(senddata->data.recv_name,"server");			
		strcpy(senddata->data.send_name,zhanghu);
		input_userinfo(conn_fd,senddata); //发

	}
	else{
		printf("密保问题错误，重置失败！\n");
	}
}


int UI_friendadd(int conn_fd){
	char friendname[20];
	memset(friendname,0,sizeof(friendname));
	printf("请输入你要添加的好友名：");
	//get_userinfo(friendname,20);
	scanf("%s",friendname);
	getchar();

	PACK *senddata = NULL;
	senddata = (PACK *)malloc(sizeof(PACK));
	senddata->type = FRI_ADD;
	senddata->data.send_fd = useruid;
	strcpy(senddata->data.send_name,username);
	strcpy(senddata->data.mes,friendname);
	input_userinfo(conn_fd,senddata); //发
	printf("发送成功\n");
}
int UI_frienddel(int delfd,int conn_fd){

	char buf[10];
	memset(buf,0,sizeof(buf));

	sprintf(buf,"%s",useruid);
	PACK *senddata = NULL;
	senddata = (PACK *)malloc(sizeof(PACK));
	senddata->type = FRI_DEL;
	senddata->data.send_fd = delfd;
	strcpy(senddata->data.send_name,username);
	strcpy(senddata->data.mes,buf);

	input_userinfo(conn_fd,senddata); //发

}
int UI_groupadd(int conn_fd){
	char groupname[20];
	memset(groupname,0,sizeof(groupname));
	printf("请输入你要参加的群名：");
	get_userinfo(groupname,20);

	PACK *senddata = NULL;
	senddata = (PACK *)malloc(sizeof(PACK));
	senddata->type = GROUP_JOIN;
	senddata->data.send_fd = useruid;
	strcpy(senddata->data.send_name,username);
	strcpy(senddata->data.mes,groupname);
	input_userinfo(conn_fd,senddata); //发
}

int UI_groupcreate(int conn_fd){

	char groupname[20];
	memset(groupname,0,sizeof(groupname));
	printf("请输入你要创建的群名：");
	get_userinfo(groupname,20);

	PACK *senddata = NULL;
	senddata = (PACK *)malloc(sizeof(PACK));

	senddata->type = GROUP_CREATE;
	senddata->data.send_fd = useruid;
	strcpy(senddata->data.send_name,username);
	strcpy(senddata->data.mes,groupname);
	input_userinfo(conn_fd,senddata); //发
}

int UI_groupmbkick(char *groupname,int conn_fd){

	while(1){
		char friendname[20];
		memset(friendname,0,sizeof(friendname));
		printf("请输入你要清除的人名：");
		get_userinfo(friendname,20);


		PACK *senddata = NULL;
		senddata = (PACK *)malloc(sizeof(PACK));

		senddata->type = GROUP_KICK;
		senddata->data.send_fd = conn_fd;
		strcpy(senddata->data.send_name,username);
		strcpy(senddata->data.recv_name,friendname);
		strcpy(senddata->data.mes,groupname);
		input_userinfo(conn_fd,senddata); //发
	}
}
int UI_groupout(char *groupname,int conn_fd){
	
	PACK *senddata = NULL;
	senddata = (PACK *)malloc(sizeof(PACK));

	senddata->type = GROUP_QIUT;
	senddata->data.send_fd = conn_fd;
	strcpy(senddata->data.send_name,username);
	strcpy(senddata->data.mes,groupname);
	input_userinfo(conn_fd,senddata); //发
}

int UI_groupdel(char *groupname,int conn_fd){
	PACK *senddata = NULL;
	senddata = (PACK *)malloc(sizeof(PACK));

	senddata->type = GROUP_DEL;
	senddata->data.send_fd = conn_fd;
	strcpy(senddata->data.send_name,username);
	strcpy(senddata->data.mes,groupname);
	input_userinfo(conn_fd,senddata); //发
}

int UI_friendchat(char *firendname,int conn_fd){
	
	
	char frimessage[MAX_CHAR];

	do{
		PACK *senddata = NULL;
		senddata = (PACK *)malloc(sizeof(PACK));
		strcpy(senddata->data.send_name,firendname);
		printf("you[|]quit>>");
		scanf("%s",frimessage);


		senddata->type = FRI_MES;
		senddata->data.send_fd = conn_fd;
		strcpy(senddata->data.send_name,username);
		strcpy(senddata->data.mes,frimessage);
		input_userinfo(conn_fd,senddata); //发

	}while(frimessage[0] = '|');
}
int UI_groupchat(char *groupname,int conn_fd){
	char gromessage[MAX_CHAR];
	memset(gromessage,0,sizeof(gromessage));

	do{
		PACK *senddata = NULL;
		senddata = (PACK *)malloc(sizeof(PACK));
		strcpy(senddata->data.send_name,gromessage);
		printf("you[|]quit>>");
		scanf("%s",gromessage);


		senddata->type = GRO_MES;
		senddata->data.send_fd = conn_fd;
		strcpy(senddata->data.send_name,username);
		strcpy(senddata->data.mes,gromessage);
		input_userinfo(conn_fd,senddata); //发

	}while(gromessage[0] = '|');

}

void watchlistbox(int conn_fd){

	int i,id;
	int choice;

	int mes;

	box_node_t *pos;
	Pagination_t paging;

	paging.offset = 0;
	paging.pageSize = PAGE_SIZE;


	paging.totalRecords = messnum;
	//返回有多少数量
	Paging_Locate_FirstPage(head,paging);
	do
	{
		printf(
				"\n=================================================\n");
		printf(
				"******************** 消息盒子信息 ********************\n");

		printf( "%3s %20s %3s %3s %50s\n","uid","发送姓名","sfd","typ","消息内容");
		printf( "---------------------------------------------------\n");

		Paging_ViewPage_ForEach(head,paging,box_node_t,pos,i)
		{
			printf("%3d %20s %3d %3d %50s\n",pos->data.recv_fd,pos->data.send_name,pos->data.send_fd,pos->data.type,pos->data.mes);	
		}
	

		printf( "---Total Recoeds: %2d----Page %2d   %2d ---\n",paging.totalRecords,Pageing_CurPage(paging),Pageing_TotalPages(paging));
	
		printf("*************************************************\n");

		printf("[1]上页 | [2]下页 | [3]回复消息 | [4]退出");
		printf( "\n=======================================================================================\n");
		printf( "your choice: ");
		scanf( "%d",&choice);
		getchar();
		switch(choice)
		{
			case 3:
				printf("请选择你删除的消息：");
				scanf("%d",&mes);
				List_ForEach(head,pos){
					if(mes == pos->data.recv_fd){
						if(pos->data.type == FRIQUE){
							List_DelNode(pos);
							judgeaddfri(mes,conn_fd);
						}
						else if(pos->data.type == GROQUE){
							List_DelNode(pos);
							judgeaddgro(mes,conn_fd);
						}
						else{
							printf("在此处无法查看此类型消息");
						}
					}
				}

				paging.totalRecords = messnum;
				List_Paging(head,paging,box_node_t);
				break;
			case 1:
				if (!Pageing_IsFirstPage(paging)){
					Paging_Locate_OffsetPage(head, paging, -1,box_node_t);
				}
				break;
			case 2:
				if (!Pageing_IsLastPage(paging)){
					Paging_Locate_OffsetPage(head, paging, 1,box_node_t);
				}
				break;
		}
	}while(choice != 4);
	//List_FreeNode(node);
}



void judgeaddfri(int mes,int conn_fd){
	printf("请输入你是否同意好友添加[y] or [n] \n");

	PACK *senddata = NULL;
	senddata = (PACK *)malloc(sizeof(PACK));
	scanf("%s",senddata->data.mes);
	senddata->type = FRIQUE;
	senddata->data.send_fd = mes;
	strcpy(senddata->data.send_name,username);
	input_userinfo(conn_fd,senddata); //发

}
void judgeaddgro(int mes,int conn_fd){
	printf("请输入你是否同意群组添加[y] or [n] \n");

	PACK *senddata = NULL;
	senddata = (PACK *)malloc(sizeof(PACK));
	scanf("%s",senddata->data.mes);
	senddata->type = GROQUE;
	senddata->data.send_fd = mes;
	strcpy(senddata->data.send_name,username);
	input_userinfo(conn_fd,senddata); //发
}
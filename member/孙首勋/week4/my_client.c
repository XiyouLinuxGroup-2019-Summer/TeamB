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
#include "list.h"

#define INVALID_USERINFO   'n' //用户信息无效
#define VALID_USERINFO     'y' //用户信息有效


void watchfrilist();   //查看好友列表
void watchgrouplist();  //查看群组列表
void login(int conn_fd);
void zhuce(int conn_fd);
void UI_zhuce(int conn_fd);
void UI_loginin(int conn_fd);
void UI_user(int conn_fd);
int get_userinfo(char *mes,int len);
void input_userinfo(int conn_fd,const PACK *senddata);
void recive(int conn_fd,const PACK *recvdata);
void changepassword(conn_fd);
void findpassword(conn_fd);


//void init();

int main(int argc,char **argv){
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
	UI_user(conn_fd);

}

void watchfrilist(){
	//if(send(conn_fd,input_buf,strlen(input_buf),0) < 0)  my_err("send",__LINE__);



	/*int i,id;
	char choice;

	FRIEND_INFO *head;
	FRIEND_INFO *pos;
	Pagination_t paging;

	List_Init(head,FRIEND_INFO);
	paging.offset = 0;
	paging.pageSize = PAGE_SIZE;


	paging.totalRecords = Play_Srv_FetchAll(head);
	Paging_Locate_FirstPage(head,paging);
	do
	{
		printf(
				"\n==========================================\n");
		printf(
				"***************** 好友信息表 *****************\n");

		printf( "%20s %20s\n","姓名","在线状态");
		printf( "------------------------------------------------\n");

		Paging_ViewPage_ForEach(head,paging,play_node_t,pos,i){
			printf( "%20s %20s\n",pos->data.name,pos->data.type);	
		}
	
		printf("--------Total Recoeds: %2d--Page %2d  %2d--------\n",paging.totalRecords,Pageing_CurPage(paging),Pageing_TotalPages(paging));
	
		printf("*************************************************\n");

		printf("[1]上页|[2]下页|[3]添加用户|[4]删除用户|[5]获取更新|[6]选择私聊|[7]返回");
		printf("\n==============================================================\n");
		setbuf(stdin,NULL);
		printf( "your choice: ");
		scanf( "%c",&choice);
		getchar( );
		setbuf(stdin,NULL);
		switch(choice)
		{
			case'3':
				setbuf(stdin,NULL);
				if(Play_UI_Add())
				{
					paging.totalRecords = Play_Srv_FetchAll(head);
					Paging_Locate_LastPage(head,paging,play_node_t);
				}
				break;
			case'4':
				setbuf(stdin,NULL);
				printf("Play_ID:");
				scanf( "%d",&id);
				getchar( );
				if(Play_UI_Delete(id))
				{
					paging.totalRecords = Play_Srv_FetchAll(head);
					List_Paging(head,paging,play_node_t);
				}
				break;
			case'5':
				setbuf(stdin,NULL);
				printf("Play_ID:");
				scanf("%d",&id);
				getchar( );
				if(Play_UI_Modify(id))
				{
					paging.totalRecords = Play_Srv_FetchAll(head);
					List_Paging(head,paging,play_node_t);
				}
				break;
			case'6':
				setbuf(stdin,NULL);
				printf( "Play_ID:");
				scanf( "%d",&id);
				getchar( );
				if(Play_UI_Query(id))
				{
					paging.totalRecords = Play_Srv_FetchAll(head);
					List_Paging(head,paging,play_node_t);
				}
				break;
			case'1':
				setbuf(stdin,NULL);
				if (!Pageing_IsFirstPage(paging)) 
				{
					Paging_Locate_OffsetPage(head, paging, -1, play_node_t);
				}
				break;
			case'2':
				setbuf(stdin,NULL);
				if (!Pageing_IsLastPage(paging)) 
				{
					Paging_Locate_OffsetPage(head, paging, 1, play_node_t);
				}
				break;
			}
	}while(choice != 7);
	*/
}


void watchgrouplist(){
	/*int i,id;
	char choice;

	FRIEND_INFO *head;
	FRIEND_INFO *pos;
	Pagination_t paging;

	List_Init(head,FRIEND_INFO);
	paging.offset = 0;
	paging.pageSize = PAGE_SIZE;


	paging.totalRecords = Play_Srv_FetchAll(head);
	Paging_Locate_FirstPage(head,paging);
	do
	{
		printf(
				"\n=============================================\n");
		printf(
				"************* Projection Play List ************\n");

		printf( "%20s %15s %15s\n","群名","群总人数","群在线人数");
		printf( "---------------------------------------------------\n");

		Paging_ViewPage_ForEach(head,paging,play_node_t,pos,i)
		{
			printf( "%20s %15s %15s\n",pos->data.name,pos->data.price,pos->data.);	
		}
	

		printf( "---Total Recoeds: %2d----Page %2d   %2d -----\n",paging.totalRecords,Pageing_CurPage(paging),Pageing_TotalPages(paging));
	
		printf("*************************************************\n");

		printf("[1]上页 | [2]下页 | [3]添加群组 | [4]解散群 |[5]获取更新 | [6]选择群聊 | [7]退出");
		printf( "\n=======================================================================================\n");
		setbuf(stdin,NULL);
		printf( "your choice: ");
		scanf( "%c",&choice);
		getchar( );
		setbuf(stdin,NULL);
		switch(choice)
		{
			case'3':
				setbuf(stdin,NULL);
				if(Play_UI_Add())
				{
					paging.totalRecords = Play_Srv_FetchAll(head);
					Paging_Locate_LastPage(head,paging,play_node_t);
				}
				break;
			case'4':
				setbuf(stdin,NULL);
				printf("Play_ID:");
				scanf( "%d",&id);
				getchar( );
				if(Play_UI_Delete(id))
				{
					paging.totalRecords = Play_Srv_FetchAll(head);
					List_Paging(head,paging,play_node_t);
				}
				break;
			case'5':
				setbuf(stdin,NULL);
				printf("Play_ID:");
				scanf("%d",&id);
				getchar( );
				if(Play_UI_Modify(id))
				{
					paging.totalRecords = Play_Srv_FetchAll(head);
					List_Paging(head,paging,play_node_t);
				}
				break;
			case'6':
				setbuf(stdin,NULL);
				printf( "Play_ID:");
				scanf( "%d",&id);
				getchar( );
				if(Play_UI_Query(id))
				{
					paging.totalRecords = Play_Srv_FetchAll(head);
					List_Paging(head,paging,play_node_t);
				}
				break;
			case'1':
				setbuf(stdin,NULL);
				if (!Pageing_IsFirstPage(paging)) 
				{
					Paging_Locate_OffsetPage(head, paging, -1, play_node_t);
				}
				break;
			case'2':
				setbuf(stdin,NULL);
				if (!Pageing_IsLastPage(paging)) 
				{
					Paging_Locate_OffsetPage(head, paging, 1, play_node_t);
				}
				break;
			}
	}while(choice != 7);
	*/
}

//获取用户输入存入到mes,mes的长度为lenmax，用户输入数据以’\n‘为结束标志
int get_userinfo(char *mes,int len){
	int i;
	int c;
	if(mes ==  NULL)
        return -1;
	i = 0;
	while(((c = getchar()) != '\n') && (c != EOF) && (i < len-2)){
		mes[i] = c;
	}
	mes[i++] = '\n';
	return 0;
}

//然后用过fd 发送出去
void input_userinfo(int conn_fd,const PACK *senddata){

	if(send(conn_fd,senddata,sizeof(PACK),0) < 0)
		my_err("send",__LINE__);
}


void login(int conn_fd){

	int flag;
	do{//输入用户信息直到正确为止

		flag = 0;
		char username[20],password[20];
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
		

		PACK * senddata = NULL;
		
		senddata = (PACK *)malloc(sizeof(PACK));
		senddata->type = LOGIN;
		//senddata->data.recv_fd = ;
		senddata->data.send_fd = conn_fd;
		strcmp(senddata->data.recv_name,"server");
		
		sprintf(senddata->data.mes,"%s%s",username,password);

		input_userinfo(conn_fd,senddata); //发
		//recive(conn_fd,recvdata);		//收
		login_data logindata;


		PACK * recvdata = NULL;
    	recvdata = (PACK *)malloc(sizeof(PACK));
		int len_remain;

		if((len_remain = recv(conn_fd,recvdata,sizeof(PACK),0)) < 0){
            my_err("recv",__LINE__);
        }
		
		//my_recv(conn_fd,logindata,2048);

		if(recvdata->type == LOGIN){
			if(recvdata->data.mes[0] == 'y'){
				flag == 1;
				//recive(conn_fd,recvdata);
				printf("登录成功\n");
			}
			else{
				printf("你的用户名和密码不匹配，请重新输入\n");
			}
		}
	}while(flag == 0);
}




void UI_loginin(int conn_fd){
	int choice;
	do {
		system("clear");
		printf("\n==============================\n");
		printf("*********** xunchat ************\n");
		printf("[1]  登录\n");
		printf("[2]  注册\n");
		printf("[3]  修改密码\n");
		printf("[4]  找回密码\n");
		printf("[5]  退出\n");

		printf("\n==============================\n");
		printf("请输出你的选择：");
		scanf("%d",&choice);
		switch (choice) {
				//输入用户名和密码
		case 1:
			login(conn_fd);
			break;
		case 2:
			zhuce(conn_fd);
			break;
		case 3:
			changepassword(conn_fd);
			break;
		case 4:
			//zhuce(conn_fd);
		break;
			//case 2:
			//zhuce(conn_fd);
			//break;
		}
	} while (5 != choice && 5 != choice);
}


void UI_zhuce(int conn_fd){

	int flag = 0;
	do{
		char username[20],password[20],mibao[20];
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
		printf("mibao:");
		if(get_userinfo(username,20) < 0){
				printf( "error return from get_userinfo\n");
				exit(1);
		}


		PACK *senddata = NULL;
		senddata = (PACK *)malloc(sizeof(PACK));
		senddata->type = REGISTER;
		//senddata->data.recv_fd = ;
		senddata->data.send_fd = conn_fd;
		strcmp(senddata->data.recv_name,"server");
		
		sprintf(senddata->data.mes,"%s%s%s",username,password,mibao);
		input_userinfo(conn_fd,senddata); //发

	}while(flag);



}

void UI_user(conn_fd){
	int choice;
	do{
		system("clear");
		printf("\n=======================\n");
		printf("******** xunchat ********\n");
		printf("【1】查看好友列表\n");
		printf("【2】查看群列表\n");
		printf("【3】退出登录\n");
		printf("\n=======================\n");
		printf("请输出你的选择：");
		scanf("%d",choice);
		switch (choice){
		setbug(stdin,NULL);
		case 1:
			//watchfrilist();
			break;
		case 2:
			//watchgrouplist();
			break;
		}
		
	} while (3 != choice);

	close(conn_fd);
	return 0;
}


void changepassword(conn_fd){

}

void findpassword(conn_fd){

}
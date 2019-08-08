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

#define INVALID_USERINFO   'n' //用户信息无效
#define VALID_USERINFO     'y' //用户信息有效


void watchfrilist();   //查看好友列表
void watchgrouplist();  //查看群组列表
void login(int conn_fd);
void zhuce(int conn_fd);
int get_userinfo(char *mes,int len);
void input_userinfo(int conn_fd,const PACK *senddata);
void recive(int conn_fd,const PACK *recvdata);

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
	int choice;
	do {
		system("clear");
		printf("\n==============================\n");
		printf("*********** xunchat ************\n");
		printf("【1】登录\n");
		printf("【2】注册\n");
		printf("【3】退出\n");
		printf("\n==============================\n");
		printf("请输出你的选择：");
		scanf("%d",choice);
		switch (choice) {
				//输入用户名和密码
		case 1:
			login(conn_fd);
			break;
		case 2:
			zhuce(conn_fd);
			break;

		}
	} while (3 != choice && 3 != choice);


	//读取欢迎信息并打印出来
	if((ret = my_recv(conn_fd,recv_buf,sizeof(recv_buf))) < 0){
		printf( "data is too long\n");
		exit(1);
	}

	for(i = 0;i < ret;i++){
		printf( "%c",recv_buf[i]);
	}
	printf("\n");


	do {
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
		
	} while (3 != choice && 3 != choice);

	close(conn_fd);
	return 0;
}



void watchfrilist(){
	//if(send(conn_fd,input_buf,strlen(input_buf),0) < 0)  my_err("send",__LINE__);

}

void watchgrouplist(){

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
	mes[i++] = '\0';

	return 0;
}

//然后用过fd 发送出去
void input_userinfo(int conn_fd,const PACK *senddata){
	//int flag_userinfo;

	if(send(conn_fd,senddata,strlen(senddata),0) < 0)
		my_err("send",__LINE__);

	/*//从连接套接字上读取上一次数据

	if(my_recv(conn_fd,recvdata,sizeof(recvdata)) < 0){
		printf( "data is too long\n");
		exit(1);
	}

	if(recvdata->data.mes[0] == VALID_USERINFO){
		flag_userinfo = VALID_USERINFO;
	}
	else{
		flag_userinfo = INVALID_USERINFO;
	}*/

}

void recive(int conn_fd,const PACK *recvdata){
	int len_remain;
	if((len_remain = recv(conn_fd,recvdata,sizeof(recvdata),0)) < 0){
		my_err("recv",__LINE__);
	}
	else if(len_remain == 0){
		return 0;
	}
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
		get_userinfo(password,20);

		PACK * senddata = NULL;
		PACK * recvdata = NULL;

		senddata = (PACK *)malloc(sizeof(PACK));
		senddata->type = LOGIN;

		recvdata = (PACK *)malloc(sizeof(PACK));

		sprintf(senddata->data.mes,"%s%s",username,password);

		input_userinfo(conn_fd,senddata);

		recive(conn_fd,recvdata);

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
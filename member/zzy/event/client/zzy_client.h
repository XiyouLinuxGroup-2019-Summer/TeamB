#ifndef _ZZY_CLIENT_H_
#define _ZZY_CLIENT_H_

#include<stdio.h>
#include<string.h>
#include<error.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include"my_recv.h"
#include"zzy_UI.h"
#define INVALID_USERINFO        'n' //用户名无效
#define VALID_USERINFO          'y' //用户名有效
#define SERV_ADDR_PORT          4057
#define INET_ADDR           "127.0.0.1"
#define USERNAME                 1  //用户名
#define PASSWORD                 0  //密码

#define MAX_USER    300
#define MAX_SIZE    400

#define LOGIN       1
#define REGISTING   2
#define SEE_FRIEND  3


//用户信息
typedef struct user_inforn{    
    char username[MAX_SIZE];      //用户名
    unsigned int password[10];    //加密后的密码 
    int statue;                   //用户状态
    int socket_id;
}USER_INFORN; 

//定义发送接收的包
typedef struct package{
      int type;                   //包类型    
      char message[MAX_SIZE*2];   //包信息
      char send_name[MAX_SIZE];   
      int send_fd;                //发送方
      char recv_name[MAX_SIZE];   
      int recv_fd;                //接收方
}pack;

//存储用户信息
USER_INFORN user_infon[MAX_USER];

//登录
int login_client();                                                                                                  

//获取用户名输入，存入到buf,buf的长度为len，用户输入数据以'\n'为结束标志                                                 
//int get_userinfo(char *buf,int len);                                                                                     

//对输入的用户名和密码进行判断，用户有三次输入的机会                                                                                             
int intput_login_userinfo();

//更新好友状态
void friend_statue();

//发送登录或注册信息并接收回馈
int send_input_client(int type,char *username,char *passward);

//具体的发送包工具
void send_pack(char *send_name,char *recv_name,int type,char *message);

//注册函数
int registering_client();

//找回密码函数i
int find_passward();

#endif

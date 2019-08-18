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
#include<pthread.h>
#include"my_recv.h"
#include"zzy_UI.h"



#define INVALID_USERINFO        'n' //用户名无效
#define VALID_USERINFO          'y' //用户名有效
#define SERV_ADDR_PORT          4057
#define INET_ADDR           "127.0.0.1"
#define USERNAME                 1  //用户名
#define PASSWORD                 0  //密码

#define MAX_USER        300
#define MAX_GROUP       10
#define MAX_GROUP_MEM   100
#define MAX_FRIEND      100
#define MAX_SIZE        400

#define LOGIN       1
#define REGISTING   2
#define SEE_FRIEND  3
#define ADD_FRIEND  4
#define DEL_FRIEND  5
#define CHAT_FRIEND 6
#define ADD_GROUP   7
#define QUIT_GROUP  8
#define DEL_GROUP   9



//用户状态
#define ONLINE  1   //下线
#define DOWLOAD 2   //下线


typedef struct group{
    int message_num;
    int statue;
    char name[MAX_SIZE];
}USER_FRIEND;

//用户信息
typedef struct user_inforn{    
    char username[MAX_SIZE];      //用户名
    int friend_num;
    int group_num;
    char group[MAX_GROUP][MAX_GROUP_MEM];
    USER_FRIEND friends[MAX_FRIEND];
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
USER_INFORN user_infon;
pthread_mutex_t friend_check_mutex;




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

//用户登录后选择
int user_menu_client();

//查看好友
int friend_check();

//添加好友删除好友
int friend_add_del(int tpye);

//查看群聊
int group_check();

//私聊
int friend_chat();





//显示聊天信息
void *message_show(void *name);

//发送信
int send_message(char *name,int type);





//创建群聊
int group_create();

//加入群聊
int group_add();

//退出群聊
int group_quit();

//解散群聊
int group_del();

//判断添加好友是否重名
int friend_name_same(char *friend_name);

#endif

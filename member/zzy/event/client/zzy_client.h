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
    
//登录
int login(int conn_fd);                                                                                                  
//获取用户名输入，存入到buf,buf的长度为len，用户输入数据以'\n'为结束标志                                                 
int get_userinfo(char *buf,int len);                                                                                     
//输入用户名，然后通过fd发出                                                                                             
void intput_userinfo(int conn_fd,const char *string);

#endif

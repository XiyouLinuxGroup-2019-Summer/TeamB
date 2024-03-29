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

#define INVALID_USERINFO        'n' //用户名无效
#define VALID_USERINFO          'y' //用户名有效

//获取用户名输入，存入到buf,buf的长度为len，用户输入数据以'\n'为结束标志
int get_userinfo(char *buf,int len){
    int i;
    int c;

    if(buf == NULL)
        return -1;

    i = 0;
    while(((c = getchar()) != '\n') && c != EOF && (i < len-2)){
        buf[i++] = c;
    }

    buf[i++] = '\n';
    buf[i++] = '\0';

    return 0;
}

//输入用户名，然后通过fd发出
void intput_userinfo(int conn_fd,const char *string){
    char input_buf[32];
    char recv_buf[BUFSIZE];
    int  flag_userinfo;

    //输入用户信息直到正确为止
    do{
        printf("%s:",string);
        if(get_userinfo(input_buf,32) < 0){
            printf("error return from get_userinfo!\n");
            exit(1);
        }

        //发送用户输入信息，让服务器进行判断
        if(send(conn_fd,input_buf,sizeof(input_buf),0) < 0){
            my_err("send",__LINE__);
        }
        
        //从套接字上读取一次数据
        //recv_buf接收到的是 y/n 
        if(my_recv(conn_fd,recv_buf,sizeof(recv_buf))<0){
            printf("data is too long! (in %d)",__LINE__);
            exit(1);
        }
        
        
        if(recv_buf[0] == VALID_USERINFO){
            flag_userinfo = VALID_USERINFO;
        }else{
            printf("%s error,input again\n",string);
            flag_userinfo = INVALID_USERINFO;
        }

    }while(flag_userinfo == INVALID_USERINFO);
}
int main(int argc,char **argv){
    int                     i;
    int                     ret;
    int                     conn_fd;
    int                     serv_port;
    struct sockaddr_in      serv_addr;
    char                    recv_buf[BUFSIZE];

    //检查参数个数
    if(argc != 5){
        printf("Usage: [-p] [serv_port] [-a] [ser_address]\n");
        exit(1);
    }

    //初始化服务器的地址结构
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    
    //从命令行获取服务器端的端口和地址
    for(i = 1;i < argc;i++){
        //获取端口
        if(strcmp("-p",argv[i]) == 0){
            serv_port = atoi(argv[i+1]);    //将字符串转化为整形，得到端口
            if(serv_port < 0 || serv_port > 65535){
                printf("invalid serv_addr.sin_port!\n");
                exit(1);
            }else{
                serv_addr.sin_port = htons(serv_port); //将unsigned short整形数据转化为网络字节顺序,与大小端有关
            }
            continue;
        }
        //获取地址
        if(strcmp("-a",argv[i]) == 0){
            if(inet_aton(argv[i+1],&serv_addr.sin_addr) == 0){//将字符串形式的IP地址转换为二进制的网络字节顺序的IP地址
                printf("invalid server ip address\n");
                exit(1);
            }
            continue;
        }
    }
    
    //检测是否少了某项参数
    if(serv_addr.sin_addr.s_addr == 0 || serv_addr.sin_port == 0){
        printf("Usage: [-p] [serv_port] [-a] [ser_address]\n");
        exit(1);
    }

    //创建一个TCP套接字
    if((conn_fd = socket(AF_INET,SOCK_STREAM,0)) < 0){
        my_err("socket",__LINE__);
    }

    //向服务器端发送连接请求
    if(connect(conn_fd,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr)) < 0){
        my_err("connect",__LINE__);
    }

    //输入用户名和密码
    intput_userinfo(conn_fd,"username");
    intput_userinfo(conn_fd,"password");

    //读取欢迎信息
    if((ret = my_recv(conn_fd,recv_buf,sizeof(recv_buf))) < 0){
        printf("data is too long!\n");
        exit(1);
    }
    

    //打印欢迎信息
    for(i = 0;i < ret;i++){
        printf("%c",recv_buf[i]);
    }
    printf("\n");
    
    //关闭端口
    close(conn_fd);
    
    return 0;
}

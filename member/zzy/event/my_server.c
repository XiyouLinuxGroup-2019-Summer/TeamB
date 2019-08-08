//Client/Server模型的服务器端
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<string.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<error.h>
#include"my_recv.h"     //自定义头文件

#define SERV_PCRT           4507    //服务器端口
#define LISTENO             12      //请求队列的最大长度

#define INVALID_USERINFO    'n'     //用户信息无效
#define VALTD_USERONFO      'y'     //用户信息有效

#define USERNAME             0      //接收到的是用户名
#define PASSWORD             1      //接收到的是密码

struct userinfo {   //保存用户名和密码的结构体
    char username[32];
    char password[32];
};

struct userinfo users[] = {{"linux","unix"},
                           {"4507","4508"},
                           {"clh","clh"},
                           {"xl","xl"},
                           {" "," "}    //以只含一个空格的字符串作为数组结束的标志
};

//查找用户名是否存在，存在返回该用户名的下标，不存在则返回-1.出错返回-2
int find_name(const char *name){
    int i;

    if(name == NULL){   //判断是否为空
        printf("input name,NULL pointer!\n");
        return -2;
    }

    for(i = 0;users[i].username[0] != ' ';i++){
        if(strcmp(users[i].username,name) == 0){    //表是否相等
            return i;
        }
    }
    return -1;
}

//发送数据
void send_data(int conn_fd,const char *string){
    if((send(conn_fd,string,strlen(string),0)) < 0){
        my_err("send",__LINE__);    //发送时发生错误
    }
}

int main(){
    int         sock_fd,conn_fd;    //套接字和连接端口
    int         optval;             //设置套接字
    int         flag_recv = USERNAME;           //标识收到的是用户名还是密码
    int         ret;                //接收函数接收到的数据数目
    int         name_num;           //判断后，记录第几个用户
    pid_t       pid;                //进程
    socklen_t   cli_len;            //计算地址长度
    struct sockaddr_in      cli_addr,serv_addr;  //IP地址
    char        recv_buf[126];      //接收数据缓存

    //创建一个套接字
    if((sock_fd = socket(AF_INET,SOCK_STREAM,0)) < 0){  //使用IPv4 TCP/IP协议   穿件TCP流套接字
        my_err("socket",__LINE__);
    }

    //设置为通用套接字,并且使之可以重新绑定端口
    optval = 1;
    if((setsockopt(sock_fd,SOL_SOCKET,SO_REUSEADDR,(void*)&optval,sizeof(int))) < 0){
        my_err("setsockopt",__LINE__);
    }

    //初始化服务器端地址结构
    memset(&serv_addr,0,sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;               //使用 IPv4 进行通信(32位地址)
    serv_addr.sin_port = htons(SERV_PCRT);        //上面宏定义的服务器端端口
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);//指定地址为 0.0.0.0

    //将套接字绑定到本地端口
    if(bind(sock_fd,(struct sockaddr*)&serv_addr,sizeof(struct sockaddr)) < 0){
        my_err("bind",__LINE__);
    }

    //将套接字转化为监听字符
    if(listen(sock_fd,LISTENO) < 0){
        my_err("sock_fd",__LINE__);
    }

    cli_len = sizeof(struct sockaddr_in);//计算地址长度
    while(1){
        //通过 accept 接收客户端的连接需求，并返回连接套接字用于收发数据
        if((conn_fd = accept(sock_fd,(struct sockaddr*)&cli_addr,&cli_len)) < 0){
            my_err("accept",__LINE__);
        }

        printf("accept a new client,ip:%s\n",inet_ntoa(cli_addr.sin_addr));

        //创建一个子进程处理刚刚接收的连接请求
        if((pid = fork()) == 0){ //子进程
            while(1){
                if((ret = recv(conn_fd,recv_buf,sizeof(recv_buf),0)) < 0){
                    my_err("recv",__LINE__);
                    exit(1);
                }
                ret = strlen(recv_buf);
                recv_buf[ret-1] = '\0'; //将数据结束标志'\n'替换成字符串结束标志
                
                if(flag_recv == USERNAME){   //接收到的是用户名
                    name_num = find_name(recv_buf);
                    switch(name_num){
                        case -1:
                            send_data(conn_fd,"n\n");
                            break;
                        case -2:
                            exit(1);
                            break;
                        default:
                            send_data(conn_fd,"y\n");
                            flag_recv = PASSWORD;
                            break;
                    }
                }else if(flag_recv == PASSWORD){
                    if(strcmp(users[name_num].password,recv_buf) == 0){
                        send_data(conn_fd,"y\n");
                        send_data(conn_fd,"Welcome login my top server\n");
                        printf("%s login\n",users[name_num].username);
                        break;  //跳出while循环
                    }
                }else{
                    send_data(conn_fd,"n\n");
                }
            }
            
            close(sock_fd);
            close(conn_fd);
            exit(0);    //结束子进程
        
        }else{//父进程关闭刚刚接收的连接请求，执行accept等待其他连接请求
            close(conn_fd);//结束父进程
        }

    }
    return 0;
}

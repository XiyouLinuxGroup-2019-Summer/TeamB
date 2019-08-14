#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include"my_recv.h"   //自定义头文件
#include <stdlib.h>
#include <sys/epoll.h>
#include "all.h"
#include "my_serthread.h"
#include <pthread.h>
#include "md5.h"
#include "list.h"

    int i,id;
	int choice;

	infouser_list_t head;
	infouser_node_t * pos;

	//List_Init(head,infouser_node_t);
    




#define SERV_PORT   4507  //     服务器的端口
#define LISTENQ 12        //连接请求队列的最大长度
//#define EPOLEN 1

#define INVALID_USERINFO 'n'  //用户信息无效
#define VALID_USERINFO 'y'   //用户信息有效
#define EPOLEN 2000
#define USERNAME 0   //接受到的是用户名
#define PASSWORD 1    //接受到的密码

//发送数据
void send_data(int conn_fd,const char *string){
    if(send(conn_fd,string,strlen(string),0) < 0){
        my_err("send",__LINE__);  //在myrecv.h
    }
}

int main(){

    int sock_fd,conn_fd;
    int optval;
    int flag_recv = USERNAME;//标识接受到的是用户名还是密码
    int ret;
    int name_num;
    pid_t pid;
    socklen_t cli_len;
    struct sockaddr_in cli_addr,serv_addr;
    PACK * recvdata;

    //创建一个tcp套接字

    sock_fd = socket(AF_INET,SOCK_STREAM,0);
    if(sock_fd < 0){
        my_err("socket",__LINE__);
    }

    //设置该套接字是指可以重新绑定端口
    optval = 1;
    if(setsockopt(sock_fd,SOL_SOCKET,SO_REUSEADDR,(void *)&optval,sizeof(int)) < 0){
        my_err("setsockopt",__LINE__);
    }

    setsockopt(sock_fd,SOL_SOCKET,SO_KEEPALIVE,(void*)&optval,sizeof(int));
    memset(&serv_addr,0,sizeof(struct sockaddr_in));

    //初始化服务器段地址结构
    memset(&serv_addr,0,sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT); //设置端口号
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);    //设置地址   INADD_ANY 是本地地址


    //将套接字绑定到本地端口
    if(bind(sock_fd,(struct sockaddr*)&serv_addr,sizeof(struct sockaddr_in)) < 0)   my_err("bind",__LINE__);

    //将套接字转化为监听套接字
    if(listen(sock_fd,LISTENQ) < 0)    my_err("listen",__LINE__);
    //LISTENQ   为最大的连接数

    cli_len = sizeof(struct sockaddr_in);

   //创建一个epoll
    //参数含义epollid，和最多监视数量
    int epfd;
    epfd = epoll_create(EPOLEN);
    //创建一个事件数组
    //
    struct epoll_event events[EPOLEN];
    struct epoll_event ev;
    ev.events = EPOLEN; //监听事件
    ev.data.fd = sock_fd; //监听套接字
    //将listenfd加入epoll

    epoll_ctl(epfd,EPOLL_CTL_ADD,sock_fd,&ev);
    int nfds = 0;

    while(1)  //通过accept 接受客户端的链接请求,并返回连接套接字用于收发数据
    {
        //fd 是经socket 创建经函数bind绑定到本地某一端口上,然后经过函数listen转化而来的套接字
        //cli_addr 用来保存发起连接请求的主机的地址和端口

        //委托内核检测事件
        nfds = epoll_wait(epfd,events,EPOLEN,-1);

        if(nfds > 0){
            for(int i = 0;i < nfds;i++){
                printf(" The event type is %d\n",events[i].events);

                if(events[i].data.fd == sock_fd){//如果是主socket的事件，则表示有新的连接 
                    struct sockaddr_in cliaddr;
                    socklen_t cliaddrlen = sizeof(struct sockaddr);   //>;
                    struct epoll_event ev;


                    conn_fd = accept(sock_fd,(struct sockaddr *)&cli_addr,&cli_len);//jie shou liang jie
                    
                    
                    if(conn_fd < 0)  my_err("accept",__LINE__);
                    printf("accept a new client,ip: %s \n",inet_ntoa(cli_addr.sin_addr));

                    ev.data.fd = conn_fd;
                    ev.events = EPOLLIN|EPOLLET;
                    epoll_ctl(epfd,EPOLL_CTL_ADD,conn_fd,&ev); //将新的fd添加到epoll的监听队列

                }
                else if(events[i].events&EPOLLIN){//接收到数据读socket
                    //printf("dsadsadsadadas\n");
                    PACK *recvdata = NULL;
                    recvdata = (PACK *)malloc(sizeof(PACK));
                    int len_remain;
                    if((len_remain = recv(events[i].data.fd,recvdata,sizeof(PACK),0)) < 0){
                        perror("recv");
                        exit(1);
                    }
                    //printf("%s",recvdata->data.mes);

                /*recv_buf.send_fd = events[i].data.fd; //发送者的套接字已经改变 应转换为accept后的套接字
                recv_t *temp=(recv_t*)malloc(sizeof(recv_t)); //防止多线程访问一个结构体
                *temp=recv_buf;
                temp->epfd=epfd;
                temp->conn_fd=events[i].data.fd;
*/
                    pthread_t tid1;
                    recvdata->data.recv_fd = events[i].data.fd;
                    pthread_create(&tid1,NULL,(void *)myallthread,recvdata);

                    /*if((sockfd = events[i].data.fd) < 0) continue;
                    int n = read(sockfd,line,MAXLINE); //读
                        ev.data.ptr = md; //md为自定义类型，添加数据
                    ev.events = EPOLLOUT|EPOLLET; 
                    epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);//修改标识符，等待下一个循环发送数据，异步处理的精髓
                    */
                }
                else
                {
                    //其他情况的处理
                }  
            }
        }
    }
    close(sock_fd);
    return 0;
}
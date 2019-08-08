#include <stdio.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>


#define PORT 4507
#define LISTENQ 10
#define EPOLEN 2000
#define MAXLINE 1024

int main(){

    /* eopll_create(20); */


    int listenfd,optval;
    struct sockaddr_in servaddr;

    //创建一个套接字
    listenfd = socket(AF_INET,SOCK_STREAM,0);
    if(listenfd < 0){
        perror("listen");
        /* exit(1); */
    }

    optval = 1;//设置该套接字使之可以重新绑定端口
    if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,(void*)&optval,sizeof(int)) < 0){
        perror("setsockop");
        /* exit(1); */
    }

    //初始化服务器地址
    memset(&servaddr,0,sizeof(struct sockaddr_in));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    //绑定套接字
    if(bind(listenfd,(struct sockaddr *)&servaddr,sizeof(struct sockaddr_in)) < 0){
        perror("bind");
        /* exit(1); */
    }

    //转化为监听套接字

    if(listen(listenfd,LISTENQ) < 0){
        perror("listensjh");
        /* exit(1); */
    }


    //创建一个epoll
    //参数含义epollid，和最多监视数量
    int epfd;
    epfd = epoll_create(EPOLEN);
    //创建一个事件数组
    //
    struct epoll_event events[EPOLEN];
    struct epoll_event ev;
    ev.events = EPOLEN; //监听事件
    ev.data.fd = listenfd; //监听套接字
    //将listenfd加入epoll

    epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev);
    int nfds = 0,connfd,sockfd;

    while(1){
        //委托内核检测事件
        nfds = epoll_wait(epfd,events,EPOLEN,-1);
        //You shi jian fa sheng
        if(nfds > 0){
            for(int i = 0;i < nfds;i++){
                if(events[i].data.fd == listenfd){//如果是主socket的事件，则表示有新的连接 
                    struct sockaddr_in cliaddr;
                    socklen_t cliaddrlen = sizeof(struct sockaddr);   //>;
                    struct epoll_event ev;
                    connfd = accept(listenfd,(sockaddr *)&cliaddr,&cliaddrlen);//jie shou liang jie
                    if(connfd == -1){
                        perror("accept");
                        /* exit(1); */
                    }

                    ev.data.fd = connfd;
                    ev.events = EPOLLIN|EPOLLET;
                    epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&ev); //将新的fd添加到epoll的监听队列

                }

                else if(events[i].events&EPOLLIN){//接收到数据读socket

                    if((sockfd = events[i].data.fd) < 0) continue;
                    int n = read(sockfd,line,MAXLINE); //读
                        ev.data.ptr = md; //md为自定义类型，添加数据
                    ev.events = EPOLLOUT|EPOLLET; 
                    epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);//修改标识符，等待下一个循环发送数据，异步处理的精髓
                }

                else if(events[i].events&EPOLLOUT) //有数据待发送，写socket
                {
                    struct myepoll_data* md = (myepoll_data *)events[i].data.ptr; //取数据
                    sockfd = md->fd;

                    send(sockfd,md->ptr,strlen((char*md)->ptr),0); //发送数据
                    ev.data.fd = sockfd;
                    ev.events=EPOLLIN|EPOLLET;
                    epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev); //修改标识符，等待下一个循环时接收数据
                }
                else
                {
                    //其他情况的处理
                }  
            }
        }
    }
    return 0;
}

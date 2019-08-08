#include<stdio.h>
#include<string.h>
#include<assert.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/epoll.h>

#define MAXDFD               10     //事件表大小
#define SERV_PCRT           4507    //服务器端口
#define LISTENO              12     //请求队列的最大长度

//向内核时间表epfd中添加新事件的文件描述符fd
void epoll_add(int epfd,int fd);
//从内核时间表中删除fd
void epoll_del(int epfd,int fd);
//创建套接字
int create_sockfd();

int main(){
    int sockfd = create_sockfd();
    assert(sockfd != -1); //条件返回错误，则终止运行
    

    //创建内核事件表
    int epfd = epoll_create(MAXDFD);
    assert(epfd != -1);

    //设置epoll_event的结构成员
    /* struct epoll_event ev; */
    /* ev.data.fd = sockfd; */
    /* ev.events = EPOLLIN;    //有数据可读 */
    //向内核事件表epfd 中添加 新事件的文件描述符fd
    epoll_add(epfd,sockfd);

    //定义events数组存放就绪描述符
    struct epoll_event events[MAXDFD];

    while(1){
        /* 
		epoll_wait返回的是前n个已经全就绪的文件描述符，
		那么我们不用全部遍历，只遍历前n个就可以
		超时时间设置为5秒
		*/
        int n = epoll_wait(epfd,events,MAXDFD,5000);
        if(n == -1){
            perror("epoll wait error\n");
        }else if(n == 0){
            printf("time out!\n");
        }
        //只遍历前n个，因为内核已经告诉我们前n个就绪事件
        else{
            for(int i = 0;i < n;i++){
                int fd = events[i].data.fd;
                if(fd == -1)
                    continue;

                //events为内核为我们返回的就绪事件
                if(events[i].events & EPOLLIN){
                    if(fd == sockfd){
                        struct sockaddr_in caddr;
                        int len = sizeof(caddr);

                        //接收一个套接字建立的连接，得到套接字connfd
                        int connfd = accept(sockfd,(struct sockaddr*) &caddr,(socklen_t *)&len);

                        if(connfd < 0)
                            continue;

                        printf("accept c = %d\n",connfd);

                        //将套接字connfd，添加到内核事件中
                        epoll_add(epfd,connfd);
                    }
                    else{
                        char buff[128] = {0};

                        //recv用来接收客户端数据
                        int res = recv(fd,buff,127,0);

                        if(res<=0){
                            //不能先close，应该先调用epoll_del()
							//因为先调用close关闭了文件描述符后，再调用epoll_del()
							//内核就找不到所要从内核事件表中移除的文件描述符
                            
                            epoll_del(epfd,fd);
                            close(fd);
                            printf("one client over\n");
                            continue;
                        }

                        printf("buff %d = %s\n",fd,buff);
                        
                        send(fd,"OK",2,0);
                    }
                }
            }
        }
    }
}


//创建套接字
int create_sockfd(){
    //创建监听套接字(socket描述符)，制定协议族IPV4，字节流服务传输
    int sockfd;
    if((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0){
        printf("creat socket error!\n");
        return -1;
    }
   
    //socket专用地址信息设置
    struct sockaddr_in saddr;
    memset(&saddr,0,sizeof(saddr));

    saddr.sin_family = AF_INET;//ipv4
    saddr.sin_port = htons(SERV_PCRT);  //宏定义的服务器端口
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);//指定地址为0.0.0.0，或者"127.0.0.1"

    //命名套接字，将socket专用地址绑定到socket描述符上
    if((bind(sockfd,(struct sockaddr*)&saddr,sizeof(saddr))) < 0){
        printf("bind socket wrong!\n");
        return -1;
    }

    //创建监听队列
    listen(sockfd,LISTENO);
    return sockfd;
}

//向内核事件表epfd中添加新事件的文件描述符fd
void epoll_add(int epfd,int fd){
    //设置epoll_event的结构成员
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = fd;

    //EPOLL_CTL_ADD添加新事件及文件描述符到内核事件表中
    if(epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&ev) == -1){     //向epfd注册fd的上的event
        perror("epoll_ctl add error!\n");
    }
}

//从内核表中移除fd
void epoll_del(int epfd,int fd){
    if(epoll_ctl(epfd,EPOLL_CTL_DEL,fd,NULL) == -1){
        perror("epoll_ctl del error!\n");
    }
}

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<string.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<error.h>
#include<sys/epoll.h>
#include<pthread.h>
#include"my_recv.h"
#include"zzy_server.h"
#include"mysql.h"




//登录函数
int login_server(pack *recv_pack){
    int id = 0;
    char a[10];

    if((id = find_name(recv_pack->send_name)) == 0){    //该用户在线，没有退出
        a[0] = '2';
    }
    else if(userinfon_t[id].statue == ONLINE){
        a[0] = '3';
    }else if(passward(id,recv_pack[id].message) == 0){
        a[0] = '1';
        userinfon_t[id].socket_id = recv_pack[id].send_fd;
        printf("\n-----------longin----------\n");
        printf("%s getonline\n",userinfon_t[id].username);
        printf("statue: %d\n",userinfon_t[id].statue);
        printf("id:%d\n",userinfon_t[id].socket_id);
    }else{
        a[0] = '0';
    }
    a[1] = '\0';
    strcpy(recv_pack->send_name,"server");
    strcpy(recv_pack->message,a);
    recv_pack->recv_fd = recv_pack->send_fd;
    recv_pack->send_fd = listenfd;
    
    //发送包
    send_data(recv_pack);
    usleep(100);

    if(a[0] == '1')
        userinfon_t->statue = ONLINE;
    free(recv_pack);
    return 0;
}

int registering_server(pack *recv_pack){
    char a[10];
    int id = 0;
    if(passward(id,recv_pack[id].message) == 0){    
        a[0] = '1';
        user_sum++;
        strcpy(userinfon_t[user_sum].username,recv_pack->recv_name);
        printf("\n----------register----------\n");
        printf("new user %s join!\n",recv_pack->recv_name);
        userinfon_t[user_sum].statue = DOWLOAD;
    }else{
        a[0] = '0';
    }
    a[1] = '\0';
    
    strcpy(recv_pack->send_name,"server");
    strcpy(recv_pack->message,a);
    recv_pack->recv_fd = recv_pack->send_fd;
    recv_pack->send_fd = listenfd;
    
    send_data(recv_pack);
    usleep(100);

    free(recv_pack);
    
    return 0;
}

//选择函数
void *choices(void *recv_pack){
    pack *recv_pack_t = (pack*)recv_pack;
    printf("\nthe choice is %d\n",recv_pack_t->type);
    switch(recv_pack_t->type){
        case LOGIN:
             login_server(recv_pack_t);
             break;
        case REGISTING:
             registering_server(recv_pack_t);
    }
    return NULL;
}

/*void pthread_init(){
    printf("pthread_init");
    pthread_t pid_send,pid_file_check;
    pthread_create(&pid_send,NULL,serv_send_thread,NULL);
    pthread_create(&pid_file_check,NULL,pthread_check_file,NULL);
}*/


int main(){
    int conn_fd;
    int ret;
    pack recv_buf;
    pack *recv_pack;
    //int flag_recv = USERNAME;
    //int name_num;
    int sockfd;
    int epfd;   //创建内核事件表
    struct epoll_event events[MAXDFD];  //定义events数组存放就绪，描述符
    
    
    pthread_mutex_init(&mutex,NULL);
    
    printf("begin!\n");
    if((sockfd = create_sockfd()) < 0){
        my_err("creat sockfd",__LINE__);
        exit(1);
    }

    listenfd = sockfd;

    if((epfd = epoll_create(MAXDFD)) < 0){
        my_err("creat epfd",__LINE__);
        exit(1);
    }

    //向内核时间表epfd中添加新事件的文件描述符fd
    epoll_add(epfd,sockfd);

    //连接数据库
    mysql_init_t();

    while(1){
        /*epoll_wait返回的是前n个已经全就绪的文件描述符，
          那么我们不用全部遍历，只遍历前n个就可以
          超时时间设置为5秒*/
        int n = epoll_wait(epfd,events,MAXDFD,5000);
        if(n  < 0){
            my_err("epoll_wait wrong",__LINE__);
            exit(1);
        }else if(n == 0){
            printf("time out!\n");
        }
        else{
            //只遍历前n个，因为只有前n个是就绪事件
            for(int i = 0;i < n;i++){
                int fd = events[i].data.fd;
                if(fd < 0)
                    continue;

                //event为内核为我们返回的就绪事件，且有数据可读
                if(events[i].events & EPOLLIN){
                    if(fd == sockfd){
                        //计算地址长度
                        int cli_len = sizeof(struct sockaddr_in);
                        
                        //接收一个套接字建立的连接，得到套接字connfd;
                        struct sockaddr_in  cli_addr;
                        conn_fd = accept(sockfd,(struct sockaddr*)&cli_addr,(socklen_t*)&cli_len);

                        if(conn_fd < 0)
                           continue;

                        printf("accept a new client,ip is %s\n",inet_ntoa(cli_addr.sin_addr));

                        //将套接字connfd，添加到内核事件中
                        epoll_add(epfd,conn_fd);
                    }else{
                        
                        if((ret = recv(events[i].data.fd,&recv_buf,sizeof(pack),0)) < 0){
                            my_err("recv",__LINE__);
                            exit(1);
                        }
                        
                        recv_buf.send_fd = events[i].data.fd;

                        if(ret == 0){
                            epoll_del(epfd,fd);
                            close(fd);
                            printf("one client over!!!!\n");
                            continue;
                        }
                        if(ret < 0){
                            close(events[i].data.fd);
                            perror("recv error!");
                            continue;
                        }
                        
                        int type = 0;
                        type = recv_buf.type;
                        recv_pack = (pack*)malloc(sizeof(pack));
                        memcpy(recv_pack,&recv_buf,sizeof(pack));

                        //开启线程
                        pthread_t pid;
                        if(pthread_create(&pid,NULL,choices,(void *)recv_pack) != 0){
                            my_err("pthread_create",__LINE__);
                        }

                        /*if(type == send_file){
                            printf("please wait a moment\n");
                            usleep(100000);
                        }*/
                    }
                }
            }
        }
    }
}


/*int login_server(int conn_fd,char recv_buf[]){
    int ret;
    int name_num;
    int flag_recv = USERNAME;
                if(flag_recv == USERNAME){
                            ret = strlen(recv_buf);
                            recv_buf[ret-1] = '\0';//将数据结束符由'\n'换成'\0'
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
                            ret = strlen(recv_buf);
                            recv_buf[ret-1] = '\0';
*/                            
                            /* printf("%s \n",users[name_num].password); */

/*                            if(strcmp(users[name_num].password,recv_buf) == 0){
                                printf("--111--\n");
                                send_data(conn_fd,"y\n");
                                send_data(conn_fd,"Welcome!\n");
                                printf("%s login\n",users[name_num].username);
                                flag_recv = 1;
*/                                /* break;  //跳出while循环 */
                            /* } */
/*                        }else if(flag_recv == 1){
                            send_data(conn_fd,"n\n");
                        }
            return 0;
}*/

//查找用户名是否存在，存在返回该用户名的下标，不存在则返回-1.出错返回-2
int find_name(const char*name){

    if(name == NULL){
        printf("please input name,NULL, pointer!\n");
        return -2;
    }
    
    
    //先用数组存着，后期改---------------------------------------------------------------------------------------
    /*for(int i = 0;users[i].username[0] != ' ';i++){
        if(strcmp(users[i].username,name) == 0){    //相同
            return i;
        }
    }*/
    


    return -1;
}

//发送数据
void send_data(int conn_fd,const char *string){
    if((send(conn_fd,string,strlen(string),0)) < 0){
        my_err("send",__LINE__);    //发送时发生错误
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
   
    int option_value;
    int length = sizeof(option_value);
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&option_value,length);

    //socket专用地址信息设置
    struct sockaddr_in saddr;
    memset(&saddr,0,sizeof(saddr));

    saddr.sin_family = AF_INET;//ipv4
    saddr.sin_port = htons(4507);  //宏定义的服务器端口
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);//指定地址为0.0.0.0>

    //命名套接字，将socket专用地址绑定到socket描述符上
    if((bind(sockfd,(struct sockaddr*)&saddr,sizeof(saddr))) < 0){
        printf("bind socket wrong!\n");
        close(sockfd);
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
    if(epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&ev) == -1){     //向epfd注
        perror("epoll_ctl add error!\n");
    }
}

//从内核表中移除fd
void epoll_del(int epfd,int fd){
    if(epoll_ctl(epfd,EPOLL_CTL_DEL,fd,NULL) == -1){
        perror("epoll_ctl del error!\n");
    }
} 

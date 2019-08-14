#include"my_recv.h"
#include"zzy_server.h"
#include"mysql.h"



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




//登录函数
int login_server(pack *recv_pack){
    int id = 0;
    char a[10];

    if((id = find_name(recv_pack->send_name)) == 0){//找不到用户
        a[0] = '2';
    }
    else if(userinfon_t[id].statue == ONLINE){  //用户在线
        a[0] = '3';
    }else if(passward(id,recv_pack[id].message) == 0){  //登录成功
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

//注册函数
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




//开启两个线程
/*void pthread_init(){
    printf("pthread_init");
    pthread_t pid_send,pid_file_check;
    pthread_create(&pid_send,NULL,downline_send_server,NULL);
    pthread_create(&pid_file_check,NULL,pthread_check_file,NULL);
}*/


//上线发包
void downline_send_server(void){
     int statue = DOWLOAD;
     int recv_fd_t,recv_fd_online;
     while(1){   
         //上锁，防止处理包时包发生改变
         pthread_mutex_lock(&mutex); 
         
         statue = ONLINE;
         for(int i = pack_send_num-1;i >= 0;i--){   
             for(int j = 1; j <= pack_send_num ;j++){   
                //
                 if(strcmp(pack_send[i].recv_name,userinfon_t[j].username) == 0){   
                     statue = userinfon_t[j].statue;
                     if(statue == ONLINE)
                         recv_fd_online = userinfon_t[j].socket_id;
                     break;
                 }   
             }   
             //判断状态，若在线，或即将登陆，或注册成功，则发包
             if(statue == ONLINE || pack_send[i].type == LOGIN || pack_send[i].type == REGISTING){   
                 if(statue == ONLINE)
                     recv_fd_t = recv_fd_online;
                 else
                     recv_fd_t = pack_send[i].recv_fd;
                     
                 if(send(recv_fd_t,&pack_send[i],sizeof(pack),0) < 0){ 
                     my_err("send",__LINE__);
                 } 
                 printf("\nsend pack\n");
                 printf("type is %d\n",pack_send[i].type);
                 printf("from %s  to  %s\n",pack_send[i].send_name,pack_send[i].recv_name);
                 printf("message : %s\n",pack_send[i].message);
                 printf("recv_fd : %d\n",pack_send[i].recv_fd);
                 pack_send_num-- ;
                 printf("pack_send_num:%d\n", pack_send_num);
                 
                 //向前移动一位
                 for(int j = i;j <= pack_send_num && pack_send_num;j++){   
                     pack_send[j] = pack_send[j+1];
                 }   
                 break;
             }   
         }
         //解锁
         pthread_mutex_unlock(&mutex);
         //停顿，等待发包结束
         usleep(100);  
     }   
 }

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




//查找用户名是否存在，存在返回该用户名的下标，不存在则返回-1.出错返回-2
int find_name(const char *name){
    //名字为空返回
    if(name == NULL){
        printf("please input name,NULL, pointer!\n");
        return -2;
    }
    //用户总数为0
    if(user_sum == 0){
        return -1;
    }
    //查找并返回用户id
    for(int i = 0;i < user_sum;i++){
        if((strcmp(name,userinfon_t[i].username)) == 0){
            return i;
        }
    }

    //先用数组存着，后期改---------------------------------------------------------------------------------------
    /*for(int i = 0;users[i].username[0] != ' ';i++){
        if(strcmp(users[i].username,name) == 0){    //相同
            return i;
        }
    }*/
    return -1;
}

//添加到发送数据的队列，发送时加锁，以防发送时数据包被改变
void send_data(pack *pack_t){
    pthread_mutex_lock(&mutex);
    memcpy(&(pack_send[pack_send_num++]),pack_t,sizeof(pack));
    pthread_mutex_unlock(&mutex);
    /*if((send(conn_fd,string,strlen(string),0)) < 0){
        my_err("send",__LINE__);    //发送时发生错误
    }*/
}




//创建函数---------------------------------------------------------------------------------------------------------
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
//创建函数-----------------------------------------------------------------------------------------------------

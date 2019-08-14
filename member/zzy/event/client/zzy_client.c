#include"zzy_client.h"

int main(){
    int conn_fd;
    //创建一个TCP套接字
    if((conn_fd = socket(AF_INET,SOCK_STREAM,0)) < 0){
        my_err("socket",__LINE__);
    }
    
    //初始化服务器地址
    struct sockaddr_in serv_addr;
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(4507);//将unsigned short整形数据转化为网络字节顺序,与大小端有关serv_addr.sin_addr.s_addr = inet_addr(INADDR_ANY);//IP地址
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//IP地址
    //向服务器发送请求
    if((connect(conn_fd,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr)) < 0)){
        my_err("connect",__LINE__);
    }
    
    int choice = welcome_login_IU();
    switch(choice){
        case 1:
            //注册函数
            break;
        case 2:
            login(conn_fd);
            break;
        case 3:
            //找回密码函数
            break;
        case 4:
            return 0;
    }
}

//登录
int login(int conn_fd){
    //输入用户名和密码
    intput_userinfo(conn_fd,"username");
    intput_userinfo(conn_fd,"password");
    
    char    recv_buf[BUFSIZE];
    int ret;
    if((ret = my_recv(conn_fd,recv_buf,sizeof(recv_buf))) < 0){
        printf("data is too long!\n");
        exit(1);
    }

    for(int i = 0;i < ret;i++){
        printf("%c",recv_buf[i]);
    }
    printf("\n");
    
    //进入菜单
    //menu();

    //关闭端口
    close(conn_fd);
    return 0;
}


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
    char input_buf[126];
    char recv_buf[128];
    int  flag_userinfo;

    //输入用户信息直到正确为止
    do{
        printf("%s:",string);
        if(get_userinfo(input_buf,126) < 0){
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


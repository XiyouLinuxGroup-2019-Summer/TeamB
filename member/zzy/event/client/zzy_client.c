#include"zzy_client.h"
int conn_fd;

int main(){
    //创建一个TCP套接字
    if((conn_fd = socket(AF_INET,SOCK_STREAM,0)) < 0){
        my_err("socket",__LINE__);
    }
    
    //初始化服务器地址
    struct sockaddr_in serv_addr;
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(4507);//将unsigned short整形数据转化为网络字节顺序,与大小端有关
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//IP地址
    //向服务器发送请求
    if((connect(conn_fd,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr)) < 0)){
        my_err("connect",__LINE__);
    }
    
    printf("\n启动成功！\n");


    friend_statue();


    int choice;
    if((choice = welcome_login_UI()) == 0){
        my_err("welcome_login_UI",__LINE__);
        exit(1);
    }
    switch(choice){
        case 1:
            //注册
            registering_client();
            break;
        case 2:
            //登录
            login_client();
            break;
        case 3:
            //找回密码
            find_passward();
            break;
        case 4:
            return 0;
    }
}

//更新好友状态
void friend_statue(){
    pack pack_friend;
    pack_friend.type = SEE_FRIEND;
    strcpy(pack_friend.send_name,user_infon->username);
    strcpy(pack_friend.recv_name,"server");
    memset(pack_friend.message,0,sizeof(pack_friend.message));
    if(send(conn_fd,&pack_friend,sizeof(pack),0) < 0){
        my_err("friend_statue",__LINE__);
    }
}


//登录
int login_client(){
    //输入用户名和密码
    int t = intput_login_userinfo();
    if(t == 0){
        return 0;
    }
    else if(intput_login_userinfo() == 1){
    //进入菜单
    //menu();
    }

    //关闭端口
    close(conn_fd);
    return 0;
}



//发送登录或注册信息并接收回馈
int send_input_client(int type,char *username,char *passward){
    pack pack_t;
    send_pack(username,"server",type,passward);

    if(recv(conn_fd,&pack_t,sizeof(pack),0) < 0){
        my_err("send_input 接收登录或注册信息 error",__LINE__);
    }

    return pack_t.message[0] - '0';
}


//对输入的用户名和密码进行判断，用户有三次输入的机会                                                                                                     
int intput_login_userinfo(){
    char username[MAX_SIZE];
    char  passward[10];
    int ans;    //登录反馈信息结果
    for(int i = 2;i >= 0;i--){
        printf("请输入你的用户名:\n");
        scanf("%s",username);
        printf("请输入你的密码:\n");
        scanf("%s",passward);
        ans = send_input_client(LOGIN,username,passward);
        
        if(ans == 1){
            strcpy(user_infon->username,username);
            printf("Welcome!!!");
            /* menu(); */
            break;
        }else if(ans == 3){
            printf("该用户正处于登录状态\n");
            return 0;
        }else if(ans == 0){
            printf("密码错误！你还有%d次机会",i);
            continue;
        }else if(ans == 2){
            printf("该用户不存在，请先注册！");
            return 0;
        }
    }
    return -1;
}


//注册函数
int registering_client(){
    char username[MAX_SIZE];
    char passward[MAX_SIZE];
    char passward_t[MAX_SIZE];
    char choice;

    while(1){
        printf("请输入你要创建的用户名：");
        scanf("%s",username);
        while(1){
            printf("请输入你的密码：");
            scanf("%s",passward);
            printf("请确认你的密码：");
            scanf("%s",passward_t);
            if((strcmp(passward,passward_t)) == 0){
                break;
            }
            else{
                do{
                    printf("两次输入密码不一样！\n");
                    printf("[1]重新输入\t[2]退出");
                    printf("请输入你的选择：");
                
                }while((choice = getchar()) != '1' || (choice = getchar()) != '2');
                if(choice == 1){
                    continue;
                }else{
                    return 0;
                }
            }
        }
    }
    //发送并接收验证
    if(send_input_client(REGISTING,username,passward) == 1){
        printf("注册成功！\n");
    }else{
        printf("该用户已被注册过！\n");
    }
}



//发送包
void send_pack(char *send_name,char *recv_name,int type,char *message){
    pack pack_send;
    pack_send.type = type;
    strcpy(pack_send.message,message);
    strcpy(pack_send.recv_name,recv_name);
    strcpy(pack_send.send_name,send_name);

    if((send(conn_fd,&pack_send,sizeof(pack),0)) < 0){
        my_err("send_pack error!",__LINE__);
    }
}

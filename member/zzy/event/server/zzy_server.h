#ifndef _SERVER_H_
#define _SERVER_H_

#include<mysql/mysql.h>

#define INVALID_USERINFO    'n'     //用户信息无效
#define VALTD_USERONFO      'y'     //用户信息有效

#define USERNAME             0      //接收到的是用户名
#define PASSWORD             1      //接收到的是密码

#define SERV_PCRT           4507    //服务器端口
#define MAXDFD               10     //事件表大小
#define LISTENO              12     //请求队列的最大长度

struct userinfo {   //保存用户名和密码的结构体
    char username[32];
    char password[32];
};


/*struct userinfo users[] = {{"linux","unix"},
                           {"4507","4508"},
                           {"clh","clh"},
                           {"xl","xl"},
                           {" "," "}    //以只含一个空格的字符串作为数组结束的标志
};*/

#define LOGIN 1
#define REGISTING 2


//用户状态
#define ONLINE  1   //在线
#define DOWLOAD 2   //下线
  
#define MAX_SIZE    400

//定义发送接收的包
typedef struct package{
      int type;                   //包类型    
      char message[MAX_SIZE*2];   //包信息
      char send_name[MAX_SIZE];   
      int send_fd;                //发送方
      char recv_name[MAX_SIZE];   
      int recv_fd;                //接收方
}pack;
pthread_mutex_t mutex;  //锁
int user_sum = 0;       //用户数
int listenfd;           //监听套接字

//登录函数
int login_server(pack *recv_pack);
//选择函数
void *choices(void *recv_pack);
//查找用户名是否存在，存在返回该用户名的下标，不存在则返回-1.出错返回-2
int find_name(const char *name);
//发送数据
void send_data(pack *pack_t);
//创建套接字
int create_sockfd();
//向内核时间表epfd中添加新事件的文件描述符fd
void epoll_add(int epfd,int fd);                                                   
//从内核时间表中删除fd
void epoll_del(int epfd,int fd);
//密码判断函数
int passward(int id,char message[]);
//注册函数
int registering_server(pack *recv_pack);
#endif


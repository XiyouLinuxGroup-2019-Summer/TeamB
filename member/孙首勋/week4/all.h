#ifndef __MY_CLIENT_H
#define __MY_CLIENT_H

#define MAX_CHAR 1024

#define LOGIN                    1
#define REGISTER                 2
#define FRI_CHA                  3
#define FRI_ADD                  4
#define FRI_DEL                  5
#define GROUP_SEE                6  
#define GROUP_CREATE             7
#define GROUP_JOIN               8
#define GROUP_QIUT               9
#define GROUP_DEL                10
#define CHAT_ONE                 11
#define CHAT_MANY                12
#define FILE_SEND_BEGIN          13
#define FILE_SEND_BEGIN_RP       14
#define FILE_SEND_STOP_RP        15
#define FILE_RECV_RE             16
#define FILE_SEND                17
#define FILE_RECV_BEGIN          18 
#define FILE_RECV_BEGIN_RP       19
#define FILE_RECV_STOP_RP        20
#define FILE_RECV                21
#define FILE_FINI_RP             22
#define MES_RECORD               23
#define GROUP_KICK               24
#define GROUP_SET                25

#define EXIT                     -1
#define USER_MAX                 100
#define GROUP_MAX                100

#define BUFSIZE                  1024
typedef struct datas{
    char     send_name[MAX_CHAR];
    char     recv_name[MAX_CHAR];
    int      send_fd;
    int      recv_fd;
    //time_t   time;
    char     mes[MAX_CHAR*2];
}DATA;
 
typedef struct package{
    int   type;
    DATA  data;
}PACK;


typedef struct infor_group
{
    char  group_name[MAX_CHAR];
    int   member_num;
    char  member_name[USER_MAX][MAX_CHAR];  //begin from 1
}INFO_GROUP;
 
INFO_GROUP   m_infor_group  [GROUP_MAX]; //begin from 1
int          m_group_num;


typedef struct infor_user 
{
    char username[MAX_CHAR];
    unsigned int password[4];
    int  statu;//don't foget to change is to 0 when the server begin
    int  socket_id;
    char friends[USER_MAX][MAX_CHAR];//begin from 1
    int  friends_num;
    char group[GROUP_MAX][MAX_CHAR];  //begin from 1
    char group_num;
}INFO_USER;
 
 
INFO_USER  m_infor_user  [USER_MAX];
int        m_user_num; 


#endif
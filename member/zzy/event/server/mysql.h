#ifndef _MY_SQL_H_
#define _MY_SQL_H_

#include<stdio.h>
#include<mysql/mysql.h>
#include"zzy_server.h"

#define USERNAME_SIZE  100   //用户名最大字节数
#define GROUPNAME_SIZE 100   //群组最大数



//关闭mysql
void mysql_close(void); 
//连接，初始化数据库 
void mysql_init_t(void);

USER_INFORN userinfon_t[MAX_SIZE];


#endif

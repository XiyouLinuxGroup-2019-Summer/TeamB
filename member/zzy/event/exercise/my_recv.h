#ifndef _MY_RECV_H
#define _MY_RECV_H

#define BUFSIZE 1024    //最大输入
void my_err(const char *err_string,int line);   //报错函数
int my_recv(int conn_fd,char *data_buf,int len);   //从套接字上读取一次数据

#endif

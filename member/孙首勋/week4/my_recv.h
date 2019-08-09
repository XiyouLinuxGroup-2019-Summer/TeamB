#ifndef __MY_RECV_H
#define __MY_RECV_H
    #define BUFSIZE 1024
    void my_err(const char * err_string,int line);
    int my_recvlogin(PACK *data2,login_data *data1,int len);

#endif


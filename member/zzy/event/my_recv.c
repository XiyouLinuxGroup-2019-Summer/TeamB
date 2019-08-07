#define MY_RECV_C

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<errno.h>
#include"my_recv.h"

//自定义一个错误输出函数，错误时输出错误代码行数和错误原因
void my_err(const char *err_string,int line){
    fprintf(stderr,"line:%d",line);
    perror(err_string);
    exit(1);
}

//从套接字上读取一次数据（以'\n'为结束标志）
//参数：conn_fd————从该套接字上接收数据
//      data_buf————读取到的数据保存到该缓冲区内
//      len————data_buf所指向的空间长度
//返回值：错误返回-1    服务器已关闭连接返回0，成功返回读取的字节数
int my_recv(int conn_fd,char *data_buf,int len){
    static char recv_buf[BUFSIZE];     //自定义缓存区
    static char *pread;     //指向下一次读取数据的位置
    static int len_remain = 0;  //自己定义缓冲区中剩余的字节数
    int i;

    //如果自定义缓冲区中没有数据，则从套接字读取数据
    if(len_remain <= 0){
        if((len_remain = recv(conn_fd,recv_buf,sizeof(recv_buf),0))< 0){
            my_err("my_recv",__LINE__);
        }else if(len_remain == 0){       //目的计算机的 socket 关闭
            return 0;
        }
        pread = recv_buf;   //重新初始化 pread 指针
    }
    
    //从自定义的缓冲区中读取一次数据
    for(i = 0;*pread != '\n';i++){      //读取以‘\n’作为结束
        if(i > len){    //防止指针越界
            printf("i = %d pread = %s\n",i,pread);
            return -1;
        }
        data_buf[i] = *pread++;
        len_remain--;
    }
    //取出结束标志
    len_remain--;
    pread++;
    
    return i;   //读取成功
}

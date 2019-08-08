/* #define MY_RECV_C

#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include "my_recv.h"
#include <stdlib.h>
#include "all.h"
//自定义错误处理函数

void my_err(const char * err_string,int line){
    fprintf(stderr,"line:%d ",line);
    perror(err_string);
    exit(1);

}

/*函数名：my_recv
 *描述：从套接字上读取一次数据（以\n为结束标志）
 *参数：conn_fd---从该链接套接字上接受数据
 *      data——buf---读取到的数据保存在此缓冲区
 *      len————data——buf所指向空间长度
 *  返回值：出错返回-1，服务器端已关闭链接则返回0，成功返回读取自接数
 *
 **/
/*int my_recv(int conn_fd,PACK *recvdata,int len){


    static char *pread; //指向下一次读取数据的位置
    static int len_remain = 0;
    int i;

    //如果自定义缓冲区没有数据，则从套接字中读取数据
    
    if(len_remain <= 0){
        if((len_remain = recv(conn_fd,recvdata,sizeof(recvdata),0)) < 0){
            my_err("recv",__LINE__);
        }
        else if(len_remain == 0){
            return 0;
        }

        pread = recvdata->data.mes; //初始化pread指针
    }

    //从自定义缓冲区中读取一次数据
    
    for(i = 0;*pread != '\n';i++){
        if(i > len){
            return -1;
        }
        recvdata->data.mes = *pread++;
        len_remain--;
    }

    //去除结束标志
    len_remain--;
    pread++;

    return i;
    //读取成功

}*/

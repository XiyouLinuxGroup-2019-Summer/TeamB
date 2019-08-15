#define MY_RECV_C
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
int my_recvlogin(PACK *data2,login_data *data1,int len){

    
    static char *pread; //指向下一次读取数据的位置
    pread = data2->data.mes; //初始化pread指针
    int i,j;
    int wei = 0;
    int xxx = strlen(data2->data.mes);
 
    for(j = 0;j < 2;j++){
        //printf("jinru\n");
        for(i = 0;*pread != '\n';i++){
            if(j == 0){
                if(i > len){
                    printf("越栈\n");
                    return -1;
                }
                data1->username[i] = *pread++;
            }
            else if(j == 1){
                if(i > len){
                    printf("越栈\n");
                    return -1;
                }
                data1->password[i] = *pread++;
            }
        }
        *pread++;
    }
    /*printf("%d\n%d\n%s\n%s",strlen(data1->username),strlen(data1->password),data1->username,data1->password);
    getchar();
    getchar();
    getchar();*/
    //pread++;
    return i;
    //读取成功
}

int my_recvregister(PACK *data2,info_user *data1,int len){
    
    static char *pread; //指向下一次读取数据的位置
    pread = data2->data.mes; //初始化pread指针
    int i,j;
    int wei = 0;

    for(j = 0;j < 4;j++){
        for(i = 0;*pread != '\n';i++){
            if(j == 0){
                if(i > len){
                    printf("越栈1\n");
                    return -1;
                }
                data1->username[i] = *pread++;
            }
            else if(j == 1){
                if(i > len){
                    printf("越栈2\n");
                    return -1;
                }
                data1->password[i] = *pread++;
            }
            else if(j == 2){
                if(i > len){
                    //printf("%s %s %s",data1->password,data1->username,data1->mibao);
                    printf("越栈3\n");
                    //printf("%s %s %s",data1->password,data1->username,data1->mibao);
                    return -1;
                }
                data1->mibao[i] = *pread++;
            }
            else if(j == 3){
                if(i > len){
                    printf("越栈4\n");
                    return -1;    
                }
                data1->sex[i] = *pread++;
            }
        }
        *pread++;
    }
    printf("%s",data1->password);
    
    //pread++;

    return i;
    //读取成功
}

// pthread_once函数的演示, run 函数仅能被执行一次

#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
pthread_once_t once = PTHREAD_ONCE_INIT;
void run (void){
    printf("Function is running in thread %lu\n",pthread_self());
}

void * thread1(void *arg){
    pthread_t thid = pthread_self();
    printf("Current thread's id is %lu\n",thid);
    pthread_once(&once,run);    //保证 run 只被执行一次
    printf("thread1 ends\n");
}

void *thread2(void *arg){
    pthread_t thid = pthread_self();
    printf("Current thread's id is %lu\n",thid);
    pthread_once(&once,run);
    printf("thread2 ends\n");
}

int main(){
    pthread_t thid1,thid2;
    pthread_create(&thid1,NULL,thread1,NULL);
    pthread_create(&thid2,NULL,thread2,NULL);
    sleep(2);
    printf("main thread exit!\n");
    exit(0);
}

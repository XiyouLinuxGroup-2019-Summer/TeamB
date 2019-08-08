#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
static void* threadfunc(void *arg)
{
    char *s=(char *)arg;
    printf("%s",s);
    return (void *)strlen(s);
}
int main()
{
    pthread_t t1;
    void *res;
    int s;
    s=pthread_create(&t1,NULL,threadfunc,"hello world\n");
    if(s!=0)
    {
        printf("errno : pthread_create\n");
    }
        printf("message from main()\n");
        s=pthread_join(t1,&res);
        if(s!=0)
        {
            printf("errno:pthread_jion\n");
        }
        printf("thread return %ld\n",(long )res);
        exit(EXIT_SUCCESS);
}


#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>

#define MAX_THREAD 3

unsigned long long main_counter, counter[MAX_THREAD];
void* thread_worker(void*);
pthread_mutex_t mutex;
int main(void)
{
        int i, ch;
        pthread_t pthread_id[MAX_THREAD] = {0};
        pthread_mutex_init(&mutex, NULL);
        for (i = 0; i < MAX_THREAD; i++) {
                pthread_create(&pthread_id[i], NULL, thread_worker, (void *)i);
        }
        do {
                unsigned long long sum = 0;
                for (i = 0; i < MAX_THREAD; i++) {
                        sum += counter[i];
                        printf("*%llu   ", counter[i]);
                }
                printf("%llu/%llu", main_counter, sum);
	    }while ((ch = getchar()) != 'q');
	        return 0;
        }

void* thread_worker(void* p)
{
        pthread_detach(pthread_self());
        int thread_num;
        thread_num = (int)p;
        pthread_mutex_lock(&mutex);
        for(;;) { 
                counter[thread_num]++;
                main_counter+=1;
                sleep(1);
        }
        pthread_mutex_unlock(&mutex);
}

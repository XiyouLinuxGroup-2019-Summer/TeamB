#include "threadpool.h"
#include "queue.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct Threadpool {
        Queue *queue;
        pthread_t *pids;
        pthread_mutex_t mutex;
        pthread_cond_t cond;
        size_t thread_count;
        int is_start;
} Threadpool;

typedef struct Task {
        void (*callback)(void *);
        void *args;
} Task;

void *threadstart(void *args){
        Threadpool *pool = (Threadpool *)args;
        while(pool->is_start){
                pthread_mutex_lock(&pool->mutex);
                while(Queuesize(pool->queue)<1&&pool->is_start){
                        pthread_cond_wait(&pool->cond, &pool->mutex);
                }
                Task *task = (Task *)Queuepop(pool->queue);
                pthread_mutex_unlock(&pool->mutex);
                if(task && task->callback){
                        task->callback(task->args);
                        free(task);
                } 
        }
        return NULL;
}

Threadpool * Threadpoolinit(size_t thread_count){
        Threadpool *pool = (Threadpool *)malloc(sizeof(Threadpool));
        pool->queue = Queueinit();
        pool->pids = (pthread_t *)malloc(thread_count*sizeof(pthread_t));
        pthread_mutex_init(&pool->mutex, NULL);
        pthread_cond_init(&pool->cond, NULL);
        pool->thread_count = thread_count;
        pool->is_start = 0;
        return pool;
}

void Threadpoolcreate(Threadpool *pool)
{
        if(pool->is_start != 0)
        {
            return ;
        }
        pool->is_start = 1;
        int i;
        for (i = 0; i < pool->thread_count; i++){
                pthread_create(&pool->pids[i], NULL, threadstart, (void *)pool);
        }
}

void Threadpoolpushtask(Threadpool *pool, void (* callback)(void *),void *args)
{
       Task *task = (Task *)malloc(sizeof(Task));
       task->callback = callback;
       task->args = args;
       pthread_mutex_lock(&pool->mutex);
       Queuepush(pool->queue,task);
       pthread_mutex_unlock(&pool->mutex);
       pthread_cond_signal(&pool->cond);
}

void Threadpoolstop(Threadpool *pool)
{
        pool->is_start = 0;
}

void Threadpooldestory(Threadpool *pool)
{
        Threadpoolstop(pool);
        pthread_cond_broadcast(&pool->cond);
        int i;
        for (i = 0; i < pool->thread_count; i++) {
            pthread_join(pool->pids[i], NULL);
        }
        Queuedestory(pool->queue, free);
        pthread_mutex_destroy(&pool->mutex);
        pthread_cond_destroy(&pool->cond);
}

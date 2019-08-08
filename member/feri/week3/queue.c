#include "queue.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct Node {
        void *data;
        struct Node *next;
} Node;

typedef struct Queue {
        Node *head;
        Node *tail;
        size_t size;
} Queue;

Queue *Queueinit()
{
        Queue *queue = (Queue *)malloc(sizeof(Queue));
        queue->head = NULL;
        queue->tail = NULL;
        queue->size = 0;
        return queue;
}

void Queuepush(Queue *queue, void *data)
{
        Node *node = (Node *)malloc(sizeof(Node));
        node->data = data;
        if (queue->tail != NULL) {
            queue->tail->next = node;
            queue->tail = node;
        queue->size++;
        }else {
            queue->head = queue->tail = node;
        queue->size++;
        }
}

void *Queuepop(Queue *queue)
{
        void *data = NULL;
        Node *tmp;
        if (queue->size == 1) {
                data = queue->head->data;
                free(queue->head);
                queue->head = queue->tail = NULL;
                queue->size--;
        }else if (queue->size > 1){
                data = queue->head->data;
                tmp = queue->head;
                queue->head = queue->head->next;
                free(tmp);
                queue->size--;
        }
        return data;
}                                    
void Queuedestory(Queue *queue,void (*datadestory)(void *))
{
        Node *node = queue->head;
        while (node != NULL) {
                Node *tmp = node->next;
                datadestory(node->data);
                free(node);
                node = tmp;
        }
        free(queue);
}

size_t Queuesize(Queue *queue)
{
    return queue->size;
}

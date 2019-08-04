#include <stddef.h>

typedef struct Queue Queue;

Queue *Queueinit(void);
void Queuepush(Queue *queue,void *data);
void *Queuepop(Queue *queue);
void Queuedestory(Queue *queue, void (*datadestory)(void *));
size_t Queuesize(Queue *queue);

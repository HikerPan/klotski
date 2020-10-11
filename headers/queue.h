#ifndef QUEUE_H
#define QUEUE_H

#include "position.h"

typedef struct queue
{
    POSITION *arr;
    int front;
    int rear;
} QUEUE;

int is_full(struct queue *q);
int is_empty(struct queue *q);
int add(struct queue *q, POSITION value);
int get(struct queue *q, POSITION *value);

#endif // QUEUE_H

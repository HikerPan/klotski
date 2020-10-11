#include "queue.h"

int is_full(struct queue *q)
{
    return q->rear == MAX;
}

int is_empty(struct queue *q)
{
    return q->front == -1 || q->front == q->rear;
}

int add(struct queue *q, POSITION value)
{
    if (is_full(q))
        return 0;

    if (q->front == -1)
        q->front = 0;

    q->arr[q->rear++] = value;

    return 1;
}

int get(struct queue *q, POSITION *value)
{
    if (is_empty(q))
        return 0;

    *value = q->arr[q->front++];

    return 1;
}

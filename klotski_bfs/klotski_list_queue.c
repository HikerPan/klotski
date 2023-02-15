/*
 * @Author: Alex.Pan
 * @Date: 2023-02-06 13:24:22
 * @LastEditTime: 2023-02-10 17:03:09
 * @LastEditors: Alex.Pan
 * @Description: 
 * @FilePath: /esp_gobang/components/klotski/klotski_list_queue.c
 * Copyright 2010-2021 LEKTEC or its affiliates. All Rights Reserved.
 */
#include "klotski_list_queue.h"

LIST_QUEUE g_lq = {0};

int list_queue_init(VISIT_TABLE *visit)
{
    if(NULL == visit)
    {
        printf("[init]visit is null.");
        return 0;
    }

    g_lq.head = g_lq.front = g_lq.rear = visit;
    visit->next = NULL;

    return 1;
}

int list_queue_in(VISIT_TABLE *visit)
{
    if(NULL == visit)
    {
        printf("[in]visit is null.");
        return 0;
    }

    visit->next = NULL;
    g_lq.rear->next = visit;
    g_lq.rear = visit;

    // if(is_list_queue_empty())
    if(NULL == g_lq.front)
    {
        g_lq.front = g_lq.rear;
    }

    return 1;
}

VISIT_TABLE *list_queue_out(void)
{
    VISIT_TABLE *p = NULL;
    p = g_lq.front;

    g_lq.front = g_lq.front->next;


    return p;
}


int is_list_queue_empty(void)
{
    if((g_lq.front == g_lq.rear)&&(g_lq.head!=g_lq.front)&&(g_lq.head != g_lq.rear))
    {
        return 1;
    }

    return 0;
}

/*
 * @Author: Alex.Pan
 * @Date: 2023-02-06 13:24:22
 * @LastEditTime: 2023-02-10 16:51:33
 * @LastEditors: Alex.Pan
 * @Description: 
 * @FilePath: /esp_gobang/components/klotski/klotski_list_queue.h
 * Copyright 2010-2021 LEKTEC or its affiliates. All Rights Reserved.
 */
#ifndef _LIST_QUEUE_H_
#define _LIST_QUEUE_H_

#include "klotski_position.h"
typedef struct _list_queue_t_
{
    VISIT_TABLE *head;       // 一直指向链表头
    VISIT_TABLE *front;      // 链表队列头
    VISIT_TABLE *rear;       // 链表队列尾部
}LIST_QUEUE;
extern LIST_QUEUE g_lq;

int list_queue_init(VISIT_TABLE *visit);
int list_queue_in(VISIT_TABLE *visit);
// int list_queue_out(VISIT_TABLE *visit);
VISIT_TABLE *list_queue_out(void);
int is_list_queue_empty(void);

#endif //

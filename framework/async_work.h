/******************************************************************************
 * @brief        �첽��ҵ
 *
 * Copyright (c) 2020, <morro_luo@163.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs: 
 * Date           Author       Notes 
 * 2020-09-22     Morro        Initial version. 
 ******************************************************************************/
#ifndef _ASYNC_WORK_H_
#define _ASYNC_WORK_H_

#include "qlink.h"
#include <stdbool.h>

/*�첽��ҵ������ -------------------------------------------------------------*/
typedef struct {
    struct qlink idle;                             /*���ж���*/
    struct qlink ready;                            /*��������*/
}async_work_t;

typedef void (*async_work_func_t)(async_work_t *w, void *object, void *params);

/*�첽��ҵ��� ---------------------------------------------------------------*/
typedef struct {
    void *object;                                  /*��ҵ����*/
    void *params;                                  /*��ҵ����*/    
    async_work_func_t work;                        /*��ҵ���*/
    struct qlink_node node;                        /*����ڵ�*/
}work_node_t;

void async_work_init(async_work_t *w, work_node_t *nodes, int max_nodes);

bool async_work_add(async_work_t *w, void *object, void *params, 
                    async_work_func_t work);

void async_work_process(async_work_t *w);

#endif

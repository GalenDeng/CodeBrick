/*******************************************************************************
* @brief         ͨ�ö���
*
* Change Logs: 
* Date           Author       Notes 
* 2017-03-19     ħ�޲�ʿ     Initial version. 
*******************************************************************************/
#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *base;                                  /*������������ַ*/
    unsigned short element_size;                 /*����Ԫ�ش�С*/
    unsigned short size;                         /*��������������Ԫ�ظ���*/
    unsigned short front;                        /*����*/
    unsigned short rear;                         /*��β*/
}queue_t;

bool queue_create(queue_t *q,  void *container, int size, int element_size); 
bool queue_peek  (queue_t *q,  void **element);
bool queue_put   (queue_t *q,  void *element);
bool queue_get   (queue_t *q,  void *element);
bool queue_del   (queue_t *q);
void queue_clr   (queue_t *q);
int  queue_len   (queue_t *q);

#ifdef __cplusplus
}
#endif

#endif

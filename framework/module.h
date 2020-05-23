/*******************************************************************************
* @brief	ģ�����(����ϵͳ��ʼ��,ʱ��Ƭ��ѯϵͳ)
*
* Change Logs: 
* Date           Author       Notes 
* 2016-06-24     ħ�޲�ʿ     Initial version. 
*******************************************************************************/

#ifndef _MODULE_H_
#define _MODULE_H_

#include "comdef.h"

/*ģ���ʼ����*/
typedef struct {
    const char *name;               //ģ������
    void (*init)(void);             //��ʼ���ӿ�
}module_init_item_t;

/*��������*/
typedef struct {
    const char *name;               //ģ������    
    void (*handle)(void);           //��ʼ���ӿ�
    unsigned int interval;          //��ѯ���
    unsigned int *timer;            //ָ��ʱ��ָ��
}task_item_t;

#define __module_initialize(name,func,section_name)     \
RESERVE ANONY_TYPE(const module_init_item_t,func)       \
SECTION("section_sys_init") UNUSED = {name,func}

/*
 * @brief       ����ע��
 * @param[in]   name    - �������� 
 * @param[in]   handle  - ��ʼ������(void func(void){...})
  * @param[in]  interval- ��ѯ���(ms)
 */
#define task_register(name, handle,interval)           \
    RESERVE static unsigned int __task_timer_##handle; \
    RESERVE ANONY_TYPE(const task_item_t,handle)       \
    SECTION("section_task_item") UNUSED =              \
    {name,handle, interval, &__task_timer_##handle}

/*
 * @brief       ģ���ʼ��ע��
 * @param[in]   name    - ģ������ 
 * @param[in]   func    - ��ʼ����ں���(void func(void){...})
 */
#define system_init(name,func)  __module_initialize(name,func,"section_sys_init")
#define driver_init(name,func)  __module_initialize(name,func,"section_drv_init")
#define module_init(name,func)  __module_initialize(name,func,"section_app_init")

void systick_increase(void);
unsigned int get_tick(void);
void module_task_init(void);
void module_task_poll(void);

    
#endif

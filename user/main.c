/******************************************************************************
 * @brief    主程序入口
 *
 * Copyright (c) 2020, <master_roger@sina.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs: 
 * Date           Author       Notes 
 ******************************************************************************/
#include "config.h"
#include "module.h"
#include <stdio.h>

/* 
 * @brief       主程序入口 
 * @return      none
 */
int main(void)
{    
    //NVIC_SetVectorTable(NVIC_VectTab_FLASH, APP_ADDRESS);
    module_task_init();                         /*模块初始化*/
    printf("software version:%s\r\n", SW_VER);
    while (1) {    
        module_task_process();                   /*任务轮询*/
    }
}

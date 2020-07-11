/******************************************************************************
 * @brief    tty���ڴ�ӡ����
 *
 * Copyright (c) 2020, <master_roger@sina.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs: 
 * Date           Author       Notes 
 *
 ******************************************************************************/
#include "stm32f4xx.h"
#include "ringbuffer.h"
#include "tty.h"
#include "public.h"
#include <string.h>

#if (TTY_RXBUF_SIZE & (TTY_RXBUF_SIZE - 1)) != 0 
    #error "TTY_RXBUF_SIZE must be power of 2!"
#endif

#if (TTY_TXBUF_SIZE & (TTY_TXBUF_SIZE - 1)) != 0 
    #error "TTY_RXBUF_SIZE must be power of 2!"
#endif


static unsigned char rxbuf[TTY_RXBUF_SIZE];         /*���ջ����� */
static unsigned char txbuf[TTY_TXBUF_SIZE];         /*���ͻ����� */
static ring_buf_t rbuf_send, rbuf_recv;             /*�շ�����������*/

/*
 * @brief	    ���ڳ�ʼ��
 * @param[in]   baudrate - ������
 * @return 	    none
 */
static void uart_init(int baudrate)
{
    ring_buf_init(&rbuf_send, txbuf, sizeof(txbuf));/*��ʼ�����λ����� */
    ring_buf_init(&rbuf_recv, rxbuf, sizeof(rxbuf));   
}

/*
 * @brief	    �򴮿ڷ��ͻ�������д�����ݲ���������
 * @param[in]   buf       -  ���ݻ���
 * @param[in]   len       -  ���ݳ���
 * @return 	    ʵ��д�볤��(�����ʱ��������,�򷵻�len)
 */
static unsigned int uart_write(const void *buf, unsigned int len)
{   
    unsigned int ret;
    ret = ring_buf_put(&rbuf_send, (unsigned char *)buf, len);  
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
    return ret; 
}
/*
 * @brief	    ��ȡ���ڽ��ջ�����������
 * @param[in]   buf       -  ���ݻ���
 * @param[in]   len       -  ���ݳ���
 * @return 	    (ʵ�ʶ�ȡ����)������ջ���������Ч���ݴ���len�򷵻�len���򷵻ػ���
 *              ����Ч���ݵĳ���
 */
static unsigned int uart_read(void *buf, unsigned int len)
{
    return ring_buf_get(&rbuf_recv, (unsigned char *)buf, len);
}

/*����̨�ӿڶ��� -------------------------------------------------------------*/
const tty_t tty = 
{
    uart_init,
    uart_write,
    uart_read
};

/*
 * @brief	    ����1�շ��ж�
 * @param[in]   none
 * @return 	    none
 */
void USART1_IRQHandler(void)
{     
    unsigned char data;
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        data = USART_ReceiveData(USART1);
        ring_buf_put(&rbuf_recv, &data, 1);           /*�����ݷ�����ջ�����*/             
    }
    if (USART_GetITStatus(USART1, USART_IT_TXE) != RESET) {
        if (ring_buf_get(&rbuf_send, &data, 1))      /*�ӻ�������ȡ������---*/
            USART_SendData(USART1, data);            
        else{
            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);    
        }
    }
    if (USART_GetITStatus(USART1, USART_IT_ORE_RX) != RESET) {
        data = USART_ReceiveData(USART1);        
    }       
}

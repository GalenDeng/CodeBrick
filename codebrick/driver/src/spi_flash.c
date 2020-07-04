/******************************************************************************
 * @brief    SPI flash ����
 *
 * Copyright (c) 2017~2020, <master_roger@sina.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs: 
 * Date           Author       Notes 
 * 2017-08-01     Morro        ����
 ******************************************************************************/
#include "stm32f4xx.h"
#include "spi_flash.h"
#include "public.h"
#include <string.h>
#include <intrinsics.h>

/* flash ״̬��---------------------------------------------------------------*/
typedef union  {
	unsigned char status;
	struct {
		unsigned char WIP	: 1;
		unsigned char WEL	: 1;
		unsigned char BP0	: 1;
		unsigned char BP1	: 1;
		unsigned char BP2	: 1;
		unsigned char BP3	: 1;
		unsigned char WPDIS	: 1;		
		unsigned char SRP	: 1;
	};
}flash_status_t;

/*Ƭѡ����*/
void spi_flash_diselect(void)
{
    GPIO_SetBits(GPIOB, GPIO_Pin_10); 
    __no_operation();
}
void spi_flash_select(void)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_10); 
    __no_operation();
}
//spiд
static unsigned char spi_write_byte(unsigned char byte)
{      
    bool timeout = 10000;
    while((SPI_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) && (timeout--));
        SPI_SendData(SPI1, byte);
    timeout = 10000;
    while((SPI_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)&& (timeout--));      
        return SPI_ReceiveData(SPI1);
}

//spi��
static unsigned char spi_read_byte(void)
{
    return (spi_write_byte(0x55));
}

/*-----------------------------------------------------------------------------*/
/*�˿�����PB3->SPI1_CLK, PB4->SPI1_MISO,PB5->SPI1->MOSI,PB10->SPI1_CS*/
static void spi_port_init(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    gpio_conf(GPIOB, GPIO_Mode_AF, GPIO_PuPd_NOPULL, GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5);
    
    /*Ƭѡ�ź�����*/
    gpio_conf(GPIOB, GPIO_Mode_OUT, GPIO_PuPd_UP, GPIO_Pin_10);
 
}

/*
 * @brief	    SPI ��ʼ��(84/4 = 21Mhz)
 * @param[in]   none
 * @return 	    none
 */
static void spi_init(void)
{
    SPI_InitTypeDef  SPI_InitStructure;    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);  
    SPI_DeInit(SPI1);
    spi_flash_diselect();    
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		                //����SPI����ģʽ:����Ϊ��SPI
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		            //����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		                    //ѡ���˴���ʱ�ӵ���̬:ʱ�����ո�
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	                    //���ݲ����ڵڶ���ʱ����
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		                    //NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;	//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ4
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	                //ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
    SPI_InitStructure.SPI_CRCPolynomial = 7;	                        //CRCֵ����Ķ���ʽ
    SPI_Init(SPI1, &SPI_InitStructure);                                 //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
    SPI_Cmd(SPI1, ENABLE);                                              //ʹ��SPI����
    spi_flash_diselect();
}
/*
 * @brief	    SPI flash ��ʼ��
 * @param[in]   none
 * @return 	    none
 */
void spi_flash_init(void)
{
    spi_port_init();
    spi_init();
}
//��״̬
static flash_status_t read_flash_status(void)
{
    flash_status_t status;
    spi_flash_select() ;
    spi_write_byte(CMD_RDSR);
    status.status = spi_read_byte();		
    spi_flash_diselect() ;
    return status;
}
//�ȴ�flash����
static bool wait_complete(void)
{
    int retry;
    for (retry = 0; retry < 30000; retry++) {
        if (!read_flash_status().WIP)
          return true;        
    }	
    return false;
}
//���͵�ַ
static void send_addr(unsigned int addr)
{
    spi_write_byte(addr >> 16);
    spi_write_byte(addr >> 8);
    spi_write_byte(addr);
}
/*����дʹ��*/
static void send_write_enable(void)
{
	spi_flash_select() ;
	spi_write_byte(CMD_WREN);
	spi_flash_diselect();
}

/*
 * @brief       flash дʹ��
 * @param[in]   none
 * @return      none
 */
static bool write_enable(void)
{
	int retry;     
	for (retry = 0; retry < 10; retry++) {
        send_write_enable(); 
        if (read_flash_status().WEL)
            return true;        
    }  
	return false;
}

/*
 * @brief       ��ȡflash��Ӳ����Ϣ(manufacture,type,capacity etc.)
 * @param[in]   info - flash��Ϣ
 * @return      none
 */
void spi_flash_get_info(flash_info_t *info)
{         
	spi_flash_select();
	spi_write_byte(CMD_READ_ID);
    info->manufacture = spi_read_byte();
    info->type        = spi_read_byte();
    info->capacity    = spi_read_byte();
	spi_flash_diselect();
}

/*
 * @brief       ҳ���
 * @param[in]   addr  - �����ַ
 * @param[in]   buf   - ������ָ��
 * @param[in]   len   - ���������ݳ���(<256,���򽫸���ǰ���Ѿ�д�������)
 * @return      true - ִ�гɹ�, false - ִ��ʧ��
 */
static bool flash_PageProg(unsigned int addr, unsigned char *buf, unsigned int len)
{   
	if (len == 0 || buf == NULL)
        return false;
    if(addr >= MAX_FLASH_SIZE)
        return false;		
	spi_flash_select() ;
	spi_write_byte(CMD_PAGE_PROG);                        /*����д���� ----------*/
	send_addr(addr);	
	while (len--)
		spi_write_byte(*buf++);
	spi_flash_diselect();	
	return wait_complete();

}
/*
 * @brief       flash д(д���������뱣���Ѿ�����Ϊff)
 * @param[in]   addr  - �����ַ
 * @param[in]   buf   - ������ָ��
 * @param[in]   len   - ���������ݳ���(<256,���򽫸���ǰ���Ѿ�д�������)
 * @return      true - ִ�гɹ�, false - ִ��ʧ��
 */
bool spi_flash_write(unsigned int addr, const void *buf ,unsigned int len)
{
    unsigned char *p = (unsigned char *)buf;
	/*addr����ҳʣ��Ŀռ�(��λΪ�ֽ�)-----------------------------------------*/
	unsigned short page_left = MAX_FLASH_PAGE_SIZE - (addr % MAX_FLASH_PAGE_SIZE);
    if(addr >= MAX_FLASH_SIZE || !write_enable())
        return false;
	/*�ж�addr����ҳ�Ƿ�������ɴ�д�������*/
	if (page_left >= len)  
		page_left = len;
	while (1) {
        if (!flash_PageProg(addr, p, page_left))
          return false;
		if (len ==  page_left)
            break;    
		else {
			addr += page_left;		/*ƫ�Ƶ���һҳ*/
			p    += page_left;     //���ݵ�ַ��
			len  -= page_left;    			
			page_left = (len > MAX_FLASH_PAGE_SIZE) ? MAX_FLASH_PAGE_SIZE : len;
		}
	}
    return true;
}

/*
 * @brief       flash ��
 * @param[in]   addr  - �����ַ
 * @param[out]  buf   - ������ָ��
 * @param[in]   len   - ��ȡ�ֽ���
 * @return      true - ִ�гɹ�, false - ִ��ʧ��
 */
bool spi_flash_read(unsigned int addr, void *buf ,unsigned int len)
{
    unsigned char *p = (unsigned char *)buf;
    if(buf == NULL || len == 0)
        return false;
	spi_flash_select();
	spi_write_byte(CMD_READ);
	send_addr(addr);
	while (len--)
		*p++ = spi_read_byte();
	spi_flash_diselect();
	return true;
}

/*
 * @brief       ��������
 * @param[in]   addr  - �������ڵ�ַ
 * @return      true - ִ�гɹ�, false - ִ��ʧ��
 */
bool spi_flash_sector_erase(unsigned int addr)
{
	if(addr >= MAX_FLASH_SIZE) 
        return false;	

	spi_flash_select();
	spi_write_byte(CMD_SECTOR_ERASE);
	send_addr(addr);
	spi_flash_diselect();
	return wait_complete();
}

/*
 * @brief       ��������
 * @param[in]   addr  - �����ڵ�ַ
 * @return      true - ִ�гɹ�, false - ִ��ʧ��
 */
bool spi_flash_block_erase(unsigned int addr)
{
	if(addr >= MAX_FLASH_SIZE) 
      return false;	
    
	spi_flash_select() ;
	spi_write_byte(CMD_BLOCK_ERASE);
	send_addr(addr);
	spi_flash_diselect();    
    return wait_complete();
}

/*
 * @brief       оƬ����
 * @param[in]   addr  - �����ڵ�ַ
 * @return      true - ִ�гɹ�, false - ִ��ʧ��
 */
bool spi_flash_chip_erase(void)
{		
	spi_flash_select();    
	spi_write_byte(CMD_CHIP_ERASE);
	spi_flash_diselect();
	return wait_complete();
}

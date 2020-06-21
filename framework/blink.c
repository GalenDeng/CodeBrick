/*******************************************************************************
* @brief         ������˸����(dev, motor, buzzer)���豸(dev, motor, buzzer)����
*
* Change Logs: 
* Date           Author       Notes 
* 2019-04-01     Morro        Initial version. 
*******************************************************************************/
#include "public.h"
#include "blink.h"                       /*get_tick()*/
#include <stddef.h>
#include <string.h>

#ifdef __CC_ARM                			 /* ARM Compiler 	*/	
    extern blink_dev_t        __section_blink_dev$$Base;           
    extern blink_dev_t        __section_blink_dev$$Limit;		
	#define blink_tbl_start    &__section_blink_dev$$Base    
	#define blink_tbl_end      &__section_blink_dev$$Limit 
#elif defined (__ICCARM__)        		/* for IAR Compiler */
	#pragma section="__section_blink_dev"
	#define blink_tbl_start  __section_begin("__section_blink_dev")        
	#define blink_tbl_end    __section_end("__section_blink_dev")     
#endif

/*
 * @brief       
 * @param[in]   name    - �豸����
 * @param[in]   ontime - ����ʱ��(�����ֵΪ0�����ùر�)
 * @param[in]   offtime- �ر�ʱ��
 * @param[in]   repeats- �ظ�����(0��ʾ����ѭ��)
 * @return      none
 */
void blink_dev_ctrl(blink_dev_t *dev, int ontime, int offtime, int repeats)
{
    dev->ontime  = ontime;
    dev->offtime = offtime + ontime;
    dev->repeats = repeats;
    dev->tick    = get_tick();
    dev->count   = 0;
    if (ontime  == 0) {
        dev->ioctrl(false);
        dev->enable  = false;
    }        
}

/*
 * @brief       �豸����
 * @param[in]   none
 * @return      none
 */
void blink_dev_process(void)
{
    blink_dev_t *s, *e;
    s = (blink_dev_t *)blink_tbl_start;
    e = (blink_dev_t *)blink_tbl_end;

    for (; s < e; s++) {
        if (s->ontime == 0) {
            continue;
        } else if (get_tick() - s->tick < s->ontime) {
            if (!s->enable) {
                s->enable = true;
                s->ioctrl(true);
            }
        } else if(get_tick() - s->tick < s->offtime) {
            if (s->enable) {
                s->enable = false;
                s->ioctrl(false);
            }
        } else {
            s->tick = get_tick();
			if (s->repeats) {
				if (++s->count >= s->repeats) {
					s->ontime = 0;
					s->ioctrl(false);
					s->enable = false;
				}
			}
        }
    }
}

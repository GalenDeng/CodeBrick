/*******************************************************************************
* @brief         ������������
*
* Change Logs 
* Date           Author       Notes 
* 2017-08-10     Morro        ��ʼ��
*******************************************************************************/

#include "key.h"
#include "framework.h"
#include <stddef.h>

static key_t *keyhead = NULL;

/*******************************************************************************
 * @brief       ����һ������
 * @param[in]   key     - ����������
 * @param[in]   readkey - �����������Ժ���ָ��
 * @param[in]   event   - �����¼�������ָ��
 * @return      0 - ����ʧ��, ��0 - �����ɹ�
 ******************************************************************************/
int key_create(key_t *key, int (*readkey)(void), 
               void (*event)(int type, unsigned int duration))
{
    key_t *keytail = keyhead;
    if (key == NULL || readkey == NULL || event == NULL)
        return 0;    
    key->event    = event;
    key->readkey  = readkey;
    key->next     = NULL;
    if (keyhead == NULL)
    {
        keyhead = key;
        return 1;
    }
    while (keytail->next != NULL)
        keytail = keytail->next;
    keytail->next = key;
    return 1;
}

/*******************************************************************************
 * @brief       ����ɨ�账��
 * @param[in]   key     - none
 * @return      none
 ******************************************************************************/
void key_scan_process(void)
{
    key_t *k;
    for (k = keyhead; k != NULL; k = k->next)
    {        
        if (k->readkey())
        {
            if (k->tick)                               
            {
                if (timespan(k->tick) > LONG_PRESS_TIME)/*�����ж� -----------*/   
                    k->event(KEY_LONG_DOWN, timespan(k->tick));           
            }
            else 
            {
                k->tick = get_jiffies();                /*��¼�״ΰ���ʱ�� ---*/
            }
        }
        else if (k->tick)
        {        
            if (timespan(k->tick) > LONG_PRESS_TIME)     /*�����ͷ� ----------*/
            {
                k->event(KEY_LONG_UP, timespan(k->tick));
            }            
            /*�̰��ͷŲ��� ---------------------------------------------------*/
            if (timespan(k->tick) > KEY_DEBOUNCE_TIME && 
                timespan(k->tick) < LONG_PRESS_TIME)
            {
                k->event(KEY_PRESS, timespan(k->tick));
            }

            k->tick = 0;
        }        
    }

}


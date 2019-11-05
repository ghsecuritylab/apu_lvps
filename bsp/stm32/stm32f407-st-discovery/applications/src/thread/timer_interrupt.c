/*
 * @ : ***************************************************************************************
 * @Description: 
 * @version: 
 * @Author: liwei
 * @Date: 2019-10-28 08:55:59
 * @LastEditTime: 2019-10-31 14:40:16
 * @  : ***************************************************************************************
 */
/********************************************************************************************
头文件
********************************************************************************************/
#include"../../inc/thread/timer_interrupt.h"
/********************************************************************************************
宏定义
********************************************************************************************/
/********************************************************************************************
变量定义
********************************************************************************************/

/********************************************************************************************
函数申明
********************************************************************************************/
/**************************************************************************************************
** 函数名称：
** 函数描述：定时器1中断函数
** 输入参数：
** 返回值  ：无
***************************************************************************************************/
void timeout1(void *parameter)
{
    rt_kprintf("timer1 is timeout\n");
    
 
}

/**************************************************************************************************
** 函数名称：
** 函数描述：定时器2中断函数
** 输入参数：
** 返回值  ：无
***************************************************************************************************/
void timeout2(void *parameter)
{
    rt_kprintf("timer2 is timeout\n");
    
}

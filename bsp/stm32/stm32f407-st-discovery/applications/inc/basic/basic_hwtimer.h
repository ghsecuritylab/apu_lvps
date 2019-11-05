/*
 * @ : ***************************************************************************************
 * @Description: 
 * @version: 
 * @Author: liwei
 * @Date: 2019-10-30 09:00:07
 * @LastEditTime: 2019-10-31 14:28:38
 * @  : ***************************************************************************************
 */
#ifndef _HWTIMER_INTERRUPT_H_
#define _HWTIMER_INTERRUPT_H_


#define HWTIMER_DEV_NAME   "timer0"     /* 定时器名称 */

//..........................头文件.......................................
#include <rtthread.h>
#include <rtdevice.h>
#include <hwtimer.h>

#include "../../inc/thread/hwtimer_interrupt.h"
        
//..........................函数申明.......................................
extern  int hwtimer_init(int hw_sec, int hw_usec);

#endif


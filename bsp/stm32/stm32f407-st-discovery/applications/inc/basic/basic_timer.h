/*
 * @ : ***************************************************************************************
 * @Description: 
 * @version: 
 * @Author: liwei
 * @Date: 2019-10-31 14:17:27
 * @LastEditTime: 2019-10-31 14:36:36
 * @  : ***************************************************************************************
 */

#ifndef _TIMER_INTERRUPT_H_
#define _TIMER_INTERRUPT_H_

//..........................头文件.......................................
#include <rtthread.h>
#include "../../inc/thread/timer_interrupt.h"

//..........................函数申明.......................................

extern int timer1_init( uint32_t msec);
extern int timer2_init( uint32_t msec);

#endif

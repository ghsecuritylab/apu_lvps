#ifndef _BSP_ADC_H_
#define _BSP_ADC_H_

//..........................头文件.......................................
#include <rtthread.h>

#include "../../inc/basic/basic_adc.h"

 extern uint16_t bsp_adchannel_middle_volt(void);
 extern uint16_t bsp_adchannel_Lvps1_Vout(void);
 extern uint16_t bsp_adchannel_Lvps1_Iout(void);
 extern uint16_t bsp_adchannel_Lvps2_Vout(void);
 extern uint16_t bsp_adchannel_Lvps2_Iout(void);
 extern uint16_t bsp_adchannel_Sinv_Vout(void);
 extern uint16_t bsp_adchannel_Sinv_Iout(void);

#endif

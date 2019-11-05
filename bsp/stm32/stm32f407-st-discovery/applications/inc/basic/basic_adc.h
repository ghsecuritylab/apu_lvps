#ifndef _BASIC_ADC_H_
#define _BASIC_ADC_H_

//..........................头文件.......................................
#include <rtthread.h>
#include <rtdevice.h>

//..........................宏定义.......................................
#define ADC_IN0     0   
#define ADC_IN4     4   
#define ADC_IN5     5   
#define ADC_IN6     6   
#define ADC_IN7     7   
#define ADC_IN8     8   
#define ADC_IN9     9
#define ADC_IN14    14   
#define ADC_IN15    15   

#define ADC_DEV_NAME      "adc%d"
#define ADC_VREF          330      //实际参考电压为3.3V  100倍关系

extern uint32_t basic_adc_get_volt(uint8_t channel);

#endif

/*
 * @ : ***************************************************************************************
 * @Description: 
 * @version: 
 * @Author: liwei
 * @Date: 2019-10-25 15:08:57
 * @LastEditTime: 2019-10-31 14:09:53
 * @  : ***************************************************************************************
 */
/******************************************************************************************
** 头文件
*******************************************************************************************/
#include "../../inc/thread/thread_adc.h"

/******************************************************************************************
** 宏定义
*******************************************************************************************/
#define  ADC_THREAD_STACK_SZ        512         // 线程栈大小，单位是字节。
#define  ADC_THREAD_PRIO            19          // 优先级 范围是从 0 ～ 255，数值越小优先级越高，0 代表最高优先级
#define  ADC_THREAD_TIMESLICE       100         // 时间片大小：调用一次调度能够运行的最大时间长度

/********************************************************************************************
** 定义变量
********************************************************************************************/
static struct rt_thread adc_thread;  
static rt_uint8_t adc_thread_stack[ADC_THREAD_STACK_SZ];

struct	  Analog_data      st_analog_data;    
//.............................................函数申明...................................
void  adc_sample(void);
uint16_t adc_sample_middle_volt(void);
uint16_t adc_sample_Lvps1_Vout(void);
uint16_t adc_sample_Lvps1_Iout(void);
uint16_t adc_sample_Lvps2_Vout(void);
uint16_t adc_sample_Lvps2_Iout(void);
uint16_t adc_sample_Sinv_Vout(void);
uint16_t adc_sample_Sinv_Iout(void);

static uint16_t adc_middle_volt_filter(void);
static uint16_t adc_Lvps1_Vout_filter(void);
static uint16_t adc_Lvps1_Iout_filter(void);
static uint16_t adc_Lvps2_Vout_filter(void);
static uint16_t adc_Lvps2_Iout_filter(void);
static uint16_t adc_Sinv_Vout_filter(void);
static uint16_t adc_Sinv_Iout_filter(void);

/**************************************************************************************************
** 函数名称：
** 函数描述：接收canopen的显示线程入口
** 输入参数：数码管要显示的内容
** 返回值  ：无
***************************************************************************************************/
static void adc_thread_entry(void *param)
{

    while(1)
    {
         adc_sample( );

         rt_thread_mdelay(100);
    }

}
/********************************************************************************************
** 函数名称：     
** 函数描述：线程的初始化
** 输入参数：无
** 返回值  ：    无
********************************************************************************************/
int adc_thread_init(void)
{
    // .....................................静态线程初始化.......................................
    rt_thread_init( &adc_thread,                                   // 线程结构体
                    "adc",                                         // 线程名称
                    adc_thread_entry, RT_NULL,                     // 线程入口函数名称和参数
                    adc_thread_stack, ADC_THREAD_STACK_SZ,  // 栈的起始位置和大小 
                    ADC_THREAD_PRIO, ADC_THREAD_TIMESLICE); // 线程优先级，时间片大小
     //..................................... 启动线程 ，进入线程入口函数...........................
      
	rt_thread_startup(&adc_thread); 
    return 0;    
}
INIT_COMPONENT_EXPORT(adc_thread_init);	

/*****************************************************************************************
** 函数名称：ADC采样
** 函数描述：
** 输入参数：
** 返回值  ：无
******************************************************************************************/
void  adc_sample(void)
{
	st_analog_data.u16_middle_V = adc_sample_middle_volt( );
	
	st_analog_data.u16_Lvps1_Vout  = adc_sample_Lvps1_Vout( );
	st_analog_data.u16_Lvps1_Iout  = adc_sample_Lvps1_Iout( );
	
	st_analog_data.u16_Lvps2_Vout  = adc_sample_Lvps2_Vout( );
	st_analog_data.u16_Lvps2_Iout  = adc_sample_Lvps2_Iout( );
	
	st_analog_data.u16_Sinv_Vout = adc_sample_Sinv_Vout( );
	st_analog_data.u16_Sinv_Iout = adc_sample_Sinv_Iout( );	
}
/*****************************************************************************************
** 函数名称：ADC比例系数换算
** 函数描述：
** 输入参数：
** 返回值  ：无
******************************************************************************************/
uint16_t adc_sample_middle_volt(void)
{	
	uint16_t adc_value = 0; 

	adc_value = adc_middle_volt_filter( ) * 1000/ 2.265 ;      // 中间级电压	   1000V对应2.265  R231不焊，R108改为5.1k
	if(adc_value <= 5) 
	{	
		adc_value = 0;
	}
	return adc_value;	
}

uint16_t adc_sample_Lvps1_Vout(void )
{	
	uint16_t adc_value = 0;

	adc_value = adc_Lvps1_Vout_filter( ) * 66.67;              // Lvps1输出电压	   150v 对应 2.25V    R78用120R

	if(adc_value <= 5) 
	{	
		adc_value = 0;
	}

	return adc_value;	
}

uint16_t adc_sample_Lvps1_Iout(void )
{	
	uint16_t adc_value = 0;

	adc_value = adc_Lvps1_Iout_filter( ) * 96.78 ;            // Lvps1输出电流   150A对应2.8125 R79用两个75并联

	if(adc_value <= 5) 
	{	
		adc_value = 0;
	}

	return adc_value;	
}
uint16_t adc_sample_Lvps2_Vout(void )
{	
	uint16_t adc_value = 0;

	adc_value = adc_Lvps2_Vout_filter( ) * 11;                // Lvps2输出电压  30V对应2.727V  R81不焊接 ，R235 用1K  R112短连

	if(adc_value <= 5) 
	{	
		adc_value = 0;
	}

	return adc_value;	
}

uint16_t adc_sample_Lvps2_Iout(void )
{	
	uint16_t adc_value = 0;

	adc_value = adc_Lvps2_Iout_filter( ) * 96.78 ;            // Lvps2输出电流   200A对应2.15V  R82用两个43并联

	if(adc_value <= 5) 
	{	
		adc_value = 0;
	}

	return adc_value;	
}  
uint16_t adc_sample_Sinv_Vout(void )
{	
	uint16_t adc_value = 0;

	adc_value = adc_Sinv_Vout_filter( ) * 11;               // Sinv输出电压  25A对应2.5V  R116不焊接

	if(adc_value <= 5) 
	{	
		adc_value = 0;
	}

	return adc_value;	
}

uint16_t adc_sample_Sinv_Iout(void )
{	
	uint16_t adc_value = 0;

	adc_value = adc_Sinv_Iout_filter( ) * 102.04 ;          // Sinv输出电流   250A对应2.45V  R115用两个3.9k并联

	if(adc_value <= 5) 
	{	
		adc_value = 0;
	}

	return adc_value;	
}  

/*****************************************************************************************
** 函数名称：滤波
** 函数描述：
** 输入参数：无
** 返回值  ：
******************************************************************************************/
static uint16_t adc_middle_volt_filter(void)
{
	static uint16_t middle_Volt[20] = {0};
	static uint8_t  middle_Volt_first_sign = 1;
	int i = 0;
	uint16_t get_value = 0, sum_value = 0, adc_volt = 0;

	
	get_value = bsp_adchannel_middle_volt( );
	
    if(middle_Volt_first_sign)
    {
    	for(i = 0; i < 20; i++)
		{
		    middle_Volt[i] = get_value;	
		}
		middle_Volt_first_sign = 0;
    }
	else	
	{
		for(i = 0; i < 19; i++)
		{
		    middle_Volt[i] = middle_Volt[i+1];	
		}
		middle_Volt[19] = get_value;
    }

	sum_value = 0;
	for(i = 0; i < 20; i++)
	{
		sum_value = sum_value + middle_Volt[i];	
	}

	adc_volt = sum_value / 20; // 除以20次

    return adc_volt;
}
/*****************************************************************************************
** 函数名称：滤波
** 函数描述：
** 输入参数：无
** 返回值  ：
******************************************************************************************/
static uint16_t adc_Lvps1_Vout_filter(void)
{
	static uint16_t Lvps1_Vout[20] = {0};
	static uint8_t  Lvps1_Vout_first_sign = 1;
	int i = 0;
	uint16_t get_value = 0, sum_value = 0, adc_volt = 0;

	get_value = bsp_adchannel_Lvps1_Vout( );
	
    if(Lvps1_Vout_first_sign == 1)
    {
    	for(i = 0; i < 20; i++)
		{
		    Lvps1_Vout[i] = get_value;	
		}
		Lvps1_Vout_first_sign = 0;
    }
	else	
	{
		for(i = 0; i < 19; i++)
		{
		    Lvps1_Vout[i] = Lvps1_Vout[i+1];	
		}
		Lvps1_Vout[19] = get_value;
    }

	sum_value = 0;
	for(i = 0; i < 20; i++)
	{
		sum_value = sum_value + Lvps1_Vout[i];	
	}

	adc_volt = sum_value / 20; // 除以20次

    return adc_volt;
}
/*****************************************************************************************
** 函数名称：滤波
** 函数描述：
** 输入参数：无
** 返回值  ：
******************************************************************************************/
static uint16_t adc_Lvps1_Iout_filter(void)
{
	static uint16_t Lvps1_Iout[20] = {0};
	static uint8_t  Lvps1_Iout_first_sign = 1;
	int i = 0;
	uint16_t get_value = 0, sum_value = 0, adc_volt = 0;

	get_value = bsp_adchannel_Lvps1_Iout( );
	
    if(Lvps1_Iout_first_sign)
    {
    	for(i = 0; i < 20; i++)
		{
		    Lvps1_Iout[i] = get_value;	
		}
		Lvps1_Iout_first_sign = 0;
    }
	else	
	{
		for(i = 0; i < 19; i++)
		{
		    Lvps1_Iout[i] = Lvps1_Iout[i+1];	
		}
		Lvps1_Iout[19] = get_value;
    }

	sum_value = 0;
	for(i = 0; i < 20; i++)
	{
		sum_value = sum_value + Lvps1_Iout[i];	
	}

	adc_volt = sum_value / 20; // 除以20次

    return adc_volt;
}
/*****************************************************************************************
** 函数名称：滤波
** 函数描述：
** 输入参数：无
** 返回值  ：
******************************************************************************************/
static uint16_t adc_Lvps2_Vout_filter(void)
{
	static uint16_t Lvps2_Vout[20] = {0};
	static uint8_t  Lvps2_Vout_first_sign = 1;
	int i = 0;
	uint16_t get_value = 0, sum_value = 0, adc_volt = 0;

	get_value = bsp_adchannel_Lvps2_Vout( );
	
    if(Lvps2_Vout_first_sign)
    {
    	for(i = 0; i < 20; i++)
		{
		    Lvps2_Vout[i] = get_value;	
		}
		Lvps2_Vout_first_sign = 0;
    }
	else	
	{
		for(i = 0; i < 19; i++)
		{
		    Lvps2_Vout[i] = Lvps2_Vout[i+1];	
		}
		Lvps2_Vout[19] = get_value;
    }

	sum_value = 0;
	for(i = 0; i < 20; i++)
	{
		sum_value = sum_value + Lvps2_Vout[i];	
	}

	adc_volt = sum_value / 20; // 除以20次

    return adc_volt;
}
/*****************************************************************************************
** 函数名称：滤波
** 函数描述：
** 输入参数：无
** 返回值  ：
******************************************************************************************/
static uint16_t adc_Lvps2_Iout_filter(void)
{
	static uint16_t Lvps2_Iout[20] = {0};
	static uint8_t  Lvps2_Iout_first_sign = 1;
	int i = 0;
	uint16_t get_value = 0, sum_value = 0, adc_volt = 0;

	get_value = bsp_adchannel_Lvps2_Iout( );
	
    if(Lvps2_Iout_first_sign)
    {
    	for(i = 0; i < 20; i++)
		{
		    Lvps2_Iout[i] = get_value;	
		}
		Lvps2_Iout_first_sign = 0;
    }
	else	
	{
		for(i = 0; i < 19; i++)
		{
		    Lvps2_Iout[i] = Lvps2_Iout[i+1];	
		}
		Lvps2_Iout[19] = get_value;
    }

	sum_value = 0;
	for(i = 0; i < 20; i++)
	{
		sum_value = sum_value + Lvps2_Iout[i];	
	}

	adc_volt = sum_value / 20; // 除以20次

    return adc_volt;
}
/*****************************************************************************************
** 函数名称：滤波
** 函数描述：
** 输入参数：无
** 返回值  ：
******************************************************************************************/
static uint16_t adc_Sinv_Vout_filter(void)
{
	static uint16_t Sinv_Vout[20] = {0};
	static uint8_t  Sinv_Vout_first_sign = 1;
	int i = 0;
	uint16_t get_value = 0, sum_value = 0, adc_volt = 0;

	get_value = bsp_adchannel_Sinv_Vout( );
	
    if(Sinv_Vout_first_sign)
    {
    	for(i = 0; i < 20; i++)
		{
		    Sinv_Vout[i] = get_value;	
		}
		Sinv_Vout_first_sign = 0;
    }
	else	
	{
		for(i = 0; i < 19; i++)
		{
		    Sinv_Vout[i] = Sinv_Vout[i+1];	
		}
		Sinv_Vout[19] = get_value;
    }

	sum_value = 0;
	for(i = 0; i < 20; i++)
	{
		sum_value = sum_value + Sinv_Vout[i];	
	}

	adc_volt = sum_value / 20; // 除以20次

    return adc_volt;
}
/*****************************************************************************************
** 函数名称：滤波
** 函数描述：
** 输入参数：无
** 返回值  ：
******************************************************************************************/
static uint16_t adc_Sinv_Iout_filter(void)
{
	static uint16_t Sinv_Iout[20] = {0};
	static uint8_t  Sinv_Iout_first_sign = 1;
	int i = 0;
	uint16_t get_value = 0, sum_value = 0, adc_volt = 0;

	get_value = bsp_adchannel_Sinv_Iout( );
	
    if(Sinv_Iout_first_sign)
    {
    	for(i = 0; i < 20; i++)
		{
		    Sinv_Iout[i] = get_value;	
		}
		Sinv_Iout_first_sign = 0;
    }
	else	
	{
		for(i = 0; i < 19; i++)
		{
		    Sinv_Iout[i] = Sinv_Iout[i+1];	
		}
		Sinv_Iout[19] = get_value;
    }

	sum_value = 0;
	for(i = 0; i < 20; i++)
	{
		sum_value = sum_value + Sinv_Iout[i];	
	}

	adc_volt = sum_value / 20; // 除以20次

    return adc_volt;
}

/******************************************************************************************
    end file
*******************************************************************************************/

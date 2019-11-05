/********************************************************************************************
头文件
********************************************************************************************/
#include "../../inc/fmod/fmod_fault_detect.h"

/********************************************************************************************
宏定义
********************************************************************************************/
/********************************************************************************************
变量定义
********************************************************************************************/
union	System_status_regs  un_sys_status;
union	System_err_regs     un_sys_err;

union	Lvps1_status_regs   un_Lvps1_status;
union	Lvps1_err_regs      un_Lvps1_err;

union	Lvps2_status_regs   un_Lvps2_status;
union	Lvps2_err_regs      un_Lvps2_err;

union	Sinv_status_regs    un_Sinv_status;
union	Sinv_err_regs       un_Sinv_err;

union	Thinv_status_regs   un_Thinv_status;
union	Thinv_err_regs      un_Thinv_err;

/********************************************************************************************
函数申明
********************************************************************************************/
void system_fault_detect(void);
void Lvps1_fault_detect(void);
void Lvps2_fault_detect(void);
void Sinv_fault_detect(void);

/** * @ : ***************************************************************************************
 * @name: 
 * @describe: 故障检测
 * @param : 
 * @return: 
 * @  : ****************************************************************************************/
void fmod_fault_detect(void)
{

	system_fault_detect( );

	Lvps1_fault_detect( );

    Lvps2_fault_detect( );

	Sinv_fault_detect( );

}
/*********************************************************************************************************
** 函数名称：系统的故障检测
** 函数描述：故障检测
** 输入参数：
** 返回值  ：无
*********************************************************************************************************///
void system_fault_detect(void)
{
	//........................................熔断器故障....................................
	if(un_Thinv_err.st_bit.Thinv_err_inputfuse == 1)    //接收的逆变器的信号
	{  
		un_sys_err.st_bit.sys_err_inputfuse = 1;
	}
	else
	{    
		un_sys_err.st_bit.sys_err_inputfuse = 0;	
	}
	
	//........................................APU输入过压....................................
	if(st_analog_data.u16_middle_V > 1050)   //大于1050V过压
	{   
		un_sys_err.st_bit.sys_over_Vin = 1;
	}
	if(st_analog_data.u16_middle_V < 1000)   //小于1000V恢复
	{    
		un_sys_err.st_bit.sys_over_Vin = 0;
	}
	
	//........................................APU输入欠压....................................
	if(st_analog_data.u16_middle_V < 4800)   //小于480v欠压
	{   
		un_sys_err.st_bit.sys_under_Vin = 1;
	}
	if(st_analog_data.u16_middle_V > 4900)   //大于490v恢复
	{    
		un_sys_err.st_bit.sys_under_Vin = 0;
	}
	
	//...........................................APU散热器过温.......................................
	if(RADIATOR_TEMP_FEED_VALUE == 1)   
	{   
		un_sys_err.st_bit.sys_over_temp = 1;
	}
	else
	{    
		un_sys_err.st_bit.sys_over_temp = 0;
	}
	
	//..........................................辅助电源故障.......................................
    if(ASSIST_POWER_FEED_VALUE == 1)   
	{   
		un_sys_err.st_bit.sys_assist_power_err = 1;
	}
	else
	{    
		un_sys_err.st_bit.sys_assist_power_err = 0;
	}
		
}

/*********************************************************************************************************
** 函数名称：Lvps1的故障检测
** 函数描述：故障检测
** 输入参数：
** 返回值  ：无
*********************************************************************************************************///
void Lvps1_fault_detect(void)
{
	static uint8_t  Lvps1_run_time = 0;
	
	//........................................Lvps1输出过压....................................
	if(st_analog_data.u16_Lvps1_Vout > 1370)   //大于137V过压
	{   
		un_Lvps1_err.st_bit.Lvps1_over_Vout = 1;
	}
	if(st_analog_data.u16_Lvps1_Vout < 1300)   //大于137V过压
	{    
		un_Lvps1_err.st_bit.Lvps1_over_Vout = 0;
	}
	
	//........................................Lvps1输出欠压....................................
	if((un_Lvps1_status.st_bit.Lvps1_run == 1) && (st_analog_data.u16_Lvps1_Vout < 900) )   //小于90V欠压
	{   
		un_Lvps1_err.st_bit.Lvps1_under_Vout = 1;
	}
	if(st_analog_data.u16_Lvps1_Vout > 950) 
	{    
		un_Lvps1_err.st_bit.Lvps1_under_Vout = 0;
	}
	
	//........................................Lvps1输出过流....................................
	if( (st_analog_data.u16_Lvps1_Iout > 1000) )   //输出电流大于100A
	{   
		un_Lvps1_err.st_bit.Lvps1_over_Iout = 1;
	}
	else
	{    
		un_Lvps1_err.st_bit.Lvps1_over_Iout = 0;
	}
	
	//...........................................Lvps1过温.......................................
	if(Lvps1_TEMP_FEED_VALUE == 1)   
	{   
		un_Lvps1_err.st_bit.Lvps1_over_temp = 1;
	}
	else
	{    
		un_Lvps1_err.st_bit.Lvps1_over_temp = 0;
	}
	
	//..........................................Lvps1 IGBT故障.......................................
    if(Lvps1_IGBT_FEED_VALUE == 1)   
	{   
		un_Lvps1_err.st_bit.Lvps1_err_igbt = 1;
	}
	else
	{    
		un_Lvps1_err.st_bit.Lvps1_err_igbt = 0;
	}
	
	//..........................................Lvps1 极性反馈.......................................
	if(Lvps1_POLARITY_FEED_VALUE == 1)   
	{   
		un_Lvps1_err.st_bit.Lvps1_err_polarity = 1;
	}
	else
	{    
		un_Lvps1_err.st_bit.Lvps1_err_polarity = 0;
	}
	
	//..........................................Lvps1 CON3接触器反馈检测..............................
	if(un_Lvps1_status.st_bit.Lvps1_run == 1)
	{
		Lvps1_run_time++;
		if(Lvps1_run_time >= 5) Lvps1_run_time = 5;
	}
	else
	{
		Lvps1_run_time = 0;
	}
	
	if( (Lvps1_run_time >= 5) && (Lvps1_CON3_FEED_VALUE == 0) ) //正常工作后判断 接触器的反馈
	{   
		un_Lvps1_err.st_bit.Lvps1_err_CON3 = 1;
	}
	else
	{    
		un_Lvps1_err.st_bit.Lvps1_err_CON3 = 0;
	}
	
	//..........................................Lvps1 正漏电......................................
	if(Lvps1_LEAKP_FEED_VALUE == 1)   
	{   
		un_Lvps1_err.st_bit.Lvps1_leak_P = 1;
	}
	else
	{    
		un_Lvps1_err.st_bit.Lvps1_leak_P = 0;
	}
	
	//..........................................Lvps1 负漏电.......................................
	if(Lvps1_LEAKN_FEED_VALUE == 1)   
	{   
		un_Lvps1_err.st_bit.Lvps1_leak_N = 1;
	}
	else
	{    
		un_Lvps1_err.st_bit.Lvps1_leak_N = 0;
	}
	
	//.............................................自检故障.......................................
	if( (un_Lvps1_status.st_bit.Lvps1_run == 0) && (Lvps1_CON3_FEED_VALUE == 1) )
	{   
		un_Lvps1_err.st_bit.Lvps1_err_selftest = 1;
	}
	else
	{    
		un_Lvps1_err.st_bit.Lvps1_err_selftest = 0;
	}
			
}

/*********************************************************************************************************
** 函数名称：Lvps2的故障检测
** 函数描述：故障检测
** 输入参数：
** 返回值  ：无
*********************************************************************************************************///
void Lvps2_fault_detect(void)
{
	static uint8_t  Lvps2_run_time = 0;
	
	//........................................Lvps2输出过压....................................
	if(st_analog_data.u16_Lvps2_Vout > 300)   //大于30V过压
	{   
		un_Lvps2_err.st_bit.Lvps2_over_Vout = 1;
	}
	if(st_analog_data.u16_Lvps2_Vout < 28)   //大于28V过压
	{    
		un_Lvps2_err.st_bit.Lvps2_over_Vout = 0;
	}
	
	//........................................Lvps2输出欠压....................................
	if((un_Lvps2_status.st_bit.Lvps2_run == 1) && (st_analog_data.u16_Lvps2_Vout < 200) )   //小于90V欠压
	{   
		un_Lvps2_err.st_bit.Lvps2_under_Vout = 1;
	}
	if(st_analog_data.u16_Lvps2_Vout > 220) 
	{    
		un_Lvps2_err.st_bit.Lvps2_under_Vout = 0;
	}
	
	//........................................Lvps2输出过流....................................
	if( (st_analog_data.u16_Lvps2_Iout > 1800) )   //输出电流大于180A
	{   
		un_Lvps2_err.st_bit.Lvps2_over_Iout = 1;
	}
	else
	{    
		un_Lvps2_err.st_bit.Lvps2_over_Iout = 0;
	}
	
	//...........................................Lvps2过温.......................................
	if(Lvps2_TEMP_FEED_VALUE == 1)   
	{   
		un_Lvps2_err.st_bit.Lvps2_over_temp = 1;
	}
	else
	{    
		un_Lvps2_err.st_bit.Lvps2_over_temp = 0;
	}
	
	//..........................................Lvps2 IGBT故障.......................................
    if(Lvps2_IGBT_FEED_VALUE == 1)   
	{   
		un_Lvps2_err.st_bit.Lvps2_err_igbt = 1;
	}
	else
	{    
		un_Lvps2_err.st_bit.Lvps2_err_igbt = 0;
	}
	
	//..........................................Lvps2 极性反馈.......................................
	if(Lvps2_POLARITY_FEED_VALUE == 1)   
	{   
		un_Lvps2_err.st_bit.Lvps2_err_polarity = 1;
	}
	else
	{    
		un_Lvps2_err.st_bit.Lvps2_err_polarity = 0;
	}
	
	//..........................................Lvps2 CON3接触器反馈检测..............................
	if(un_Lvps2_status.st_bit.Lvps2_run == 1)
	{
		Lvps2_run_time++;
		if(Lvps2_run_time >= 5) Lvps2_run_time = 5;
	}
	else
	{
		Lvps2_run_time = 0;
	}
	
	if( (Lvps2_run_time >= 5) && (Lvps2_CON4_FEED_VALUE == 0) ) //正常工作后判断 接触器的反馈
	{   
		un_Lvps2_err.st_bit.Lvps2_err_CON4 = 1;
	}
	else
	{    
		un_Lvps2_err.st_bit.Lvps2_err_CON4 = 0;
	}
	
	//..........................................Lvps2 正漏电.......................................
	if(Lvps2_LEAKP_FEED_VALUE == 1)   
	{   
		un_Lvps2_err.st_bit.Lvps2_leak_P = 1;
	}
	else
	{    
		un_Lvps2_err.st_bit.Lvps2_leak_P = 0;
	}
	
	//..........................................Lvps2 负漏电.......................................
	if(Lvps2_LEAKN_FEED_VALUE == 1)   
	{   
		un_Lvps2_err.st_bit.Lvps2_leak_N = 1;
	}
	else
	{    
		un_Lvps2_err.st_bit.Lvps2_leak_N = 0;
	}
	
	//.............................................自检故障.......................................
	if( (un_Lvps2_status.st_bit.Lvps2_run == 0) && (Lvps2_CON4_FEED_VALUE == 1) )
	{   
		un_Lvps2_err.st_bit.Lvps2_err_selftest = 1;
	}
	else
	{    
		un_Lvps2_err.st_bit.Lvps2_err_selftest = 0;
	}
			
}


/*********************************************************************************************************
** 函数名称：Sinv的故障检测
** 函数描述：故障检测
** 输入参数：
** 返回值  ：无
*********************************************************************************************************///
void Sinv_fault_detect(void)
{
	static uint8_t  Sinv_run_time = 0;
	
	
	if(un_Sinv_status.st_bit.Sinv_run == 1)
	{
		Sinv_run_time++;
		if(Sinv_run_time >= 5) Sinv_run_time = 5;
	}
	else
	{
		Sinv_run_time = 0;
	}
	
	//........................................Sinv输出过压....................................
	if(st_analog_data.u16_Sinv_Vout > 2500)   //大于250V过压
	{   
		un_Sinv_err.st_bit.Sinv_over_Vout = 1;
	}
	if(st_analog_data.u16_Sinv_Vout < 2350)   //大于137V过压
	{    
		un_Sinv_err.st_bit.Sinv_over_Vout = 0;
	}
	
	//........................................Sinv输出欠压....................................
	if((un_Sinv_status.st_bit.Sinv_run == 1) && (st_analog_data.u16_Sinv_Vout < 1900) )   //小于190V欠压
	{   
		un_Sinv_err.st_bit.Sinv_under_Vout = 1;
	}
	if(st_analog_data.u16_Sinv_Vout > 2050) 
	{    
		un_Sinv_err.st_bit.Sinv_under_Vout = 0;
	}
	
	//........................................Sinv输出过流....................................
	if( (st_analog_data.u16_Sinv_Iout > 160) )   //输出电流大于16A
	{   
		un_Sinv_err.st_bit.Sinv_over_Iout = 1;
	}
	else
	{    
		un_Sinv_err.st_bit.Sinv_over_Iout = 0;
	}
	
	//...........................................Sinv过温.......................................
	if(Sinv_TEMP_FEED_VALUE == 1)   
	{   
		un_Sinv_err.st_bit.Sinv_over_temp = 1;
	}
	else
	{    
		un_Sinv_err.st_bit.Sinv_over_temp = 0;
	}
	
	//..........................................Sinv CON5接触器反馈检测..............................
//	if( (Sinv_run_time >= 5) && (Sinv_CON5_FEED_VALUE == 0) ) //正常工作后判断 接触器的反馈
//	{   
//		un_Sinv_err.st_bit.Sinv_err_CON5 = 1;
//	}
//	else
//	{    
//		un_Sinv_err.st_bit.Sinv_err_CON5 = 0;
//	}
	
	//.............................................自检故障.......................................
//	if( (un_Sinv_status.st_bit.Sinv_run == 0) && (Sinv_CON5_FEED_VALUE == 1) )
//	{   
//		un_Sinv_err.st_bit.Sinv_err_selftest = 1;
//	}
//	else
//	{    
//		un_Sinv_err.st_bit.Sinv_err_selftest = 0;
//	}
	
	//..........................................Sinv IGBT故障.......................................
    if(Sinv_IGBT_FEED_VALUE == 1)   
	{   
		un_Sinv_err.st_bit.Sinv_err_igbt = 1;
	}
	else
	{    
		un_Sinv_err.st_bit.Sinv_err_igbt = 0;
	}
	
	//..........................................Sinv 逆变故障.......................................
	if(Sinv_NB_FEED_VALUE == 1)   
	{   
		un_Sinv_err.st_bit.Sinv_err_NB = 1;
	}
	else
	{    
		un_Sinv_err.st_bit.Sinv_err_NB = 0;
	}		
}


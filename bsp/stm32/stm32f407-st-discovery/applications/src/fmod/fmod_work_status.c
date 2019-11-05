/********************************************************************************************
头文件
********************************************************************************************/
#include "../../inc/fmod/fmod_work_status.h"

/********************************************************************************************
宏定义
********************************************************************************************/
/********************************************************************************************
变量定义
********************************************************************************************/
/********************************************************************************************
函数申明
********************************************************************************************/
void system_status_judge(void);
void Lvps1_status_judge(void);
void Lvps1_operate_control(void);
void Lvps2_status_judge(void);
void Lvps2_operate_control(void);
void Sinv_status_judge(void);
void Sinv_operate_control(void);
void Thinv_status_judge(void);
void Thinv_operate_control(void);

/** * @ : ***************************************************************************************
 * @name: 
 * @describe: 工作状态判定
 * @param : 
 * @return: 
 * @  : ****************************************************************************************/
void fmod_work_status(void)
{
	
	system_status_judge( );

    //...............................Lvps1的工作判定和控制.............................
	Lvps1_status_judge( );
	Lvps1_operate_control( );

    //...............................Lvps2的工作判定和控制.............................
	Lvps2_status_judge( );
	Lvps2_operate_control( );

    //...............................Sinv的工作判定和控制.............................
	Sinv_status_judge( );
	Sinv_operate_control( );

   //...............................THinv的工作判定和控制.............................
	Sinv_status_judge( );
	Sinv_operate_control( );
   
   //...................................DAC输出.................................
   



}


/*********************************************************************************************************
** 函数名称：系统的工作状态判定
** 函数描述：
** 输入参数：
** 返回值  ：无
*********************************************************************************************************///
void system_status_judge(void)
{
	
	//.............................................预充电完成标志...............................................
	if( (PRE_CH_FEED_VALUE == 1) || un_sys_status.st_bit.sys_preCH_ok == 1)  //硬件反馈或者通讯
	{
		un_sys_status.st_bit.sys_preCH_ok = 1;
	}
	else
	{
		un_sys_status.st_bit.sys_preCH_ok = 0;
	}

	//........................................系统整体状态判断(正常时可启动电源)...................................
	if( ((un_sys_err.u16_all & 0x000F) == 0) && (un_sys_status.st_bit.sys_preCH_ok == 1))  //无系统故障和预充电完成
	{	
		un_sys_status.st_bit.sys_status_ok =1;
	}
	else
	{
		un_sys_status.st_bit.sys_status_ok = 0;
	}

    if(EX_Sinv_RUN_FEED_VALUE == 1)
    {	
		un_sys_status.st_bit.sys_ex_Sinv_run = 1;
	}
	else
	{
		un_sys_status.st_bit.sys_ex_Sinv_run = 0;
	}

}

/*********************************************************************************************************
** 函数名称：Lvps1的工作状态判定
** 函数描述：
** 输入参数：
** 返回值  ：无
*********************************************************************************************************///
void Lvps1_status_judge(void)
{	
	//.............................................判断Lvps1是否关机和重启...............................................
    if( (un_sys_status.st_bit.sys_status_ok == 0)  || ((un_Lvps1_err.u16_all & 0x003f) != 0x0) )   //系统状态和单机故障
    {
		un_Lvps1_status.st_bit.Lvps1_shutdown = 1;
		un_Lvps1_status.st_bit.Lvps1_reset = 1;
	}
	else
	{
		un_Lvps1_status.st_bit.Lvps1_shutdown = 0;
		un_Lvps1_status.st_bit.Lvps1_reset = 0;
	}
	
	//.............................................判断Lvps1是否3次锁死...............................................
    if(st_power_lock.Lvps1_lock_count >= 3 ) 
    {
		un_Lvps1_status.st_bit.Lvps1_lock = 1;
	}
	else
	{
		un_Lvps1_status.st_bit.Lvps1_lock = 0;
	}
	
	//.............................................判断Lvps1是否工作...............................................
    if((un_Lvps1_status.u16_all & 0x0007) == 0 )    //无关机重启和锁死
    {
		un_Lvps1_status.st_bit.Lvps1_run = 1;
	}
	else
	{
		un_Lvps1_status.st_bit.Lvps1_run = 0;
	}
	
	 //.............................................判断Lvps1是否有故障...............................................
    if(un_Lvps1_err.u16_all != 0x0)
    {
		un_Lvps1_status.st_bit.Lvps1_fault = 1;
	}
	else
	{
		un_Lvps1_status.st_bit.Lvps1_fault = 0;
	}
	
	//.............................................判断Lvps1输出接触器的输出状态...............................................
	if(un_Lvps1_err.st_bit.Lvps1_err_polarity == 0) 
	{
		un_Lvps1_status.st_bit.Lvps1_CON3_closed = 1;
	}
	else
	{
		un_Lvps1_status.st_bit.Lvps1_CON3_closed = 0;
	}	
}

/*********************************************************************************************************
** 函数名称：Lvps1的运行动作控制
** 函数描述：
** 输入参数：
** 返回值  ：无
*********************************************************************************************************///
void Lvps1_operate_control(void)
{	
    //.............................................控制Lvps1的CON3的吸合...............................................
    if(un_Lvps1_status.st_bit.Lvps1_CON3_closed == 1)
	{ 
		Lvps1_CON3_A2_ON;    //吸合CON3
	}
	else
	{
		Lvps1_CON3_A2_OFF;   //弹开CON3
	}
	//.............................................控制Lvps1的启动信号...............................................
    if(un_Lvps1_status.st_bit.Lvps1_run == 1)
    {
		Lvps1_START_ON;
	}
	else
	{
		Lvps1_START_OFF;
	}	
	//.............................................控制Lvps1的过载信号...............................................	
	if(un_Lvps1_err.st_bit.Lvps1_over_Iout == 1)
	{
		Lvps1_OVERLOAD_ON;	 //过载信号开启
	}
	else
	{
		Lvps1_OVERLOAD_OFF;
	}
}

/*********************************************************************************************************
** 函数名称：Lvps2的工作状态判定
** 函数描述：
** 输入参数：
** 返回值  ：无
*********************************************************************************************************///
void Lvps2_status_judge(void)
{	
	//.............................................判断Lvps2是否关机和重启...............................................
    if( (un_sys_status.st_bit.sys_status_ok == 0)  || ((un_Lvps2_err.u16_all & 0x003f) != 0x0) )   //系统状态和单机故障
    {
		un_Lvps2_status.st_bit.Lvps2_shutdown = 1;
		un_Lvps2_status.st_bit.Lvps2_reset = 1;
	}
	else
	{
		un_Lvps2_status.st_bit.Lvps2_shutdown = 0;
		un_Lvps2_status.st_bit.Lvps2_reset = 0;
	}
	
	//.............................................判断Lvps2是否3次锁死...............................................
    if(st_power_lock.Lvps2_lock_count >= 3 ) 
    {
		un_Lvps2_status.st_bit.Lvps2_lock = 1;
	}
	else
	{
		un_Lvps2_status.st_bit.Lvps2_lock = 0;
	}
	
	//.............................................判断Lvps2是否工作...............................................
    if((un_Lvps2_status.u16_all & 0x0007) == 0 )    //无关机重启和锁死
    {
		un_Lvps2_status.st_bit.Lvps2_run = 1;
	}
	else
	{
		un_Lvps2_status.st_bit.Lvps2_run = 0;
	}
	
	 //.............................................判断Lvps2是否有故障...............................................
    if(un_Lvps2_err.u16_all != 0x0)
    {
		un_Lvps2_status.st_bit.Lvps2_fault = 1;
	}
	else
	{
		un_Lvps2_status.st_bit.Lvps2_fault = 0;
	}
	
	//.............................................判断Lvps2输出接触器的输出状态...............................................
	if(un_Lvps2_err.st_bit.Lvps2_err_polarity == 0) 
	{
		un_Lvps2_status.st_bit.Lvps2_CON4_closed = 1;
	}
	else
	{
		un_Lvps2_status.st_bit.Lvps2_CON4_closed = 0;
	}	
}

/*********************************************************************************************************
** 函数名称：Lvps2的运行动作控制
** 函数描述：
** 输入参数：
** 返回值  ：无
*********************************************************************************************************///
void Lvps2_operate_control(void)
{	
    //.............................................控制Lvps2的CON3的吸合...............................................
    if(un_Lvps2_status.st_bit.Lvps2_CON4_closed == 1)
	{ 
		Lvps2_CON4_A2_ON;    //吸合CON3
	}
	else
	{
		Lvps2_CON4_A2_OFF;   //弹开CON3
	}
	//.............................................控制Lvps2的启动信号...............................................
    if(un_Lvps2_status.st_bit.Lvps2_run == 1)
    {
		Lvps2_START_ON;
	}
	else
	{
		Lvps2_START_OFF;
	}	
	//.............................................控制Lvps2的过载信号...............................................	
	if(un_Lvps2_err.st_bit.Lvps2_over_Iout == 1)
	{
		Lvps2_OVERLOAD_ON;	 //过载信号开启
	}
	else
	{
		Lvps2_OVERLOAD_OFF;
	}
}


/*********************************************************************************************************
** 函数名称：Sinv的工作状态判定
** 函数描述：
** 输入参数：
** 返回值  ：无
*********************************************************************************************************///
void Sinv_status_judge(void)
{	
	//.............................................判断Sinv是否关机和重启...............................................
    if( (un_sys_status.st_bit.sys_status_ok == 0) || ((un_Sinv_err.u16_all & 0x003f) != 0x0) ||       //系统状态或Sinv单机故障,
		( (un_sys_status.st_bit.sys_ex_Sinv_run == 0) && (un_sys_status.st_bit.sys_ex_Sinv_run == 0) ) )  //外部启动信号和 除霜信号同时维0       
    {
		un_Sinv_status.st_bit.Sinv_shutdown = 1;
		un_Sinv_status.st_bit.Sinv_reset = 1;
	}
	else
	{
		un_Sinv_status.st_bit.Sinv_shutdown = 0;
		un_Sinv_status.st_bit.Sinv_reset = 0;
	}
	
	//.............................................判断Sinv是否3次锁死...............................................
    if(st_power_lock.Sinv_lock_count >= 3 ) 
    {
		un_Sinv_status.st_bit.Sinv_lock = 1;
	}
	else
	{
		un_Sinv_status.st_bit.Sinv_lock = 0;
	}
	
	//.............................................判断Sinv是否工作...............................................
    if((un_Sinv_status.u16_all & 0x0007) == 0 )    //无关机重启和锁死
    {
		un_Sinv_status.st_bit.Sinv_run = 1;
	}
	else
	{
		un_Sinv_status.st_bit.Sinv_run = 0;
	}
	
	 //.............................................判断Sinv是否有故障...............................................
    if(un_Sinv_err.u16_all != 0x0)
    {
		un_Sinv_status.st_bit.Sinv_fault = 1;
	}
	else
	{
		un_Sinv_status.st_bit.Sinv_fault = 0;
	}
	
	//.............................................判断Sinv输出接触器的输出状态...............................................
	un_Sinv_status.st_bit.Sinv_CON5_closed = 1;
}

/*********************************************************************************************************
** 函数名称：Sinv的运行动作控制
** 函数描述：
** 输入参数：
** 返回值  ：无
*********************************************************************************************************///
void Sinv_operate_control(void)
{	
    //.............................................控制Sinv的CON5的吸合...............................................	
    if(un_Sinv_status.st_bit.Sinv_CON5_closed == 1)
	{ 
		Sinv_CON5_A2_ON;    //吸合CON5
	}
	else
	{
		Sinv_CON5_A2_OFF;   //弹开CON3
	}
	
	//.............................................控制Sinv的启动信号...............................................
    if(un_Sinv_status.st_bit.Sinv_run == 1)
    {
		Sinv_START_ON;
	}
	else
	{
		Sinv_START_OFF;
	}	
	//.............................................控制Sinv的过载信号...............................................	
	if(un_Sinv_err.st_bit.Sinv_over_Iout == 1)
	{
		Sinv_OVERLOAD_ON;	 //过载信号开启
	}
	else
	{
		Sinv_OVERLOAD_OFF;
	}
}

/*********************************************************************************************************
** 函数名称：Thinv的工作状态判定
** 函数描述：
** 输入参数：
** 返回值  ：无
*********************************************************************************************************///
void Thinv_status_judge(void)
{	
	//.............................................判断Thinv的启动信号...............................................
    if( un_sys_status.st_bit.sys_status_ok == 0)    //系统状态和车辆通讯决定逆变器的启动
    {
		un_Thinv_status.st_bit.Thinv_run = 1;
		Thinv_START_ON;
	}
	else
	{
		un_Thinv_status.st_bit.Thinv_run = 0;
		Thinv_START_OFF;
	}
}

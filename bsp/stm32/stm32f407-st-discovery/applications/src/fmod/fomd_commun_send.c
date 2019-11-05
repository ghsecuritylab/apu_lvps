/********************************************************************************************
头文件
********************************************************************************************/
#include "../../inc/fmod/fmod_commun_send.h"
union   Se_Thinv_packdata   un_se_thinv_data;
union   Re_Thinv_packdata1  un_re_thinv_data1;
union   Re_Thinv_packdata2  un_re_thinv_data2;

union  Uart_re_Canopen_packdata  un_reCanopen_data;
union  Uart_se_Canopen_packdata  un_seCanopen_data;

uint8_t BCC_XOR_check(uint8_t *data,  uint8_t len);

/*********************************************************************************************************
** 函数名称：发送给逆变器板的数据更新
** 函数描述：
** 输入参数：
** 返回值  ：
*********************************************************************************************************///
static void fmod_send_THinv_updata(void)
{
	
	un_se_thinv_data.st_pack.u8_life++;
	if(un_se_thinv_data.st_pack.u8_life >= 0xFF)
	{
		un_se_thinv_data.st_pack.u8_life = 0;
	}
	
	un_se_thinv_data.st_pack.u8_time[0] = un_reCanopen_time.st_pack.st_tm.u8_year;
	un_se_thinv_data.st_pack.u8_time[1] = un_reCanopen_time.st_pack.st_tm.u8_month;
	un_se_thinv_data.st_pack.u8_time[2] = un_reCanopen_time.st_pack.st_tm.u8_day;
	un_se_thinv_data.st_pack.u8_time[3] = un_reCanopen_time.st_pack.st_tm.u8_hour;
	un_se_thinv_data.st_pack.u8_time[4] = un_reCanopen_time.st_pack.st_tm.u8_min;
	un_se_thinv_data.st_pack.u8_time[5] = un_reCanopen_time.st_pack.st_tm.u8_sec;
	
	un_se_thinv_data.st_pack.un_command.st_bit.Thinv_selftest = un_reCanopen_data.st_pack.st_RPDO.st_RPDO4.st_bit1.APU_sleftest;
	un_se_thinv_data.st_pack.un_command.st_bit.Thinv_lock_reset = un_reCanopen_data.st_pack.st_RPDO.st_RPDO4.st_bit1.Thinv_lock_reset; 
	un_se_thinv_data.st_pack.un_command.st_bit.APm1_APx1xPTR = un_reCanopen_data.st_pack.st_RPDO.st_RPDO7.st_bit.APm1_APx1xPTR;
	un_se_thinv_data.st_pack.un_command.st_bit.APm1_APx1xSupplyMode = un_reCanopen_data.st_pack.st_RPDO.st_RPDO7.st_bit.APm1_APx1xSupplyMode;
	un_se_thinv_data.st_pack.un_command.st_bit.APm1_APx1xReceiveMode = un_reCanopen_data.st_pack.st_RPDO.st_RPDO7.st_bit.APm1_APx1xReceiveMode;
	
}

/*********************************************************************************************************
** 函数名称：接收逆变器板的数据更新
** 函数描述：
** 输入参数：
** 返回值  ：
*********************************************************************************************************///
static void fmod_receive_THinv_updata(void)
{
	
	st_analog_data.u16_APU_Vin = un_re_thinv_data1.st_pack.u16_APU_Vin;
	st_analog_data.u16_APU_Iin = un_re_thinv_data1.st_pack.u8_APU_Iin;          // 逆变器的是U8
	
	st_analog_data.u16_Thinv_Vout = un_re_thinv_data1.st_pack.u16_Thinv_Vout;
	st_analog_data.u16_Thinv_Iout = un_re_thinv_data1.st_pack.u8_Thinv_Iout;	// 逆变器的是U8
	
	un_Thinv_status.u16_all = un_re_thinv_data2.st_pack.un_status.u16_all;
	un_Thinv_err.u16_all = un_re_thinv_data2.st_pack.un_err.u16_all;
	
}

/*********************************************************************************************************
** 函数名称：发送给canopen板的数据更新
** 函数描述：
** 输入参数：
** 返回值  ：
*********************************************************************************************************///
static void fmod_send_canopen_updata(void)
{
	un_seCanopen_data.st_pack.u8_headdata[0] = 0X7E;	
	un_seCanopen_data.st_pack.u8_headdata[1] = 0X10;	
	un_seCanopen_data.st_pack.u8_headdata[2] = 0X61;	
	un_seCanopen_data.st_pack.u8_headdata[3] = 0X11;	
	un_seCanopen_data.st_pack.u8_headdata[4] = 0X00;	
	
	
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO1.st_bit.inputV_valid       = un_Thinv_status.st_bit.Thinv_inputV_valid;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO1.st_bit.Thinv_run          = un_Thinv_status.st_bit.Thinv_run ;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO1.st_bit.Sinv_run           = un_Sinv_status.st_bit.Sinv_run ;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO1.st_bit.Lvps1_run          = un_Lvps1_status.st_bit.Lvps1_run;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO1.st_bit.Lvps2_run          = un_Lvps2_status.st_bit.Lvps2_run;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO1.st_bit.Thinv_CON1_closed  = un_Thinv_status.st_bit.Thinv_CON1_closed;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO1.st_bit.Thinv_CON2_closed  = un_Thinv_status.st_bit.Thinv_CON2_closed;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO1.st_bit.Lvps1_CON3_closed  = un_Lvps1_status.st_bit.Lvps1_CON3_closed;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO1.st_bit.Lvps2_CON4_closed  = un_Lvps2_status.st_bit.Lvps2_CON4_closed;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO1.st_bit.Thinv_standard_mode = un_Thinv_status.st_bit.Thinv_standard_mode;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO1.st_bit.Thinv_supply_mode   = un_Thinv_status.st_bit.Thinv_supply_mode;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO1.st_bit.Thinv_receive_mode  = un_Thinv_status.st_bit.Thinv_receive_mode;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO1.st_bit.Thinv_power_request = un_Thinv_status.st_bit.Thinv_power_request;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO1.st_bit.sys_selftest      =   un_sys_status.st_bit.sys_selftest ;
	
	
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO2.sysHV           =  (st_analog_data.u16_APU_Vin / 10) / 6;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO2.sysInv_OutV     =  st_analog_data.u16_Thinv_Vout ;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO2.sysInv_OutC     =  st_analog_data.u16_Thinv_Iout ;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO2.sysSnglPhs_OutV =  (st_analog_data.u16_Sinv_Vout / 10) /2 ;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO2.sysSnglPhs_OutC =  (st_analog_data.u16_Sinv_Iout / 10) ;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO2.sysLvps1_OutV   =  (st_analog_data.u16_Lvps1_Vout / 10) ;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO2.sysLvps1_OutC   =  (st_analog_data.u16_Lvps1_Iout / 10) ;
	
	
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit1.sys_err_inputfuse = un_sys_err.st_bit.sys_err_inputfuse;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit1.sys_over_Vin = un_sys_err.st_bit.sys_over_Vin;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit1.Thinv_lock = un_Thinv_status.st_bit.Thinv_lock;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit1.Thinv_leak = un_Thinv_err.st_bit.Thinv_leak;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit1.Thinv_over_Vout = un_Thinv_err.st_bit.Thinv_over_Vout;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit1.Thinv_under_Vout = un_Thinv_err.st_bit.Thinv_under_Vout;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit1.Thinv_over_Iout = un_Thinv_err.st_bit.Thinv_over_Iout;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit1.Thinv_over_temp = un_Thinv_err.st_bit.Thinv_over_temp;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit1.Thinv_err_igbt = un_Thinv_err.st_bit.Thinv_err_igbt;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit1.Thinv_err_selftest = un_Thinv_err.st_bit.Thinv_err_selftest;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit1.Sinv_shutdown = un_Sinv_status.st_bit.Sinv_shutdown;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit1.Sinv_over_Vout = un_Sinv_err.st_bit.Sinv_over_Vout;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit1.Sinv_under_Vout = un_Sinv_err.st_bit.Sinv_under_Vout;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit1.Sinv_over_Iout = un_Sinv_err.st_bit.Sinv_over_Iout;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit1.Sinv_over_temp = un_Sinv_err.st_bit.Sinv_over_temp;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit1.Lvps1_err_selftest = un_Lvps1_err.st_bit.Lvps1_err_selftest;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit1.Lvps1_lock = un_Lvps1_status.st_bit.Lvps1_lock;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit1.Lvps1_leak_P = un_Lvps1_err.st_bit.Lvps1_leak_P;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit1.Lvps1_leak_N = un_Lvps1_err.st_bit.Lvps1_leak_N ;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit1.Lvps1_over_Vout = un_Lvps1_err.st_bit.Lvps1_over_Vout;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit1.Lvps1_under_Vout = un_Lvps1_err.st_bit.Lvps1_under_Vout;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit1.Lvps1_over_Iout = un_Lvps1_err.st_bit.Lvps1_over_Iout;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit1.Lvps1_over_temp = un_Lvps1_err.st_bit.Lvps1_over_temp;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit1.Lvps1_err_igbt = un_Lvps1_err.st_bit.Lvps1_err_igbt;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit1.Lvps1_err_polarity = un_Lvps1_err.st_bit.Lvps1_err_polarity;

	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit2.Lvps2_lock = un_Lvps2_status.st_bit.Lvps2_lock;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit2.Lvps2_leak_P = un_Lvps2_err.st_bit.Lvps2_leak_P;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit2.Lvps2_leak_N = un_Lvps2_err.st_bit.Lvps2_leak_N;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit2.Lvps2_over_Vout = un_Lvps2_err.st_bit.Lvps2_over_Vout;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit2.Lvps2_under_Vout = un_Lvps2_err.st_bit.Lvps2_under_Vout;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit2.Lvps2_over_Iout= un_Lvps2_err.st_bit.Lvps2_over_Iout;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit2.Lvps2_over_temp = un_Lvps2_err.st_bit.Lvps2_over_temp;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit2.Lvps2_err_polarity = un_Lvps2_err.st_bit.Lvps2_err_polarity;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit2.sys_incommun_err = un_sys_err.st_bit.sys_incommun_err;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit2.Thinv_err_CON1 = un_Thinv_err.st_bit.Thinv_err_CON1;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit2.Thinv_err_CON2 = un_Thinv_err.st_bit.Thinv_err_CON2;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit2.Sinv_err_CON5 = un_Sinv_err.st_bit.Sinv_err_CON5;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit2.Lvps1_err_CON3 = un_Lvps1_err.st_bit.Lvps1_err_CON3;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit2.Lvps2_err_CON4 = un_Lvps2_err.st_bit.Lvps2_err_CON4;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO3.st_bit2.sys_over_temp = un_sys_err.st_bit.sys_over_temp;

	
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO4.sysbLvps2_OutV =  (st_analog_data.u16_Lvps2_Vout / 10) *5 ;
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO4.sysbLvps2_OutC =  (st_analog_data.u16_Lvps2_Iout / 10) ;
	
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO5.THinv_DSP_version =  0X00000031 ;  //版本为1，31是ASCII嘛的1 
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO5.THinv_FPGA_version =  0X00000031 ;  //版本为1，31是ASCII嘛的1
	
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO6.Sinv_DSP1_version =  0X00000031 ;  //版本为1，31是ASCII嘛的1 
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO6.Sinv_DSP2_version =  0X00000031 ;  //版本为1，31是ASCII嘛的1
	
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO7.Lvps1_DSP_version =  0X00000031 ;  //版本为1，31是ASCII嘛的1 
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO7.Lvps1_FPGA_version =  0X00000031 ;  //版本为1，31是ASCII嘛的1
	
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO8.Lvps2_DSP_version =  0X00000031 ;  //版本为1，31是ASCII嘛的1 
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO8.Canopen_DSP_version =  0X00000031 ;  //版本为1，31是ASCII嘛的1
	
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO9.Canopen_FPGA_version =  0X00000031 ;  //版本为1，31是ASCII嘛的1 
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO9.SystemIO_version =  0X00000031 ;  //版本为1，31是ASCII嘛的1
	
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO10.THinv_PCB_version =  0X00000031 ;  //版本为1，31是ASCII嘛的1 
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO10.Sinv_PCB_version =   0X00000031 ;  //版本为1，31是ASCII嘛的1
	
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO11.Lvps1_PCB_version =  0X00000031 ;  //版本为1，31是ASCII嘛的1 
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO11.Lvps2_PCB_version =  0X00000031 ;  //版本为1，31是ASCII嘛的1
	
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO12.Canopen_PCB_version =   0X00000031 ;  //版本为1，31是ASCII嘛的1 
	un_seCanopen_data.st_pack.st_TPDO.st_TPDO12.SystemIO_PCB_version =  0X00000031 ;  //版本为1，31是ASCII嘛的1
	
	un_seCanopen_data.st_pack.u8_enddata = BCC_XOR_check(un_seCanopen_data.u8_data, 101);

}

/*********************************************************************************************************
** 函数名称：接收CANOPEN板的数据更新
** 函数描述：
** 输入参数：
** 返回值  ：
*********************************************************************************************************///
static void fmod_receive_canopen_updata(void)
{
	 st_product_info.u16_car_num  = un_reCanopen_data.st_pack.st_RPDO.st_RPDO3.u16_VN4wVehicleID;
	 un_sys_status.st_bit.sys_selftest = un_reCanopen_data.st_pack.st_RPDO.st_RPDO4.st_bit1.APU_sleftest;
	 un_Thinv_command.st_bit.Thinv_lock_reset = un_reCanopen_data.st_pack.st_RPDO.st_RPDO4.st_bit1.Thinv_lock_reset;
	
	 un_Lvps1_status.st_bit.Lvps1_lock_reset = un_reCanopen_data.st_pack.st_RPDO.st_RPDO4.st_bit2.Lvps1_lock_reset;
	 un_Lvps2_status.st_bit.Lvps2_lock_reset = un_reCanopen_data.st_pack.st_RPDO.st_RPDO4.st_bit2.Lvps2_lock_reset;
	 un_Sinv_status.st_bit.Sinv_lock_reset = un_reCanopen_data.st_pack.st_RPDO.st_RPDO4.st_bit2.Sinv_lock_reset;
	 un_sys_status.st_bit.sys_defroster_ON = un_reCanopen_data.st_pack.st_RPDO.st_RPDO5.st_bit.defrost_ON;
	
	 un_Thinv_command.st_bit.APm1_APx1xPTR = un_reCanopen_data.st_pack.st_RPDO.st_RPDO7.st_bit.APm1_APx1xPTR;
	 un_Thinv_command.st_bit.APm1_APx1xSupplyMode = un_reCanopen_data.st_pack.st_RPDO.st_RPDO7.st_bit.APm1_APx1xSupplyMode;
	 un_Thinv_command.st_bit.APm1_APx1xReceiveMode = un_reCanopen_data.st_pack.st_RPDO.st_RPDO7.st_bit.APm1_APx1xReceiveMode;	
}

/*********************************************************************************************************
** 函数名称：BCC(异或)校验
** 函数描述：计算校验码 和00异或其值不变
** 输入参数：
** 返回值  ：
*********************************************************************************************************///
uint8_t BCC_XOR_check(uint8_t *data,  uint8_t len)
{  
    uint8_t i = 0;
	uint8_t value = 0;
	
	value = 0x00;
	
	for(i = 0; i < len; i++) 
	{   
		value ^= data[i];		
	}
	return 	value; 	
}

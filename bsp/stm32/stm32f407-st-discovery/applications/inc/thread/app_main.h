#ifndef _APP_MAIN_H_
#define _APP_MAIN_H_
/*********************************************************************************************************
  文件包含
*********************************************************************************************************/
#include <rtthread.h>
#include <rtdevice.h>
#include <dfs_posix.h>  //虚拟文件系统
#include <board.h>

/*********************************************************************************************************
  宏定义
*********************************************************************************************************/

#define  CAR_A           1      // 车号 ，组合方式为 A-C,D-B;
#define  CAR_B           2      
#define  CAR_C           3     
#define  CAR_D           4      




struct Product_info   
{             
	char      i8_project_name[20];  //项目名称
	char      i8_product_name[20];	//产品名称
	uint16_t  u16_product_num;      //产品编号  
	uint16_t  u16_car_num;	        //产品车号 
	
	char      i8_soft_version[8];    //软件版本 
	uint16_t  u16_resv0;	
};

//.............................................模拟量数据采样............................................
struct	Analog_data                      //模拟量
{
	uint16_t	u16_APU_Vin;             // 输入电压             1=1V   
	uint16_t    u16_APU_Iin;             // 输入电流             1=1A  
	
	uint16_t	u16_Thinv_Vout;          // 3相逆变器输出平均电压   1 = 2V
	uint16_t	u16_Thinv_Iout;          // 3相逆变器输出平均电流   1:1
	
	uint16_t	u16_Thinv_freq;          // 3相逆变器频率        1:1
	uint16_t	u16_middle_V;            // 中间级电压           1=1V   
	
	uint16_t	u16_Lvps1_Vout;          // Lvps1输出电压        1=0.1V 
	uint16_t	u16_Lvps1_Iout;          // Lvps1输出电流        1=0.1A
	
	uint16_t	u16_Lvps2_Vout;          // Lvps2输出电压        1=0.1V 
	uint16_t	u16_Lvps2_Iout;          // Lvps2输出电流        1=0.1A
	
	uint16_t    u16_Sinv_Vout;           // 单逆输出电压         1=0.1V 	
	uint16_t    u16_Sinv_Iout;           // 单逆输出电流         1=0.1A	

};


//..............................................系统状态........................................
struct	System_status_bits
{
	uint16_t	sys_inputV_valid:1;          // 高压有效
	uint16_t	sys_preCH_ok:1;              // 预充电完成
	uint16_t	sys_status_ok:1;              // 系统状态OK,允许电源启动
	uint16_t	sys_FJ:1;                    // APU热继
		
	uint16_t	sys_ex_carid_A:1;            // 外部CAR_ID设定
	uint16_t	sys_ex_carid_B:1; 
	uint16_t	sys_ex_carid_C:1;	
	uint16_t	sys_ex_carid_D:1;
	
	uint16_t	sys_ex_Sinv_run:1;           // Sinv 外部启动信号
	uint16_t    sys_selftest:1;              // 接收的车辆通信信号 APU自检 
	uint16_t    sys_defroster_ON;            // 接收的车辆通信信号 除霜器开启信号
			
};

union	System_status_regs
{
	struct	  System_status_bits	 st_bit;
	uint16_t  u16_all;                  
};

struct	System_err_bits
{
	uint16_t	sys_err_inputfuse:1;               // 输入保险熔断    自恢复
	uint16_t	sys_over_Vin:1;                    // 750V输入过压    自恢复
	uint16_t	sys_under_Vin:1;                   // 750V输入欠压	 自恢复
	uint16_t	sys_over_temp:1;                   // APU过温故障     自恢复
	
	uint16_t	sys_assist_power_err:1;            // 应急辅助电源故障
	uint16_t	sys_incommun_err:1;                // 内部通讯故障
};

union	System_err_regs
{
	struct	  System_err_bits	 st_bit;
	uint16_t  u16_all;                  
};

//.........................................Lvps1工作状态.................................
struct	Lvps1_status_bits
{	
	uint16_t    Lvps1_shutdown:1;            // Lvps1 关机汇总
	uint16_t	Lvps1_lock:1;                // Lvps1 锁定
	uint16_t	Lvps1_reset:1;               // Lvps1 不锁死时的重启
	uint16_t	Lvps1_run:1;                 // Lvps1 正常工作
	
	uint16_t	Lvps1_lock_reset:1;          // Lvps1 外部信号 锁死时重启
	uint16_t    Lvps1_fault:1;               // Lvps1 故障汇总
	uint16_t	Lvps1_CON3_closed:1;         // Lvps1 输出接触器输出状态，1为吸合
	uint16_t	Lvps1_output_polarity:1;     // Lvps1 输出极性
		
};

union	Lvps1_status_regs
{
	struct	  Lvps1_status_bits	st_bit;
	uint16_t  u16_all;                  
};


struct	Lvps1_err_bits
{
	uint16_t	Lvps1_over_Vout:1;                 // Lvps1 输出过压；     3次关机
	uint16_t	Lvps1_err_igbt:1;                  // Lvps1 igbt故障       3次关机
	uint16_t	Lvps1_err_CON3:1;                  // Lvps1 CON3反馈故障   3次关机
	uint16_t	Lvps1_over_temp:1;                 // Lvps1 输出过温       自恢复	
	
	uint16_t	Lvps1_err_polarity:1;              // Lvps1 极性反接故障   自恢复
	uint16_t	Lvps1_err_selftest:1;              // Lvps1 自检故障      自恢复	
	uint16_t	Lvps1_over_Iout:1;                 // Lvps1 输出过流； 持续工作，启动过载信号
	uint16_t	Lvps1_under_Vout:1;                // Lvps1 输出欠压； 只报警不影响工作
	
	
	uint16_t	Lvps1_leak_P:1;	                   // Lvps1 正漏电     只报警不影响工作
	uint16_t	Lvps1_leak_N:1;	                   // Lvps1 负漏电     只报警不影响工作		
};

union	Lvps1_err_regs
{
	struct	  Lvps1_err_bits	 st_bit;
	uint16_t  u16_all;                  
};



//...............................................Lvps2工作状态...............................................
struct	Lvps2_status_bits
{
	uint16_t    Lvps2_shutdown:1;               // Lvps2 关机汇总
	uint16_t	Lvps2_lock:1;                // Lvps2 锁定
	uint16_t	Lvps2_reset:1;               // Lvps2 重启
	uint16_t	Lvps2_run:1;                 // Lvps2 正常工作
	
	uint16_t	Lvps2_lock_reset:1;          // Lvps2 外部信号 锁死时重启
	uint16_t    Lvps2_fault:1;               // Lvps2 故障汇总
	uint16_t	Lvps2_CON4_closed:1;         // Lvps2 输出接触器输出状态，1为吸合
	uint16_t	Lvps2_output_polarity:1;     // Lvps2 输出极性
};

union	Lvps2_status_regs
{
	struct	  Lvps2_status_bits	 st_bit;
	uint16_t  u16_all;                  
};

struct	Lvps2_err_bits
{
	uint16_t	Lvps2_over_Vout:1;                 // Lvps2 输出过压；    3次关机
	uint16_t	Lvps2_err_igbt:1;                  // Lvps2 igbt故障      3次关机
	uint16_t	Lvps2_err_CON4:1;                  // Lvps2 CON4反馈故障   3次关机
	uint16_t	Lvps2_over_temp:1;                 // Lvps2 输出过温      自恢复	
	
	uint16_t	Lvps2_err_polarity:1;              // Lvps2 极性反接故障   自恢复
	uint16_t	Lvps2_err_selftest:1;             // Lvps2 自检故障      自恢复	
	uint16_t	Lvps2_over_Iout:1;                 // Lvps2 输出过流； 持续工作，启动过载信号
	uint16_t	Lvps2_under_Vout:1;                // Lvps2 输出欠压； 只报警不影响工作
	
	
	uint16_t	Lvps2_leak_P:1;	                   // Lvps2 正漏电     只报警不影响工作
	uint16_t	Lvps2_leak_N:1;	                   // Lvps2 负漏电     只报警不影响工作		
			
};

union	Lvps2_err_regs
{
	struct	  Lvps2_err_bits	 st_bit;
	uint16_t  u16_all;                  
};

//..............................................单逆变Sinv工作状态..........................................
struct	Sinv_status_bits
{
	uint16_t    Sinv_shutdown:1;            // Sinv 关机汇总
	uint16_t	Sinv_lock:1;                // Sinv 锁定
	uint16_t	Sinv_reset:1;               // Sinv 重启
	uint16_t	Sinv_run:1;                 // Sinv 正常工作
	
	uint16_t	Sinv_lock_reset:1;          // Sinv 外部信号 锁死时重启
	uint16_t    Sinv_fault:1;               // Sinv 故障汇总
	uint16_t	Sinv_CON5_closed:1;         // Sinv输出接触器输出状态，1为吸合		
};

union	Sinv_status_regs
{
	struct	  Sinv_status_bits	 st_bit;
	uint16_t  u16_all;                  
};

struct	Sinv_err_bits
{
	uint16_t	Sinv_over_Vout:1;                 // Sinv 输出过压      3次关机	
	uint16_t	Sinv_err_CON5:1;                  // Sinv CON3反馈故障  3次关机
	uint16_t	Sinv_err_igbt:1;                  // Sinv 原边过流信号  3次关机
	uint16_t	Sinv_err_NB:1;                    // Sinv 逆变故障      3次关机
	
	uint16_t	Sinv_over_temp:1;                 // Sinv 输出过温      自恢复
	uint16_t	Sinv_err_selftest:1;             // Sinv 自检故障      自恢复
	uint16_t	Sinv_over_Iout:1;                 // Sinv 输出过流      持续工作，启动过载信号
	uint16_t	Sinv_under_Vout:1;                // Sinv 输出欠压      只报警不影响工作
			
};

union	Sinv_err_regs
{
	struct	  Sinv_err_bits	 st_bit;
	uint16_t  u16_all;                  
};

//.............................................Thinv工作状态.............................................
struct	Thinv_status_bits                    // 从逆变器发送来的状态信息
{ 
	uint16_t	Thinv_inputV_valid:1;        // Thinv 逆变器输入高压有效 即APU输入电压
	uint16_t    Thinv_run:1;                 // Thinv 故障汇总 接收逆变器的信号
	uint16_t	Thinv_lock:1;                // Thinv 锁定     接收逆变器的信号	
	uint16_t	Thinv_preCH_ok:1;            // Thinv 输出接触器状态，1为吸合	 接收逆变器的信号
	
	uint16_t	Thinv_CON1_closed:1;         // Thinv 输出接触器状态，1为吸合	 接收逆变器的信号
	uint16_t	Thinv_CON2_closed:1;         // Thinv 输出接触器状态，1为吸合	 接收逆变器的信号
    uint16_t	Thinv_standard_mode:1;        // Thinv sys为本车 逆变器标准工作模式，只给本车供电
	uint16_t	Thinv_supply_mode:1;          // Thinv  给本车和邻车供电
	
	uint16_t	Thinv_receive_mode:1;         // Thinv 本车接收邻车的供电
    uint16_t    Thinv_power_request:1;        // Thinv 该逆变器出现故障，要求相邻对其供电
};
union	Thinv_status_regs
{
	struct	  Thinv_status_bits	 st_bit;
	uint16_t  u16_all;                  
};

struct	Thinv_command_bits                   // 需发送给逆变器的指令
{
	uint8_t	   Thinv_start:1;                     // 逆变器启动
	uint8_t    Thinv_selftest:1;                  // Thinv 逆变器自检
	uint8_t	   Thinv_lock_reset:1;                // Thinv 锁死复位信号
	uint8_t    APm1_APx1xPTR:1;	            //邻车供电请求命令，要求本车给邻车供电
	
	uint8_t    APm1_APx1xSupplyMode:1;       //邻车供电命令，本车给邻车供电
	uint8_t    APm1_APx1xReceiveMode:1;      //接收邻车供电命令，邻车给本车供电
	
		
};
union	Thinv_command_regs
{
	struct	Thinv_command_bits  st_bit;	
    uint8_t u8_all;
};

struct	Thinv_err_bits 
{
	uint16_t    Thinv_err_inputfuse:1;             // 逆变器输入保险熔断
	uint16_t	Thinv_over_Vin:1;                  // Thinv 输出过压
	uint16_t	Thinv_under_Vin:1;                 // Thinv 输出欠压
	uint16_t	Thinv_leak:1;                      // Thinv 漏电
	
	uint16_t	Thinv_over_Vout:1;                 // Thinv 输出过压
	uint16_t	Thinv_under_Vout:1;                // Thinv 输出欠压
	uint16_t	Thinv_over_Iout:1;                 // Thinv 输出过流
	uint16_t	Thinv_over_temp:1;                 // Thinv 输出过温
	
	uint16_t	Thinv_err_igbt:1;                  // Thinv igbt故障
	uint16_t	Thinv_err_selftest:1;              // Thinv 自检故障
	uint16_t	Thinv_err_CON1:1;                  // Thinv CON1接触器故障
	uint16_t	Thinv_err_CON2:1;                  // Thinv CON2接触器故障
		
};
union	Thinv_err_regs
{
	struct	  Thinv_err_bits 	 st_bit;
	uint16_t  u16_all;                  
};


//.............................................电源故障锁死计数.............................................
struct Power_lock_count
{
	uint8_t   Lvps1_lock_count;
	uint8_t   Lvps2_lock_count;
	uint8_t   Sinv_lock_count;
};


struct Timer_sign                  //32byte 实时发送数据
{
   uint16_t   sign_10ms;
   uint16_t   sign_50s;
   uint16_t   sign_100ms;
   uint16_t   sign_500ms;
   uint16_t   sign_1s;
   uint16_t   sign_10s;
   uint16_t   sign_30s;
   uint16_t   sign_60s;
};


//.............................................和逆变器的CAN通讯.............................................
struct Re_Thinv_pack1
{   
	uint8_t	  u8_life;	
	uint8_t	  u8_resv0;
	
	uint16_t  u16_APU_Vin;
	uint16_t  u16_Thinv_Vout;       //   输出电压  1=1v 
	
	uint8_t  u8_APU_Iin;         //   输出电流  1=1A 
	uint8_t  u8_Thinv_Iout;         //   输出电流  1=1A 
};

union  Re_Thinv_packdata1
{
	struct  Re_Thinv_pack1  st_pack;
    uint8_t u8_data[8];                           
};

struct Re_Thinv_pack2
{   
    union	  Thinv_status_regs   un_status;
	union	  Thinv_err_regs      un_err;	
	
	uint16_t  u16_soft_version;       //   软件版本高字节
	uint8_t   u8_resv0;
	uint8_t   u8_resv1;
};

union  Re_Thinv_packdata2
{
	struct  Re_Thinv_pack2  st_pack;
    uint8_t u8_data[8];                           
};



struct Se_Thinv_pack
{   
	uint8_t	 u8_life;		
	uint8_t  u8_time[6];            
	union	 Thinv_command_regs  un_command;	
};

union  Se_Thinv_packdata
{
	struct  Se_Thinv_pack  st_pack;
    uint8_t u8_data[8];                           
};


//.............................................和Canopen板的UART通讯.............................................


//.............................................接收RPDO.............................................

struct RPDO1_bit
{
	uint8_t   resv0:1;	
	uint8_t   resv1:1;
	uint8_t   VAm1xLoadShedMode:1;        //压缩机1减载模式
	uint8_t   resv3:1;
	
	uint8_t   resv4:1;	
	uint8_t   resv5:1;
	uint8_t   resv6:1;
	uint8_t   resv7:1;
};

struct RPDO1_byte
{
	struct    RPDO1_bit st_bit;	
	uint8_t   u8_data[7];
};

struct RPDO2_bit
{
	uint8_t   resv0:1;	
	uint8_t   resv1:1;
	uint8_t   VAm2xLoadShedMode:1;        //压缩机1减载模式
	uint8_t   resv3:1;
	
	uint8_t   resv4:1;	
	uint8_t   resv5:1;
	uint8_t   resv6:1;
	uint8_t   resv7:1;
};

struct RPDO2_byte
{
	struct    RPDO2_bit   st_bit;	
	uint8_t   u8_data[7];
};

struct RPDO3_byte
{	
	uint16_t   u16_VN4wVehicleID;       // 车号
	uint8_t    u8_data[6];
};

struct RPDO4_bit1
{
	uint8_t   resv0:1;	
	uint8_t   resv1:1;
	uint8_t   resv2:1;
	uint8_t   resv3:1;
	
	uint8_t   resv4:1;	
	uint8_t   resv5:1;
	uint8_t   APU_sleftest:1;             // APU自检
	uint8_t   Thinv_lock_reset:1;         // 逆变器锁死复位
}; 
struct RPDO4_bit2
{
	uint8_t   Lvps1_lock_reset:1;	     // LVPS1锁死复位
	uint8_t   Lvps2_lock_reset:1;        // LVPS2锁死复位
	uint8_t   Sinv_lock_reset:1;         // 单逆锁死复位
	uint8_t   resv3:1;
	
	uint8_t   resv4:1;	
	uint8_t   resv5:1;
	uint8_t   resv6:1;
	uint8_t   resv7:1;
};

struct RPDO4_byte
{
	
	uint8_t   u8_data1[3];
	struct    RPDO4_bit1 st_bit1;
	struct    RPDO4_bit2 st_bit2;
	uint8_t   u8_data2[3];
};

struct RPDO5_bit
{
	uint8_t   resv0:1;	
	uint8_t   VN3xVN5_NetFailDup:1;     //显示屏不正常
	uint8_t   defrost_ON:1;             //除霜/除雾器打开命令
	uint8_t   resv3:1;
	
	uint8_t   resv4:1;	
	uint8_t   VN3xVN5_NetFail:1;       //虚拟节点5（触摸屏）不正常
	uint8_t   resv6:1;
	uint8_t   resv7:1;
};

struct RPDO5_byte
{
	struct    RPDO5_bit  st_bit;
	uint8_t   u8_data2[7];
};

struct RPDO6_bit
{
	uint8_t   VN3xVAm1_NetFail:1;	 //压缩机1网络通讯故障
	uint8_t   VN3xVAm2_NetFail:1;    //压缩机2网络通讯故障
	uint8_t   resv2:1;
	uint8_t   resv3:1;
	
	uint8_t   resv4:1;	
	uint8_t   resv5:1;
	uint8_t   resv6:1;
	uint8_t   resv7:1;
};

struct RPDO6_byte
{
	uint8_t   u8_data1[4];
	struct    RPDO6_bit  st_bit;
	uint8_t   u8_data2[3];
};
struct RPDO7_bit
{
	uint8_t   APm1_APx1xPTR:1;	            //邻车供电请求命令，要求本车给邻车供电
	uint8_t   APm1_APx1xSupplyMode:1;       //邻车供电命令，本车给邻车供电
	uint8_t   APm1_APx1xReceiveMode:1;      //接收邻车供电命令，邻车给本车供电
	uint8_t   resv3:1;
	
	uint8_t   resv4:1;	
	uint8_t   resv5:1;
	uint8_t   resv6:1;
	uint8_t   resv7:1;
};

struct RPDO7_byte
{
	struct    RPDO7_bit  st_bit;
	uint8_t   u8_data[7];
};

struct RPDO_pack
{
	struct RPDO1_byte    st_RPDO1;
	struct RPDO2_byte    st_RPDO2;
	struct RPDO3_byte    st_RPDO3;
	struct RPDO4_byte    st_RPDO4;
	struct RPDO5_byte    st_RPDO5;
	struct RPDO6_byte    st_RPDO6;
	struct RPDO7_byte    st_RPDO7;
};

struct Uart_re_Canopen_pack
{   
	uint8_t	 u8_headdata[5];
	
	struct   RPDO_pack   st_RPDO;     //   56字节
	
	uint8_t  u8_enddata;             //   
};

union  Uart_re_Canopen_packdata
{
	struct  Uart_re_Canopen_pack  st_pack;
    uint8_t u8_data[62];                           
};


struct Timestamp_RPDO8_byte
{
	uint8_t   u8_year;
	uint8_t   u8_month;
	uint8_t   u8_day;
	
	uint8_t   u8_hour;
	uint8_t   u8_min;
	uint8_t   u8_sec;
	uint8_t   u8_resv7;
	uint8_t   u8_resv8;
};

struct Canopen_timestamp
{
    uint8_t	 u8_headdata[5];
    struct   Timestamp_RPDO8_byte  st_tm;
    uint8_t	 u8_enddata;
};

union Canopen_timestamp_pack
{
    struct   Canopen_timestamp  st_pack;
	uint8_t  u8_data[14];
};

//.............................................发送TPDO.............................................
struct TPDO1_bits
{
	uint32_t	  inputV_valid:1;                 // 输入高压有效 
	uint32_t	  Thinv_run:1;                    // 逆变器工作 
	uint32_t	  Sinv_run:1;                     // 单逆变器工作 
	uint32_t	  Lvps1_run:1;                    // Lvps1工作 
	uint32_t	  Lvps2_run:1;                    // Lvps2工作
	uint32_t	  Thinv_CON1_closed:1;            // 逆变器输出接触器CON1闭合
	uint32_t	  Thinv_CON2_closed:1;            // 逆变器转换接触器CON2闭合
	uint32_t	  Reserved7:1;
	
	uint32_t	  Lvps1_CON3_closed:1;            // CON3闭合
	uint32_t	  Lvps2_CON4_closed:1;            // CON4闭合
	uint32_t	  Thinv_standard_mode:1;           // Thinv sys为本车 逆变器标准工作模式，只给本车供电
	uint32_t	  Thinv_supply_mode:1;             // Thinv 给本车和邻车供电
	uint32_t	  Thinv_receive_mode:1;            // Thinv 本车接收邻车的供电
	uint32_t	  Thinv_power_request:1;           // Thinv 该逆变器出现故障，
	uint32_t	  sys_selftest:1;                // APU自检
	uint32_t	  Reserved15:1;
	
	uint32_t	  Reserved16:1;
};
struct TPDO1_bytes
{
	struct    TPDO1_bits    st_bit;		
	uint8_t   u8_data[4];

};

struct TPDO2_bytes
{
		
	uint8_t   sysHV;                  // APU输入电压    1=6V
	uint8_t   Reserved;
	uint8_t   sysInv_OutV;            // 逆变器输出电压  1=2v
	uint8_t   sysInv_OutC;            // 逆变器输出电流  1:1
	
	uint8_t   sysSnglPhs_OutV;        // 单相逆变器输出电压  1=2V
	uint8_t   sysSnglPhs_OutC;        // 单相逆变器输出电流  1:1
	uint8_t   sysLvps1_OutV;          // LVPS 1输出电压      1:1
	uint8_t   sysLvps1_OutC;          // LVPS 1输出电流      1:1
};

struct TPDO3_bits1
{		
	uint32_t	  sys_err_inputfuse:1;         //APU输入保险熔断
	uint32_t	  Reserved1:1;
	uint32_t	  sys_over_Vin:1;
	uint32_t	  Reserved3:1;
	uint32_t	  Reserved4:1;
	uint32_t	  Reserved5:1;
	uint32_t	  Thinv_lock:1;                 // 逆变器锁定
	uint32_t	  Thinv_leak:1;                 // 逆变器接地故障
	
	uint32_t	  Thinv_over_Vout:1;            // Thinv 输出过压
	uint32_t	  Thinv_under_Vout:1;           // Thinv 输出欠压
	uint32_t	  Thinv_over_Iout:1;            // Thinv 输出过流
	uint32_t	  Thinv_over_temp:1;            // Thinv 输出过温
	uint32_t	  Thinv_err_igbt:1;             // Thinv igbt故障 
	uint32_t	  Thinv_err_selftest:1;         // Thinv 自检故障
	uint32_t	  Sinv_shutdown:1;              // 单逆锁定
	uint32_t	  Sinv_over_Vout:1;             // Sinv 输出过压
	
	uint32_t	  Sinv_under_Vout:1;            // Sinv 输出欠压
	uint32_t	  Sinv_over_Iout:1;             // Sinv 输出过流
	uint32_t	  Sinv_over_temp:1;             // Sinv 输出过温
	uint32_t	  Reserved19:1;
	uint32_t	  Reserved20:1;
	uint32_t	  Reserved21:1;
	uint32_t	  Lvps1_err_selftest:1;         // Lvps自检失败
	uint32_t	  Lvps1_lock:1;                 // Lvps1 锁定
	
	uint32_t	  Lvps1_leak_P:1;               // Lvps1 正漏电
	uint32_t	  Lvps1_leak_N:1;               // Lvps1 负漏电
	uint32_t	  Lvps1_over_Vout:1;            // Lvps1 输出过压；  
	uint32_t	  Lvps1_under_Vout:1;           // Lvps1 输出欠压； 只报警不影响工作
	uint32_t	  Lvps1_over_Iout:1;            // Lvps1 输出过流； 持续工作，启动过载信号
	uint32_t	  Lvps1_over_temp:1;            // Lvps1 输出过温       自恢复
	uint32_t	  Lvps1_err_igbt:1;             // Lvps1 igbt故障       3次关机
	uint32_t	  Lvps1_err_polarity:1;         // Lvps1 极性反接故障   自恢复
	
};

struct TPDO3_bits2
{		
	uint32_t	  Reserved0:1;
	uint32_t	  Reserved1:1;
	uint32_t	  Lvps2_lock:1;                // Lvps2 锁定
	uint32_t	  Lvps2_leak_P:1;	           // Lvps2 正漏电;
	uint32_t	  Lvps2_leak_N:1;	           // Lvps2 负漏电
	uint32_t	  Lvps2_over_Vout:1;           // Lvps2 输出过压
	uint32_t	  Lvps2_under_Vout:1;          // Lvps2 输出欠压
	uint32_t	  Lvps2_over_Iout:1;           // Lvps2 输出过流；
	
	uint32_t	  Lvps2_over_temp:1;           // Lvps2 输出过温
	uint32_t	  Reserved9:1;
	uint32_t	  Lvps2_err_polarity:1;        // Lvps2 极性反接故障 
	uint32_t	  sys_incommun_err:1;          // APU内部通讯故障
	uint32_t	  Reserved12:1;
	uint32_t	  Reserved13:1;
	uint32_t	  Thinv_err_CON1:1;            // Thinv CON1接触器故障
	uint32_t	  Thinv_err_CON2:1;            // Thinv CON2接触器故障
	
	uint32_t	  Sinv_err_CON5:1;             // Sinv  CON5接触器故障
	uint32_t	  Lvps1_err_CON3:1;            // Lvps1 CON3接触器故障
	uint32_t	  Lvps2_err_CON4:1;            // Lvps2 CON4接触器故障
	uint32_t	  sys_over_temp:1;            // APU过温故障 
	
};

struct TPDO3_bytes
{
	struct TPDO3_bits1  st_bit1;
	struct TPDO3_bits2  st_bit2;	
};

struct TPDO4_bytes
{		
	uint8_t   sysbLvps2_OutV;           // LVPS2  输出电压       为实际值*5
	uint8_t   sysbLvps2_OutC;           // LVPS2  输出电流       1:1
	uint8_t   u8_Reserved[6];
};

struct TPDO5_bytes
{		
	uint32_t   THinv_DSP_version;         //逆变器DSP版本
	uint32_t   THinv_FPGA_version;        //逆变器FPGA版本
};

struct TPDO6_bytes
{		
	uint32_t   Sinv_DSP1_version;         //单逆DSP1版本
	uint32_t   Sinv_DSP2_version;         //单逆DSP2版本
};

struct TPDO7_bytes
{		
	uint32_t   Lvps1_DSP_version;        // LVPS1 DSP版本
	uint32_t   Lvps1_FPGA_version;       // LVPS1 FPGA版本
};

struct TPDO8_bytes
{		
	uint32_t   Lvps2_DSP_version;        // LVPS2 DSP版本
	uint32_t   Canopen_DSP_version;      // Canopen DSP版本
};
struct TPDO9_bytes
{		
	uint32_t   Canopen_FPGA_version;      // Canopen FPGA版本
	uint32_t   SystemIO_version;         // 系统IO版本
};
struct TPDO10_bytes
{		
	uint32_t   THinv_PCB_version;         // 逆变器PCB版本
	uint32_t   Sinv_PCB_version;          // 单逆PCB版本
};
struct TPDO11_bytes
{		
	uint32_t   Lvps1_PCB_version;          // Lvps1 PCB版本
	uint32_t   Lvps2_PCB_version;          // Lvps2 PCB版本
};

struct TPDO12_bytes
{		
	uint32_t   Canopen_PCB_version;        // Canopen PCB版本
	uint32_t   SystemIO_PCB_version;       // System PCB版本
};


struct TPDO_pack
{
	struct   TPDO1_bytes   st_TPDO1;
	struct   TPDO2_bytes    st_TPDO2;
	struct   TPDO3_bytes    st_TPDO3;
	struct   TPDO4_bytes    st_TPDO4;
	struct   TPDO5_bytes    st_TPDO5;
	struct   TPDO6_bytes    st_TPDO6;
	struct   TPDO7_bytes    st_TPDO7;
	struct   TPDO8_bytes    st_TPDO8;
	struct   TPDO9_bytes    st_TPDO9;
	struct   TPDO10_bytes   st_TPDO10;
	struct   TPDO11_bytes   st_TPDO11;
	struct   TPDO12_bytes   st_TPDO12;

};

struct Uart_Se_Canopen_pack
{   
	uint8_t	 u8_headdata[5];
	
	struct   TPDO_pack   st_TPDO;     //   96字节
	
	uint8_t  u8_enddata;              //   
};

union  Uart_se_Canopen_packdata
{
	struct  Uart_Se_Canopen_pack  st_pack;
    uint8_t u8_data[102];                           
};


//.............................................系统运行数据，共同存储的数据内容.............................................
struct Sys_run_data         //46字节
{   
	struct	    Analog_data         st_adc_data;    //24字节
	
	union	    System_status_regs  un_sys_status;
	union	    System_err_regs     un_sys_err;
	
	union	    Lvps1_status_regs   un_Lvps1_status;
	union	    Lvps1_err_regs      un_Lvps1_err;
	
	union	    Lvps2_status_regs   un_Lvps2_status;
	union	    Lvps2_err_regs      un_Lvps2_err;
	
	union	    Sinv_status_regs    un_Sinv_status;
	union	    Sinv_err_regs       un_Sinv_err;

	union	    Thinv_status_regs   un_Thinv_status;
	union	    Thinv_err_regs      un_Thinv_err;
};

struct Sys_time_data    //62字节
{   
    uint8_t     u8_year;
	uint8_t	    u8_mon;                   
	
	uint8_t     u8_day;           
	uint8_t		u8_hour;

	uint8_t		u8_min;        	
	uint8_t		u8_sec;
	
	uint8_t		u8_msec_h;
	uint8_t		u8_msec_l;	
};

//......................................flash存储数据 1s一次  一天（24h）的大概5-6M,一个月总存储量150M容量.......................................
//........................................故障存储，故障前后各3秒.............................................
struct Status_store_data    //62字节
{   
    uint8_t     u8_headdata[2];
	uint16_t	u16_product_num;                    // 产品编号
	
	struct      Sys_run_data     st_data;           //46字节
	struct      Sys_time_data	 st_time;           //上传时间 8字节
	
	uint8_t     u8_enddata[2];
};

union Status_store_datapack
{   
	struct    Status_store_data    st_data;
	uint8_t   u8_data[60];
};


//.............................................2G通讯数据 1min一次 一次1024BYTE 则 1*60*24 = 1.5M...........................
struct Uart2g_send_data
{   
	//......................part1.....................................
	char         i8_project_name[32];                // 项目名称
	char		 i8_product_name[32];                // 产品名称
	uint16_t	 u16_product_num;                    // 产品编号
	
    //......................part2.....................................
	char	     i8_namespace[32];                   // 
	char		 i8_version[8];		                 // 软件版本
	
    //......................part3数据内容.............................             
	struct      Sys_run_data   st_data;  	         // 数据内容
	
	//......................part4.....................................  
	char		i8_timestring[20];	                 // 上传时间
};

extern  struct  TIMER_SIGN          timer_interval;

extern  struct  Product_info        st_product_info;
extern  struct	Analog_data         st_analog_data;        

extern  union	System_status_regs  un_sys_status;
extern  union	System_err_regs     un_sys_err;

extern  union	Lvps1_status_regs   un_Lvps1_status;
extern  union	Lvps1_err_regs      un_Lvps1_err;

extern  union	Lvps2_status_regs   un_Lvps2_status;
extern  union	Lvps2_err_regs      un_Lvps2_err;

extern  union	Sinv_status_regs  un_Sinv_status;
extern  union	Sinv_err_regs     un_Sinv_err;

extern  union	Thinv_status_regs   un_Thinv_status;
extern  union	Thinv_err_regs      un_Thinv_err;
extern  union	Thinv_command_regs  un_Thinv_command;


extern  struct  Power_lock_count    st_power_lock;

extern  union   Se_Thinv_packdata   un_se_thinv_data;
extern  union   Re_Thinv_packdata1  un_re_thinv_data1;
extern  union   Re_Thinv_packdata2  un_re_thinv_data2;


extern  union   Uart_re_Canopen_packdata  un_reCanopen_data;
extern  union   Uart_se_Canopen_packdata  un_seCanopen_data;

extern  union   Canopen_timestamp_pack    un_reCanopen_time;



extern  struct  Sys_run_data             st_sys_run_data;  //46字节

extern  struct  Sys_time_data            st_savefile_time[6];
extern  union   Status_store_datapack    un_status_wdata;        //实时工作状态存储

extern  union   Status_store_datapack    un_fault_wdata[30];     //故障信息存储
extern  union   Status_store_datapack    un_2g_store_wdata;      //2g数据存储
extern  union   Status_store_datapack    un_2g_store_rdata;

extern  struct  Uart2g_send_data         st_2g_send_data;
extern  struct  tm                       real_tm;  /* years since 1900 */
extern  time_t  real_sec;                          //从70年1月1号开始的秒数	
#endif 



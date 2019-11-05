/**************************************************************************************
故障存储：
		1秒5条数据（周期200ms）；
		前后各3秒共6秒，即一次故障条数=5*6=30；
		共记录200次故障；
		总容量：共1M
****************************************************************************************/
/********************************************************************************************
头文件
********************************************************************************************/
#include "../../inc/fmod/fmod_fault_store.h"

#define  FAULT_TIME       200        // 总共记录的故障次数
#define  FAULT_NUM        30         // 每一次故障的数据组数
#define  FAULT_DATA_LEN   60         // 每1组数据的字节长度

//........................................变量的定义.........................................
struct Fault_store_bits
{
	uint8_t	  sys_err_save_flag:1;                 //  
	uint8_t	  Lvps1_err_save_flag:1;               // 
	uint8_t	  Lvps2_err_save_flag:1;               //  
	uint8_t	  Sinv_err_save_flag:1;                // 
	
	uint8_t	  Thinv_err_save_flag:1;               // 
	uint8_t	  total_err_save_flag:1;               //  总故障保存
	uint8_t   record_delay_start_flag:1;           //  故障延时记录开启标志
	uint8_t   record_delay_finish_flag:1;          //  故障延时记录完成标志,并开始存储
};
union Fault_store_regs
{
    struct Fault_store_bits  st_bit;
	uint8_t  u8_all;
};


union   Fault_store_regs    un_fault_store;

union   Status_store_datapack    un_fault_wdata[30];
union   Status_store_datapack    un_fault_rdata;

union	System_err_regs     un_last_sys_err;    //上一次的状态
union	Lvps1_err_regs      un_last_Lvps1_err;
union	Lvps2_err_regs      un_last_Lvps2_err;
union	Sinv_err_regs       un_last_Sinv_err;
union	Thinv_err_regs      un_last_Thinv_err;

uint16_t   u16_fault_offset = 0;		//故障存储偏移次数，具体偏移量 = u16_fault_offset * FAULT_DATA_LEN

//........................................函数申明.........................................
static uint16_t fmod_flash_find_last_addr(uint16_t len);
void fault_store_updata(void);
void fault_store_judge(void);
void fault_store_data(void);
uint8_t *basic_flash_read_data(uint8_t *flash_data, uint8_t len, uint8_t offest );
int basic_flash_write_data(uint8_t *flash_data,  uint8_t len, uint8_t offest , uint8_t size) ;

/******************************************************************************************
函数功能：故障存储的过程
输入参数：数据长度
返回值 ：
*******************************************************************************************/
void fault_store_process(void)
{
	static uint8_t fault_nofristtime_flag = 0; //1表示非第一次启动标志 
	
	//...........................计算初始存储位置.............................................
	if (fault_nofristtime_flag == 0)
	{   
		u16_fault_offset = fmod_flash_find_last_addr(FAULT_DATA_LEN) * FAULT_NUM;
		fault_nofristtime_flag = 1;
		
		//...........................计算下一条要存的位置...........................
		u16_fault_offset = u16_fault_offset + 1;  
		if(u16_fault_offset >= FAULT_TIME * FAULT_NUM ) 
		{	
			u16_fault_offset = 0;	
		}
	}
	
	//.....................存储数据更新.............................
	fault_store_updata( );
	
	//.....................判断是否存储故障..........................
	fault_store_judge( );
	
	//.....................故障存储..................................
	if(un_fault_store.st_bit.record_delay_finish_flag == 1 )   
	{
		fault_store_data( );
		un_fault_store.st_bit.record_delay_finish_flag = 0;
	}
	
}
/******************************************************************************************
函数功能：每次上寻找flash最后一次存储位置
输入参数：数据长度
返回值 ：last_offset 最后一次存储位置
*******************************************************************************************/
static uint16_t fmod_flash_find_last_addr(uint16_t len)
{
    uint16_t i = 0 ,last_offset = 0;        //最后一次存储位置 
	uint32_t real_time = 0;                 //实时的时间
    uint32_t read_time = 0;			        //内存每次保存数据的时间
    uint32_t read_maxtime = 0;		        //保存的最大时间
	
	
	//...................................计算当前时间.....................................
	real_time = un_fault_rdata.st_data.st_time.u8_year * 31536000 + un_fault_rdata.st_data.st_time.u8_mon * 2592000 
                           + un_fault_rdata.st_data.st_time.u8_day * 86400 + un_fault_rdata.st_data.st_time.u8_hour * 3600 
			               + un_fault_rdata.st_data.st_time.u8_min * 60 + un_fault_rdata.st_data.st_time.u8_sec;
	
	//...................................寻找偏移位置,只搜索每组数据第一个.........................................
	for(i = 0; i < FAULT_TIME; i++)   //200条故障
	{
		basic_flash_read_data(un_fault_rdata.u8_data, len, i * FAULT_NUM );	//	读取数据
			
		if(  un_fault_rdata.st_data.u8_headdata[0] == 0XA1 && un_fault_rdata.st_data.u8_headdata[1] == 0X5F &&
		     un_fault_rdata.st_data.u8_headdata[1] == 0X01 && un_fault_rdata.st_data.u8_headdata[1] == 0XFE )      
		{
	         
			read_time = un_fault_rdata.st_data.st_time.u8_year * 31536000 + un_fault_rdata.st_data.st_time.u8_mon * 2592000 
                        + un_fault_rdata.st_data.st_time.u8_day * 86400 + un_fault_rdata.st_data.st_time.u8_hour * 3600 
			            + un_fault_rdata.st_data.st_time.u8_min * 60 + un_fault_rdata.st_data.st_time.u8_sec;
			
			if(read_time <= real_time)     //
			{	
				if(read_time >= read_maxtime)	//等于时取最后一个记录的时间
				{
					read_maxtime = read_time;
					last_offset = i;
				}
			}
			
		}
	}
	return last_offset;
}


/*********************************************************************************************************
** 函数名称：故障存储数据更新
** 函数描述：
** 输入参数：
** 返回值  ：无
*********************************************************************************************************///
void fault_store_updata(void)
{
	uint8_t i = 0;
	
	for(i = 0; i < 29; i++)
	{
		un_fault_wdata[i] = un_fault_wdata[i+1];
	}
	un_fault_wdata[29].st_data.u8_headdata[0] = 0XA1;
	un_fault_wdata[29].st_data.u8_headdata[1] = 0X5F;
	
	un_fault_wdata[29].st_data.st_data = st_sys_run_data;
	
    un_fault_wdata[29].st_data.st_time= un_fault_wdata[29].st_data.st_time;
	
	un_fault_wdata[29].st_data.u8_enddata[0] = 0X01;
	un_fault_wdata[29].st_data.u8_enddata[1] = 0XFE;
}

/*********************************************************************************************************
** 函数名称：判断是否存储故障
** 函数描述：故障检测
** 输入参数：
** 返回值  ：无
*********************************************************************************************************///
void fault_store_judge(void)
{
	static  uint8_t  u8_record_delay_num = 0;        //故障记录的持续延后时间 ，记录后15个的数据的序号    
	
	//.........................判断SYS是否有故障需要保存..............................
	if(un_sys_err.u16_all && (un_last_sys_err.u16_all != un_sys_err.u16_all) )
    {
		un_fault_store.st_bit.sys_err_save_flag = 1;
	}
    un_last_sys_err.u16_all = un_sys_err.u16_all;
		
	//.........................判断Lvps1是否有故障需要保存..............................
	if(un_Lvps1_err.u16_all && (un_last_Lvps1_err.u16_all != un_Lvps1_err.u16_all) )
    {
		un_fault_store.st_bit.Lvps1_err_save_flag = 1;
	}
    un_last_Lvps1_err.u16_all = un_Lvps1_err.u16_all;
	
	//.........................判断Lvps2是否有故障需要保存..............................
	if(un_Lvps2_err.u16_all && (un_last_Lvps2_err.u16_all != un_Lvps2_err.u16_all) )
    {
		un_fault_store.st_bit.Lvps2_err_save_flag = 1;
	}
    un_last_Lvps2_err.u16_all = un_Lvps2_err.u16_all;
	
	//.........................判断Sinv是否有故障需要保存..............................
	if(un_Sinv_err.u16_all && (un_last_Sinv_err.u16_all != un_Sinv_err.u16_all) )
    {
		un_fault_store.st_bit.Sinv_err_save_flag = 1;
	}
    un_last_Sinv_err.u16_all = un_Sinv_err.u16_all;
	
   //.........................判断Thinv是否有故障需要保存..............................
	if(un_Thinv_err.u16_all && (un_last_Thinv_err.u16_all != un_Thinv_err.u16_all) )
    {
		un_fault_store.st_bit.Thinv_err_save_flag = 1;
	}
    un_last_Thinv_err.u16_all = un_Thinv_err.u16_all;	
 
    //.........................总故障判断是否需要保存..............................
    if( (un_fault_store.u8_all & 0x0F) != 0)
    {
		un_fault_store.st_bit.total_err_save_flag = 1;
	}
	else
	{	
		un_fault_store.st_bit.total_err_save_flag = 0;
	}
	//.............................开启故障延时纪律................................
	if(un_fault_store.st_bit.total_err_save_flag)
	{ 
		un_fault_store.st_bit.record_delay_start_flag = 1;     //记录延时时间开启标志
		u8_record_delay_num = 0;                               // 新故障来时计数清0，故障的下一次的数组要求从st_fault_data[15]开始
	}
	
	//....................判断故障成完延时记录，准备好把此时的数据记录在flash的TXT中................................
	if(un_fault_store.st_bit.record_delay_start_flag)     
	{   
		u8_record_delay_num++;         //num =1 对应的是st_fault_data[14] 16对应的是st_fault_data[29]的数
		if(u8_record_delay_num >= 16)
		{	
			un_fault_store.st_bit.record_delay_start_flag = 0;
			un_fault_store.st_bit.record_delay_finish_flag = 1;  //故障完成标志
			u8_record_delay_num = 0;
		}	
	}
}

/*********************************************************************************************************
** 函数名称：存储故障
** 函数描述：故障检测
** 输入参数：
** 返回值  ：无
*********************************************************************************************************///
void fault_store_data(void)
{
	uint8_t  i = 0;
	
	for(i = 0 ; i < FAULT_NUM; i++)
	{
		basic_flash_write_data(un_fault_wdata[i].u8_data, FAULT_DATA_LEN, u16_fault_offset, i);        //把当前的数据次数放与开始位置
		
		u16_fault_offset++;
		if(u16_fault_offset >= FAULT_TIME * FAULT_NUM ) 
		{	
			u16_fault_offset = 0;	
		}
	}
}
/*********************************************************************************************************
** 函数名称：故障存储数据更新
** 函数描述：
** 输入参数：
** 返回值  ：无
*********************************************************************************************************///
//void fault_store_updata(void)
//{
//	uint8_t i = 0;
//	struct  Fault_store_data    run_data[30];  //实时工作数据	
//	static  uint8_t  u8_record_delay_num = 0;      //故障记录的持续延后时间 ，记录后15个的数据的序号                     

//	
//	for(i = 0; i < 29; i++)
//	{
//		run_data[i] = run_data[i + 1];

//	}
//	run_data[29].head_data[0] = 0XA1;
//	run_data[29].head_data[1] = 0X5F;
//	
//	run_data[29].st_data = st_sys_store_data;
//    run_data[29].st_time= run_data[29].st_time;
//	
//	run_data[29].end_data[0] = 0X01;
//	run_data[29].end_data[1] = 0XFE;
//	
//	
//	if(un_fault_store.st_bit.total_err_save_flag)
//	{ 
//		for(i = 0; i < 15; i++)                     //把故障的前15个先存入故障缓存 0-14
//		{
//			st_fault_data[i] = run_data[i + 15];    //是这时的15至30的数据
//		}
//		un_fault_store.st_bit.record_delay_start_flag = 1;     //记录延时时间开启标志
//		u8_record_delay_num = 14;                              //故障的下一次的数组要求从st_fault_data[15]开始
//	}
//	
//	if(un_fault_store.st_bit.record_delay_start_flag)     
//	{
//		if((u8_record_delay_num >= 15) && (u8_record_delay_num < 30))
//		{	
//			st_fault_data[u8_record_delay_num] = run_data[29];
//		}
//		
//		u8_record_delay_num++;
//		
//		if (u8_record_delay_num >= 30)
//		{
//			un_fault_store.st_bit.record_delay_start_flag = 0;
//			un_fault_store.st_bit.record_delay_finish_flag = 1;  //故障完成标志
//			u8_record_delay_num = 14;
//		}
//	}
//}

/********************************************************************************************
** 函数名称：	
** 函数描述：flash读数据接口
** 输入参数：len表示读取数据的长度，offest表示读取数据的位置 ,flash_data 数据
** 返回值  ：	 无
********************************************************************************************/
uint8_t *basic_flash_read_data(uint8_t *flash_data, uint8_t len, uint8_t offest )
{
	int  i = 0,  fd = 0;

    fd = open("/opt/apu_fdata.txt", O_RDWR|O_CREAT);

	if(fd < 0)
	{ 
		 rt_kprintf("open file flash read error!\n ");
	}

	i = lseek(fd, (offest * len), SEEK_SET);               //移动文件位置的偏移数，以字节为单位
	if(i < 0)
	{
		 rt_kprintf("flash seek failed!\n");
	}
	
	i = read(fd, flash_data, len);
	if (i < 0) 
	{ 
		 rt_kprintf("flash read failed!\n ");
	} 
		
	close (fd);
	
	return flash_data;
}

/********************************************************************************************
** 函数名称：	
** 函数描述：flash写数据接口
** 输入参数：len表示读取数据的长度，offest表示读取数据的位置 , size为每一组数据发送完成时 同步一次
** 返回值  ：	 无
********************************************************************************************/
int basic_flash_write_data(uint8_t *flash_data,  uint8_t len, uint8_t offest , uint8_t size) 
{
	int  i = 0,  fd = 0;
  
    fd = open("/opt/my_fdata.txt", O_RDWR|O_CREAT);
	if (fd < 0)
	{ 
		 rt_kprintf("open file wirte flash error!\n");
		return -1;
	}
	
    for(i = 0; i < len; i++)                 //数据赋值
    {    
    	if(i % 20 ==0)	 //18的整数倍
		{
			if(i >= 0)  rt_kprintf("\n");
			 rt_kprintf("flash_wdata:");
		}
		 rt_kprintf(" %02x",flash_data[i]);
    }
	 rt_kprintf("\n");
   	   
	i = lseek(fd, (offest * len) , SEEK_SET); //移动文件位置的偏移数，以字节为单位
	if(i<0)
	{
		 rt_kprintf("flash seek failed!\n");
		return -1;
	}

	i = write(fd, flash_data, len);            //写数据
	if(i <0)  
	{
		 rt_kprintf("flash write failed!\n");
		return -1;
	}
    //....................系统数据同步.....................
	if( size == (FAULT_NUM -1)) //每次的30组数据发送完成时同步一次  
	{	
		fsync(fd); 
	}
	
	
	
	close(fd);
	return 0;
}
/********************************************************************************************
    end file
********************************************************************************************/

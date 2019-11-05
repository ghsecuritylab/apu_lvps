
#include "../../inc/thread/thread_uart_2g.h"
/******************************************************************************************
** 宏定义
*******************************************************************************************/
#define  UART2G_THREAD_STACK_SZ        512         // 线程栈大小，单位是字节。
#define  UART2G_THREAD_PRIO            21          // 优先级 范围是从 0 ～ 255，数值越小优先级越高，0 代表最高优先级
#define  UART2G_THREAD_TIMESLICE       100         // 时间片大小：调用一次调度能够运行的最大时间长度

#define  STORE_2G_TIME       1500       // 总共记录的故障次数
#define  STORE_2G_DATA_LEN   60         // 每1组数据的字节长度
#define  REAL_DATA           1          // 发送实时数据
#define  HISTORY_DATA        2          // 发送历史数据
/********************************************************************************************
** 定义变量
********************************************************************************************/
static struct rt_thread uart2g_thread;  
static rt_uint8_t uart2g_thread_stack[UART2G_THREAD_STACK_SZ];

union   Status_store_datapack    un_2g_store_rdata;        //读取2g数据
union   Status_store_datapack    un_2g_store_wdata;        //存储2g数据
union   Status_store_datapack    un_2g_real_data;          //2g实时数据

struct  Uart2g_send_data         st_2g_send_data;          //2g数据发送

uint8_t uart2g_com_flag = 0;          //2g通讯正常标志,=1为通讯正常
uint8_t uart2g_timesync_flag = 0;     //2g通讯正常后，发送的时间和当前时间同步标志
extern  union Lvps2_err_regs     un_Lvps2_err;

uint16_t  start_offset_time = 0;      //发送数据的开始偏移次数
uint16_t  insert_offset_time = 0;     //新增存入数据的偏移次数 = 最后一次存入数据的位置+1

/********************************************************************************************
** 函数申明
********************************************************************************************/
void fmod_uart_2g_data_process(void) ;

void fmod_uart2g_real_updata(void);
uint8_t uart2g_receive(void);
void fmod_find_last_addr_2g(uint16_t len);
void uart2g_send_store_timesync_judge(uint16_t len);

void fmod_Json_2g_send_updata(uint8_t ch);	// 2G发送的数据更新.......................
void fmod_uart2g_data_read(void);           // 读出需要发送的数据
void fmod_uart2g_data_clean(void);          // 对发送的数据清0
void fmod_uart2g_data_store(void);          // 存储数据	
void fmod_uart2g_data_send(void);           

static  char  *fmod_make_jsonbody_2g(void);
static void fmod_json_data_uptime_2g(struct Sys_time_data  st_time);
void fmod_uart2g_jsondata_send(void);

uint8_t *basic_my_read_filedata(uint8_t *u8_data, uint8_t len, uint8_t offest );	//
int basic_my_write_filedata(uint8_t *u8_data,  uint8_t len, uint8_t offest );


/*********************************************************************************************************
** 函数名称：
** 函数描述：接收canopen的显示线程入口
** 输入参数：数码管要显示的内容
** 返回值  ：无
*********************************************************************************************************/
static void uart2g_thread_entry(void *param)
{

    while(1)
    {
         rt_thread_mdelay(100);
    }

}
/********************************************************************************************
** 函数名称：     
** 函数描述：线程的初始化
** 输入参数：无
** 返回值  ：    无
********************************************************************************************/
int uart2g_thread_init(void)
{
    // .....................................静态线程初始化.......................................
    rt_thread_init( &uart2g_thread,                                   // 线程结构体
                    "uart2g",                                         // 线程名称
                    uart2g_thread_entry, RT_NULL,                     // 线程入口函数名称和参数
                    uart2g_thread_stack, UART2G_THREAD_STACK_SZ,      // 栈的起始位置和大小 
                    UART2G_THREAD_PRIO,  UART2G_THREAD_TIMESLICE);    // 线程优先级，时间片大小
     //..................................... 启动线程 ，进入线程入口函数...........................
    rt_thread_startup(&uart2g_thread);
	return 0 ;    
}
INIT_COMPONENT_EXPORT(uart2g_thread_init);	


/******************************************************************************************
** 函数名称：2g数据处理
** 函数描述：UART数据发送
** 输入参数：无
** 返回值  ：    无
*******************************************************************************************/
void fmod_uart_2g_data_process(void)
{
	static uint8_t uart2g_nofristtime_flag = 0;
	static uint8_t uart2g_send_time_count = 0;
	static uint8_t uart2g_send_time_flag = 0; 

    //...................................10秒1次时间计数	........................................
    uart2g_send_time_count++;
	if(uart2g_send_time_count >= 6)
	{	
		uart2g_send_time_count = 0;
		uart2g_send_time_flag = 1;  
		//............................当前数据更新......................................
		fmod_uart2g_real_updata(); 	
	}	

	//............................第一次时寻找开始需要发送的数据置位和插入新数据的位置.................
	if (uart2g_nofristtime_flag == 0)
	{   
		uart2g_nofristtime_flag = 1;
		fmod_find_last_addr_2g(STORE_2G_DATA_LEN); 
	}
	//............................判断通讯是否正常......................................
	uart2g_com_flag =  uart2g_receive(); 
	
	//............................判断时间是否同步......................................
	uart2g_send_store_timesync_judge(STORE_2G_DATA_LEN); 
	
	//......................................................发送或存储2g数据........................................
	if(uart2g_com_flag)      //2G通讯是否正常
	{	
		if(uart2g_timesync_flag) 	    //发送时间同步,历史数据已经发送完成
		{		
			if(uart2g_send_time_flag == 1)               //1分钟时间判断
			{	
				uart2g_send_time_flag = 0;
	            fmod_Json_2g_send_updata(REAL_DATA);	 //实时数据作为本次需发送的数据.......................
				fmod_uart2g_data_send( );                //2g正常速率发送数据； 60s 1次	
			}	
		}
		else                //时间不同步，有历史数据需要发送
		{   
			//........................................发送历史数据........................................
			fmod_uart2g_data_read( );                 // 读出来需要发送的数据
			fmod_Json_2g_send_updata(HISTORY_DATA);	  //历史数据作为本次需发送的数据.......................
			fmod_uart2g_data_send( );                 // 加速发送历史数据；  10s 1次
			fmod_uart2g_data_clean( );                // 对已发送的历史数据清0；
			start_offset_time++;
			if( start_offset_time >= STORE_2G_TIME)
			{
				start_offset_time = 0;
			}	
			//........................................是否需要存储当前产生的新数据.................................
			if(uart2g_send_time_flag == 1)
			{	
				uart2g_send_time_flag = 0;
				fmod_uart2g_data_store( );               //存储当前产生的数据
			}		
		}	
	}
	else    //通讯不正常则一直存储数据
	{	
		if(uart2g_send_time_flag == 1)
		{			
			uart2g_send_time_flag = 0;
			fmod_uart2g_data_store( );  
		}
	}
	
}
/******************************************************************************************
函数功能：每次上寻找未发出的开始位置和需要新插入的位置
输入参数：数据长度
返回值 ：last_offset 最后一次存储位置
*******************************************************************************************/
uint8_t uart2g_receive(void)
{

}
/******************************************************************************************
函数功能：每次上寻找未发出的开始位置和需要新插入的位置
输入参数：数据长度
返回值 ：last_offset 最后一次存储位置
*******************************************************************************************/
void fmod_find_last_addr_2g(uint16_t len)
{
    uint16_t i = 0 ;        //最后一次存储位置 
	uint32_t real_time = 0;                     //实时的时间
    uint32_t read_time = 0;			            //内存每次保存数据的时间
	uint32_t read_mintime = 0;		            //保存的最小时间
    uint32_t read_maxtime = 0;		            //保存的最大时间
	uint16_t  min_offset =0, max_offset = 0;
	
	//...................................计算当前时间.....................................
	real_time = real_tm.tm_year * 31536000 + real_tm.tm_mon * 2592000 + real_tm.tm_mday * 86400 + 
	            real_tm.tm_hour * 3600 + real_tm.tm_min * 60 + real_tm.tm_sec;
	
	//...................................寻找开始和结束的偏移位置.........................................
	read_mintime = real_time;
	read_maxtime = 0;
	
	for(i = 0; i < STORE_2G_TIME; i++)   //1500条故障
	{
		basic_my_read_filedata(un_2g_store_rdata.u8_data, len, i );	//	读取文件中指定位置的数据
			
		if(  un_2g_store_rdata.st_data.u8_headdata[0] == 0XA1 && un_2g_store_rdata.st_data.u8_headdata[1] == 0X5F &&
		     un_2g_store_rdata.st_data.u8_headdata[1] == 0X01 && un_2g_store_rdata.st_data.u8_headdata[1] == 0XFE )      
		{
	         
			read_time = un_2g_store_rdata.st_data.st_time.u8_year * 31536000 + un_2g_store_rdata.st_data.st_time.u8_mon * 2592000 
                        + un_2g_store_rdata.st_data.st_time.u8_day * 86400 + un_2g_store_rdata.st_data.st_time.u8_hour * 3600 
			            + un_2g_store_rdata.st_data.st_time.u8_min * 60 + un_2g_store_rdata.st_data.st_time.u8_sec;
			
			
			if( read_time <= real_time )     //
			{	
				if( read_maxtime <= read_time )	//等于时取最后一个记录的时间
				{
					read_maxtime = read_time;
					max_offset = i;
				}
				if( read_mintime >= read_time )	//等于时取最后一个记录的时间
				{
					read_mintime = read_time;
					min_offset = i;
				}
			}		
		}
	}
	//...................................偏移位置赋值.........................................
	start_offset_time  = min_offset;
	insert_offset_time = max_offset;
	
	if(start_offset_time > 0) //里面有未发送的数据
	{	
		insert_offset_time = insert_offset_time + 1;
		if (insert_offset_time >= STORE_2G_TIME ) insert_offset_time = 0;
	}
}

/******************************************************************************************
** 函数名称：uart2g_send_store_timesync_judge
** 函数描述：判断发送的数据和存储的书否同步。
** 输入参数：无
** 返回值  ：无
*******************************************************************************************/
void uart2g_send_store_timesync_judge(uint16_t len)
{
	basic_my_read_filedata(un_2g_store_rdata.u8_data, len, start_offset_time );	//	读取文件中指定位置的数据
		
	if(  un_2g_store_rdata.st_data.u8_headdata[0] == 0XA1 && un_2g_store_rdata.st_data.u8_headdata[1] == 0X5F &&
		 un_2g_store_rdata.st_data.u8_headdata[1] == 0X01 && un_2g_store_rdata.st_data.u8_headdata[1] == 0XFE )      
	{
		 uart2g_timesync_flag = 0;
	}
	else
	{
		 uart2g_timesync_flag = 1;      //数据已发送完 
	}	
}


/******************************************************************************************
** 函数名称：fmod_uart2g_real_updata
** 函数描述：实时数据更新
** 输入参数：无
** 返回值  ：    无
*******************************************************************************************/
void fmod_uart2g_real_updata(void)
{
	//........................................读取此时系统时间.....................................
	//basic_read_rtc_time( );           //年月日时分秒的值
	//gettimeofday(&real_tm, NULL);     //1970年至今的总的秒数和微秒数
	
	un_2g_real_data.st_data.st_time.u8_year = real_tm.tm_year;
	un_2g_real_data.st_data.st_time.u8_mon = real_tm.tm_mon;
	un_2g_real_data.st_data.st_time.u8_day = real_tm.tm_mday;    //月的天数
	un_2g_real_data.st_data.st_time.u8_hour = real_tm.tm_hour;
	un_2g_real_data.st_data.st_time.u8_min = real_tm.tm_min;
	un_2g_real_data.st_data.st_time.u8_sec = real_tm.tm_sec;
	
	//................................存储数据的更新，实时数据................................
	un_2g_real_data.st_data.st_data.st_adc_data = st_analog_data;
	un_2g_real_data.st_data.st_data.un_sys_status.u16_all = un_sys_status.u16_all;
    un_2g_real_data.st_data.st_data.un_sys_err.u16_all = un_sys_err.u16_all;
	un_2g_real_data.st_data.st_data.un_Lvps1_status.u16_all = un_Lvps1_status.u16_all;
    un_2g_real_data.st_data.st_data.un_Lvps1_err.u16_all = un_Lvps1_err.u16_all;
	un_2g_real_data.st_data.st_data.un_Lvps2_status.u16_all = un_Lvps2_status.u16_all;
    un_2g_real_data.st_data.st_data.un_Lvps2_err.u16_all = un_Lvps2_err.u16_all;
	un_2g_real_data.st_data.st_data.un_Sinv_status.u16_all = un_Sinv_status.u16_all;
	un_2g_real_data.st_data.st_data.un_Sinv_err.u16_all = un_Sinv_err.u16_all;
	un_2g_real_data.st_data.st_data.un_Thinv_status.u16_all = un_Thinv_status.u16_all;
	un_2g_real_data.st_data.st_data.un_Thinv_err.u16_all = un_Thinv_err.u16_all;

}
/******************************************************************************************
** 函数名称：fmod_Json_updata_2g
** 函数描述：fmod_Json_updata_2g
** 输入参数：无
** 返回值  ：    无
*******************************************************************************************/
void fmod_Json_2g_send_updata(uint8_t ch)	
{
	
	strcpy(st_2g_send_data.i8_project_name, st_product_info.i8_project_name);
	strcpy(st_2g_send_data.i8_product_name, st_product_info.i8_product_name);
	st_2g_send_data.u16_product_num = st_product_info.u16_product_num;
	strcpy(st_2g_send_data.i8_version, st_product_info.i8_soft_version);

	if(ch == REAL_DATA )     //实时数据
	{
		//..............................2g数据...................................
		st_2g_send_data.st_data.st_adc_data = un_2g_real_data.st_data.st_data.st_adc_data;
		st_2g_send_data.st_data.un_sys_status.u16_all = un_2g_real_data.st_data.st_data.un_sys_status.u16_all;
		st_2g_send_data.st_data.un_sys_err.u16_all = un_2g_real_data.st_data.st_data.un_sys_err.u16_all;
		st_2g_send_data.st_data.un_Lvps1_status.u16_all = un_2g_real_data.st_data.st_data.un_Lvps1_status.u16_all;
		st_2g_send_data.st_data.un_Lvps1_err.u16_all = un_2g_real_data.st_data.st_data.un_Lvps1_err.u16_all;
		st_2g_send_data.st_data.un_Lvps2_status.u16_all = un_2g_real_data.st_data.st_data.un_Lvps2_status.u16_all;
		st_2g_send_data.st_data.un_Lvps2_err.u16_all = un_2g_real_data.st_data.st_data.un_Lvps2_err.u16_all;
		st_2g_send_data.st_data.un_Sinv_status.u16_all = un_2g_real_data.st_data.st_data.un_Sinv_status.u16_all;
		st_2g_send_data.st_data.un_Sinv_err.u16_all = un_2g_real_data.st_data.st_data.un_Sinv_err.u16_all; 
		st_2g_send_data.st_data.un_Thinv_status.u16_all = un_2g_real_data.st_data.st_data.un_Thinv_status.u16_all;
		st_2g_send_data.st_data.un_Thinv_err.u16_all = un_2g_real_data.st_data.st_data.un_Thinv_err.u16_all;
		
		//..............................上传时间..................................
		fmod_json_data_uptime_2g( un_2g_real_data.st_data.st_time); 
	}
    else                  //历史数据
	{
		//..............................2g数据...................................
		st_2g_send_data.st_data.st_adc_data = un_2g_store_rdata.st_data.st_data.st_adc_data;
		st_2g_send_data.st_data.un_sys_status.u16_all = un_2g_store_rdata.st_data.st_data.un_sys_status.u16_all;
		st_2g_send_data.st_data.un_sys_err.u16_all = un_2g_store_rdata.st_data.st_data.un_sys_err.u16_all;
		st_2g_send_data.st_data.un_Lvps1_status.u16_all = un_2g_store_rdata.st_data.st_data.un_Lvps1_status.u16_all;
		st_2g_send_data.st_data.un_Lvps1_err.u16_all = un_2g_store_rdata.st_data.st_data.un_Lvps1_err.u16_all;
		st_2g_send_data.st_data.un_Lvps2_status.u16_all = un_2g_store_rdata.st_data.st_data.un_Lvps2_status.u16_all;
		st_2g_send_data.st_data.un_Lvps2_err.u16_all = un_2g_store_rdata.st_data.st_data.un_Lvps2_err.u16_all;
		st_2g_send_data.st_data.un_Sinv_status.u16_all = un_2g_store_rdata.st_data.st_data.un_Sinv_status.u16_all;
		st_2g_send_data.st_data.un_Sinv_err.u16_all =un_2g_store_rdata.st_data.st_data.un_Sinv_err.u16_all; 
		st_2g_send_data.st_data.un_Thinv_status.u16_all = un_2g_store_rdata.st_data.st_data.un_Thinv_status.u16_all;
		st_2g_send_data.st_data.un_Thinv_err.u16_all = un_2g_store_rdata.st_data.st_data.un_Thinv_err.u16_all;
		
		//..............................上传时间..................................
		fmod_json_data_uptime_2g( un_2g_store_rdata.st_data.st_time); 	
	}
		
}

/******************************************************************************************
** 函数名称：fmod_uart2g_senddata_clean
** 函数描述：读取文件中指定的数据
** 输入参数：无
** 返回值  ：    无
*******************************************************************************************/
void fmod_uart2g_data_read(void)  
{
	basic_my_read_filedata(un_2g_store_rdata.u8_data, STORE_2G_DATA_LEN, start_offset_time );	//	读取文件中指定位置的数据
}
/******************************************************************************************
** 函数名称：fmod_uart2g_senddata_clean
** 函数描述：发送出去的数据清0
** 输入参数：无
** 返回值  ：    无
*******************************************************************************************/
void fmod_uart2g_data_clean(void)          //对已发送的历史数据清0；
{
	uint8_t  u8_clean[STORE_2G_DATA_LEN] = {0};
	
	basic_my_write_filedata(u8_clean, STORE_2G_DATA_LEN, start_offset_time);    //把发送的数据清0
}


/******************************************************************************************
** 函数名称：fmod_uart2g_store
** 函数描述：2g数据的存储，
** 输入参数：
** 返回值	：无
*******************************************************************************************/
void fmod_uart2g_data_store(void) 
{	
	un_2g_store_wdata = un_2g_real_data;
	
	//............................数据的存储.............................
	basic_my_write_filedata(un_2g_store_wdata.u8_data, STORE_2G_DATA_LEN, start_offset_time);    //把发送的数据清0
}

/******************************************************************************************
** 函数名称：fmod_UART_send_2g
** 函数描述：UART数据发送
** 输入参数：无
** 返回值  ：    无
*******************************************************************************************/
void fmod_uart2g_send(void)
{     
	char *jsonBody;

	//.......................申请内存初始化......................
    mallco_dev.init();
	//.......................定义动态内存大小....................
	jsonBody = mymalloc(512*sizeof(char *)); 
	//........................封装Json...........................
	jsonBody = fmod_make_jsonbody_2g();		 
	//......................打印封装好的内存.....................
	printf("\n");							 
	printf("%s\n",jsonBody);
	//......................发送或存储2g数据.....................
	fmod_uart2g_jsondata_send( );
	//.........................释放内存..................................
    myfree(jsonBody);	
}
/******************************************************************************************
** 函数名称：
** 函数描述：制作Json封装
** 输入参数：
** 返回值	：无
*******************************************************************************************/
static char *fmod_make_jsonbody_2g(void)
{
	cJSON * pJsonRoot = malloc(512*sizeof(cJSON *));
	cJSON * pSubJson = malloc(512*sizeof(cJSON *));
	cJSON * pSubJson0 = malloc(512*sizeof(cJSON *));
	cJSON * pBack = malloc(512*sizeof(cJSON *));
	char * p;

	pJsonRoot = cJSON_CreateObject();
	if(NULL == pJsonRoot)
	{
	    cJSON_Delete(pJsonRoot);
	    return NULL;
	}
	
	cJSON_AddStringToObject(pJsonRoot, "appkey", st_2g_send_data.i8_project_name);
	cJSON_AddStringToObject(pJsonRoot, "devicename", st_2g_send_data.i8_product_name);
    cJSON_AddNumberToObject(pJsonRoot, "devicekey", st_2g_send_data.u16_product_num);

    //...............pSubJson0..........................
	pSubJson0 = NULL;
	pSubJson0 = cJSON_CreateObject();
	if(NULL == pSubJson0)
	{ 
	   cJSON_Delete(pJsonRoot);
	   return NULL;
	}
	cJSON_AddStringToObject(pSubJson0, "namespace", "jd.data.origin" );  //不需要进制解析的数据
	cJSON_AddStringToObject(pSubJson0, "version", st_2g_send_data.i8_version);
	cJSON_AddItemToObject(pJsonRoot,  "header", pSubJson0);

	//...............pSubJson..........................
    pSubJson = NULL;
    pSubJson = cJSON_CreateObject();
    if(NULL == pSubJson)
    { 
		cJSON_Delete(pJsonRoot);
		return NULL;
    }
	cJSON_AddNumberToObject(pSubJson, "APU_in_V", st_2g_send_data.st_data.st_adc_data.u16_APU_Vin);
	cJSON_AddNumberToObject(pSubJson, "APU_in_I", st_2g_send_data.st_data.st_adc_data.u16_APU_Iin);
	cJSON_AddNumberToObject(pSubJson, "Thinv_Vout",  st_2g_send_data.st_data.st_adc_data.u16_Thinv_Vout);
	cJSON_AddNumberToObject(pSubJson, "Thinv_Iout", st_2g_send_data.st_data.st_adc_data.u16_Thinv_Iout);
	cJSON_AddNumberToObject(pSubJson, "Thinv_freq", st_2g_send_data.st_data.st_adc_data.u16_Thinv_freq);
	cJSON_AddNumberToObject(pSubJson, "middle_V", st_2g_send_data.st_data.st_adc_data.u16_middle_V);
	cJSON_AddNumberToObject(pSubJson, "Lvps1_Vout", st_2g_send_data.st_data.st_adc_data.u16_Lvps1_Vout);
	cJSON_AddNumberToObject(pSubJson, "Lvps1_Iout", st_2g_send_data.st_data.st_adc_data.u16_Lvps1_Iout);
	cJSON_AddNumberToObject(pSubJson, "Lvps2_Vout", st_2g_send_data.st_data.st_adc_data.u16_Lvps2_Vout);
	cJSON_AddNumberToObject(pSubJson, "Lvps2_Iout", st_2g_send_data.st_data.st_adc_data.u16_Lvps2_Iout);
	cJSON_AddNumberToObject(pSubJson, "Sinv_Vout", st_2g_send_data.st_data.st_adc_data.u16_Sinv_Vout);
	cJSON_AddNumberToObject(pSubJson, "Sinv_Iout", st_2g_send_data.st_data.st_adc_data.u16_Sinv_Iout);
		
	cJSON_AddNumberToObject(pSubJson, "sys_status", st_2g_send_data.st_data.un_sys_status.u16_all);
	cJSON_AddNumberToObject(pSubJson, "sys_err", st_2g_send_data.st_data.un_sys_err.u16_all);
	cJSON_AddNumberToObject(pSubJson, "Lvps1_status", st_2g_send_data.st_data.un_Lvps1_status.u16_all);
	cJSON_AddNumberToObject(pSubJson, "Lvps1_err", st_2g_send_data.st_data.un_Lvps1_err.u16_all);
	cJSON_AddNumberToObject(pSubJson, "Lvps2_status", st_2g_send_data.st_data.un_Lvps2_status.u16_all);
	cJSON_AddNumberToObject(pSubJson, "Lvps2_err", st_2g_send_data.st_data.un_Lvps2_err.u16_all);
	cJSON_AddNumberToObject(pSubJson, "Sinv_status", st_2g_send_data.st_data.un_Sinv_status.u16_all);
	cJSON_AddNumberToObject(pSubJson, "Sinv_err", st_2g_send_data.st_data.un_Sinv_err.u16_all);
	cJSON_AddNumberToObject(pSubJson, "Thinv_status", st_2g_send_data.st_data.un_Thinv_status.u16_all);
	cJSON_AddNumberToObject(pSubJson, "Thinv_err", st_2g_send_data.st_data.un_Thinv_err.u16_all);

	cJSON_AddItemToObject(pJsonRoot,  "payload", pSubJson);
	

    pBack = cJSON_CreateObject();
    if(NULL == pBack)
    { 
		cJSON_Delete(pJsonRoot);
		return NULL;
    }
    cJSON_AddStringToObject(pJsonRoot, "uptime",  st_2g_send_data.i8_timestring);

    p = cJSON_Print(pJsonRoot);
    if(NULL == p)
    {
		cJSON_Delete(pJsonRoot);
		return NULL;
    }

    cJSON_Delete(pJsonRoot);
    return p;
}

/******************************************************************************************
** 函数名称：
** 函数描述：JSON数据的上传时间设定 将字节转华为字符
** 输入参数：
** 返回值	：无
*******************************************************************************************/
static void fmod_json_data_uptime_2g(struct Sys_time_data  st_time) 
{
	char time_buf[6] = {0};

    //.............	timestring[4][7]为- [10]空格,[13] [16]为“:” 如2018-01-01 00:00:00.............
    sprintf(time_buf, "%4d-", st_time.u8_year+1900);	     //年
	st_2g_send_data.i8_timestring[0] = time_buf[0];
	st_2g_send_data.i8_timestring[1] = time_buf[1];
	st_2g_send_data.i8_timestring[2] = time_buf[2];
	st_2g_send_data.i8_timestring[3] = time_buf[3];
	st_2g_send_data.i8_timestring[4] = time_buf[4];
	
	sprintf(time_buf, "%02d-", st_time.u8_mon+1);        //月
	st_2g_send_data.i8_timestring[5] = time_buf[0];
	st_2g_send_data.i8_timestring[6] = time_buf[1];
	st_2g_send_data.i8_timestring[7] = time_buf[2];
	
	sprintf(time_buf, "%02d ", st_time.u8_day);          //日
	st_2g_send_data.i8_timestring[8] = time_buf[0];
	st_2g_send_data.i8_timestring[9] = time_buf[1];	
	st_2g_send_data.i8_timestring[10] = time_buf[2];
	
	sprintf(time_buf, "%02d:", st_time.u8_hour);          //时
	st_2g_send_data.i8_timestring[11] = time_buf[0];
	st_2g_send_data.i8_timestring[12] = time_buf[1];
	st_2g_send_data.i8_timestring[13] = time_buf[2];
	
	sprintf(time_buf,  "%02d:", st_time.u8_min);         //分
	st_2g_send_data.i8_timestring[14] = time_buf[0];
	st_2g_send_data.i8_timestring[15] = time_buf[1];
	st_2g_send_data.i8_timestring[16] = time_buf[2];
	
	sprintf(time_buf,  "%02d", st_time.u8_sec);          //秒
	st_2g_send_data.i8_timestring[17] = time_buf[0];
	st_2g_send_data.i8_timestring[18] = time_buf[1];
}
/******************************************************************************************
** 函数名称：2g JSON数据的发送
** 函数描述：
** 输入参数：
** 返回值	：无
*******************************************************************************************/
void fmod_uart2g_jsondata_send(void) 
{


}

/********************************************************************************************
    end file
********************************************************************************************/

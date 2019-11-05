/**************************************************************************************
 *  Copyright :  zhizhuo
 *  Date :     2019.10.22
 *  function: 状态数据的存储
 *  describe：1. 如何挂载未nandflash编写。
             
****************************************************************************************/
#include "../../inc/fmod/fmod_real_status_store.h"

//........................................宏定义.........................................
#define  REAL_STATUS_DATA_LEN   60                       // 每1组数据的字节长度

struct Status_savefile_time    //62字节
{   
    uint8_t     u8_headdata[2];
	struct      Sys_time_data	 st_tm[6];               //6*8	
	uint8_t     u8_enddata[2];
};
union Status_savefile_tm    //52字节
{   
    struct      Status_savefile_time	 st_data;
	uint8_t     u8_data[52];
};

//........................................变量定义.........................................
union   Status_store_datapack      un_status_wdata;        //实时工作数据状态存储
union   Status_savefile_tm         un_savefile_rtime;       //存储文件的时间记录
union   Status_savefile_tm         un_savefile_wtime; 

uint8_t nandflash_mount_sign = 0 ;
char    status_filename[60]={0} ; 
 
//........................................函数申明.........................................
void   fmod_statusstore_time_read(void);
void   fmod_statusstore_time_write(void);

void   fmod_real_status_store_updata(void);
void   real_status_store_data(void);

static void fmod_set_status_filename(void) ;
int fmod_status_file_wdata(char *filename,  uint8_t *data) ;

uint8_t *basic_flash_read_data(uint8_t *flash_data, uint8_t len, uint8_t offest );
int basic_flash_write_data(uint8_t *flash_data,  uint8_t len, uint8_t offest );

/******************************************************************************************
** 函数名称：
** 函数描述：sdcard数据存储
** 输入参数：*Jsondata：需要存储的数据
** 返回值	：无
*******************************************************************************************/
void fmod_real_status_store(void)
{
	//........................读取存取的时间.......................
	fmod_statusstore_time_read( );
	
	//........................数据更新.......................
	fmod_real_status_store_updata( );	

	
	//...........................每次刚上电时挂载dev/mmcblk0p1..........................	
	if(!nandflash_mount_sign)
	{	
		// basic_sd_mount( );       //挂载nandflash文件夹，判断文件夹是否挂载成功		
	}

	//...........................挂载成功，则存储SD卡数据...............................
	if(nandflash_mount_sign)
	{	
		//......................设置存储nandflash的文件名称................
		fmod_set_status_filename( );

		//......................存储数据在nandflash卡中.................. 
		fmod_status_file_wdata(status_filename,  un_status_wdata.u8_data);	
	}		
}
/*********************************************************************************************************
** 函数名称：读取存储的文件名称
** 函数描述：
** 输入参数：
** 返回值  ：无
*********************************************************************************************************/
void fmod_statusstore_time_read(void )
{
    int  i = 0,  fd = 0;
    union   Status_savefile_tm   un_time;
	
	
    fd = open("/opt/statusstore_time.txt", O_RDWR|O_CREAT);
	if (fd < 0)
	{ 
		printf("read_statusstore_time file open error!\n ");
	}

	i = lseek(fd, 0, SEEK_SET);               //(offest * len) =0 从开始部分读写 移动文件位置的偏移数，以字节为单位
	if(i<0)
	{
		printf("statusstore_time file seek failed!\n");
	}
	
	i = read(fd, un_time.u8_data, sizeof(un_time.u8_data));
	if (i < 0) 
	{ 
		printf("statusstore_time file read failed!\n ");
	} 	
	close (fd);
	
	if((un_time.st_data.u8_headdata[0] == 0XAA)  && (un_time.st_data.u8_enddata[1] == 0XCC) )
	{
		un_savefile_rtime = un_time;
	}	
}
/*********************************************************************************************************
** 函数名称：读取存储的文件名称
** 函数描述：
** 输入参数：
** 返回值  ：无
*********************************************************************************************************/
void fmod_statusstore_time_write(void)
{
	int  i = 0,  fd = 0;

	un_savefile_wtime.st_data.u8_headdata[0] = 0XAA;
	un_savefile_wtime.st_data.u8_headdata[1] = 0X66;
	un_savefile_wtime.st_data.u8_enddata[0] = 0X22;
	un_savefile_wtime.st_data.u8_enddata[1] = 0XCC;	
		
    fd = open("/opt/statusstore_time.txt", O_RDWR|O_CREAT);
	if (fd < 0)
	{ 
		printf("write_statusstore_time file open error!\n");
		
	}	
    for(i = 0; i < 52; i++)                 //数据赋值
    {    
    	if(i % 20 ==0)	 //18的整数倍
		{
			if(i >= 0) printf("\n");
			printf("statusstore_time:");
		}
		printf(" %02x",un_savefile_wtime.u8_data[i]);
    }
	printf("\n");
  	   
	i = lseek(fd, 0 , SEEK_SET); //0无需偏移 移动文件位置的偏移数，以字节为单位
	if(i<0)
	{
		printf("statusstore_time seek failed!\n");
		
	}
	i = write(fd, un_savefile_wtime.u8_data, sizeof(un_savefile_wtime.u8_data));            //写数据
	if(i <0)  
	{
		printf("statusstore_time write failed!\n");	
	}

    //....................系统数据同步.....................
	fsync(fd); 
	close(fd);
}

/*********************************************************************************************************
** 函数名称：数据更新
** 函数描述：
** 输入参数：
** 返回值  ：无
*********************************************************************************************************///
void fmod_real_status_store_updata(void)
{  
	
	
    //........................................读取此时系统时间.....................................
	//basic_read_rtc_time( );           //年月日时分秒的值
	//gettimeofday(&real_tm, NULL);     //1970年至今的总的秒数和微秒数
	
	un_status_wdata.st_data.st_time.u8_year = real_tm.tm_year;
	un_status_wdata.st_data.st_time.u8_mon = real_tm.tm_mon;
	un_status_wdata.st_data.st_time.u8_day = real_tm.tm_mday;    //月的天数
	un_status_wdata.st_data.st_time.u8_hour = real_tm.tm_hour;
	un_status_wdata.st_data.st_time.u8_min = real_tm.tm_min;
	un_status_wdata.st_data.st_time.u8_sec = real_tm.tm_sec;
	
	//........................................更新数据.....................................
	un_status_wdata.st_data.u8_headdata[0] = 0XA1;
	un_status_wdata.st_data.u8_headdata[1] = 0X5F;
	
	un_status_wdata.st_data.st_data = st_sys_run_data;
	
    un_status_wdata.st_data.st_time= un_fault_wdata[29].st_data.st_time;
	
	un_status_wdata.st_data.u8_enddata[0] = 0X01;
	un_status_wdata.st_data.u8_enddata[1] = 0XFE;
}


/******************************************************************************************
** 函数名称：
** 函数描述：确定SD卡存储文件的文件名
** 输入参数：
** 返回值	：无
*******************************************************************************************/
static void fmod_set_status_filename(void) 
{
	uint8_t i = 0;
	int  ret = 0;
	char path[60] = "/my_sdcard/";
	struct stat buf;

	//.................如果电池首次上电，当前时间作为第一次文件建立时间........................
	if((un_savefile_rtime.st_data.st_tm[5].u8_year == 0) && (un_savefile_rtime.st_data.st_tm[5].u8_mon == 0) )	 
	{	
		un_savefile_wtime.st_data.st_tm[5].u8_year = real_tm.tm_year;
		un_savefile_wtime.st_data.st_tm[5].u8_mon =  real_tm.tm_mon;
		un_savefile_wtime.st_data.st_tm[5].u8_day =  real_tm.tm_mday;
		//...........................对时间进行存储....................
        fmod_statusstore_time_write( );		
	} 
	else    //......................... 非首次上电查看原来的文件大小，确定文件名称.........................
	{   
		//....................................查看原来的文件是否已经超过指定大小........................
		snprintf(status_filename, sizeof(status_filename), "apu_%04d-%02d-%02d.txt", 
				 un_savefile_wtime.st_data.st_tm[5].u8_year+1900, un_savefile_wtime.st_data.st_tm[5].u8_mon+1, un_savefile_wtime.st_data.st_tm[5].u8_day);	

		ret = stat(status_filename, &buf);       //读取文件的大小
		if(ret == 0)
		{
			rt_kprintf("status file size = %d\n", buf.st_size);     //字节数
			if( buf.st_size >= (20 * 1024* 1024) )                //文件大于20M 则删除最早的文件
			{	
				if(un_savefile_wtime.st_data.st_tm[0].u8_year > 0)   //表示已经存储的次数到达最高个数了
				{
					snprintf(status_filename, sizeof(status_filename), "tjb1_bms_%04d-%02d-%02d.txt", 
							 un_savefile_wtime.st_data.st_tm[0].u8_year+1900, un_savefile_wtime.st_data.st_tm[0].u8_mon+1,un_savefile_wtime.st_data.st_tm[0].u8_day);

					strcat(path, status_filename);
					unlink(path);
				}
			
				for(i = 0; i < 5; i++)
				{
					un_savefile_wtime.st_data.st_tm[i].u8_year = un_savefile_wtime.st_data.st_tm[i+1].u8_year;
					un_savefile_wtime.st_data.st_tm[i].u8_mon = un_savefile_wtime.st_data.st_tm[i+1].u8_mon;
					un_savefile_wtime.st_data.st_tm[i].u8_day = un_savefile_wtime.st_data.st_tm[i+1].u8_day;
				}	 
				//........................此时的时间为新文件的名称..................
				un_savefile_wtime.st_data.st_tm[5].u8_year = real_tm.tm_year;  
				un_savefile_wtime.st_data.st_tm[5].u8_mon =  real_tm.tm_mon;
				un_savefile_wtime.st_data.st_tm[5].u8_day =  real_tm.tm_mday;
				
				//...........................对时间进行存储....................
                fmod_statusstore_time_write( );		
			}
		}	
		else
		{
			rt_kprintf("status file not fonud\n");
		}
	}
		
	//....................................确定文件名称........................
	snprintf(status_filename, sizeof(status_filename), "apu_%04d-%02d-%02d.txt", 
		     un_savefile_wtime.st_data.st_tm[5].u8_year+1900, un_savefile_wtime.st_data.st_tm[5].u8_mon+1, un_savefile_wtime.st_data.st_tm[5].u8_day);		
}

/******************************************************************************************
** 函数名称：
** 函数描述：状态数据的存储
** 输入参数：*Jsondata：需要存储的数据
** 返回值	：无
*******************************************************************************************/
int fmod_status_file_wdata(char *filename,  uint8_t *data) 
{
	FILE *fp;
	char path[60] = "/my_sdcard/";

	strcat(path, filename);

	//........................存储数据...........................		
	if((fp = fopen(path,"a+")) == NULL)     //追加的方式打开文件，写的内容放在文件结尾，文件不存在则建立文件
	{
		rt_kprintf("open status_file failed!\n");
		return -1;
	}
	else
	{
		fwrite(data, sizeof(data), 1, fp); //写入数据
		fclose(fp);
		rt_kprintf("status_file write ok!\n");
	}
  
	return 0;
}

/********************************************************************************************
    end file
********************************************************************************************/

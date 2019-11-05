/*
 * @ : ***************************************************************************************
 * @Description: 
 * @version: 
 * @Author: liwei
 * @Date: 2019-10-23 16:41:11
 * @LastEditTime: 2019-10-30 12:00:58
 * @  : ***************************************************************************************
 */
/********************************************************************************************
头文件
********************************************************************************************/
#include "../../inc/thread/thread_can_re_thinv.h"


/******************************************************************************************
** 宏定义
*******************************************************************************************/
#define  RE_THINV_THREAD_STACK_SZ        512         // 线程栈大小，单位是字节。
#define  RE_THINV_THREAD_PRIO            19          // 优先级 范围是从 0 ～ 255，数值越小优先级越高，0 代表最高优先级
#define  RE_THINV_THREAD_TIMESLICE       100         // 时间片大小：调用一次调度能够运行的最大时间长度

/********************************************************************************************
** 定义变量
********************************************************************************************/
static struct rt_thread re_thinv_thread;  
static rt_uint8_t re_thinv_thread_stack[RE_THINV_THREAD_STACK_SZ];
/*********************************************************************************************************
** 函数名称：
** 函数描述：接收canopen的显示线程入口
** 输入参数：数码管要显示的内容
** 返回值  ：无
*********************************************************************************************************/
static void re_thinv_thread_entry(void *param)
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
int re_thinv_thread_init(void)
{
    // .....................................静态线程初始化.......................................
    rt_thread_init( &re_thinv_thread,                                   // 线程结构体
                    "re_thinv",                                         // 线程名称
                    re_thinv_thread_entry, RT_NULL,                     // 线程入口函数名称和参数
                    re_thinv_thread_stack, RE_THINV_THREAD_STACK_SZ,  // 栈的起始位置和大小 
                    RE_THINV_THREAD_PRIO, RE_THINV_THREAD_TIMESLICE); // 线程优先级，时间片大小
     //..................................... 启动线程 ，进入线程入口函数........................... 
    rt_thread_startup(&re_thinv_thread);  
    return  0 ;  
}
INIT_COMPONENT_EXPORT(re_thinv_thread_init);	
/*
 * @*: ***************************************************************************************
 * @Description: 
 * @version: 
 * @Author: liwei
 * @Date: 2019-10-24 13:03:33
 * @LastEditTime: 2019-11-04 20:06:39
 * @**: **************************************************************************************/
/******************************************************************************************
** 头文件
*******************************************************************************************/
#include "../../inc/basic/basic_uart.h"
/********************************************************************************************
宏定义
********************************************************************************************/
/********************************************************************************************
变量定义
********************************************************************************************/
rt_device_t serial_uart1;      //设备号  句柄
rt_device_t serial_uart3;
rt_device_t serial_uart4;
rt_device_t serial_uart5;
rt_device_t serial_uart6;

struct rt_messagequeue rx_mq1; //消息队列
struct rt_messagequeue rx_mq3;
struct rt_messagequeue rx_mq4;
struct rt_messagequeue rx_mq5;
struct rt_messagequeue rx_mq6;

/********************************************************************************************
函数申明
********************************************************************************************/
static rt_err_t basic_uart1_input(rt_device_t dev, rt_size_t size);
static rt_err_t basic_uart3_input(rt_device_t dev, rt_size_t size);
static rt_err_t basic_uart4_input(rt_device_t dev, rt_size_t size);
static rt_err_t basic_uart5_input(rt_device_t dev, rt_size_t size);
static rt_err_t basic_uart6_input(rt_device_t dev, rt_size_t size);
/********************************************************************************************
** 函数名称：     
** 函数描述：uart1的初始化
** 输入参数：无
** 返回值  ：    无
********************************************************************************************/
void basic_uart1_init(uint32_t baudrate, uint32_t len)
{
    static uint8_t msg_pool1[256];  //消息队列中用到的放置消息的内存池

    rt_err_t ret = RT_EOK;
    //..............................初始化消息队列 ..............................
    rt_mq_init(&rx_mq1, "rx_mq1",        // 信息队列的句柄指针，和消息队列名称
               msg_pool1,                /* 存放消息的缓冲区 */
               sizeof(struct rx_msg),    /* 一条消息的大小 消息中只放设备号和读取文件大小 */
               sizeof(msg_pool1),        /* 存放消息的缓冲区大小 */
               RT_IPC_FLAG_FIFO);        /* 如果有多个线程等待，按照先来先得到的方法分配消息 */
    //..............................查找串口...............................
    serial_uart1 = rt_device_find("uart1");

    //................... 串口配置结构体，使用serial.h的宏定义................
    struct serial_configure gps_use_config = 
    {
        baudrate,             /* 波特率 bits/s */
        DATA_BITS_8,          /* 8 databits */
        STOP_BITS_1,          /* 1 stopbit */
        PARITY_NONE,          /* No parity  */ 
        BIT_ORDER_LSB,        /* LSB first sent */
        NRZ_NORMAL,           /* Normal mode */
        1024,                 /* 接收数据缓冲区大小 */
        0   
    };
    //..............................打开串口设备............................
    if (serial_uart1)
    {
        rt_device_open(serial_uart1, RT_DEVICE_FLAG_DMA_RX);        /* DMA模式 */
    }
    else
    {
        return;
    }
    //..............................修改串口参数配置............................
    if (RT_EOK != rt_device_control(serial_uart1, RT_DEVICE_CTRL_CONFIG,(void *)&gps_use_config))
    {
        rt_kprintf("uart1 config baud rate failed.\n");
    }

     //..............................设置设备接收回调函数............................
    rt_device_set_rx_indicate(serial_uart1,  basic_uart1_input);
    
}


/*** @ : ***************************************************************************************
 * @name: 
 * @describe: 接收数据回调函数
 * @param :   dev：设备号   size：大小
 * @return: 
 * @  : ****************************************************************************************/
static rt_err_t basic_uart1_input(rt_device_t dev, rt_size_t size)
{
    struct rx_msg msg;
    rt_err_t result;
    
    msg.dev = dev;
    msg.size = size;

    result = rt_mq_send(&rx_mq1, &msg, sizeof(msg));
    if ( result == -RT_EFULL)
    {
        //********************************************消息队列满****************************
        rt_kprintf(" uart1 message queue full！\n");
    }
    return result;
}

/********************************************************************************************
** 函数名称：     
** 函数描述：uart3的初始化
** 输入参数：无
** 返回值  ：    无
********************************************************************************************/
void basic_uart3_init(uint32_t baudrate, uint32_t re_msg_size)
{
    static uint8_t msg_pool3[256];  //消息队列中用到的放置消息的内存池

    rt_err_t ret = RT_EOK;
    //..............................初始化消息队列 ..............................
    rt_mq_init(&rx_mq3, "rx_mq3",        // 信息队列的句柄指针，和消息队列名称
               msg_pool3,                /* 存放消息的缓冲区 */
               sizeof(struct rx_msg),    /* 一条消息的大小 消息中只放设备号和读取文件大小 */
               sizeof(msg_pool3),        /* 存放消息的缓冲区大小 */
               RT_IPC_FLAG_FIFO);        /* 如果有多个线程等待，按照先来先得到的方法分配消息 */
    //..............................查找串口...............................
    serial_uart3 = rt_device_find("uart3");

    //................... 串口配置结构体，使用serial.h的宏定义................
    struct serial_configure gps_use_config = 
    {
        baudrate,             /* 波特率 bits/s */
        DATA_BITS_8,          /* 8 databits */
        STOP_BITS_1,          /* 1 stopbit */
        PARITY_NONE,          /* No parity  */ 
        BIT_ORDER_LSB,        /* LSB first sent */
        NRZ_NORMAL,           /* Normal mode */
        1024,                 /* 接收数据缓冲区大小 */
        0   
    };
    //..............................打开串口设备............................
    if (serial_uart3)
    {
        rt_device_open(serial_uart3, RT_DEVICE_FLAG_DMA_RX);        /* DMA模式 */
    }
    else
    {
        return;
    }
    //..............................修改串口参数配置............................
    if (RT_EOK != rt_device_control(serial_uart3, RT_DEVICE_CTRL_CONFIG,(void *)&gps_use_config))
    {
        rt_kprintf("uart3 config baud rate failed.\n");
    }

     //..............................设置设备接收回调函数............................
    rt_device_set_rx_indicate(serial_uart3,  basic_uart3_input);
    
}


/*** @ : ***************************************************************************************
 * @name: 
 * @describe: 接收数据回调函数
 * @param :   dev：设备号   size：大小
 * @return: 
 * @  : ****************************************************************************************/
static rt_err_t basic_uart3_input(rt_device_t dev, rt_size_t size)
{
    struct rx_msg msg;
    rt_err_t result;
    
    msg.dev = dev;
    msg.size = size;

    result = rt_mq_send(&rx_mq3, &msg, sizeof(msg));
    if ( result == -RT_EFULL)
    {
        //********************************************消息队列满****************************
        rt_kprintf(" uart3 message queue full！\n");
    }
    return result;
}
/********************************************************************************************
** 函数名称：     
** 函数描述：uart4的初始化
** 输入参数：无
** 返回值  ：    无
********************************************************************************************/
void basic_uart4_init(uint32_t baudrate, uint32_t re_msg_size)
{
    static uint8_t msg_pool4[256];  //消息队列中用到的放置消息的内存池

    rt_err_t ret = RT_EOK;
    //..............................初始化消息队列 ..............................
    rt_mq_init(&rx_mq4, "rx_mq4",        // 信息队列的句柄指针，和消息队列名称
               msg_pool4,                /* 存放消息的缓冲区 */
               sizeof(struct rx_msg),    /* 一条消息的大小 消息中只放设备号和读取文件大小 */
               sizeof(msg_pool4),        /* 存放消息的缓冲区大小 */
               RT_IPC_FLAG_FIFO);        /* 如果有多个线程等待，按照先来先得到的方法分配消息 */
    //..............................查找串口...............................
    serial_uart4 = rt_device_find("uart4");

    //................... 串口配置结构体，使用serial.h的宏定义................
    struct serial_configure gps_use_config = 
    {
        baudrate,             /* 波特率 bits/s */
        DATA_BITS_8,          /* 8 databits */
        STOP_BITS_1,          /* 1 stopbit */
        PARITY_NONE,          /* No parity  */ 
        BIT_ORDER_LSB,        /* LSB first sent */
        NRZ_NORMAL,           /* Normal mode */
        1024,                 /* 接收数据缓冲区大小 */
        0   
    };
    //..............................打开串口设备............................
    if (serial_uart4)
    {
        rt_device_open(serial_uart4, RT_DEVICE_FLAG_DMA_RX);        /* DMA模式 */
    }
    else
    {
        return;
    }
    //..............................修改串口参数配置............................
    if (RT_EOK != rt_device_control(serial_uart4, RT_DEVICE_CTRL_CONFIG,(void *)&gps_use_config))
    {
        rt_kprintf("uart4 config baud rate failed.\n");
    }

     //..............................设置设备接收回调函数............................
    rt_device_set_rx_indicate(serial_uart4,  basic_uart4_input);
    
}


/*** @ : ***************************************************************************************
 * @name: 
 * @describe: 接收数据回调函数
 * @param :   dev：设备号   size：大小
 * @return: 
 * @  : ****************************************************************************************/
static rt_err_t basic_uart4_input(rt_device_t dev, rt_size_t size)
{
    struct rx_msg msg;
    rt_err_t result;
    
    msg.dev = dev;
    msg.size = size;

    result = rt_mq_send(&rx_mq4, &msg, sizeof(msg));
    if ( result == -RT_EFULL)
    {
        //********************************************消息队列满****************************
        rt_kprintf(" uart4 message queue full！\n");
    }
    return result;
}

/********************************************************************************************
** 函数名称：     
** 函数描述：uart5的初始化
** 输入参数：无
** 返回值  ：    无
********************************************************************************************/
void basic_uart5_init(uint32_t baudrate, uint32_t re_msg_size)
{
    static uint8_t msg_pool5[256];  //消息队列中用到的放置消息的内存池

    rt_err_t ret = RT_EOK;
    //..............................初始化消息队列 ..............................
    rt_mq_init(&rx_mq5, "rx_mq5",        // 信息队列的句柄指针，和消息队列名称
               msg_pool5,                /* 存放消息的缓冲区 */
               sizeof(struct rx_msg),    /* 一条消息的大小 消息中只放设备号和读取文件大小 */
               sizeof(msg_pool5),        /* 存放消息的缓冲区大小 */
               RT_IPC_FLAG_FIFO);        /* 如果有多个线程等待，按照先来先得到的方法分配消息 */
    //..............................查找串口...............................
    serial_uart5 = rt_device_find("uart5");

    //................... 串口配置结构体，使用serial.h的宏定义................
    struct serial_configure gps_use_config = 
    {
        baudrate,             /* 波特率 bits/s */
        DATA_BITS_8,          /* 8 databits */
        STOP_BITS_1,          /* 1 stopbit */
        PARITY_NONE,          /* No parity  */ 
        BIT_ORDER_LSB,        /* LSB first sent */
        NRZ_NORMAL,           /* Normal mode */
        1024,                 /* 接收数据缓冲区大小 */
        0   
    };
    //..............................打开串口设备............................
    if (serial_uart5)
    {
        rt_device_open(serial_uart5, RT_DEVICE_FLAG_DMA_RX);        /* DMA模式 */
    }
    else
    {
        return;
    }
    //..............................修改串口参数配置............................
    if (RT_EOK != rt_device_control(serial_uart5, RT_DEVICE_CTRL_CONFIG,(void *)&gps_use_config))
    {
        rt_kprintf("uart5 config baud rate failed.\n");
    }

     //..............................设置设备接收回调函数............................
    rt_device_set_rx_indicate(serial_uart5,  basic_uart5_input);
    
}


/*** @ : ***************************************************************************************
 * @name: 
 * @describe: 接收数据回调函数
 * @param :   dev：设备号   size：大小
 * @return: 
 * @  : ****************************************************************************************/
static rt_err_t basic_uart5_input(rt_device_t dev, rt_size_t size)
{
    struct rx_msg msg;
    rt_err_t result;
    
    msg.dev = dev;
    msg.size = size;

    result = rt_mq_send(&rx_mq5, &msg, sizeof(msg));
    if ( result == -RT_EFULL)
    {
        //********************************************消息队列满****************************
        rt_kprintf(" uart5 message queue full！\n");
    }
    return result;
}

/********************************************************************************************
** 函数名称：     
** 函数描述：uart6的初始化
** 输入参数：无
** 返回值  ：    无
********************************************************************************************/
void basic_uart6_init(uint32_t baudrate, uint32_t re_msg_size)
{
    static uint8_t msg_pool6[256];  //消息队列中用到的放置消息的内存池

    rt_err_t ret = RT_EOK;
    //..............................初始化消息队列 ..............................
    rt_mq_init(&rx_mq6, "rx_mq6",        // 信息队列的句柄指针，和消息队列名称
               msg_pool6,                /* 存放消息的缓冲区 */
               sizeof(struct rx_msg),    /* 一条消息的大小 消息中只放设备号和读取文件大小 */
               sizeof(msg_pool6),        /* 存放消息的缓冲区大小 */
               RT_IPC_FLAG_FIFO);        /* 如果有多个线程等待，按照先来先得到的方法分配消息 */
    //..............................查找串口...............................
    serial_uart6 = rt_device_find("uart6");

    //................... 串口配置结构体，使用serial.h的宏定义................
    struct serial_configure gps_use_config = 
    {
        baudrate,             /* 波特率 bits/s */
        DATA_BITS_8,          /* 8 databits */
        STOP_BITS_1,          /* 1 stopbit */
        PARITY_NONE,          /* No parity  */ 
        BIT_ORDER_LSB,        /* LSB first sent */
        NRZ_NORMAL,           /* Normal mode */
        1024,                 /* 接收数据缓冲区大小 */
        0   
    };
    //..............................打开串口设备............................
    if (serial_uart6)
    {
        rt_device_open(serial_uart6, RT_DEVICE_FLAG_DMA_RX);        /* DMA模式 */
    }
    else
    {
        return;
    }
    //..............................修改串口参数配置............................
    if (RT_EOK != rt_device_control(serial_uart6, RT_DEVICE_CTRL_CONFIG,(void *)&gps_use_config))
    {
        rt_kprintf("uart6 config baud rate failed.\n");
    }

     //..............................设置设备接收回调函数............................
    rt_device_set_rx_indicate(serial_uart6,  basic_uart6_input);
    
}

/*** @ : ***************************************************************************************
 * @name: 
 * @describe: 接收数据回调函数
 * @param :   dev：设备号   size：大小
 * @return: 
 * @  : ****************************************************************************************/
static rt_err_t basic_uart6_input(rt_device_t dev, rt_size_t size)
{
    struct rx_msg msg;
    rt_err_t result;
    
    msg.dev = dev;
    msg.size = size;

    result = rt_mq_send(&rx_mq6, &msg, sizeof(msg));
    if ( result == -RT_EFULL)
    {
        //********************************************消息队列满****************************
        rt_kprintf(" uart6 message queue full！\n");
    }
    return result;
}

/*
 * @ : ***************************************************************************************
 * @Description: 
 * @version: 
 * @Author: liwei
 * @Date: 2019-10-30 09:21:37
 * @LastEditTime: 2019-11-04 18:27:49
 * @  : ***************************************************************************************
 */
#ifndef _BASIC_UART_H_
#define _BASIC_UART_H_
//..........................头文件.......................................

#include <rtthread.h>
#include <rtdevice.h>

struct rx_msg
{
    rt_device_t  dev;      //设备号
    rt_size_t    size;     //读取数据长度
};



extern rt_device_t serial_uart1;      //设备号  句柄
extern rt_device_t serial_uart3;
extern rt_device_t serial_uart4;
extern rt_device_t serial_uart5;
extern rt_device_t serial_uart6;

extern struct rt_messagequeue rx_mq1;        //消息队列
extern struct rt_messagequeue rx_mq3;
extern struct rt_messagequeue rx_mq4;
extern struct rt_messagequeue rx_mq5;
extern struct rt_messagequeue rx_mq6;

#endif

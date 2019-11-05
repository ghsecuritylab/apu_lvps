/*
 * 程序清单：udp 客户端
 *
 * 这是一个 udp 客户端的例程
 * 导出 udpclient 命令到控制终端
 * 命令调用格式：udpclient URL PORT [COUNT = 10]
 * URL：服务器地址  PORT：端口号  COUNT：可选参数 默认为 10
 * 程序功能：发送 COUNT 条数据到服务远端
*/

/******************************************************************************************
** 头文件
*******************************************************************************************/
#include "../../inc/thread/thread_udp_client.h"
/******************************************************************************************
** 宏定义
*******************************************************************************************/
#define SERV_PORT 		5001           	 // The port which is communicate with server
#define SERV_IPADDR     "10.0.7.101"
#define BUFSZ           1024
#define SEND_LEN        60

/********************************************************************************************
** 定义变量
********************************************************************************************/
union   Status_store_datapack    un_eth_send_data; 
/********************************************************************************************
** 函数申明
********************************************************************************************/
uint8_t send_data[SEND_LEN] = {0}; /* 发送用到的数据 */

void udpclient(void)
{
    int sockfd;
    struct sockaddr_in server_addr;
	struct hostent *host;


    /* 创建一个socket，类型是SOCK_DGRAM，UDP类型 */
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        rt_kprintf("Socket error\n");
        return;
    }
	
    /* 通过函数入口参数url获得host地址（如果是域名，会做域名解析） */
	host = (struct hostent *) gethostbyname(SERV_IPADDR);
	
	
    /* 初始化预连接的服务端地址 */

    server_addr.sin_family = AF_INET;                 // Protocol Family   IPV4
    server_addr.sin_port = htons(SERV_PORT);          // Protocol Family   IPV4
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);
    rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero)); // 结构体填充数据清0
  
	while (1)
    {
        uint8_t i = 0;
        for(i = 0; i < SEND_LEN ; i++)
        {
            send_data[i] = un_eth_send_data.u8_data[i];
        } 
        //..................................发送数据................................
        sendto(sockfd, send_data, sizeof(send_data), 0,
               (struct sockaddr *)&server_addr, sizeof(struct sockaddr)); 
		rt_thread_mdelay(500); 

        //..................................接收数据................................
        if( recvfrom(sockfd, send_data,sizeof(send_data), 0,NULL, NULL) != -1 )    	
    	{

        }

    }
  
    closesocket(sockfd);
	
}
MSH_CMD_EXPORT(udpclient, a udp client sample);

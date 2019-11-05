
/******************************************************************************************
 * 程序清单：udp 服务端
 *
 * 这是一个 udp 服务端的例程
 * 导出 udpserv 命令到控制终端
 * 命令调用格式：udpserv
 * 无参数
 * 程序功能：作为一个服务端，接收并显示客户端发来的数据 ，接收到 exit 退出程序
*******************************************************************************************/

/******************************************************************************************
** 头文件
*******************************************************************************************/
#include "../../inc/thread/thread_udp_server.h"

/******************************************************************************************
** 宏定义
*******************************************************************************************/
#define SERV_PORT 		5001           	 // The port which is communicate with server
#define SERV_IPADDR     "10.0.7.101"     // 没用上
#define CLIENT_IPADDR   "10.0.7.106"     // 接收客户端的IP
#define BUFSZ           1024
#define SEND_LEN        60

/********************************************************************************************
** 定义变量
********************************************************************************************/
//union   Status_store_datapack    un_eth_send_data;       //实时工作状态数据，ETH发送的就是该数据


/********************************************************************************************
** 函数申明
********************************************************************************************/
static void udpserv(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in server_addr; //服务器的（本地的）IP和端口信息结构体
    struct sockaddr_in client_addr;  //客户端的（远程的）IP和端口信息结构体
    uint8_t *rev_data;			// 接收的缓冲区   
    socklen_t addr_len; 
    int bytes_read;                 //每一次实际接收的字节总数

    /* 0. 分配接收用的数据缓冲.*/
    rev_data = rt_malloc(BUFSZ);
    if (rev_data == RT_NULL)
    {   
        rt_kprintf("No memory\n");// 分配内存失败，返回
        return;
    }
	
    /*1. 创建一个socket，AF_INET表示ipv4 SOCK_DGRAM表示UDP类型 */
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        rt_kprintf("Socket error\n");
        rt_free(rev_data);                  // 释放接收用的数据缓冲
        return;
    }
    /* 允许绑定地址快速重用 */
    int b_reuse = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &b_reuse, sizeof (int));
	
	/*2. 绑定 */
    /*2.1 初始化服务端地址 */
    server_addr.sin_family = AF_INET;                 // Protocol Family   IPV4
    server_addr.sin_port = htons(SERV_PORT);          // Protocol Family   IPV4
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);  // 让服务器程序能绑定在任意的IP上
    rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero)); // 结构体填充数据清0
  
	/*2.2 绑定socket到服务端地址 */
    if (bind(sockfd, (struct sockaddr *)&server_addr,sizeof(struct sockaddr)) == -1)
    {    
        rt_kprintf("Bind error\n"); // 绑定地址失败  
        rt_free(rev_data);         // 释放接收用的数据缓冲
        return;
    }
    addr_len = sizeof(struct sockaddr);
    rt_kprintf("UDPServer Waiting for client on port 5000...\n");
	

    while (1)
    {
        // uint8_t i = 0;
        
        /* 从sock中收取最大BUFSZ - 1字节数据 */
		/* UDP不同于TCP，它基本不会出现收取的数据失败的情况，除非设置了超时等待 */
        //....client_addr没有赋值，是表明接收所有客户端发来的数据，client_addr的值由接收到的对方的地址而定.....
        bytes_read = recvfrom(sockfd, rev_data, BUFSZ - 1, 0,(struct sockaddr *)&client_addr, &addr_len);     	
		rev_data[bytes_read] = '\0';     // 把末端清零 
      
		/* 输出接收的数据 */
		// rt_kprintf("\n(%s , %d) said : ", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        // rt_kprintf("%s", rev_data);
        //接收到的ip地址正确则接收数据
//        if(client_addr.sin_addr == CLIENT_IPADDR)
//        {
//          

//        }
       
       //....................................发送数据..........................................
    //    rt_thread_mdelay(500);
        
        // for(i = 0; i < SEND_LEN ; i++)
        // {
        //     send_data[i] = un_eth_send_data[i];
        // } 

	//    sendto(sockfd, rev_data, SEND_LEN, 0, (struct sockaddr *)&client_addr, sizeof(client_addr)); 

    }
    
    closesocket(sockfd);   
    rt_free(rev_data); 

	return;
}
MSH_CMD_EXPORT(udpserv, a udp server sample);





























 /*
 * Change Logs:
 * Date           Author       Notes
 * 2019-9-11      guolz     first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <gui.h>

static rt_uint8_t gui_thread_stack[RT_GUI_THREAD_STACK_SZ];

static struct rt_messagequeue gui_mq;
static rt_uint8_t gui_mq_pool[(GUI_MQ_MSG_SZ+sizeof(void*))*GUI_MQ_MAX_MSG];;
                    //0123456789.ACEF
const rt_uint8_t tab[] = {0xee,0x28,0xcd,0x6d,0x2b,0x67,0xe7,0x2c,0xef,0x6f,0x10,0xaf,0xc6,0xc7,0x87};
const char pic_index[] = {'0','1','2','3','4','5','6','7','8','9','.','A','C','E','F'};

/*********************************************************************************************************
** 函数名称：led_gui_entry
** 函数描述：数码管的显示线程入口
** 输入参数：数码管要显示的内容
** 返回值  ：无
*********************************************************************************************************/
static void led_gui_entry(void *param)
{
    int8_t num,c,byte;
    rt_device_t dev = RT_NULL;
    char buf[] = "hello zhizhuo\r\n";

    dev = rt_device_find("vcom");     //用于查找当前设备是否在系统设备注册表里,如果是返回设备指针，否则返回NULL
   
    if (dev)
        rt_device_open(dev, RT_DEVICE_FLAG_RDWR);    /* 可读写模式 */
    else
        return ;
	
    while (RT_TRUE)
    {
        struct hal_message msg;
        /* receive message  从消息队列中接收消息 */
        if(rt_mq_recv(&gui_mq, &msg, sizeof(struct hal_message),
                    RT_WAITING_FOREVER) != RT_EOK )
            continue;

        char *pic_show = (char *)msg.content;
        byte = -1;
        for(c = 0; c < 15; c++)
		{
            if(pic_index[c] == *pic_show)
			{
                byte = c;
                break;
            }
        }
		
        if(byte >= 0)
		{
            num = tab[byte];
            for(c = 0; c < 8; c++)
			{
                rt_pin_write(DigitLedclk, PIN_LOW);
                if((num&0x01) == 1)
				{
                    rt_pin_write(DigitLedData, PIN_HIGH);
                }
				else
				{
                    rt_pin_write(DigitLedData, PIN_LOW);
                }

                rt_pin_write(DigitLedclk, PIN_HIGH);
                num>>=1;
            }
        }

        rt_device_write(dev, 0, buf, rt_strlen(buf));

        rt_thread_mdelay(100);
    }
 
}
/*********************************************************************************************************
** 函数名称：g_Gui_show_pic
** 函数描述：数码管的显示
** 输入参数：数码管要显示的内容
** 返回值  ：无
*********************************************************************************************************/
void g_Gui_show_pic(const char *pic)
{   
    struct hal_message gui_msg;
    gui_msg.what = 0x01;             //消息类型
    gui_msg.content = (void *)pic;   //消息内容
    gui_msg.freecb = NULL;

    rt_mq_send(&gui_mq, (void*)&gui_msg, sizeof(struct hal_message)); /* 发送消息到消息队列中 */
}
/*********************************************************************************************************
** 函数名称：g_Gui_init
** 函数描述：LED数码管初始化
** 输入参数：无
** 返回值  ：系统返回值
*********************************************************************************************************/

rt_err_t g_Gui_init(void)
{
    static struct rt_thread gui_thread;
  
	rt_pin_mode(DigitLedData, PIN_MODE_OUTPUT);   // 数码管的引脚定义
    rt_pin_mode(DigitLedclk, PIN_MODE_OUTPUT);
    
    rt_mq_init(&gui_mq,
            "gui_mq",
            gui_mq_pool, GUI_MQ_MSG_SZ,
            sizeof(gui_mq_pool),
            RT_IPC_FLAG_FIFO);

    rt_thread_init(&gui_thread,             // gui_thread 线程 初始化
                   "gui",
                   led_gui_entry, RT_NULL,
                   gui_thread_stack, RT_GUI_THREAD_STACK_SZ,
                   RT_GUI_THREAD_PRIO, 20);

			
    return rt_thread_startup(&gui_thread);  // 启动 gui_thread ，线程的函数入口名为初始化中的led_gui_entry
}
INIT_COMPONENT_EXPORT(g_Gui_init);	


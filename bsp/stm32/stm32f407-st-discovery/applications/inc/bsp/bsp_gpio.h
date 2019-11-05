#ifndef _BSP_GPIO_H_
#define _BSP_GPIO_H_

//..........................头文件.......................................
#include <rtthread.h>

#include <drv_gpio.h>

/*********************************************************************************************************
  宏定义
*********************************************************************************************************/
//..............................................输入IO脚宏定义.................................
#define Lvps1_IGBT_FEED_PIN            GET_PIN(E, 11)     //Lvps1原边过流反馈
#define Lvps1_TEMP_FEED_PIN            GET_PIN(F, 15)     //Lvps1过温反馈

#define Lvps2_IGBT_FEED_PIN            GET_PIN(F, 13)     //Lvps2原边过流反馈
#define Lvps2_TEMP_FEED_PIN            GET_PIN(F, 11)     //Lvps1过温反馈

#define Sinv_TEMP_FEED_PIN             GET_PIN(D, 13)     //单逆过温反馈
#define Sinv_NB_FEED_PIN               GET_PIN(D, 10)     //单逆逆变故障反馈
#define Sinv_IGBT_FEED_PIN             GET_PIN(E, 14)     //单逆原边过流反馈

#define Thinv_NB_FEED_PIN              GET_PIN(E, 13)     //3相逆变故障反馈
#define PRE_CH_FEED_PIN                GET_PIN(E, 12)     //预充电完成信号反馈
#define RESERV9_FEED_PIN               GET_PIN(G, 2)      //预留信号反馈

#define Lvps1_POLARITY_FEED_PIN        GET_PIN(G, 15)     //Lvps1输出极性检测
#define Lvps2_POLARITY_FEED_PIN        GET_PIN(E, 1)      //Lvps2输出极性检测

#define Lvps1_CON3_FEED_PIN            GET_PIN(G, 12)     //Lvps1输出极性检测
#define Lvps1_LEAKP_FEED_PIN           GET_PIN(G, 10)     //Lvps1正漏电
#define Lvps1_LEAKN_FEED_PIN           GET_PIN(E, 0)      //Lvps1负漏电

#define Lvps2_CON4_FEED_PIN            GET_PIN(E, 2)      //Lvps2输出极性检测
#define Lvps2_LEAKP_FEED_PIN           GET_PIN(C, 13)     //Lvps2正漏电
#define Lvps2_LEAKN_FEED_PIN           GET_PIN(E, 3)      //Lvps2负漏电

#define CARID_A_FEED_PIN               GET_PIN(C, 6)      //车号A设置
#define CARID_B_FEED_PIN               GET_PIN(C, 8)      //车号B设置
#define CARID_C_FEED_PIN               GET_PIN(C, 3)      //车号C设置
#define CARID_D_FEED_PIN               GET_PIN(B, 0)      //车号D设置

#define EX_Sinv_RUN_FEED_PIN           GET_PIN(B, 1)      //单逆外部启动信号
#define RADIATOR_TEMP_FEED_PIN         GET_PIN(F, 12)     //APU散热器过温反馈
#define ASSIST_POWER_FEED_PIN          GET_PIN(F, 14)     //辅助电源故障反馈
#define RESERV25_FEED_PIN              GET_PIN(A, 0)      //预留信号反馈


//.............................................输出I/O脚宏定义..................................
#define Lvps1_START_PIN                GET_PIN(E, 5)     //Lvps1启动信号
#define Lvps1_OVERLOAD_PIN             GET_PIN(E, 4)     //Lvps1过载

#define Lvps2_START_PIN                GET_PIN(F, 0)     //Lvps2启动信号
#define Lvps2_OVERLOAD_PIN             GET_PIN(D, 2)     //Lvps2过载

#define Sinv_START_PIN                 GET_PIN(G, 7)     //Sinv启动信号
#define Sinv_OVERLOAD_PIN              GET_PIN(G, 6)     //Sinv过载

#define Thinv_START_PIN                GET_PIN(G, 5)     //Thinv启动信号

#define Lvps1_CON3_A2_PIN              GET_PIN(B, 5)     //Lvps1 CON3控制引脚
#define Lvps2_CON4_A2_PIN              GET_PIN(B, 4)     //Lvps2 CON4控制引脚
#define RESERV_OUT4_PIN                GET_PIN(C, 2)     //预留1
#define FJ_CONTROL_PIN                 GET_PIN(F, 2)     //风机控制引脚
#define Sinv_CON5_A2_PIN               GET_PIN(C, 12)    //Sinv CON5控制引脚
#define RESERV_OUT2_PIN                GET_PIN(E, 6)    //预留2


//.............................................输入反馈值宏定义...............................
#define Lvps1_IGBT_FEED_VALUE            rt_pin_read(Lvps1_IGBT_FEED_PIN)     //Lvps1原边过流反馈
#define Lvps1_TEMP_FEED_VALUE            rt_pin_read(Lvps1_TEMP_FEED_PIN)     //Lvps1过温反馈

#define Lvps2_IGBT_FEED_VALUE            rt_pin_read(Lvps2_IGBT_FEED_PIN)     //Lvps2原边过流反馈
#define Lvps2_TEMP_FEED_VALUE            rt_pin_read(Lvps2_TEMP_FEED_PIN)     //Lvps1过温反馈

#define Sinv_TEMP_FEED_VALUE             rt_pin_read(Sinv_TEMP_FEED_PIN)     //单逆过温反馈
#define Sinv_NB_FEED_VALUE               rt_pin_read(Sinv_NB_FEED_PIN)        //单逆逆变故障反馈
#define Sinv_IGBT_FEED_VALUE             rt_pin_read(Sinv_IGBT_FEED_PIN)      //单逆原边过流反馈

#define Thinv_NB_FEED_VALUE              rt_pin_read(Thinv_NB_FEED_PIN)     //3相逆变故障反馈
#define PRE_CH_FEED_VALUE                rt_pin_read(PRE_CH_FEED_PIN)       //预充电完成信号反馈
#define RESAVE9_FEED_VALUE               rt_pin_read(RESERV9_FEED_PIN)      //预留信号反馈

#define Lvps1_POLARITY_FEED_VALUE        rt_pin_read(Lvps1_POLARITY_FEED_PIN)     //Lvps1输出极性检测
#define Lvps2_POLARITY_FEED_VALUE        rt_pin_read(Lvps2_POLARITY_FEED_PIN)     //Lvps2输出极性检测

#define Lvps1_CON3_FEED_VALUE            rt_pin_read(Lvps1_CON3_FEED_PIN)       //Lvps1输出极性检测
#define Lvps1_LEAKP_FEED_VALUE           rt_pin_read(Lvps1_LEAKP_FEED_PIN)      //Lvps1正漏电
#define Lvps1_LEAKN_FEED_VALUE           rt_pin_read(Lvps1_LEAKN_FEED_PIN)      //Lvps1负漏电

#define Lvps2_CON4_FEED_VALUE            rt_pin_read(Lvps2_CON4_FEED_PIN)      //Lvps2输出极性检测
#define Lvps2_LEAKP_FEED_VALUE           rt_pin_read(Lvps2_LEAKP_FEED_PIN)     //Lvps2正漏电
#define Lvps2_LEAKN_FEED_VALUE           rt_pin_read(Lvps2_LEAKN_FEED_PIN)     //Lvps2负漏电

#define CARID_A_FEED_VALUE               rt_pin_read(CARID_A_FEED_PIN)      //车号A设置
#define CARID_B_FEED_VALUE               rt_pin_read(CARID_B_FEED_PIN)      //车号B设置
#define CARID_C_FEED_VALUE               rt_pin_read(CARID_C_FEED_PIN)      //车号C设置
#define CARID_D_FEED_VALUE               rt_pin_read(CARID_D_FEED_PIN)      //车号D设置

#define EX_Sinv_RUN_FEED_VALUE           rt_pin_read(EX_Sinv_RUN_FEED_PIN)    //单逆外部启动信号
#define RADIATOR_TEMP_FEED_VALUE         rt_pin_read(RADIATOR_TEMP_FEED_PIN)  //APU散热器过温反馈
#define ASSIST_POWER_FEED_VALUE          rt_pin_read(ASSIST_POWER_FEED_PIN)   //辅助电源故障反馈
#define RESERV25_FEED_VALUE              rt_pin_read(RESERV25_FEED_PIN)       //预留信号反馈

//...........................................输出引脚置位.................................
#define Lvps1_START_ON                 rt_pin_write(Lvps1_START_PIN, PIN_HIGH);     //Lvps1启动信号
#define Lvps1_START_OFF                rt_pin_write(Lvps1_START_PIN, PIN_LOW);      

#define Lvps1_OVERLOAD_ON              rt_pin_write(Lvps1_OVERLOAD_PIN, PIN_HIGH);  //Lvps1过载
#define Lvps1_OVERLOAD_OFF             rt_pin_write(Lvps1_OVERLOAD_PIN, PIN_LOW);     

#define Lvps2_START_ON                 rt_pin_write(Lvps2_START_PIN, PIN_HIGH);     //Lvps2启动信号
#define Lvps2_START_OFF                rt_pin_write(Lvps2_START_PIN, PIN_LOW);

#define Lvps2_OVERLOAD_ON              rt_pin_write(Lvps2_OVERLOAD_PIN, PIN_HIGH);  //Lvps2过载
#define Lvps2_OVERLOAD_OFF             rt_pin_write(Lvps2_OVERLOAD_PIN, PIN_LOW);

#define Sinv_START_ON                  rt_pin_write(Sinv_START_PIN, PIN_HIGH);      //Sinv启动信号
#define Sinv_START_OFF                 rt_pin_write(Sinv_START_PIN, PIN_LOW);

#define Sinv_OVERLOAD_ON               rt_pin_write(Sinv_OVERLOAD_PIN, PIN_HIGH);   //Sinv过载
#define Sinv_OVERLOAD_OFF              rt_pin_write(Sinv_OVERLOAD_PIN, PIN_LOW);

#define Thinv_START_ON                 rt_pin_write(Thinv_START_PIN, PIN_HIGH);     //Thinv启动信号
#define Thinv_START_OFF                rt_pin_write(Thinv_START_PIN, PIN_LOW);

#define Lvps1_CON3_A2_ON               rt_pin_write(Lvps1_CON3_A2_PIN, PIN_HIGH);   //Lvps1 CON3控制引脚
#define Lvps1_CON3_A2_OFF              rt_pin_write(Lvps1_CON3_A2_PIN, PIN_LOW);

#define Lvps2_CON4_A2_ON               rt_pin_write(Lvps2_CON4_A2_PIN, PIN_HIGH);   //Lvps2 CON4控制引脚
#define Lvps2_CON4_A2_OFF              rt_pin_write(Lvps2_CON4_A2_PIN, PIN_LOW);

#define RESERV_OUT4_ON                 rt_pin_write(RESERV_OUT4_PIN, PIN_HIGH);     //预留1
#define RESERV_OUT4_OFF                rt_pin_write(RESERV_OUT4_PIN, PIN_LOW);

#define FJ_CONTROL_ON                  rt_pin_write(FJ_CONTROL_PIN, PIN_HIGH);      //风机控制引脚
#define FJ_CONTROL_OFF                 rt_pin_write(FJ_CONTROL_PIN, PIN_LOW);

#define Sinv_CON5_A2_ON                rt_pin_write(Sinv_CON5_A2_PIN, PIN_HIGH);    //Sinv CON5控制引脚
#define Sinv_CON5_A2_OFF               rt_pin_write(Sinv_CON5_A2_PIN, PIN_LOW);

#define RESERV_OUT2_ON                 rt_pin_write(RESERV_OUT2_PIN, PIN_HIGH);  //预留2
#define RESERV_OUT2_OFF                rt_pin_write(RESERV_OUT2_PIN, PIN_LOW);

#endif






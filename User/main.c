/*
*********************************************************************************************************
*
*	模块名称 : 主程序模块
*	文件名称 : main.c
*	版    本 : V1.0
*	说    明 : V6开发板综合Demo之RTX实时操作系统及其所有中间件
*              使用前的注意事项，视频演示和各个应用的截图说明看此贴（重要！！）：
*                http://bbs.armfly.com/read.php?tid=17330
*              版本信息：
*                RTOS:         RL-RTX      V4.74
*                File System:  RL-FlashFS  V4.74
*                TCP/IP:       RL-TCPnet   V4.74
*                USB:          RL-USB      V4.74
*                CAN:          RL-CAN      V4.74
*                GUI:          emWin       V5.36
*                Modbus:       Designed by armfly
*              任务设置：
*                总共创建了15个任务
*                1. os_idle_demon，   优先级0: 系统空闲任务。
*                2. AppTaskStatistic  优先级1：CPU利用率检测。
*                3. AppTaskGUI，      优先级3：GUI主任务。
*               // 4. AppTaskTCPMain，  优先级4：TCPnet网络任务。
*                5. AppTaskUserIF，   优先级5：按键消息处理任务。
*                6. AppTaskMsgPro，   优先级6：主要用于截图任务。
*                7. AppTaskStart，    优先级7：启动任务。
*               // 8. AppTaskCAN，      优先级8：CAN总线任务。
*                9. AppTaskModbus，   优先级9：Modbus任务。
*              // 10. AppTaskUSB，      优先级10：USB任务，
*               11. AppTaskMusic，    优先级11：音乐任务，用于音乐播放器，视频播放器，录音机和收音机处理。
*               12. AppTaskPinDetect，优先级12：SD卡插拔检测任务。
*              // 13. AppTaskTCPTimer， 优先级13：TCPnet网络定时更新任务。
*                   USB在运行的过程中会创建2两个任务
*               14. USBD_RTX_EndPoint0          优先级2：USB设备端点0任务。
*               15. USBD_RTX_MSC_EP_BULK_Event  优先级2：USB BULK数据传输任务。
*             中断优先级：
*               NVIC优先级分组采用4，仅抢占优先级，范围0-15。
*               LTDC中断，       抢占优先级 0
*               ETH以太网中断，  抢占优先级 0
*               USB FS中断，     抢占优先级 0
*               CAN2 TX中断，    抢占优先级 1
*               CAN2 RX0中断，   抢占优先级 1
*               DMA2_Steam5中断，抢占优先级 1
*               DMA2_Steam1中断，抢占优先级 1
*               EXTI6中断，      抢占优先级 1
*               USART3中断，     抢占优先级 2
*               EXTI2中断，      抢占优先级 2
*               TIM2中断，       抢占优先级 4
*               USART1中断，     抢占优先级 13
*               RTC Alarm中断，  抢占优先级 13
*               SVC中断，        抢占优先级 14
*               Systick中断，    抢占优先级 15
*               PendSV中断，     抢占优先级 15
*              内存分配：
*               内部SRAM ,         CCM SRAM和SDRAM安排。
*               内部192KB SRAM   ：80KB用于动态内存分配，其余用于任务堆栈，系统堆栈，文件系统，网络协议栈等。
*               内部64KB CCM SRAM：用于视频播放器时，JPEG解码。
*               外部16MB SDRAM   ：前4MB用于TFT显存。
*               外部16MB SDRAM   ：后12MB用于emWin动态内存。
*              注意事项：
*                1. 每个MDK的安装目录里面都会有一个RTX源码，对于MDK4.XX来说，大家使用的
*                   那个MDK版本，务必使用那个MDK版本下面的RTX，这样使用MDK自带的RTX调试组件
*                   时，才能显示正确的调试信息。
*                2. 当前使用的RTX源码是MDK4.74里面的，KEIL官方已经放弃MDK4系列的更新了，
*                   这个版本号是MDK4系列里面最新版本了。如果需要使用MDK自带的RTX调试组件显示
*                   信息，请务必使用MDK4.74.
*                3. 对于MDK5.XX，RTX也在其安装目录里面，但是RTX已经不作为单独版本发布了，
*                   它有一个全新的名字叫CMSIS-RTOS RTX。ARM官方在RTX的基础上给RTX又做了一层封装。
*                4. 本实验推荐使用串口软件SecureCRT，要不串口打印效果不整齐。此软件在
*                   V6开发板光盘里面有。
*                5. 务必将编辑器的缩进参数和TAB设置为4来阅读本文件，要不代码显示不整齐。
*
*	修改记录 :
*		版本号    日期         作者        说明
*		V1.0    2017-03-20   Eric2013      首发
*
*
*	Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/	
#include "includes.h"	
#include "MainTask.h"




/*
*********************************************************************************************************
*	                                  用于本文件的调试
*********************************************************************************************************
*/
#if 0
	#define printf_taskdbg printf
#else
	#define printf_taskdbg(...)
#endif


/*
**********************************************************************************************************
											函数声明
**********************************************************************************************************
*/
//在RTX中新建一个任务就是__task，区别于其他的操作系统
__task void AppTaskStatistic (void);
__task void AppTaskGUI(void);
__task void AppTaskUserIF(void);
__task void AppTaskMsgPro(void);
__task void AppTaskStart(void);
__task void AppTaskModbus(void);
__task void AppTaskMusic(void);
__task void AppTaskPinDetect(void);
static void AppTaskCreate (void);
static void AppObjCreate (void);


/*
**********************************************************************************************************
											 变量
**********************************************************************************************************
*/
static uint64_t AppTaskStatStk[1024/8];    	  /* 任务栈 */
static uint64_t AppTaskGUIStk[4096/8];     	  /* 任务栈 */
static uint64_t AppTaskUserIFStk[1024/8];     /* 任务栈 */
static uint64_t AppTaskMsgProStk[1024/8];  	  /* 任务栈 */
static uint64_t AppTaskStartStk[1024/8];   	  /* 任务栈 */
static uint64_t AppTaskModbusStk[1024/8];     /* 任务栈 */
static uint64_t AppTaskMusicStk[4096/8];      /* 任务栈 */
static uint64_t AppTaskPinDetectStk[1024/8];  /* 任务栈 */



/* 任务句柄 */
OS_TID HandleTaskStat = NULL;
OS_TID HandleTaskGUI = NULL;
OS_TID HandleTaskTCPMain = NULL;
OS_TID HandleTaskUserIF = NULL;
OS_TID HandleTaskMsgPro = NULL;
OS_TID HandleTaskStart = NULL;
OS_TID HandleTaskModbus = NULL;
OS_TID HandleMusicStart = NULL;
OS_TID HandlePinDetectStart = NULL;


/*
**********************************************************************************************************
											 变量
**********************************************************************************************************
*/
static OS_SEM semaphore;
volatile int16_t  iTemp;        

/* 统计任务使用 */
__IO uint8_t   OSStatRdy;        /* 统计任务就绪标志 */
__IO uint32_t  OSIdleCtr;        /* 空闲任务计数 */
__IO float     OSCPUUsage;       /* CPU百分比 */
uint32_t       OSIdleCtrMax;     /* 1秒内最大的空闲计数 */
uint32_t       OSIdleCtrRun;     /* 1秒内空闲任务当前计数 */

os_mbx_declare (mailbox, 5);     /* 声明一个支持5个消息的消息邮箱 */

/* 80KB的内部SRAM和64KB的CCM RAM，通过函数os_init_mem初始化为两组动态内存池 */
uint64_t AppMalloc[80*1024/8];         
uint64_t AppMallocCCM[64*1024/8] __attribute__((at(0x10000000))); /* JPEG软解 */

/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: 标准c程序入口。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
int main (void) 
{	
	/* 初始化外设 */
	bsp_Init();
	
	/* 创建启动任务 */
 	os_sys_init_user (AppTaskStart,              /* 任务函数 */
	                  7,                         /* 任务优先级 */
	                  &AppTaskStartStk,          /* 任务栈 */
	                  sizeof(AppTaskStartStk));  /* 任务栈大小，单位字节数 */
	while(1);
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskStatistic
*	功能说明: 统计任务，用于实现CPU利用率的统计。为了测试更加准确，可以开启注释调用的全局中断开关
*	形    参: 无
*	返 回 值: 无
*   优 先 级: 1 (数值越小优先级越低，这个跟uCOS相反)
*********************************************************************************************************
*/
void  OSStatInit (void)
{
	OSStatRdy = FALSE;
	
    os_dly_wait(2u);            /* 时钟同步 */
    OSIdleCtr    = 0uL;         /* 清空闲计数 */	
    os_dly_wait(100);           /* 统计100ms内，最大空闲计数 */
    OSIdleCtrMax = OSIdleCtr;   /* 保存最大空闲计数 */
    OSStatRdy    = TRUE;
}

__task void  AppTaskStatistic (void)
{
    while (OSStatRdy == FALSE) 
	{
        os_dly_wait(200);     /* 等待统计任务就绪 */
    }

    OSIdleCtrMax /= 100uL;
    if (OSIdleCtrMax == 0uL) 
	{
        OSCPUUsage = 0u;
    }
    OSIdleCtr = OSIdleCtrMax * 100uL;  /* 设置初始CPU利用率 0% */	
    for (;;) 
	{
        OSIdleCtrRun = OSIdleCtr;    /* 获得100ms内空闲计数 */
        OSIdleCtr    = 0uL;          /* 复位空闲计数 */
        OSCPUUsage   = (100uL - (float)OSIdleCtrRun / OSIdleCtrMax);
        os_dly_wait(100);            /* 每100ms统计一次 */
    }
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskGUI
*	功能说明: GUI任务。
*	形    参: 无
*	返 回 值: 无
*   优 先 级: 3 (数值越小优先级越低，这个跟uCOS相反)
*********************************************************************************************************
*/
__task void AppTaskGUI(void)
{
    while(1)
    {
		MainTask();
    }
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskUserIF
*	功能说明: 按键消息处理		
*	形    参: 无
*	返 回 值: 无
*   优 先 级: 5
*********************************************************************************************************
*/
__task void AppTaskUserIF(void)
{
	uint8_t ucKeyCode;

    while(1)
    {
		ucKeyCode = bsp_GetKey();
		
		if (ucKeyCode != KEY_NONE)
		{
			switch (ucKeyCode)
			{
				/* K1键按下，打印调试说明 */
				case KEY_DOWN_K1:
					printf("K1键按下，使用MDK中自带的RTX调试组件，请务必使用MDK4.74版本进行调试\r\n");
					break;	
				
				/* K2键按下，直接发送信号量同步信号给任务AppTaskMsgPro */
				case KEY_DOWN_K2:
					printf("K2键按下，直接发送信号量同步信号给任务AppTaskMsgPro\r\n");
					os_sem_send (&semaphore);
					break;

				/* 其他的键值不处理 */
				default:                     
					break;
			}
		}		
		os_dly_wait(20);
	}
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskMsgPro
*	功能说明: 消息处理，等待任务AppTaskUserIF发来的事件标志，实现截图功能，将BMP图片存储到SD卡
*	形    参: 无
*	返 回 值: 无
*   优 先 级: 6 
*********************************************************************************************************
*/
__task void AppTaskMsgPro(void)
{
	OS_RESULT xResult;
	const uint16_t usMaxBlockTime = 0xFFFF;
	uint8_t	Pic_Name = 0;
	char buf[20];
	
    while(1)
    {
		xResult = os_sem_wait (&semaphore, usMaxBlockTime);
		
		switch (xResult)
		{
			/* 无需等待接受到信号量同步信号 */
			case OS_R_OK:

			/* 信号量不可用，usMaxBlockTime等待时间内收到信号量同步信号 */
			case OS_R_SEM:
				sprintf((char *)buf,"M0:\\PicSave\\%d.bmp",Pic_Name);
			
				foutbmp = fopen (buf, "w"); 
				if (foutbmp != NULL) 
				{
					/* 向SD卡绘制BMP图片 */
					GUI_BMP_Serialize(_WriteByte2File, foutbmp);
					
					/* 关闭文件 */
					fclose(foutbmp);
				}
				printf_taskdbg("截图完成\r\n");			
				Pic_Name++; 	
				break;

			/* 超时 */
			case OS_R_TMO:
				break;
			
			/* 其他值不处理 */
			default:                     
				break;
		}	
    }
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskStart
*	功能说明: 启动任务，用于按键，蜂鸣器和触摸的检测。
*	形    参: 无
*	返 回 值: 无
*   优 先 级: 7  
*********************************************************************************************************
*/
__task void AppTaskStart(void)
{	
	/* 优先创建统计任务-----------------*/
	HandleTaskStat = os_tsk_create_user(AppTaskStatistic,        /* 任务函数 */ 
	                                    1,                       /* 任务优先级 */ 
					                    &AppTaskStatStk,         /* 任务栈 */
					                    sizeof(AppTaskStatStk)); /* 任务栈大小，单位字节数 */
	
	OSStatInit();	
	/* 先创建任务间通信机制，防止任务先创建后，直接就可以使用 */
	AppObjCreate();	
	/* 创建任务 */
	AppTaskCreate();	
    while(1)
    {		
		/* 10ms一次触摸扫描，电容触摸屏GT811 */
		if(g_GT811.Enable == 1)
		{
			bsp_KeyScan();  //扫描按键是否被按下
			GT811_OnePiontScan();
			BEEP_Pro();
			os_dly_wait(10);			
		}
    }
}


/*
*********************************************************************************************************
*	函 数 名: AppTaskModbus
*	功能说明: RS485 Modbus总线处理，这里实现的是Modbus主站
*	形    参: 无
*	返 回 值: 无
*   优 先 级: 9  
*********************************************************************************************************
*/
__task void AppTaskModbus(void)
{
	OS_RESULT xResult;
	uint8_t ret = 0;
 
	
    while(1) 
    {
		if(os_evt_wait_or(0x01FF, 0xFFFF) == OS_R_EVT)
        {
            xResult = os_evt_get ();
            switch (xResult)
            {	
				/* O1H寄存器 */
				case 0x0001:
					ret = MODH_ReadParam_01H(REG_D01, 4);
					break;
				
				/* 02H寄存器 */
				case 0x0002:
					ret = MODH_ReadParam_02H(REG_T01, 3);
					break;
				
				/* 03H寄存器 */
				case 0x0004:
					ret = MODH_ReadParam_03H(REG_P01, 2);
					break;
				
				/* 04H寄存器 */
				case 0x0008:
					ret = MODH_ReadParam_04H(REG_A01, 1);
				break;
				
				/* 05H寄存器 */
				case 0x0010:
					ret = MODH_WriteParam_05H(REG_D01, 1);
					break;
				
				/* 05H寄存器 */
				case 0x0020:
					ret = MODH_WriteParam_05H(REG_D01, 0);
					break;
				
				/* 06H寄存器 */
				case 0x0040:
					ret = MODH_WriteParam_06H(REG_P01, 1);
					break;
				
				/* 06H寄存器 */
				case 0x0080:
					ret = MODH_WriteParam_06H(REG_P01, 0);
					break;
				
				/* 10H寄存器 */
				case 0x0100:
					{
						uint8_t buf[4];						
						buf[0] = 0x01;
						buf[1] = 0x02;
						buf[2] = 0x03;
						buf[3] = 0x04;
						ret = MODH_WriteParam_10H(REG_P01, 2, buf);
					}
					break;
				
				default:
					break;
			}
			
			if(ret == 1)
			{
				/* 向hWinModbus窗口发消息，寄存器命令成功 */
				if(hWinModbus != WM_HWIN_NULL)
				{
					WM_SendMessageNoPara(hWinModbus, MSG_ModbusSuccess);	
				}
			}
			else 
			{
				/* 向hWinModbus窗口发消息，寄存器命令失败 */
				if(hWinModbus != WM_HWIN_NULL)
				{
					WM_SendMessageNoPara(hWinModbus, MSG_ModbusErr);
				}
			}
		}			
    }
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskMusic
*	功能说明: 音乐播放器，视频播放器，录音机，收音机都是通过这个任务实现。
*	形    参: 无
*	返 回 值: 无
*   优 先 级: 11
*********************************************************************************************************
*/
__task void AppTaskMusic(void)
{
	MusicMsg_T *pMsg;
	OS_RESULT xResult;
	const uint16_t usMaxBlockTime = 0xFFFF; /* 延迟周期 */
	
    while(1)
    {
		xResult = os_mbx_wait(&mailbox, (void *)&pMsg, usMaxBlockTime);
		switch (xResult)
		{
			/* 无需等待接受到消息邮箱数据 */
			case OS_R_MBX:
			case OS_R_OK:
				/* DMA禁止后要等到当前转换完毕才可以停止，所以这里需要清除相应标志 */
				os_evt_clr(MusicTaskWaitFlag, HandleMusicStart);
			
				if(pMsg->ucType == MusicType_WAV)
				{
					/* WAV音频播放 */
					WavMusicPlay((const char *)pMsg->ucName);					
				}
				else if(pMsg->ucType == MusicType_MP3)
				{
					/* MP3音频播放 */
					MP3MusicPlay((const char *)pMsg->ucName);		
				}
				else if(pMsg->ucType == MusicType_REC)
				{
					/* 录音机 */
					WavRecPlay((const char *)pMsg->ucName);
				}	
				else if(pMsg->ucType == MusicType_Radio)
				{
					/* 收音机 */
					RadioPlay();				
				}
				break;	
			
			/* 其他值不处理 */
			default:                     
				break;
		}	
    }
}
/*
*********************************************************************************************************
*	函 数 名: AppTaskPinDetect
*	功能说明: 主要实现SD卡插拔检测
*	形    参: 无
*	返 回 值: 无
*   优 先 级: 12 
*********************************************************************************************************
*/
__task void AppTaskPinDetect(void)
{
	OS_RESULT xResult;
	const uint16_t usMaxBlockTime = 0xFFFF; /* 设置无限等待 */
	
	/* 无限等待，直到GUI的状态栏对话框已经创建 */
	if(os_evt_wait_or(GUIDetectBIT_15, usMaxBlockTime) == OS_R_EVT){}
	
	/* 
		检测上电前SD卡是否已经插入，这种情况是无法通过中断引脚检测出来，
	    因为没有上升沿或者下降沿的跳变。SD插入是低电平，拔出是高电平。
		PE2
	*/
	if(!(GPIOE->IDR & (1 << 2)))
	{
		os_evt_set(SDDetectBIT_0, os_tsk_self());
	}
		
    while(1)
    {
		/*
			1. 无限等待SD卡插拔状态变化。
		    2. 无限等待网线插拔状态变化，暂未放在这个任务实现。
		*/
		if(os_evt_wait_or(PinDetect_ALL, usMaxBlockTime) == OS_R_EVT)
		{
			xResult = os_evt_get ();
		
			switch (xResult)
			{
				case SDDetectBIT_0:
				    /* 设置一段延迟时间，滤除插拔SD卡时造成的抖动, 拔出过程误差较大，这里等待100ms再检测 */
					os_dly_wait(100);
					if((GPIOE->IDR & (1 << 2)))
					{
						UnmountSD(); /* 卸载SD卡 */
						WM_SendMessageNoPara(hWinStatus, MSG_SDPlugOut);
					}
					else
					{
						MountSD();  /* 挂载SD卡 */
						WM_SendMessageNoPara(hWinStatus, MSG_SDPlugIn);
					}
					EXTI->IMR |= (1<<2); /* 使能中断  */
					break;
					
				/* 其他值不处理 */
				default:                     
					break;
			}	
		}
    }
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskCreate
*	功能说明: 创建应用任务
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void AppTaskCreate (void)
{

	HandleTaskGUI = os_tsk_create_user(AppTaskGUI,              /* 任务函数 */ 
	                                     3,                       /* 任务优先级 */ 
	                                     &AppTaskGUIStk,          /* 任务栈 */
	                                     sizeof(AppTaskGUIStk));  /* 任务栈大小，单位字节数 */
	
	HandleTaskUserIF = os_tsk_create_user(AppTaskUserIF,             /* 任务函数 */ 
	                                      5,                         /* 任务优先级 */ 
	                                      &AppTaskUserIFStk,         /* 任务栈 */
	                                      sizeof(AppTaskUserIFStk)); /* 任务栈大小，单位字节数 */
	
	HandleTaskMsgPro = os_tsk_create_user(AppTaskMsgPro,             /* 任务函数 */ 
	                                      6,                         /* 任务优先级 */ 
	                                      &AppTaskMsgProStk,         /* 任务栈 */
	                                      sizeof(AppTaskMsgProStk)); /* 任务栈大小，单位字节数 */
		
	HandleTaskModbus = os_tsk_create_user(AppTaskModbus,             /* 任务函数 Modbus任务*/ 
	                                      9,                         /* 任务优先级 */ 
	                                      &AppTaskModbusStk,         /* 任务栈 */
	                                      sizeof(AppTaskModbusStk)); /* 任务栈大小，单位字节数 */							   
		
	HandleMusicStart = os_tsk_create_user(AppTaskMusic,             /* 任务函数 */ 
	                                      11,                       /* 任务优先级 */ 
	                                      &AppTaskMusicStk,         /* 任务栈 */
	                                      sizeof(AppTaskMusicStk)); /* 任务栈大小，单位字节数 */
	
	HandlePinDetectStart = os_tsk_create_user(AppTaskPinDetect,        /* 任务函数 SD卡插拔检测任务*/ 
									     12,                           /* 任务优先级 */ 
									     &AppTaskPinDetectStk,         /* 任务栈 */
									     sizeof(AppTaskPinDetectStk)); /* 任务栈大小，单位字节数 */
}

/*
*********************************************************************************************************
*	函 数 名: AppObjCreate
*	功能说明: 创建任务通信机制
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void AppObjCreate (void)
{
	/* 创建信号量计数值是0, 用于任务同步 */
	os_sem_init (&semaphore, 0);
	
	 /* 创建消息邮箱 */
	os_mbx_init (&mailbox, sizeof(mailbox));
	
	/* 从内部SRAM分配100KB给动态内存使用 */	
	os_init_mem(AppMalloc, 1024*80);

	/* 将内部CCM SRAM的64KB全部供动态内存使用，用于JPEG软解 */
	os_init_mem(AppMallocCCM, 1024*64);
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

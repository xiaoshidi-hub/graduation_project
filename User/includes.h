/*
*********************************************************************************************************
*
*	模块名称 : 头文件汇总
*	文件名称 : includes.h
*	版    本 : V1.0
*	说    明 : 当前使用头文件汇总
*
*	修改记录 :
*		版本号    日期        作者     说明
*		V1.0    2017-03-11  Eric2013   首次发布
*
*	Copyright (C), 2017-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#ifndef  __INCLUDES_H__
#define  __INCLUDES_H__


/*
*********************************************************************************************************
*                                         标准库
*********************************************************************************************************
*/
#include  <stdarg.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <math.h>
#include  "time.h"


/*
*********************************************************************************************************
*                                          OS和系统库
*********************************************************************************************************
*/
#include "RTL.h"
#include "File_Config.h"
/*
*********************************************************************************************************
*                                           宏定义
*********************************************************************************************************
*/
/* SD卡 */
#define SDDetectBIT_0	(1 << 0)
#define GUIDetectBIT_15	(1 << 15)
#define PinDetect_ALL   (SDDetectBIT_0 | GUIDetectBIT_15)

/* 音乐播放器用到的事件标志 */
#define MusicTaskReserved_0	        	(1 << 0)
#define MusicTaskAudioAdjust_1	        (1 << 1)
#define MusicTaskAudioPause_2			(1 << 2)
#define MusicTaskAudioResume_3			(1 << 3)
#define MusicTaskReserved_4				(1 << 4)
#define MusicTaskAudioPlay_5			(1 << 5)
#define MusicTaskAudioReturn_6			(1 << 6)
#define MusicTaskAudioGetTime_7			(1 << 7)
#define MusicTaskAudioStart_8			(1 << 8)
#define MusicTaskAudioFillBuffer1_9		(1 << 9)
#define MusicTaskAudioFillBuffer0_10	(1 << 10)
#define MusicTaskReserved_11			(1 << 11)
#define MusicTaskReserved_12			(1 << 12)
#define MusicTaskReserved_13			(1 << 13)
#define MusicTaskReserved_14			(1 << 14)
#define MusicTaskReserved_15			(1 << 15)

#define MusicTaskWaitFlag   		    (RecTaskAudioFillBuffer1_0 | RecTaskAudioFillBuffer0_1 | MusicTaskAudioFillBuffer0_10 | MusicTaskAudioFillBuffer1_9 | MusicTaskAudioReturn_6 | MusicTaskAudioGetTime_7)
#define MusicTaskWaitBuffer  		    (MusicTaskAudioFillBuffer0_10 | MusicTaskAudioFillBuffer1_9)


/* 录音机用到的事件标志 */
#define RecTaskAudioFillBuffer1_0	    (1 << 0)
#define RecTaskAudioFillBuffer0_1	    (1 << 1)
#define RecTaskAudioPause_2				(1 << 2)
#define RecTaskAudioResume_3			(1 << 3)
#define RecTaskAudioPlay_4				(1 << 4)
#define RecTaskAudioRecorde_5			(1 << 5)
#define RecTaskAudioReturn_6			(1 << 6)
#define RecTaskAudioGetTime_7			(1 << 7)
#define RecTaskAudioStart_8				(1 << 8)
#define RecTaskAudioFillBuffer1_9		(1 << 9)
#define RecTaskAudioFillBuffer0_10		(1 << 10)
#define RecTaskReserved_11				(1 << 11)
#define RecTaskReserved_12				(1 << 12)
#define RecTaskReserved_13				(1 << 13)
#define RecTaskReserved_14				(1 << 14)
#define RecTaskReserved_15				(1 << 15)

/* 收音机用到的事件标志 */
#define RadioTaskSearch_0	        	(1 << 0)
#define RadioTaskGetRSSI_1	        	(1 << 1)
#define RadioTaskSetFM_2				(1 << 2)
#define RadioTaskClose_3				(1 << 3)

/* USB用到的事件标志 */
#define USBTaskOpen_0	        	(1 << 0)
#define USBTaskClose_1	        	(1 << 1)
#define USBTaskReMountSD_2			(1 << 2)


/*
*********************************************************************************************************
*                                         APP / BSP
*********************************************************************************************************
*/
#include  <bsp.h>
#include "stm32_dsp.h"
#include "rt_Memory.h"

#include "coder.h"
#include <mp3dec.h>

#include "app_audio_lib.h"
#include "app_wav_lib.h"
#include "app_mp3_lib.h"
#include "app_si4704_lib.h"

#include "modbus_host.h"

/*
*********************************************************************************************************
*                                          变量和函数
*********************************************************************************************************
*/
extern OS_TID HandlePinDetectStart;
extern OS_TID HandleMusicStart;
extern OS_TID HandleTaskGUI;

extern FILE *foutbmp;
extern uint32_t bw;
extern Media_INFO info;
extern FAT_VI *mc0; 
extern uint64_t ullSdUnusedCapacity;
extern uint64_t ullSdCapacity;

void MountSD(void);
void UnmountSD(void);
void _WriteByte2File(U8 Data, void * p);
void SD_Link_EXTIConfig(void);

extern uint64_t AppMalloc[80*1024/8];   
extern uint64_t AppMallocCCM[64*1024/8] __attribute__((at(0x10000000)));

extern 	void WavRecPlay(const char *filename);

typedef struct
{
	uint8_t Rxlen;
	char RxBuf[20];
	uint8_t Txlen;
	char TxBuf[20];
}PRINT_MODS_T;

extern PRINT_MODS_T g_tPrint;

/* main.c文件使用 */
extern __IO uint8_t g_ucEthLinkStatus;
extern  void init_ethernet (void);
extern  void TCPnetTest(void);

extern __IO float OSCPUUsage;

extern struct tm g_time;
extern uint8_t g_SNTPStatus;

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

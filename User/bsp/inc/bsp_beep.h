/*
*********************************************************************************************************
*
*	模块名称 : 蜂鸣器模块
*	文件名称 : bsp_beep.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*	Copyright (C), 2012-2013, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_BEEP_H
#define __BSP_BEEP_H

typedef struct _BEEP_T
{
	uint8_t ucEnalbe; //用于使能或者禁止蜂鸣器。
	uint8_t ucState;  //状态变量，用于蜂鸣器鸣叫和停止的区分。
	uint16_t usBeepTime;//鸣叫时间，单位 10ms。
	uint16_t usStopTime;//停止鸣叫时间，单位 10ms。
	uint16_t usCycle;//鸣叫和停止的循环次数。
	uint16_t usCount;//用于鸣叫和停止时的计数。
	uint16_t usCycleCount;//用于循环次数计数。
	uint8_t ucMute;		/* 1表示静音 */	
}BEEP_T;

/* 供外部调用的函数声明 */
void BEEP_InitHard(void);
void BEEP_Start(uint16_t _usBeepTime, uint16_t _usStopTime, uint16_t _usCycle);
void BEEP_Stop(void);
void BEEP_KeyTone(void);
void BEEP_Pro(void);

void BEEP_Pause(void);
void BEEP_Resume(void);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

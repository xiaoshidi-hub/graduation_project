/*
*********************************************************************************************************
*
*	模块名称 : FM收音机头文件
*	文件名称 : app_si4704_lib.h
*	版    本 : V1.0
*	说    明 : Si4704收音机处理程序
*              1. 为了更好搜索电台，务必给耳机接口插上耳机当做收音机天线用。
*
*	修改记录 :
*		版本号   日期        作者       说明
*		V1.0    2017-01-13  Eric2013    首发
*
*	Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#ifndef _APP_SI4730_LIB_H
#define _APP_SI4730_LIB_H


typedef struct
{
	uint8_t ucFMCount;	    /* FM 电台个数 */
	uint16_t usFMList[32];	/* FM 电台列表 */
	uint8_t ucIndexFM;	    /* 当前电台索引 */
	
	uint32_t usFreq;	/* 当前电台频率 */
	uint8_t ucVolume;	/* 音量 */
	
	uint8_t ucRSSI;	   /* 音量 */
	uint8_t ucSNR;	   /* 音量 */		
}RADIO_T;


/* 供外部调用的函数声明 */
void BSP_RadioInit(void);
void RadioPlay(void);

extern RADIO_T g_tRadio;

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

/*
*********************************************************************************************************
*
*	ģ������ : RTC
*	�ļ����� : bsp_rtc.c
*	��    �� : V1.0
*	˵    �� : RTC�ײ�����
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_RTC_H
#define __BSP_RTC_H


void bsp_InitRTC(void);
void RTC_Config(void);
uint8_t RTC_CalcWeek(uint16_t _year, uint8_t _mon, uint8_t _day);

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

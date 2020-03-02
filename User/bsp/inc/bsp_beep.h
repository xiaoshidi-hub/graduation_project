/*
*********************************************************************************************************
*
*	ģ������ : ������ģ��
*	�ļ����� : bsp_beep.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2012-2013, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_BEEP_H
#define __BSP_BEEP_H

typedef struct _BEEP_T
{
	uint8_t ucEnalbe; //����ʹ�ܻ��߽�ֹ��������
	uint8_t ucState;  //״̬���������ڷ��������к�ֹͣ�����֡�
	uint16_t usBeepTime;//����ʱ�䣬��λ 10ms��
	uint16_t usStopTime;//ֹͣ����ʱ�䣬��λ 10ms��
	uint16_t usCycle;//���к�ֹͣ��ѭ��������
	uint16_t usCount;//�������к�ֹͣʱ�ļ�����
	uint16_t usCycleCount;//����ѭ������������
	uint8_t ucMute;		/* 1��ʾ���� */	
}BEEP_T;

/* ���ⲿ���õĺ������� */
void BEEP_InitHard(void);
void BEEP_Start(uint16_t _usBeepTime, uint16_t _usStopTime, uint16_t _usCycle);
void BEEP_Stop(void);
void BEEP_KeyTone(void);
void BEEP_Pro(void);

void BEEP_Pause(void);
void BEEP_Resume(void);

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

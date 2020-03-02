/*
*********************************************************************************************************
*
*	ģ������ : ��Ƶ���Ż���ͷ�ļ�
*	�ļ����� : app_audio_lib.h
*	��    �� : V1.0
*	˵    �� : WAV��Ƶ��MP3��Ƶ���Ĺ����ļ�����ǰ��Ҫ������¹���
*              1. WM8978�ĳ�ʼ����
*              2. 64��FFT�����Ƶ��Ӧ���������ֲ��ŵ�Ƶ����ʾ��
*
*	�޸ļ�¼ :
*		�汾��    ����         ����         ˵��
*       V1.0    2016-02-16   Eric2013       �׷�
*
*	Copyright (C), 2015-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#ifndef _APP_AUDIO_LIB_H_
#define _APP_AUDIO_LIB_H_


typedef struct
{
	__IO uint8_t  ucDataSize;   	/* ��Ƶ��ʽ��λ�� */
	__IO uint8_t  ucVolume;			/* ��ǰ�������� */
	__IO uint8_t  ucMicGain;		/* ��ǰMIC����  */
	__IO uint8_t  ucSpectrum;		/* Ƶ��ģʽ  */	
	__IO uint8_t  ucDispUpdate;		/* ���±�־  */
	__IO uint8_t  ucSpeakerStatus;	/* WM8978�ľ������Ǿ������� */	
	
    __IO uint8_t  usInfo[40];	/* ��Ƶ��Ϣ��¼ */
    __IO uint16_t uiFFT[32];	/* ��¼64��FFT��ǰ32���ֵ */
	
	__IO uint32_t uiTotalTime;	/* �ܲ���ʱ�� */
	__IO uint32_t uiCurTime;	/* ��ǰ����ʱ�� */	
}WavControl_T;

typedef struct
{
    uint8_t  *ucName;	
    uint8_t  ucType;	
}MusicMsg_T;

/* �������ͣ�MP3, WAV, */
typedef enum
{
	MusicType_WAV = 1,
	MusicType_MP3,
	MusicType_Radio,
	MusicType_REC,	
}MusicType_ENUM;


/* ���ⲿ�ļ����õĺ��� */
extern WavControl_T g_tWav;
void BSP_WM8978Init(void);
void DSP_FFT64(uint8_t *pBuf);

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

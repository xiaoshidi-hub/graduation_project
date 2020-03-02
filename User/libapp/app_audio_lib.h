/*
*********************************************************************************************************
*
*	模块名称 : 音频播放汇总头文件
*	文件名称 : app_audio_lib.h
*	版    本 : V1.0
*	说    明 : WAV音频和MP3音频软解的公共文件，当前主要完成如下工作
*              1. WM8978的初始化。
*              2. 64点FFT计算幅频响应，用于音乐播放的频谱显示。
*
*	修改记录 :
*		版本号    日期         作者         说明
*       V1.0    2016-02-16   Eric2013       首发
*
*	Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#ifndef _APP_AUDIO_LIB_H_
#define _APP_AUDIO_LIB_H_


typedef struct
{
	__IO uint8_t  ucDataSize;   	/* 音频格式：位长 */
	__IO uint8_t  ucVolume;			/* 当前放音音量 */
	__IO uint8_t  ucMicGain;		/* 当前MIC增益  */
	__IO uint8_t  ucSpectrum;		/* 频谱模式  */	
	__IO uint8_t  ucDispUpdate;		/* 更新标志  */
	__IO uint8_t  ucSpeakerStatus;	/* WM8978的静音，非静音设置 */	
	
    __IO uint8_t  usInfo[40];	/* 音频信息记录 */
    __IO uint16_t uiFFT[32];	/* 记录64点FFT的前32点幅值 */
	
	__IO uint32_t uiTotalTime;	/* 总播放时间 */
	__IO uint32_t uiCurTime;	/* 当前播放时间 */	
}WavControl_T;

typedef struct
{
    uint8_t  *ucName;	
    uint8_t  ucType;	
}MusicMsg_T;

/* 歌曲类型，MP3, WAV, */
typedef enum
{
	MusicType_WAV = 1,
	MusicType_MP3,
	MusicType_Radio,
	MusicType_REC,	
}MusicType_ENUM;


/* 供外部文件调用的函数 */
extern WavControl_T g_tWav;
void BSP_WM8978Init(void);
void DSP_FFT64(uint8_t *pBuf);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

/*
*********************************************************************************************************
*
*	模块名称 : 录音机
*	文件名称 : app_recorder_lib.c
*	版    本 : V1.0
*	说    明 : 录音机功能：
*              1. 录音机的功能做的比较简单。
*              2. 采样率固定为32KHz，16bit，单通道。
*              3. 点击开始按钮，开始录音，录音30秒，务必保证录音结束后开始放音。
*              4. 显示的频谱是对数谱，即对FFT变化后幅值取对数。
*
*	修改记录 :
*		版本号     日期        作者     说明
*		V1.0    2017-01-13   Eric2013   首发
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
	#define printf_Recoderdbg printf
#else
	#define printf_Recoderdbg(...)
#endif


/*
*********************************************************************************************************
*	                                    变量       
*********************************************************************************************************
*/
extern uint8_t *g_pRecorder;
uint16_t g_uiNum = 0;


/*
*********************************************************************************************************
*	函 数 名: WavRecPlay
*	功能说明: 录音机功能实现
*	形    参: filename  暂时未用到
*	返 回 值: 无
*********************************************************************************************************
*/
void WavRecPlay(const char *filename)
{
	uint32_t temp;
	OS_RESULT xResult;
	
	uint8_t *pI2SBuffer0;
	uint8_t *pI2SBuffer1;
	uint8_t *pI2SBuffer2;
	uint8_t *pI2SBuffer3;
	
	/* 申请所需的动态内存 */
	pI2SBuffer0 = (uint8_t *)os_alloc_mem(AppMalloc, 8*1024);
	pI2SBuffer1 = (uint8_t *)os_alloc_mem(AppMalloc, 8*1024); 
	
	pI2SBuffer2 = (uint8_t *)os_alloc_mem(AppMalloc, 8*1024);
	pI2SBuffer3 = (uint8_t *)os_alloc_mem(AppMalloc, 8*1024); 

	g_uiNum = 0;
	
	/* 配置WM8978芯片，输入为MIC，无输出 */	
	wm8978_OutMute(1);
	wm8978_CfgAudioPath(MIC_LEFT_ON|ADC_ON,  OUT_PATH_OFF);
	wm8978_OutMute(0);
	
	while(1)
	{ 
		if(os_evt_wait_or(0xFFFF, 0xFFFF) == OS_R_EVT)
		{
			xResult = os_evt_get ();
			printf_Recoderdbg("x = %x\r\n", xResult);
			switch (xResult)
			{
				/* 录音：当前使用的是pI2SBuffer2，可以复制pI2SBuffer3中的数据到缓冲  */
				case RecTaskAudioFillBuffer1_0:
					temp = g_uiNum*2000;
					DSP_FFT64(pI2SBuffer3);
					memcpy(g_pRecorder+temp, pI2SBuffer3, 2000);
					g_uiNum++;
				
					/* 给emWin的音乐播放器发消息，切换到下一首歌曲播放 */
					WM_SendMessageNoPara(hWinRecorder, MSG_MusicStart);	
				
					/* 32KHz采样率，单声道，16bits，录制30s DMA的缓冲大小设置2000字节
				       30秒采集数据30 * 32000 * 2 = 1920_000字节
					   采集完成uiNum的计数为1920_000 / 2000 = 960
				    */
					if(g_uiNum == 960)
					{
						AUDIO_Stop();
						wm8978_CfgAudioPath(DAC_ON,  EAR_LEFT_ON | EAR_RIGHT_ON | SPK_ON);
					}
					break;

				/* 录音：当前使用的是pI2SBuffer3，可以复制pI2SBuffer2中的数据到缓冲  */
				case RecTaskAudioFillBuffer0_1:
					temp = g_uiNum*2000;
					DSP_FFT64(pI2SBuffer2);
					memcpy(g_pRecorder+temp, pI2SBuffer2, 2000);
					g_uiNum++;
				
					/* 给emWin的音乐播放器发消息，切换到下一首歌曲播放 */
					WM_SendMessageNoPara(hWinRecorder, MSG_MusicStart);	
				
					/* 32KHz采样率，单声道，16bits，录制30s DMA的缓冲大小设置2000字节
				       30秒采集数据30 * 32000 * 2 = 1920_000字节
					   采集完成uiNum的计数为1920_000 / 2000 = 960
				    */
					if(g_uiNum == 960)
					{
						AUDIO_Stop();
						//wm8978_OutMute(1);
						wm8978_CfgAudioPath(DAC_ON,  EAR_LEFT_ON | EAR_RIGHT_ON | SPK_ON);
					}
					break;	

				/* 暂停 */
				case RecTaskAudioPause_2:
					break;

				/* 继续 */
				case RecTaskAudioResume_3:
					break;	

				/* 开始放音 */
				case RecTaskAudioPlay_4:
					g_uiNum = 0;
					AUDIO_Init(1, I2S_Standard_Phillips, SAI_DataSize_16b, 32000, 1);
					Play_DMA_Init(pI2SBuffer0, pI2SBuffer1, 1000);
					memcpy(pI2SBuffer0, g_pRecorder, 2000);
					g_uiNum++;
					memcpy(pI2SBuffer1, g_pRecorder+2000, 2000);
					g_uiNum++;
					wm8978_OutMute(0);
					AUDIO_Play();
					break;	
				
				/* 开始录音 */
				case RecTaskAudioRecorde_5:
					g_uiNum = 0;
					wm8978_OutMute(1);
					memset(pI2SBuffer0, 0, 2000);
					memset(pI2SBuffer1, 0, 2000);
					memset(pI2SBuffer2, 0, 2000);
					memset(pI2SBuffer3, 0, 2000);
					AUDIO_Init(3, I2S_Standard_Phillips, SAI_DataSize_16b, 32000, 1);
					Play_DMA_Init(pI2SBuffer0, pI2SBuffer1, 1000);
					AUDIO_Play();
					Rec_DMA_Init(pI2SBuffer2, pI2SBuffer3, 1000);
					REC_Play();
					break;	
					
				/* 退出 */				
				case RecTaskAudioReturn_6:
					AUDIO_Stop();
					wm8978_OutMute(1);
					wm8978_CfgAudioPath(DAC_ON,  EAR_LEFT_ON | EAR_RIGHT_ON | SPK_ON);
					os_free_mem(AppMalloc, pI2SBuffer0);
					os_free_mem(AppMalloc, pI2SBuffer1);
					os_free_mem(AppMalloc, pI2SBuffer2);
					os_free_mem(AppMalloc, pI2SBuffer3);
					wm8978_OutMute(0);
					return;
						
				/* 暂未使用 */
				case RecTaskAudioGetTime_7:
					break;
					
				/* 暂未使用 */
				case RecTaskAudioStart_8:
					break;
					
				/* 声音播放 */
				case RecTaskAudioFillBuffer1_9:
					temp = g_uiNum*2000;
					DSP_FFT64(pI2SBuffer0);
					memcpy(pI2SBuffer1, g_pRecorder+temp, 2000);
					g_uiNum++;
					/* 给emWin的音乐播放器发消息，切换到下一首歌曲播放 */
					WM_SendMessageNoPara(hWinRecorder, MSG_MusicStart);

					/* 32KHz采样率，单声道，16bits，录制30s DMA的缓冲大小设置2000字节
				       30秒采集数据30 * 32000 * 2 = 1920_000字节
					   采集完成uiNum的计数为1920_000 / 2000 = 960
				    */
					if(g_uiNum == 960)
					{
						AUDIO_Stop();
						
						/* 播放完毕，切换到录音 */
						wm8978_CfgAudioPath(MIC_LEFT_ON|ADC_ON,   OUT_PATH_OFF);
					}
					break;
				
				/* 声音播放 */
				case RecTaskAudioFillBuffer0_10:
					temp = g_uiNum*2000;
					DSP_FFT64(pI2SBuffer1);
					memcpy(pI2SBuffer0, g_pRecorder+temp, 2000);
					g_uiNum++;
				
					/* 给emWin的音乐播放器发消息，切换到下一首歌曲播放 */
					WM_SendMessageNoPara(hWinRecorder, MSG_MusicStart);
				
					/* 32KHz采样率，单声道，16bits，录制30s DMA的缓冲大小设置2000字节
				       30秒采集数据30 * 32000 * 2 = 1920_000字节
					   采集完成uiNum的计数为1920_000 / 2000 = 960
				    */
					if(g_uiNum == 960)
					{
						AUDIO_Stop();
						/* 播放完毕，切换到录音 */
						wm8978_CfgAudioPath(MIC_LEFT_ON|ADC_ON,   OUT_PATH_OFF);
					}
					break;
				
				/* 后5位暂未使用 */
				default: 
					break;
			}	
		}
	}
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

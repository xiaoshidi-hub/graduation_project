/*
*********************************************************************************************************
*
*	ģ������ : ¼����
*	�ļ����� : app_recorder_lib.c
*	��    �� : V1.0
*	˵    �� : ¼�������ܣ�
*              1. ¼�����Ĺ������ıȽϼ򵥡�
*              2. �����ʹ̶�Ϊ32KHz��16bit����ͨ����
*              3. �����ʼ��ť����ʼ¼����¼��30�룬��ر�֤¼��������ʼ������
*              4. ��ʾ��Ƶ���Ƕ����ף�����FFT�仯���ֵȡ������
*
*	�޸ļ�¼ :
*		�汾��     ����        ����     ˵��
*		V1.0    2017-01-13   Eric2013   �׷�
*
*	Copyright (C), 2015-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"
#include "MainTask.h"



/*
*********************************************************************************************************
*	                                  ���ڱ��ļ��ĵ���
*********************************************************************************************************
*/
#if 0
	#define printf_Recoderdbg printf
#else
	#define printf_Recoderdbg(...)
#endif


/*
*********************************************************************************************************
*	                                    ����       
*********************************************************************************************************
*/
extern uint8_t *g_pRecorder;
uint16_t g_uiNum = 0;


/*
*********************************************************************************************************
*	�� �� ��: WavRecPlay
*	����˵��: ¼��������ʵ��
*	��    ��: filename  ��ʱδ�õ�
*	�� �� ֵ: ��
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
	
	/* ��������Ķ�̬�ڴ� */
	pI2SBuffer0 = (uint8_t *)os_alloc_mem(AppMalloc, 8*1024);
	pI2SBuffer1 = (uint8_t *)os_alloc_mem(AppMalloc, 8*1024); 
	
	pI2SBuffer2 = (uint8_t *)os_alloc_mem(AppMalloc, 8*1024);
	pI2SBuffer3 = (uint8_t *)os_alloc_mem(AppMalloc, 8*1024); 

	g_uiNum = 0;
	
	/* ����WM8978оƬ������ΪMIC������� */	
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
				/* ¼������ǰʹ�õ���pI2SBuffer2�����Ը���pI2SBuffer3�е����ݵ�����  */
				case RecTaskAudioFillBuffer1_0:
					temp = g_uiNum*2000;
					DSP_FFT64(pI2SBuffer3);
					memcpy(g_pRecorder+temp, pI2SBuffer3, 2000);
					g_uiNum++;
				
					/* ��emWin�����ֲ���������Ϣ���л�����һ�׸������� */
					WM_SendMessageNoPara(hWinRecorder, MSG_MusicStart);	
				
					/* 32KHz�����ʣ���������16bits��¼��30s DMA�Ļ����С����2000�ֽ�
				       30��ɼ�����30 * 32000 * 2 = 1920_000�ֽ�
					   �ɼ����uiNum�ļ���Ϊ1920_000 / 2000 = 960
				    */
					if(g_uiNum == 960)
					{
						AUDIO_Stop();
						wm8978_CfgAudioPath(DAC_ON,  EAR_LEFT_ON | EAR_RIGHT_ON | SPK_ON);
					}
					break;

				/* ¼������ǰʹ�õ���pI2SBuffer3�����Ը���pI2SBuffer2�е����ݵ�����  */
				case RecTaskAudioFillBuffer0_1:
					temp = g_uiNum*2000;
					DSP_FFT64(pI2SBuffer2);
					memcpy(g_pRecorder+temp, pI2SBuffer2, 2000);
					g_uiNum++;
				
					/* ��emWin�����ֲ���������Ϣ���л�����һ�׸������� */
					WM_SendMessageNoPara(hWinRecorder, MSG_MusicStart);	
				
					/* 32KHz�����ʣ���������16bits��¼��30s DMA�Ļ����С����2000�ֽ�
				       30��ɼ�����30 * 32000 * 2 = 1920_000�ֽ�
					   �ɼ����uiNum�ļ���Ϊ1920_000 / 2000 = 960
				    */
					if(g_uiNum == 960)
					{
						AUDIO_Stop();
						//wm8978_OutMute(1);
						wm8978_CfgAudioPath(DAC_ON,  EAR_LEFT_ON | EAR_RIGHT_ON | SPK_ON);
					}
					break;	

				/* ��ͣ */
				case RecTaskAudioPause_2:
					break;

				/* ���� */
				case RecTaskAudioResume_3:
					break;	

				/* ��ʼ���� */
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
				
				/* ��ʼ¼�� */
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
					
				/* �˳� */				
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
						
				/* ��δʹ�� */
				case RecTaskAudioGetTime_7:
					break;
					
				/* ��δʹ�� */
				case RecTaskAudioStart_8:
					break;
					
				/* �������� */
				case RecTaskAudioFillBuffer1_9:
					temp = g_uiNum*2000;
					DSP_FFT64(pI2SBuffer0);
					memcpy(pI2SBuffer1, g_pRecorder+temp, 2000);
					g_uiNum++;
					/* ��emWin�����ֲ���������Ϣ���л�����һ�׸������� */
					WM_SendMessageNoPara(hWinRecorder, MSG_MusicStart);

					/* 32KHz�����ʣ���������16bits��¼��30s DMA�Ļ����С����2000�ֽ�
				       30��ɼ�����30 * 32000 * 2 = 1920_000�ֽ�
					   �ɼ����uiNum�ļ���Ϊ1920_000 / 2000 = 960
				    */
					if(g_uiNum == 960)
					{
						AUDIO_Stop();
						
						/* ������ϣ��л���¼�� */
						wm8978_CfgAudioPath(MIC_LEFT_ON|ADC_ON,   OUT_PATH_OFF);
					}
					break;
				
				/* �������� */
				case RecTaskAudioFillBuffer0_10:
					temp = g_uiNum*2000;
					DSP_FFT64(pI2SBuffer1);
					memcpy(pI2SBuffer0, g_pRecorder+temp, 2000);
					g_uiNum++;
				
					/* ��emWin�����ֲ���������Ϣ���л�����һ�׸������� */
					WM_SendMessageNoPara(hWinRecorder, MSG_MusicStart);
				
					/* 32KHz�����ʣ���������16bits��¼��30s DMA�Ļ����С����2000�ֽ�
				       30��ɼ�����30 * 32000 * 2 = 1920_000�ֽ�
					   �ɼ����uiNum�ļ���Ϊ1920_000 / 2000 = 960
				    */
					if(g_uiNum == 960)
					{
						AUDIO_Stop();
						/* ������ϣ��л���¼�� */
						wm8978_CfgAudioPath(MIC_LEFT_ON|ADC_ON,   OUT_PATH_OFF);
					}
					break;
				
				/* ��5λ��δʹ�� */
				default: 
					break;
			}	
		}
	}
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

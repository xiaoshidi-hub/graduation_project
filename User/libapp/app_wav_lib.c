/*
*********************************************************************************************************
*
*	ģ������ : WAV����Ӧ��
*	�ļ����� : app_wav_lib.c
*	��    �� : V1.0
*	˵    �� : WAV����ʵ�֣�WAV��ԱȽϼ򵥣�����ͷ�ļ���ʣ�µľ�����Ƶ���ݡ�
*              1. ����LoadWavHead��DumpWavHead����WAV�ļ��Ľ�����
*              2. WavMusicPlay��ʵ���ǻ���RTX����ϵͳ�ġ�
*              3. ֧����һ������һ��������Ϳ��ˣ����õĲ����ʶ�֧�֣���������������Ҳ��֧�֡�
*                 ��ǰ��֧��16λ��Ƶ�������24λ��WAV�ᱻת����16λ���в��š�
*              4. ��g_tWav��ȫ�ֱ��������඼��ʹ�þֲ������Ͷ�̬�ڴ档
*
*	�޸ļ�¼ :
*		�汾��    ����         ����         ˵��
*       V1.0    2016-02-16   Eric2013       �׷�
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
	#define printf_Wavdbg printf
#else
	#define printf_Wavdbg(...)
#endif


/*
*********************************************************************************************************
*	                                         �궨��
*********************************************************************************************************
*/
#define Wav_DMABufferSize     1024*10  /* �����DMA˫�����С����λ�ֽ� */


/*
*********************************************************************************************************
                                          �ļ��ڲ����õĺ���
*********************************************************************************************************
*/
static uint8_t LoadWavHead(FILE *_pFile, WavHead_T *_WavHead);
static void DumpWavHead(WavHead_T *_WavHead);
static uint32_t Wav_FillAudio(FILE *_pFile, 
	                          uint8_t *_ptr, 
                              uint8_t *_ptrtemp, 
                              uint16_t _usSize, 
                              uint8_t usBitsPerSample);
static void Wav_TimeShow(FILE *_pFile, WavHead_T * _WavHead);
static void Wav_TimeSet(FILE *_pFile, WavHead_T * _WavHead);


/*
*********************************************************************************************************
*	�� �� ��: LoadWavHead
*	����˵��: ����Wav��Ƶͷ�ļ�
*	��    ��: _pFile     �ļ����
*             _WavHead   Wavͷ�ļ��ṹ�������ַ
*	�� �� ֵ: TRUE(1) ��ʾ�ɹ��� FALSE(0)��ʾʧ��
*********************************************************************************************************
*/
static uint8_t LoadWavHead(FILE *_pFile, WavHead_T *_WavHead)
{
	RiffHead_T   tRiffChunk;
	FactBlock_T  tFactChunk;
	
	/* ��λ���ļ���ʼ��ַ���ж�ȡ */
	fseek (_pFile, 0, SEEK_SET);

	/* ��1�����ȶ�ȡRiffChunk����֤ID��Type�Ƿ���ȷ *******************************************************/
	fread(&tRiffChunk, sizeof(tRiffChunk), 1, _pFile);
	if(strncmp((char *)tRiffChunk.ucRiffID, "RIFF", 4) || strncmp((char *)tRiffChunk.ucWaveID, "WAVE", 4))
	{
		return FALSE;	
	}
		
	/* ��2������ȡFmtChunk ******************************************************************************/
	fread(&_WavHead->tFmt, 1, sizeof(_WavHead->tFmt)-2, _pFile);
	/*
	   һ�������uiSizeΪ16����ʱ��󸽼���Ϣû�У����Ϊ18��������2���ֽڵĸ�����Ϣ��
       ��Ҫ��һЩ����Ƴɵ�wav��ʽ�к��и�2���ֽڵģ�����и�����Ϣ����ж�ȡ��
	*/
	if(_WavHead->tFmt.uiSize == 18)
	{
		fread(_WavHead->tFmt.ucAddtional, 1, 2, _pFile);
	}
		
	/* ��3������ȡFactChunk ****************************************************************************/
	fread(&tFactChunk, 1, sizeof(tFactChunk), _pFile);
	/* ����Ƿ�ΪFactChunk���������ִ�����´��� */
	if(strncmp((char *)tFactChunk.ucFactID, "fact", 4))
	{
		/* ��һ������Ƿ�ΪDataChunk*/
		if(strncmp((char*)tFactChunk.ucFactID, "data", 4)==0)
		{
			memcpy(_WavHead->tData.ucDataID, tFactChunk.ucFactID, 4);
			_WavHead->tData.uiDataSize = tFactChunk.uiFactSize;
			/* tRiffChunk + FmtChunkǰ������Ա + DataBlock_T = 12 + 8 + 8 = 28 */
			_WavHead->uiDataOffset = 28 + _WavHead->tFmt.uiSize;
		
			printf_Wavdbg("FormatTag = %d\r\n", _WavHead->tFmt.usFormatTag);
			printf_Wavdbg("Channels = %d\r\n", _WavHead->tFmt.usChannels);
			printf_Wavdbg("uiSamplePerSec = %d\r\n", _WavHead->tFmt.uiSamplePerSec);
			printf_Wavdbg("uiAvgBytesPerSec = %d\r\n", _WavHead->tFmt.uiAvgBytesPerSec);
			printf_Wavdbg("BlockAlign = %d\r\n", _WavHead->tFmt.usBlockAlign);
			printf_Wavdbg("BitsPerSample = %d\r\n", _WavHead->tFmt.usBitsPerSample);
			
			return TRUE;
		}
		else 
		{
			return FALSE;
		}
	}
	/* ����Ƿ�ΪFactChunk�������ִ�����´��� */
	else 
	{
		fseek (_pFile, ftell(_pFile)+tFactChunk.uiFactSize, SEEK_SET);
	}
	
	/* ��4������ȡDataChunk ***********************************************************************/
	fread(&_WavHead->tData, 1, sizeof(_WavHead->tData), _pFile);
	if(strncmp((char*)_WavHead->tData.ucDataID,"data",4))
	{
		return FALSE;
	}
	
	/* tRiffChunk + FmtChunkǰ������Ա + FactChunkǰ������Ա + DataBlock_T = 12 + 8 + 8 + 8 = 36 */
	_WavHead->uiDataOffset = 36 + _WavHead->tFmt.uiSize +  tFactChunk.uiFactSize;
	
	return TRUE;
}

/*
*********************************************************************************************************
*	�� �� ��: DumpWavHead
*	����˵��: ����Wav��Ƶͷ�ļ��������Ƶִ��ʱ�����ص���Ƶ��Ϣ��¼������g_tWav.usInfo����
*	��    ��: _WavHead   Wavͷ�ļ��ṹ�������ַ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
typedef struct
{
	uint32_t Fs;
	uint8_t  pFs[8]; 
}DispFS_T;

const DispFS_T g_tDispFS[]=
{ 
	{8000 ,  "8KHz"}, 
	{16000,  "16KHz"},	
	{32000,  "32KHz"},	
	{48000,  "48KHz"},	
	{96000,  "96KHz"},  
	{192000, "192KHz"}, 
	
	{11020,  "11.02KHz"}, 
	{22050,  "22.05KHz"},  
	{44100,  "44.1KHz"},  
}; 

static void DumpWavHead(WavHead_T *_WavHead)
{
	uint32_t n;
	
	/* 1. ͨ�������ʻ�ò����ʵ��ַ��� */
	for(n = 0; n < (sizeof(g_tDispFS)/sizeof(g_tDispFS[0])); n++)
	{
		if(_WavHead->tFmt.uiSamplePerSec == g_tDispFS[n].Fs)
		{
			break;
		}	
	}
	
	/* 2. ��¼�����ʣ�����λ��������������ͨ����������g_tWav.usInfo���� */
	sprintf((char *)g_tWav.usInfo, "%s/%dbits/",  g_tDispFS[n].pFs, _WavHead->tFmt.usBitsPerSample);
	switch(_WavHead->tFmt.usChannels)
	{
		case 2:
			strcat((char *)g_tWav.usInfo, "Stereo");
			break;

		case 1:
			strcat((char *)g_tWav.usInfo, "Mono");
			break;
		
		default:
			break;
	}
	
	printf_Wavdbg("%s\r\n", g_tWav.usInfo);
	printf_Wavdbg("DataSize : %d\r\n", _WavHead->tData.uiDataSize);
	
	/* 3.�����Ƶ��ִ��ʱ�� */
	if((_WavHead->tFmt.uiSamplePerSec != 0 && _WavHead->tFmt.usChannels != 0) && (_WavHead->tFmt.usBitsPerSample != 0))
	{
		g_tWav.uiTotalTime = _WavHead->tData.uiDataSize / _WavHead->tFmt.uiSamplePerSec / _WavHead->tFmt.usChannels;
		/* ����ʱ��Ŵ�10������MP3����ͳһ */
		g_tWav.uiTotalTime = g_tWav.uiTotalTime * 80 / _WavHead->tFmt.usBitsPerSample;
		
		printf_Wavdbg("uiTotalTime : %d:%d\r\n", g_tWav.uiTotalTime/600, g_tWav.uiTotalTime/10%60);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: Wav_FillAudio
*	����˵��: ��ȡ��Ƶ������䵽DMA��˫������
*	��    ��: _pFile            FIL�ļ�
*             _ptr              ��ȡ����Ƶ���ݷŵ��˻�����
*             _ptrtemp          �������ݵ���ʱ���
*             _usSize           Ҫ��ȡ�����ݴ�С
*             usBitsPerSample   ��Ƶ������С��16λ����24λ
*	�� �� ֵ: ����ʵ�ʶ�ȡ���ֽ���
*********************************************************************************************************
*/
static uint32_t Wav_FillAudio(FILE *_pFile, uint8_t *_ptr, uint8_t *_ptrtemp, uint16_t _usSize, uint8_t usBitsPerSample)
{
	uint32_t NumBytesReq;
	uint32_t NumBytesRead;
	uint32_t i;
	uint8_t *p;
	
	/* 24λ��Ƶת��Ϊ16λ���в��ţ�SAI��Ƶ�ӿڵ�24λ��Ƶ������ʱδ��� */
	if(usBitsPerSample == 24)
	{
		/* ��Ҫ��ȡ���ֽ��� */
		NumBytesReq=(_usSize / 2) * 3;						

		NumBytesRead = fread((void *)_ptrtemp, 1, NumBytesReq, _pFile);
		p = _ptrtemp;
		for(i = 0; i < _usSize; i += 2, p += 3)
		{
			_ptr[i]   = p[1];
			_ptr[i+1] = p[2]; 
		} 
		
		/* ʵ�����ȡ���ֽ��� */
		NumBytesRead = (NumBytesRead / 3) * 2;		
	}
	/* 16λ��Ƶ */
	else 
	{
		NumBytesRead = fread((void *)_ptr, 1, _usSize, _pFile);
	}
	
	if(NumBytesRead < _usSize)
	{
		/* ����Ĳ�0 */
		for(i = NumBytesRead; i < _usSize; i++)
		{
			_ptr[i] = 0; 				
		}
	}
	
	return NumBytesRead;
} 

/*
*********************************************************************************************************
*	�� �� ��: Wav_TimeShow
*	����˵��: ���ڻ�ȡ��������Ƶ��ǰ�Ĳ���ʱ�� 
*	��    ��: _pFile   FIL�ļ�
*             _WavHead Wavͷ�ļ��ṹ�������ַ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void Wav_TimeShow(FILE *_pFile, WavHead_T * _WavHead)
{	
	uint32_t uiPos;
	
	uiPos = ftell(_pFile) -  _WavHead->uiDataOffset;
	g_tWav.uiCurTime  = uiPos / _WavHead->tFmt.uiSamplePerSec / _WavHead->tFmt.usChannels;
	/* ����ʱ��Ŵ�10������MP3����ͳһ */
	g_tWav.uiCurTime  = g_tWav.uiCurTime * 80/ _WavHead->tFmt.usBitsPerSample;
}

static void Wav_TimeSet(FILE *_pFile, WavHead_T * _WavHead)
{	
	uint32_t uiPos;
	
	/* ����ʱ�䱶�Ŵ�10������MP3����ͳһ */
	uiPos  = g_tWav.uiCurTime * _WavHead->tFmt.usBitsPerSample / 80;
	uiPos = uiPos *  _WavHead->tFmt.usChannels *  _WavHead->tFmt.uiSamplePerSec;
	uiPos = _WavHead->uiDataOffset + uiPos;
	fseek (_pFile, uiPos, SEEK_SET);
}

/*
*********************************************************************************************************
*	�� �� ��: GetWavRuntime
*	����˵��: ר�����ڻ�ȡWav��Ƶ������ʱ�䣬����emWin��ʾ�����б�ʱʱ�����ʾ��
*	��    ��: _pPath       �ļ�����·��
*             uiTotalTime  ����ִ��ʱ��
*	�� �� ֵ: TRUE(1) ��ʾ�ɹ��� FALSE(0)��ʾʧ��
*********************************************************************************************************
*/
uint32_t GetWavRuntime(uint8_t *_pPath, uint32_t *uiTotalTime)
{
	WavHead_T tWavHead;
	FILE *fout;
	
	/* 1. ��wav��Ƶ�ļ�������ͷ�ļ���Ϣ */
	fout = fopen((char *)_pPath, "r");
	
	printf_Wavdbg("%s\r\n", _pPath);
	if(fout != NULL)
	{
		LoadWavHead(fout, &tWavHead);
		fclose(fout);
	}
	else
	{
		return FALSE;		
	}

	/* 2. ���WAV��Ƶ��ִ��ʱ�䣬����ʱ��Ŵ�10������MP3����ͳһ */
	if((tWavHead.tFmt.uiSamplePerSec!=0 && tWavHead.tFmt.usChannels!=0) && (tWavHead.tFmt.usBitsPerSample!=0))
	{
		*uiTotalTime = tWavHead.tData.uiDataSize / tWavHead.tFmt.uiSamplePerSec / tWavHead.tFmt.usChannels;
		*uiTotalTime = *uiTotalTime * 80 / tWavHead.tFmt.usBitsPerSample;
		return FALSE;
	}
	
	return TRUE;
}

/*
*********************************************************************************************************
*	�� �� ��: GetWavInfo
*	����˵��: ר�����ڻ�ȡWav��Ƶ������ʱ�䣬����emWin��ʾ�����б�ʱʱ�����ʾ��
*	��    ��: _pPath       �ļ�����·��
*	�� �� ֵ: TRUE(1) ��ʾ�ɹ��� FALSE(0)��ʾʧ��
*********************************************************************************************************
*/
uint32_t GetWavInfo(uint8_t *_pPath)
{
	WavHead_T tWavHead;
	FILE *fout;

	/* 1. ��wav��Ƶ�ļ�������ͷ�ļ���Ϣ */	
	fout = fopen((char *)_pPath, "r");
	
	if(fout != NULL)
	{
		LoadWavHead(fout, &tWavHead);
		DumpWavHead(&tWavHead);
		fclose(fout);
		return TRUE;
	}
	
	return FALSE;
}

/*
*********************************************************************************************************
*	�� �� ��: WavMusicPlay
*	����˵��: ����WAV��Ƶ
*	��    ��: filename  ������·�� 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void WavMusicPlay(const char *filename)
{
	WavHead_T tWavHead;
	uint32_t uiNumBytesFill = Wav_DMABufferSize; 
	FILE *fout = NULL;
	OS_RESULT xResult = 0;
	uint8_t ucFileOpenFlag = 0;
	uint8_t *pI2SBuffer0;
	uint8_t *pI2SBuffer1;
	uint8_t *pI2STempBuff;   
	
	/* ��������Ķ�̬�ڴ� */
	pI2SBuffer0 = (uint8_t *)os_alloc_mem(AppMalloc, 10*1024);
	pI2SBuffer1 = (uint8_t *)os_alloc_mem(AppMalloc, 10*1024); 
	pI2STempBuff = (uint8_t *)os_alloc_mem(AppMalloc, 15*1024);
	
	printf_Wavdbg("pI2SBuffer0 = %x, %x, %x", (int)pI2SBuffer0, (int)pI2SBuffer1, (int)pI2STempBuff);	
	
	while(1)
	{ 
		if(os_evt_wait_or(0xFFFE, 0xFFFF) == OS_R_EVT)
		{
			xResult = os_evt_get ();
			switch (xResult)
			{
				/* �������emWin������Ļ�����λ�Ǳ�emWin������ʹ�� */
				case MusicTaskReserved_0:
					break;
				
				/* ʵ�ֿ������ */
				case MusicTaskAudioAdjust_1:
					Wav_TimeSet(fout, &tWavHead);
					break;

				/* ��ͣ */
				case MusicTaskAudioPause_2:
				    AUDIO_Pause();
					break;

				/* ���� */
				case MusicTaskAudioResume_3:
				    AUDIO_Resume();
					break;	

				/* ��λ��δʹ�� */
				case MusicTaskReserved_4:
					break;	
				
				/* ��ʼ���� */
				case MusicTaskAudioPlay_5:
					/* ��WAV��Ƶ�ļ�����������Ϣ */
				    fout = fopen (filename, "r"); 
					ucFileOpenFlag = 1;
					LoadWavHead(fout, &tWavHead);
					DumpWavHead(&tWavHead);
					Wav_TimeShow(fout, &tWavHead);

					/* ��˫������������� */
					uiNumBytesFill = Wav_FillAudio(fout, pI2SBuffer0, pI2STempBuff, Wav_DMABufferSize, tWavHead.tFmt.usBitsPerSample);
					uiNumBytesFill = Wav_FillAudio(fout, pI2SBuffer1, pI2STempBuff, Wav_DMABufferSize, tWavHead.tFmt.usBitsPerSample);

					/* ����STM32��SAI��Ƶ�ӿ� */	
					AUDIO_Init(1, I2S_Standard_Phillips, SAI_DataSize_16b, tWavHead.tFmt.uiSamplePerSec, tWavHead.tFmt.usChannels);
					Play_DMA_Init(pI2SBuffer0, pI2SBuffer1, Wav_DMABufferSize/2);
					AUDIO_Play();
					break;	
					
				/* �˳���Ƶ���� */				
				case MusicTaskAudioReturn_6:
					AUDIO_Stop();
					if(fout != NULL)
					{
						fclose (fout); 
					}
					os_free_mem(AppMalloc, pI2STempBuff);
					os_free_mem(AppMalloc, pI2SBuffer1);
					os_free_mem(AppMalloc, pI2SBuffer0);
					return;

				/* ��ȡ����������Ϣ */
				case MusicTaskAudioGetTime_7:
					if(ucFileOpenFlag == 1)
					{
						Wav_TimeShow(fout, &tWavHead);							
					}
					break;
					
				/* ��ʼ���ţ������л���һ��ʹ�� */
				case MusicTaskAudioStart_8:
					/* ��WAV��Ƶ�ļ�����������Ϣ */
				    fout = fopen (filename, "r"); 
					ucFileOpenFlag = 1;
					LoadWavHead(fout, &tWavHead);
					DumpWavHead(&tWavHead);
					Wav_TimeShow(fout, &tWavHead);

					/* ��˫������������� */
					uiNumBytesFill = Wav_FillAudio(fout, pI2SBuffer0, pI2STempBuff, Wav_DMABufferSize, tWavHead.tFmt.usBitsPerSample);
					uiNumBytesFill = Wav_FillAudio(fout, pI2SBuffer1, pI2STempBuff, Wav_DMABufferSize, tWavHead.tFmt.usBitsPerSample);

					/* ����STM32��SAI��Ƶ�ӿ� */	
					AUDIO_Init(1, I2S_Standard_Phillips, SAI_DataSize_16b, tWavHead.tFmt.uiSamplePerSec, tWavHead.tFmt.usChannels);
					Play_DMA_Init(pI2SBuffer0, pI2SBuffer1, Wav_DMABufferSize/2);
					AUDIO_Play();
					AUDIO_Pause();
					break;
					
				/* ��ǰʹ�õ��ǻ���0����仺��1 */
				case MusicTaskAudioFillBuffer1_9:
					DSP_FFT64(&pI2SBuffer0[Wav_DMABufferSize - 256]);
				    uiNumBytesFill = Wav_FillAudio(fout, 
				                                   pI2SBuffer1, 
				                                   pI2STempBuff, 
				                                   Wav_DMABufferSize, 
				                                   tWavHead.tFmt.usBitsPerSample);
					break;

				/* ��ǰʹ�õ��ǻ���1����仺��0 */
				case MusicTaskAudioFillBuffer0_10:
				   DSP_FFT64(&pI2SBuffer1[Wav_DMABufferSize - 256]);
				   uiNumBytesFill = Wav_FillAudio(fout, 
				                                  pI2SBuffer0, 
				                                  pI2STempBuff, 
				                                  Wav_DMABufferSize, 
				                                  tWavHead.tFmt.usBitsPerSample);
					break;	
				
				/* ��5λ��δʹ�� */
				default: 
					break;
			}	
		}
		
		/* ��һ����Ҫ���ڸ������Զ��л� */
		if(uiNumBytesFill < Wav_DMABufferSize)//���Ž���?
		{	
			/* �ȴ�����һ��������� */
			os_evt_wait_or(MusicTaskWaitBuffer, 0xFFFF);
			
			/* �������㣬��ֹ���ſ������ʱ���ظ����Ŵ洢����Ƶ���� */
			if(xResult & MusicTaskAudioFillBuffer1_9)
			{
				memset(pI2SBuffer1, 0, Wav_DMABufferSize);
			}
			else
			{
				memset(pI2SBuffer0, 0, Wav_DMABufferSize);
			}
			
			/* �ȴ���һ��������� */
			os_evt_wait_or(MusicTaskWaitBuffer, 0xFFFF);
			
			if(xResult & MusicTaskAudioFillBuffer1_9)
			{
				memset(pI2SBuffer1, 0, Wav_DMABufferSize);
			}
			else
			{
				memset(pI2SBuffer0, 0, Wav_DMABufferSize);
			}
			
			/* �ȴ�һ�β���ʱ���ȡ����֤���ֲ��ŵĽ�����������ʾ��� */
			os_evt_wait_or(MusicTaskAudioGetTime_7, 0xFFFF);
			Wav_TimeShow(fout, &tWavHead);	
			os_dly_wait(800);
				
			/* �ر�SAI��Ƶ�ӿں�MP3�ļ� */
			AUDIO_Stop();
			
			if(fout != NULL)
			{
				fclose (fout); 
			}

			/* �ͷ���ض�̬�ڴ� */
			os_free_mem(AppMalloc, pI2STempBuff);
			os_free_mem(AppMalloc, pI2SBuffer1);
			os_free_mem(AppMalloc, pI2SBuffer0);
			
			/* ��emWin�����ֲ���������Ϣ���л�����һ�׸������� */
			WM_SendMessageNoPara(hWinMusic, MSG_NextMusic);	
			return;			
		} 	
	}
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

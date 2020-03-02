/*
*********************************************************************************************************
*
*	模块名称 : WAV播放应用
*	文件名称 : app_wav_lib.c
*	版    本 : V1.0
*	说    明 : WAV软解的实现，WAV相对比较简单，除了头文件，剩下的就是音频数据。
*              1. 函数LoadWavHead和DumpWavHead用于WAV文件的解析。
*              2. WavMusicPlay的实现是基于RTX操作系统的。
*              3. 支持上一曲，下一曲，快进和快退，常用的采样率都支持，单声道和立体声也都支持。
*                 当前仅支持16位音频，如果是24位的WAV会被转换成16位进行播放。
*              4. 仅g_tWav是全局变量，其余都是使用局部变量和动态内存。
*
*	修改记录 :
*		版本号    日期         作者         说明
*       V1.0    2016-02-16   Eric2013       首发
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
	#define printf_Wavdbg printf
#else
	#define printf_Wavdbg(...)
#endif


/*
*********************************************************************************************************
*	                                         宏定义
*********************************************************************************************************
*/
#define Wav_DMABufferSize     1024*10  /* 定义的DMA双缓冲大小，单位字节 */


/*
*********************************************************************************************************
                                          文件内部调用的函数
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
*	函 数 名: LoadWavHead
*	功能说明: 解析Wav音频头文件
*	形    参: _pFile     文件句柄
*             _WavHead   Wav头文件结构体变量地址
*	返 回 值: TRUE(1) 表示成功， FALSE(0)表示失败
*********************************************************************************************************
*/
static uint8_t LoadWavHead(FILE *_pFile, WavHead_T *_WavHead)
{
	RiffHead_T   tRiffChunk;
	FactBlock_T  tFactChunk;
	
	/* 定位到文件起始地址进行读取 */
	fseek (_pFile, 0, SEEK_SET);

	/* 第1步：先读取RiffChunk并验证ID和Type是否正确 *******************************************************/
	fread(&tRiffChunk, sizeof(tRiffChunk), 1, _pFile);
	if(strncmp((char *)tRiffChunk.ucRiffID, "RIFF", 4) || strncmp((char *)tRiffChunk.ucWaveID, "WAVE", 4))
	{
		return FALSE;	
	}
		
	/* 第2步：读取FmtChunk ******************************************************************************/
	fread(&_WavHead->tFmt, 1, sizeof(_WavHead->tFmt)-2, _pFile);
	/*
	   一般情况下uiSize为16，此时最后附加信息没有；如果为18则最后多了2个字节的附加信息。
       主要由一些软件制成的wav格式中含有该2个字节的，如果有附件信息则进行读取。
	*/
	if(_WavHead->tFmt.uiSize == 18)
	{
		fread(_WavHead->tFmt.ucAddtional, 1, 2, _pFile);
	}
		
	/* 第3步：读取FactChunk ****************************************************************************/
	fread(&tFactChunk, 1, sizeof(tFactChunk), _pFile);
	/* 检查是否为FactChunk，如果不是执行如下代码 */
	if(strncmp((char *)tFactChunk.ucFactID, "fact", 4))
	{
		/* 进一步检查是否为DataChunk*/
		if(strncmp((char*)tFactChunk.ucFactID, "data", 4)==0)
		{
			memcpy(_WavHead->tData.ucDataID, tFactChunk.ucFactID, 4);
			_WavHead->tData.uiDataSize = tFactChunk.uiFactSize;
			/* tRiffChunk + FmtChunk前两个成员 + DataBlock_T = 12 + 8 + 8 = 28 */
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
	/* 检查是否为FactChunk，如果是执行如下代码 */
	else 
	{
		fseek (_pFile, ftell(_pFile)+tFactChunk.uiFactSize, SEEK_SET);
	}
	
	/* 第4步：读取DataChunk ***********************************************************************/
	fread(&_WavHead->tData, 1, sizeof(_WavHead->tData), _pFile);
	if(strncmp((char*)_WavHead->tData.ucDataID,"data",4))
	{
		return FALSE;
	}
	
	/* tRiffChunk + FmtChunk前两个成员 + FactChunk前两个成员 + DataBlock_T = 12 + 8 + 8 + 8 = 36 */
	_WavHead->uiDataOffset = 36 + _WavHead->tFmt.uiSize +  tFactChunk.uiFactSize;
	
	return TRUE;
}

/*
*********************************************************************************************************
*	函 数 名: DumpWavHead
*	功能说明: 解析Wav音频头文件，获得音频执行时间和相关的音频信息记录到变量g_tWav.usInfo里面
*	形    参: _WavHead   Wav头文件结构体变量地址
*	返 回 值: 无
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
	
	/* 1. 通过采样率获得采样率的字符型 */
	for(n = 0; n < (sizeof(g_tDispFS)/sizeof(g_tDispFS[0])); n++)
	{
		if(_WavHead->tFmt.uiSamplePerSec == g_tDispFS[n].Fs)
		{
			break;
		}	
	}
	
	/* 2. 记录采样率，采样位数和立体声（单通道）到变量g_tWav.usInfo里面 */
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
	
	/* 3.获得音频的执行时间 */
	if((_WavHead->tFmt.uiSamplePerSec != 0 && _WavHead->tFmt.usChannels != 0) && (_WavHead->tFmt.usBitsPerSample != 0))
	{
		g_tWav.uiTotalTime = _WavHead->tData.uiDataSize / _WavHead->tFmt.uiSamplePerSec / _WavHead->tFmt.usChannels;
		/* 播放时间放大10倍，跟MP3保持统一 */
		g_tWav.uiTotalTime = g_tWav.uiTotalTime * 80 / _WavHead->tFmt.usBitsPerSample;
		
		printf_Wavdbg("uiTotalTime : %d:%d\r\n", g_tWav.uiTotalTime/600, g_tWav.uiTotalTime/10%60);
	}
}

/*
*********************************************************************************************************
*	函 数 名: Wav_FillAudio
*	功能说明: 读取音频数据填充到DMA的双缓冲中
*	形    参: _pFile            FIL文件
*             _ptr              读取的音频数据放到此缓存中
*             _ptrtemp          用于数据的临时存放
*             _usSize           要读取的数据大小
*             usBitsPerSample   音频采样大小，16位或者24位
*	返 回 值: 返回实际读取的字节数
*********************************************************************************************************
*/
static uint32_t Wav_FillAudio(FILE *_pFile, uint8_t *_ptr, uint8_t *_ptrtemp, uint16_t _usSize, uint8_t usBitsPerSample)
{
	uint32_t NumBytesReq;
	uint32_t NumBytesRead;
	uint32_t i;
	uint8_t *p;
	
	/* 24位音频转换为16位进行播放，SAI音频接口的24位音频播放暂时未解决 */
	if(usBitsPerSample == 24)
	{
		/* 需要读取的字节数 */
		NumBytesReq=(_usSize / 2) * 3;						

		NumBytesRead = fread((void *)_ptrtemp, 1, NumBytesReq, _pFile);
		p = _ptrtemp;
		for(i = 0; i < _usSize; i += 2, p += 3)
		{
			_ptr[i]   = p[1];
			_ptr[i+1] = p[2]; 
		} 
		
		/* 实际需读取的字节数 */
		NumBytesRead = (NumBytesRead / 3) * 2;		
	}
	/* 16位音频 */
	else 
	{
		NumBytesRead = fread((void *)_ptr, 1, _usSize, _pFile);
	}
	
	if(NumBytesRead < _usSize)
	{
		/* 不足的补0 */
		for(i = NumBytesRead; i < _usSize; i++)
		{
			_ptr[i] = 0; 				
		}
	}
	
	return NumBytesRead;
} 

/*
*********************************************************************************************************
*	函 数 名: Wav_TimeShow
*	功能说明: 用于获取和设置音频当前的播放时间 
*	形    参: _pFile   FIL文件
*             _WavHead Wav头文件结构体变量地址
*	返 回 值: 无
*********************************************************************************************************
*/
static void Wav_TimeShow(FILE *_pFile, WavHead_T * _WavHead)
{	
	uint32_t uiPos;
	
	uiPos = ftell(_pFile) -  _WavHead->uiDataOffset;
	g_tWav.uiCurTime  = uiPos / _WavHead->tFmt.uiSamplePerSec / _WavHead->tFmt.usChannels;
	/* 播放时间放大10倍，跟MP3保持统一 */
	g_tWav.uiCurTime  = g_tWav.uiCurTime * 80/ _WavHead->tFmt.usBitsPerSample;
}

static void Wav_TimeSet(FILE *_pFile, WavHead_T * _WavHead)
{	
	uint32_t uiPos;
	
	/* 播放时间倍放大10倍，跟MP3保持统一 */
	uiPos  = g_tWav.uiCurTime * _WavHead->tFmt.usBitsPerSample / 80;
	uiPos = uiPos *  _WavHead->tFmt.usChannels *  _WavHead->tFmt.uiSamplePerSec;
	uiPos = _WavHead->uiDataOffset + uiPos;
	fseek (_pFile, uiPos, SEEK_SET);
}

/*
*********************************************************************************************************
*	函 数 名: GetWavRuntime
*	功能说明: 专门用于获取Wav音频的运行时间，方便emWin显示歌曲列表时时间的显示。
*	形    参: _pPath       文件所在路径
*             uiTotalTime  歌曲执行时间
*	返 回 值: TRUE(1) 表示成功， FALSE(0)表示失败
*********************************************************************************************************
*/
uint32_t GetWavRuntime(uint8_t *_pPath, uint32_t *uiTotalTime)
{
	WavHead_T tWavHead;
	FILE *fout;
	
	/* 1. 打开wav音频文件解析其头文件信息 */
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

	/* 2. 获得WAV音频的执行时间，播放时间放大10倍，跟MP3保持统一 */
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
*	函 数 名: GetWavInfo
*	功能说明: 专门用于获取Wav音频的运行时间，方便emWin显示歌曲列表时时间的显示。
*	形    参: _pPath       文件所在路径
*	返 回 值: TRUE(1) 表示成功， FALSE(0)表示失败
*********************************************************************************************************
*/
uint32_t GetWavInfo(uint8_t *_pPath)
{
	WavHead_T tWavHead;
	FILE *fout;

	/* 1. 打开wav音频文件解析其头文件信息 */	
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
*	函 数 名: WavMusicPlay
*	功能说明: 播放WAV音频
*	形    参: filename  歌曲的路径 
*	返 回 值: 无
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
	
	/* 申请所需的动态内存 */
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
				/* 如果启用emWin多任务的话，此位是被emWin多任务使用 */
				case MusicTaskReserved_0:
					break;
				
				/* 实现快进快退 */
				case MusicTaskAudioAdjust_1:
					Wav_TimeSet(fout, &tWavHead);
					break;

				/* 暂停 */
				case MusicTaskAudioPause_2:
				    AUDIO_Pause();
					break;

				/* 继续 */
				case MusicTaskAudioResume_3:
				    AUDIO_Resume();
					break;	

				/* 此位暂未使用 */
				case MusicTaskReserved_4:
					break;	
				
				/* 开始播放 */
				case MusicTaskAudioPlay_5:
					/* 打开WAV音频文件并获得相关信息 */
				    fout = fopen (filename, "r"); 
					ucFileOpenFlag = 1;
					LoadWavHead(fout, &tWavHead);
					DumpWavHead(&tWavHead);
					Wav_TimeShow(fout, &tWavHead);

					/* 往双缓冲中填充数据 */
					uiNumBytesFill = Wav_FillAudio(fout, pI2SBuffer0, pI2STempBuff, Wav_DMABufferSize, tWavHead.tFmt.usBitsPerSample);
					uiNumBytesFill = Wav_FillAudio(fout, pI2SBuffer1, pI2STempBuff, Wav_DMABufferSize, tWavHead.tFmt.usBitsPerSample);

					/* 配置STM32的SAI音频接口 */	
					AUDIO_Init(1, I2S_Standard_Phillips, SAI_DataSize_16b, tWavHead.tFmt.uiSamplePerSec, tWavHead.tFmt.usChannels);
					Play_DMA_Init(pI2SBuffer0, pI2SBuffer1, Wav_DMABufferSize/2);
					AUDIO_Play();
					break;	
					
				/* 退出音频播放 */				
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

				/* 获取歌曲播放信息 */
				case MusicTaskAudioGetTime_7:
					if(ucFileOpenFlag == 1)
					{
						Wav_TimeShow(fout, &tWavHead);							
					}
					break;
					
				/* 开始播放，歌曲切换下一曲使用 */
				case MusicTaskAudioStart_8:
					/* 打开WAV音频文件并获得相关信息 */
				    fout = fopen (filename, "r"); 
					ucFileOpenFlag = 1;
					LoadWavHead(fout, &tWavHead);
					DumpWavHead(&tWavHead);
					Wav_TimeShow(fout, &tWavHead);

					/* 往双缓冲中填充数据 */
					uiNumBytesFill = Wav_FillAudio(fout, pI2SBuffer0, pI2STempBuff, Wav_DMABufferSize, tWavHead.tFmt.usBitsPerSample);
					uiNumBytesFill = Wav_FillAudio(fout, pI2SBuffer1, pI2STempBuff, Wav_DMABufferSize, tWavHead.tFmt.usBitsPerSample);

					/* 配置STM32的SAI音频接口 */	
					AUDIO_Init(1, I2S_Standard_Phillips, SAI_DataSize_16b, tWavHead.tFmt.uiSamplePerSec, tWavHead.tFmt.usChannels);
					Play_DMA_Init(pI2SBuffer0, pI2SBuffer1, Wav_DMABufferSize/2);
					AUDIO_Play();
					AUDIO_Pause();
					break;
					
				/* 当前使用的是缓冲0，填充缓冲1 */
				case MusicTaskAudioFillBuffer1_9:
					DSP_FFT64(&pI2SBuffer0[Wav_DMABufferSize - 256]);
				    uiNumBytesFill = Wav_FillAudio(fout, 
				                                   pI2SBuffer1, 
				                                   pI2STempBuff, 
				                                   Wav_DMABufferSize, 
				                                   tWavHead.tFmt.usBitsPerSample);
					break;

				/* 当前使用的是缓冲1，填充缓冲0 */
				case MusicTaskAudioFillBuffer0_10:
				   DSP_FFT64(&pI2SBuffer1[Wav_DMABufferSize - 256]);
				   uiNumBytesFill = Wav_FillAudio(fout, 
				                                  pI2SBuffer0, 
				                                  pI2STempBuff, 
				                                  Wav_DMABufferSize, 
				                                  tWavHead.tFmt.usBitsPerSample);
					break;	
				
				/* 后5位暂未使用 */
				default: 
					break;
			}	
		}
		
		/* 这一步主要用于歌曲的自动切换 */
		if(uiNumBytesFill < Wav_DMABufferSize)//播放结束?
		{	
			/* 等待其中一个缓冲完成 */
			os_evt_wait_or(MusicTaskWaitBuffer, 0xFFFF);
			
			/* 缓冲清零，防止播放快结束的时候重复播放存储的音频数据 */
			if(xResult & MusicTaskAudioFillBuffer1_9)
			{
				memset(pI2SBuffer1, 0, Wav_DMABufferSize);
			}
			else
			{
				memset(pI2SBuffer0, 0, Wav_DMABufferSize);
			}
			
			/* 等待另一个缓冲完成 */
			os_evt_wait_or(MusicTaskWaitBuffer, 0xFFFF);
			
			if(xResult & MusicTaskAudioFillBuffer1_9)
			{
				memset(pI2SBuffer1, 0, Wav_DMABufferSize);
			}
			else
			{
				memset(pI2SBuffer0, 0, Wav_DMABufferSize);
			}
			
			/* 等待一次播放时间获取，保证音乐播放的进度条完整显示完毕 */
			os_evt_wait_or(MusicTaskAudioGetTime_7, 0xFFFF);
			Wav_TimeShow(fout, &tWavHead);	
			os_dly_wait(800);
				
			/* 关闭SAI音频接口和MP3文件 */
			AUDIO_Stop();
			
			if(fout != NULL)
			{
				fclose (fout); 
			}

			/* 释放相关动态内存 */
			os_free_mem(AppMalloc, pI2STempBuff);
			os_free_mem(AppMalloc, pI2SBuffer1);
			os_free_mem(AppMalloc, pI2SBuffer0);
			
			/* 给emWin的音乐播放器发消息，切换到下一首歌曲播放 */
			WM_SendMessageNoPara(hWinMusic, MSG_NextMusic);	
			return;			
		} 	
	}
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

/*
*********************************************************************************************************
*
*	模块名称 : MP3播放应用
*	文件名称 : app_mp3_lib.c
*	版    本 : V1.0
*	说    明 : MP3播放主要是移植了Helix Decoder库实现软解。
*              1. 函数DecodeID3V1和DecodeID3V2用于MP3文件标签的解析，函数GetMP3TagInfo用于
*                 解析和相关信息记录。
*              2. MP3MusicPlay的实现是基于RTX操作系统的。
*              3. 支持上一曲，下一曲，快进和快退，常用的采样率和码率都支持，单声道和立体声也都支持。
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
	#define printf_MP3dbg printf
#else
	#define printf_MP3dbg(...)
#endif


/*
*********************************************************************************************************
                                          宏定义
*********************************************************************************************************
*/
#define MP3TimeScale        10       /* 将MP3的播放时间放大10倍，方便跟视频播放同步 */
#define DecoderMP3BufSize   5*1024	 /* 解码MP3缓冲大小，单位字节 */
#define MaxInputBufSize     1940     /* 输入缓冲区，最大MP3帧大小，单位字节 */
#define MaxOutputBufSize    4*1152   /* 输出缓冲区，处理立体声音频数据时，输出缓冲区最大值，单位字节 */


/*
*********************************************************************************************************
                                          文件内部调用的函数
*********************************************************************************************************
*/
static uint8_t DecodeID3V1(uint8_t *_pBuf,  MP3Control_T *pMP3Control);
static uint8_t DecodeID3V2(uint8_t *_pBuf, uint32_t _uiSize, MP3Control_T *pMP3Control);
static uint8_t GetMP3TagInfo(uint8_t *pFileName, MP3Control_T *pMP3Control);
static void MP3_TimeShow(FILE *_pFile, MP3Control_T *pMP3Control);
static void MP3_TimeSet(FILE *_pFile, MP3Control_T *pMP3Control);
static void MP3_FillAudio(uint16_t *_ptr,  uint16_t *_ptrtemp, uint16_t _uiSize);
static void GetMP3DecoderData(FILE *pFile,
                              uint8_t *_pBuf,
                              MP3FrameInfo *mp3frameinfo,
                              HMP3Decoder mp3Decoder,
                              MP3Control_T *pMP3Control);


/*
*********************************************************************************************************
*	函 数 名: DecodeID3V1
*	功能说明: 解析MP3的ID3V1标签，并获取歌曲名和歌曲作者
*	形    参: _pBuf  128字节的缓冲地址
*	返 回 值: TRUE(1) 表示成功， FALSE(0)表示失败
*********************************************************************************************************
*/
static uint8_t DecodeID3V1(uint8_t *_pBuf,  MP3Control_T *pMP3Control)
{
    ID3V1_T *ptID3V1_Tag;

    ptID3V1_Tag=(ID3V1_T *)_pBuf;

    /* 判断ID3V1标签的前三个字母是否为TAG */
    if(strncmp("TAG", (char *)ptID3V1_Tag->ucTAG, 3) == 0)
    {
        /* 获得歌曲名 */
        strncpy((char *)pMP3Control->ucTitle, (char *)ptID3V1_Tag->ucSongTitle, 30);

        /* 获得歌曲名作者 */
        strncpy((char *)pMP3Control->ucArtist, (char *)ptID3V1_Tag->ucArtist, 30);
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}

/*
*********************************************************************************************************
*	函 数 名: DecodeID3V2
*	功能说明: 解析MP3的ID3V2标签，获取数据开始位置，标题和作者。
*             每个ID3V2.3的标签都由一个标签头和若干个标签帧或一个扩展标签头组成。
*             关于曲目的信息，如标题、作者等都存放在不同的标签帧中，扩展标签头和标签帧
*             并不是必要的，但每个标签至少要有一个标签帧。标签头和标签帧一起顺序存放在
*             mp3文件的首部。
*	形    参: _pBuf        缓冲地址
*             _uiSize      缓冲大小
*             pMP3Control
*	返 回 值: TRUE(1) 表示成功， FALSE(0)表示失败
*********************************************************************************************************
*/
static uint8_t DecodeID3V2(uint8_t *_pBuf, uint32_t _uiSize, MP3Control_T *pMP3Control)
{
    ID3V2Header_T *tID3V2Header;
    ID3V23FrameHead_T *tID3V23FrameHead;
    uint32_t uiCount;
    uint32_t uiTagSize;
    uint32_t uiFrameSize;
    uint32_t uiFrameHeaderSize;


    tID3V2Header = (ID3V2Header_T*)_pBuf;

    if(strncmp("ID3", (const char*)tID3V2Header->ucID3, 3) == 0)
    {
        /*
          1. 所有标签帧的大小，不包含标签头的10个字节
          2. 标签大小一共4个字节，但每个字节只用了7位，最高位不使用恒为0，计算大小时
             要将0去掉，得到一个28位的2进制数，就是标签大小。
        */
        uiTagSize = ((uint32_t)tID3V2Header->ucSize[0] << 21)
                    |((uint32_t)tID3V2Header->ucSize[1] << 14)
                    |((uint32_t)tID3V2Header->ucSize[2] << 7)
                    |(uint32_t)tID3V2Header->ucSize[3];

        printf_MP3dbg("uiTagSize = %d\r\n", uiTagSize);

        /* 数据开始位置，不包括标签头的10个字节 */
        pMP3Control->uiFrameStart = uiTagSize + 10;

        /* ID3V2.3版本及其以上 */
        if(tID3V2Header->ucMajorVer >= 3)
        {
            uiFrameHeaderSize = 10;
        }
        /* ID3V2.3版本以下版本未支持 */
        else
        {
            return FALSE;
        }

        /* 防止标签大小大于缓冲大小 */
        if(uiTagSize > _uiSize)
        {
            uiTagSize = _uiSize;
        }

        /* 头标签是10个字节，从10开始获取数据 */
        uiCount = 10;

        while(uiCount < uiTagSize)
        {
            tID3V23FrameHead=(ID3V23FrameHead_T*)(_pBuf + uiCount);

            /*  每个标签帧都有一个10个字节的帧头和至少一个字节的不固定长度的内容组成 */
            uiFrameSize = ((uint32_t)tID3V23FrameHead->ucSize[0] << 24)
                          |((uint32_t)tID3V23FrameHead->ucSize[1] << 16)
                          |((uint32_t)tID3V23FrameHead->ucSize[2] << 8)
                          |(uint32_t)tID3V23FrameHead->ucSize[3];

            /*
                帧标识：
            		TIT2  标题
            		TPE1  作者
            		TALB  专辑
            		TRCK  音轨
            		TYER  年代
            		TCON  类型
            		COMM  备注
            */
            /* 找到标题并获取 */
            if (strncmp("TT2", (char*)tID3V23FrameHead->ucFrameID, 3) == 0
                    ||strncmp("TIT2",(char*)tID3V23FrameHead->ucFrameID, 4) == 0)
            {
                strncpy((char *)pMP3Control->ucTitle,
                        (char *)(_pBuf + uiCount + uiFrameHeaderSize + 1),
                        MIN(uiFrameSize - 1, 40 - 1));
            }

            /* 找到作者并获取 */
            if (strncmp("TP1",(char*)tID3V23FrameHead->ucFrameID,3) == 0
                    ||strncmp("TPE1",(char*)tID3V23FrameHead->ucFrameID,4) == 0)
            {
                strncpy((char *)pMP3Control->ucArtist,
                        (char *)(_pBuf + uiCount + uiFrameHeaderSize + 1),
                        MIN(uiFrameSize - 1, 40 - 1));
            }

            uiCount += uiFrameSize + uiFrameHeaderSize;
        }
    }
    else
    {
        /* 不存在ID3V2标签 */
        pMP3Control->uiFrameStart = 0;
    }

    return TRUE;
}

/*
*********************************************************************************************************
*	函 数 名: GetMP3Info
*	功能说明: 解析MP3的ID3V1和ID3V2标签，以及MP3播放时间的获取。
*	形    参: pFileName    文件路径
*             pMP3Control  MP3Control_T类型结构体变量
*	返 回 值: TRUE(1) 表示成功， FALSE(0)表示失败
*********************************************************************************************************
*/
static uint8_t GetMP3TagInfo(uint8_t *pFileName, MP3Control_T *pMP3Control)
{
    MP3FrameXing_T *pFrameXing;
    MP3FrameVBRI_T *pFrameVBRI;
    FILE *fout_mp3 = NULL;
    uint8_t *buf;
    uint32_t uiNumOfReadBytes;
    int32_t  offset;
    uint32_t p;
    uint32_t SamplesPerFrame;
    uint32_t uiTotalFrames;
    HMP3Decoder Decoder;
    MP3FrameInfo tFrameInfo;

    /* 第1步：申请10KB的空间并进行ID3V2标签解析 ------------------------------------------*/
    buf = (uint8_t *)os_alloc_mem(AppMalloc, 10*1024);
    fout_mp3 = fopen ((const char *)pFileName, "r");

    if(fout_mp3 == NULL)
    {
        return FALSE;
    }

    /* 解析ID3V2，读取前10KB数据 */
    uiNumOfReadBytes = fread(buf, 1, 10*1024, fout_mp3);
    DecodeID3V2(buf, uiNumOfReadBytes, pMP3Control);

    /* 第2步：解析ID3V1标签 ------------------------------------------*/
    /*
       解析ID3V1数据, 读取文件末尾128字节即可
       有些MP3音频文件没有ID3V2标签，ID3V1可以不使用
    */
    fseek (fout_mp3, -128L, SEEK_END);
    fread(buf, 1, 128, fout_mp3);
    DecodeID3V1(buf, pMP3Control);

    /* 第3步：MP3解码 -----------------------------------------------*/
    Decoder = MP3InitDecoder();
    fseek(fout_mp3, pMP3Control->uiFrameStart, SEEK_SET);
    uiNumOfReadBytes = fread(buf, 1, 10*1024, fout_mp3);

    /* 查找帧同步信息 */
    offset = MP3FindSyncWord(buf, uiNumOfReadBytes);

    /* 找到帧同步信息了,且下一阵信息获取正常 */
    if(offset >= 0
            && MP3GetNextFrameInfo(Decoder, &tFrameInfo, &buf[offset]) == 0)
    {
        /*
          VBRI头起始位置 = MPEG第一帧帧头起始位置 +  帧头大小 + 32。
          帧头大小 = 4（或6,当Protection bit==0时，帧头后会有16bit=2byte的CRC,bi5）。
        */
        if(buf[offset + 1] & 0x01)
        {
            p = offset + 4 + 32;
        }
        else
        {
            p = offset + 6 + 32;
        }

        pFrameVBRI = (MP3FrameVBRI_T*)(buf + p);
		
		/* VBR格式 VBRI帧 */
		if(strncmp("VBRI", (char*)pFrameVBRI->ucVBRI, 4)==0)
		{
			if (tFrameInfo.version == MPEG1)
			{
				/* MPEG1, Layer III每帧的MPEG音频数据的样本数1152 */
				SamplesPerFrame = 1152;			
			}
			else 
			{
				/* MPEG2或者MPEG2.5, Layer III每帧的MPEG音频数据的样本数576 */
				SamplesPerFrame = 576; 
			}
			
			uiTotalFrames = ((uint32_t)pFrameVBRI->ucFrames[0] << 24)
			                |((uint32_t)pFrameVBRI->ucFrames[1] << 16)
			                |((uint32_t)pFrameVBRI->ucFrames[2] << 8)
			                |(uint32_t)pFrameVBRI->ucFrames[3];
			
			  /* 音频播放总时长 */
            g_tWav.uiTotalTime = uiTotalFrames * SamplesPerFrame * MP3TimeScale / tFrameInfo.samprate;

            /* 记录是VBR还是CBR格式 */
            pMP3Control->ucFmtVBRCBR = 1;
			
			printf_MP3dbg("VBRI = VBR\r\n");
			
		}
		/* VBR格式Xing帧或者CBR格式，这一部分区分出来的VBR和CBR格式还不够准确 */
		else	
		{  
			if (tFrameInfo.version==MPEG1)	
			{
				/* 边界信息大小 */
				p = tFrameInfo.nChans == 2? 32 : 17;
				
				/* MPEG1, Layer III每帧的MPEG音频数据的样本数1152 */
				SamplesPerFrame = 1152;	
			}
			else
			{
				/* 边界信息大小 */
				p = tFrameInfo.nChans == 2? 17 : 9;
				
				/* MPEG2或者MPEG2.5, Layer III每帧的MPEG音频数据的样本数576 */
				SamplesPerFrame = 576;		
			}
			
			/* 此处存在疑问，感觉应该分4和6两种情况 */
			p += offset + 4;
			pFrameXing = (MP3FrameXing_T *)(buf + p);
			
			/* VBR格式,查询是否是xing帧 ******************************/
			if(strncmp("Xing",(char*)pFrameXing->ucXing, 4) == 0)
			{
				/* 存在Frames域 */
				if(pFrameXing->ucFlags[3] & 0x01)
				{
					/* 得到总帧数 */
					uiTotalFrames = ((uint32_t)pFrameXing->ucFrames[0] << 24)
								  |((uint32_t)pFrameXing->ucFrames[1] << 16)
								  |((uint32_t)pFrameXing->ucFrames[2] << 8)
								  |(uint32_t)pFrameXing->ucFrames[3];

					/* 音频播放总时长 */
					g_tWav.uiTotalTime = uiTotalFrames*SamplesPerFrame * MP3TimeScale / tFrameInfo.samprate;

					printf_MP3dbg("Xing === VBR \r\n");
				}
				/* 不存在Frames域 */
				else	
				{
					fseek (fout_mp3, 0L, SEEK_END);
                    g_tWav.uiTotalTime = (ftell(fout_mp3) -pMP3Control->uiFrameStart) * MP3TimeScale / (tFrameInfo.bitrate/8);
					
					printf_MP3dbg("Xing === CBR \r\n");
				} 
				
				/* 记录是VBR格式 */
				pMP3Control->ucFmtVBRCBR = 1;
			}
			/* CBR格式 **************************************************/
			else if(strncmp("Info", (char*)pFrameXing->ucXing, 4) == 0)
			{
				/* 存在Frames域 */
				if(pFrameXing->ucFlags[3] & 0x01)
				{
					/* 得到总帧数 */
					uiTotalFrames = ((uint32_t)pFrameXing->ucFrames[0] << 24)
								  |((uint32_t)pFrameXing->ucFrames[1] << 16)
								  |((uint32_t)pFrameXing->ucFrames[2] << 8)
								  |(uint32_t)pFrameXing->ucFrames[3];

					/* 音频播放总时长 */
                    g_tWav.uiTotalTime = uiTotalFrames*SamplesPerFrame * MP3TimeScale / tFrameInfo.samprate;
					
					printf_MP3dbg("info === VBR \r\n");
				}
				/* 不存在Frames域 */
				else	
				{
					fseek (fout_mp3, 0L, SEEK_END);
                    g_tWav.uiTotalTime = (ftell(fout_mp3) -pMP3Control->uiFrameStart) * MP3TimeScale / (tFrameInfo.bitrate/8);
					printf_MP3dbg("info === CBR \r\n");
                }
					
				/* 记录是CBR格式 */
				pMP3Control->ucFmtVBRCBR = 2;
			}
			/* CBR格式 **************************************************/
			else 		
			{
				fseek (fout_mp3, 0L, SEEK_END);
                g_tWav.uiTotalTime = (ftell(fout_mp3) -pMP3Control->uiFrameStart) * MP3TimeScale / (tFrameInfo.bitrate/8);
				
				/* 记录是CBR格式 */
				pMP3Control->ucFmtVBRCBR = 2;
				
				printf_MP3dbg("LAST === CBR \r\n");
			}
		} 

        /* 记录码率 */
        pMP3Control->uiBitrate = tFrameInfo.bitrate;
        /* 记录采样率 */
        pMP3Control->uiSamplerate = tFrameInfo.samprate;

        if(tFrameInfo.nChans == 2)
        {
            /* 记录声道数*/
            pMP3Control->usChannels = 2;
            pMP3Control->usOutsamples = tFrameInfo.outputSamps;
        }
        else
        {
          /* 跟函数MP3_FillAudio要统一 */
			#if 1
				/* 输出单声道，即用SAI音频接口的一个slot */
				pMP3Control->usOutsamples = tFrameInfo.outputSamps;
			#else
				/* 改为输出双声道，即用SAI音频接口的两个slot */			
				pMP3Control->usOutsamples = tFrameInfo.outputSamps * 2;			
			#endif
        }
    }
    else
    {
        return FALSE;
    }

    /* 第4步：释放动态内存并关闭相关此打开的文件 -----------------------------------------------*/
    MP3FreeDecoder(Decoder);
    fclose(fout_mp3);
    os_free_mem(AppMalloc, buf);
    return TRUE;
}

/*
*********************************************************************************************************
*	函 数 名: MP3_TimeShow
*	功能说明: 用于获取音频当前的播放时间，注意返回时间的单位是0.1s，也就是放大了10倍
*	形    参: _pFile      FIL文件
*             pMP3Control MP3Control_T类型结构体指针变量
*	返 回 值: 无
*********************************************************************************************************
*/
static void MP3_TimeShow(FILE *_pFile, MP3Control_T *pMP3Control)
{
    uint64_t uiPos;
    uint32_t uiTempPos;

    /* 获取当前播放位置 */
    uiTempPos = ftell(_pFile);
    uiPos = uiTempPos - pMP3Control->uiFrameStart;

    /* 通过这种方法获得文件的大小 */
    fseek (_pFile, 0L, SEEK_END);
    /* 通过这种方法获得文件的大小 */
    g_tWav.uiCurTime = (uint64_t)(uiPos * g_tWav.uiTotalTime) / (ftell(_pFile) -  pMP3Control->uiFrameStart);

    /* 恢复当前正在播放的位置 */
    fseek (_pFile, uiTempPos, SEEK_SET);
}

/*
*********************************************************************************************************
*	函 数 名: MP3_TimeSet
*	功能说明: 用于设置MP3播放位置
*	形    参: _pFile      FIL文件
*             pMP3Control MP3Control_T类型结构体指针变量
*	返 回 值: 无
*********************************************************************************************************
*/
static void MP3_TimeSet(FILE *_pFile, MP3Control_T *pMP3Control)
{
    uint64_t uiPos;

    fseek (_pFile, 0L, SEEK_END);
    uiPos  = (uint64_t)(ftell(_pFile) -  pMP3Control->uiFrameStart) * g_tWav.uiCurTime / g_tWav.uiTotalTime ;
    uiPos = uiPos + pMP3Control->uiFrameStart;
    fseek (_pFile, uiPos, SEEK_SET);
}

/*
*********************************************************************************************************
*	函 数 名: MP3_FillAudio
*	功能说明: 将解码出的数据填充到DMA的缓冲中
*	形    参: _ptr      DMA缓冲地址
*             _ptrtemp  解码出的实际地址
*             _uiSize   要填充的数据大小
*	返 回 值: 无
*********************************************************************************************************
*/
static void MP3_FillAudio(uint16_t *_ptr,  uint16_t *_ptrtemp, uint16_t _uiSize)
{
    uint16_t i;

#if 1
    for(i = 0; i < _uiSize; i++)
    {
        _ptr[i] = _ptrtemp[i];
    }
#else
    if(_ucChannelMode == 2)
    {
        /* 双通道 */
        for(i = 0; i < _uiSize; i++)
        {
            _ptr[i] = _ptrtemp[i];
        }
    }
    else
    {
        /* 单通道转换为双通道 */
        for (i = _uiSize/2 - 1; i >= 0; i--)
        {
            _ptr[i * 2] = _ptrtemp[i];
            _ptr[i * 2 + 1] = _ptrtemp[i];
        }
    }
#endif
}

/*
*********************************************************************************************************
*	函 数 名: GetMP3DecoderData
*	功能说明: 解码一帧MP3音频数据。
*	形    参: pFile         文件系统句柄
*             _pBuf         记录解码一帧MP3数据输出
*             mp3frameinfo  MP3FrameInfo类型指针变量
*             mp3Decoder    HMP3Decoder类型变量
*             pMP3Control   MP3Control_T类型指针变量，用户定义
*	返 回 值: 无
*********************************************************************************************************
*/
static void GetMP3DecoderData(FILE *pFile,
                              uint8_t *_pBuf,
                              MP3FrameInfo *mp3frameinfo,
                              HMP3Decoder mp3Decoder,
                              MP3Control_T *pMP3Control)
{
    int32_t  offset;
    int32_t  err;
    uint32_t br;
    uint8_t  res = 1;

    while(res)
    {
        /* 从地址pReadPos开始的位置查找同步字符，可以查找iBytesleft字节大小的范围 */
        offset = MP3FindSyncWord(pMP3Control->pReadPos, pMP3Control->iBytesleft);

        if(offset < 0)
        {
            /* 输入缓冲区未包含帧起始点，不做处理，直接读取另一帧 */
            pMP3Control->iBytesleft = MaxInputBufSize - 1;
            printf_MP3dbg("Syn Err\r\n");

            /* 退出while(1)循环 */
            res = 0;
        }
        else
        {
            /* 找到同步字符了，更新读取位置到同步字符处 */
            pMP3Control->pReadPos += offset;
            /* 更新剩余字节数 */
            pMP3Control->iBytesleft -= offset;

            /* 获取要解码的下一帧的相关信息 */
            err = MP3GetNextFrameInfo(mp3Decoder, mp3frameinfo, pMP3Control->pReadPos);

            if(err != ERR_MP3_NONE)
            {
                switch(err)
                {
					case ERR_MP3_INDATA_UNDERFLOW:
						printf_MP3dbg("ERR_MP3_INDATA_UNDERFLOW\r\n");
						break;

					case ERR_MP3_MAINDATA_UNDERFLOW:
						printf_MP3dbg("ERR_MP3_MAINDATA_UNDERFLOW\r\n");
						break;

					case ERR_MP3_FREE_BITRATE_SYNC:
						printf_MP3dbg("ERR_MP3_FREE_BITRATE_SYNC\r\n");
						break;

					/*
					  1. 这里主要是实现MP3歌曲播放的快进快退功能时要用到，正常播放基本用不到。
					  2. 这意味着 MP3FindSyncWord 函数已找到同步字，
						 但并不是帧起始点。发生这种情况的原因可能是在ID3标签中找到了同步字。
					  3. 这里我们调整100个字节，继续监测是否还有错误，这里的实现待更好的完善。
					*/
					case ERR_MP3_INVALID_FRAMEHEADER:
						if(pMP3Control->iBytesleft < 100)
						{
							/* 退出while(1)循环 */
							res = 0;;
						}
						else
						{
							pMP3Control->pReadPos += 100;
							pMP3Control->iBytesleft -= 100;
						}
						printf_MP3dbg("ERR_MP3_INVALID_FRAMEHEADER\r\n");
						break;

					default:
						printf_MP3dbg("OTHER ERR = %d\r\n", err);
						break;
                }
            }
            else
            {
                /*
                 1. 通过函数MP3GetNextFrameInfo获取要解码的下一帧数据没有错误时才进行解码操作。
                 2. 解码一帧MP3数据，解码后更新pReadPos和iBytesleft
                 3. 当err = -6的时候，出现的错误时，不会更新pMP3Control->pReadPos和pMP3Control->iBytesleft
                */
                MP3Decode(mp3Decoder, &(pMP3Control->pReadPos), &(pMP3Control->iBytesleft), (short *)_pBuf, 0);

                /* 暂时没有用到，注释掉 */
				#if 0
					MP3GetLastFrameInfo(mp3Decoder, mp3frameinfo);
					if(pMP3Control->uiBitrate != mp3frameinfo->bitrate)
					{
						pMP3Control->uiBitrate = mp3frameinfo->bitrate;
					}
				#endif

                /* 退出while(1)循环 */
                res = 0;;
            }
        }
    }


    /* 1940字节是输入缓冲区最大的MP3帧大小，小于1940字节时进行数据填充，重新填充为DecoderMP3BufSize字节大小 */
    if(pMP3Control->iBytesleft < MaxInputBufSize)
    {
        /* 移动剩余的字节到缓冲的首部 */
        memmove(pMP3Control->pInputBuf, pMP3Control->pReadPos, pMP3Control->iBytesleft);

        /* 再次读取数据添加MP3解码缓冲 */
        br = fread(pMP3Control->pInputBuf + pMP3Control->iBytesleft,
                   1,
                   DecoderMP3BufSize - pMP3Control->iBytesleft,
                   pFile);

        /* 如果读取数据小于指定的读取大小，说明读到文件末尾了 */
        if(br < DecoderMP3BufSize - pMP3Control->iBytesleft)
        {
            memset(pMP3Control->pInputBuf + pMP3Control->iBytesleft + br,
                   0,
                   DecoderMP3BufSize - pMP3Control->iBytesleft - br);
        }

        /* 从新设置iBytesleft和pReadPos */
        pMP3Control->iBytesleft = DecoderMP3BufSize;
        pMP3Control->pReadPos = pMP3Control->pInputBuf;
    }
}

/*
*********************************************************************************************************
*	函 数 名: GetMP3DispInfo
*	功能说明: 解析MP3音频头文件，获得相关的音频信息记录到变量g_tWav.usInfo里面
*	形    参: pMP3Control   MP3Control_T类型指针变量，用户定义
*             mp3frameinfo  MP3FrameInfo类型指针变量
*	返 回 值: 无
*********************************************************************************************************
*/
typedef struct
{
    uint32_t Fs;
    uint8_t  pFs[8];
} DispFSMP3_T;

const DispFSMP3_T g_tDispFSMP3[]=
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

static void GetMP3DispInfo(MP3Control_T *pMP3Control, MP3FrameInfo *mp3frameinfo)
{
    uint32_t n;

    /* 1. 通过采样率获得采样率的字符型 */
    for(n = 0; n < (sizeof(g_tDispFSMP3)/sizeof(g_tDispFSMP3[0])); n++)
    {
        if(pMP3Control->uiSamplerate == g_tDispFSMP3[n].Fs)
        {
            break;
        }
    }

    /* 2. 记录采样率，码率和立体声（单通道）到变量g_tWav.usInfo里面 */
    sprintf((char *)g_tWav.usInfo, "%s/%dKbps/",  g_tDispFSMP3[n].pFs, pMP3Control->uiBitrate/1000);
    switch(pMP3Control->usChannels)
    {
		case 2:
			strcat((char *)g_tWav.usInfo, "Stereo/");
			break;

		case 1:
			strcat((char *)g_tWav.usInfo, "Mono/");
			break;

		default:
			break;
    }

    /* 3. 记录VBR或者CBR格式到变量g_tWav.usInfo里面 */
    switch(pMP3Control->ucFmtVBRCBR)
    {
		case 1:
			strcat((char *)g_tWav.usInfo, "VBR");
			break;

		case 2:
			strcat((char *)g_tWav.usInfo, "CBR");
			break;

		default:
			break;
    }
}

/*
*********************************************************************************************************
*	函 数 名: GetMP3Runtime
*	功能说明: 专门用于获取MP3音频的运行时间，方便emWin显示歌曲列表时时间的显示。
*	形    参: pFileName     文件句柄
*             uiTotalTime   歌曲执行时间
*	返 回 值: TRUE(1) 表示成功， FALSE(0)表示失败
*********************************************************************************************************
*/
uint32_t GetMP3Runtime(uint8_t *pFileName, uint32_t *uiTotalTime)
{
    MP3Control_T  MP3Control;

    /* 获取MP3相关信息，主要是执行时间 */
    GetMP3TagInfo(pFileName, &MP3Control);
    *uiTotalTime = g_tWav.uiTotalTime;

    return TRUE;
}

/*
*********************************************************************************************************
*	函 数 名: GetMP3Infoo
*	功能说明: 专门用于获取MP3音频的相关信息，方便播放MP3歌曲时相关信息的显示。
*	形    参: _pPath   文件句柄
*	返 回 值: 无
*********************************************************************************************************
*/
void GetMP3Info(uint8_t *_pPath)
{
    MP3FrameInfo mp3FrameInfo;
    MP3Control_T  MP3Control;

    GetMP3TagInfo(_pPath, &MP3Control);
    GetMP3DispInfo(&MP3Control, &mp3FrameInfo);
}

/*
*********************************************************************************************************
*	函 数 名: MP3MusicPlay
*	功能说明: 播放MP3音频
*	形    参: filename  歌曲的路径
*	返 回 值: 无
*********************************************************************************************************
*/
__IO int8_t VedioSynCount = 0;
void MP3MusicPlay(const char *filename)
{
    FILE *fout = NULL;
    OS_RESULT xResult;
    MP3FrameInfo mp3FrameInfo;
    HMP3Decoder mp3Decoder = NULL;
    MP3Control_T  *ptMP3;

    uint8_t ucFileOpenFlag = 0, i, uiTempCount;

    /* 申请动态内存 */
    ptMP3 = (MP3Control_T *)os_alloc_mem(AppMalloc, sizeof(MP3Control_T));
    memset(ptMP3, 0, sizeof(MP3Control_T));

    /* 申请MP3音频播放所需的缓冲 */
    ptMP3->pInputBuf = (uint8_t *)os_alloc_mem(AppMalloc, DecoderMP3BufSize);
    ptMP3->pI2SBuffer0=(uint8_t *)os_alloc_mem(AppMalloc, MaxOutputBufSize);
    ptMP3->pI2SBuffer1=(uint8_t *)os_alloc_mem(AppMalloc, MaxOutputBufSize);
    ptMP3->pI2STempBuff=(uint8_t *)os_alloc_mem(AppMalloc, MaxOutputBufSize);
    memset(ptMP3->pI2SBuffer0, 0, MaxOutputBufSize);
    memset(ptMP3->pI2SBuffer1, 0, MaxOutputBufSize);

    VedioSynCount = 0;
	
    printf_MP3dbg("ptMP3 = %x\r\n", (int)ptMP3);
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
					AUDIO_Pause();
					MP3_TimeSet(fout, ptMP3);

					/* 初始化部分变量 */
					ptMP3->pReadPos = ptMP3->pInputBuf;
					ptMP3->iBytesleft = fread(ptMP3->pInputBuf, 1, DecoderMP3BufSize, fout);

					/* 获取3帧解码好的数据 */
					GetMP3DecoderData(fout, ptMP3->pI2STempBuff, &mp3FrameInfo, mp3Decoder, ptMP3);
					memcpy(ptMP3->pI2SBuffer0, ptMP3->pI2STempBuff, MaxOutputBufSize);
					GetMP3DecoderData(fout, ptMP3->pI2STempBuff, &mp3FrameInfo, mp3Decoder, ptMP3);
					memcpy(ptMP3->pI2SBuffer1, ptMP3->pI2STempBuff, MaxOutputBufSize);
					GetMP3DecoderData(fout, ptMP3->pI2STempBuff, &mp3FrameInfo, mp3Decoder, ptMP3);
					AUDIO_Resume();
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
					/* 获取要播放mp3文件的相关信息 */
					GetMP3TagInfo((uint8_t *)filename, ptMP3);
					GetMP3DispInfo(ptMP3, &mp3FrameInfo);

					/* mp3文件被打开标志设置 */
					ucFileOpenFlag = 1;

					/* 打开MP3音频文件*/
					fout = fopen ((const char *)filename, "r");
					fseek(fout, ptMP3->uiFrameStart, SEEK_SET);
					MP3_TimeShow(fout, ptMP3);

					/* 初始化部分变量 */
					ptMP3->pReadPos = ptMP3->pInputBuf;
					ptMP3->iBytesleft = fread(ptMP3->pInputBuf, 1, DecoderMP3BufSize, fout);

					/* 初始化Helix MP3解码器 */
					mp3Decoder = MP3InitDecoder();

					/* 获取3帧解码好的数据 */
					GetMP3DecoderData(fout, ptMP3->pI2STempBuff, &mp3FrameInfo, mp3Decoder, ptMP3);
					memcpy(ptMP3->pI2SBuffer0, ptMP3->pI2STempBuff, MaxOutputBufSize);
					GetMP3DecoderData(fout, ptMP3->pI2STempBuff, &mp3FrameInfo, mp3Decoder, ptMP3);
					memcpy(ptMP3->pI2SBuffer1, ptMP3->pI2STempBuff, MaxOutputBufSize);
					GetMP3DecoderData(fout, ptMP3->pI2STempBuff, &mp3FrameInfo, mp3Decoder, ptMP3);

					/* 初始化SAI音频接口进行播放 */
					AUDIO_Init(1, I2S_Standard_Phillips, SAI_DataSize_16b, ptMP3->uiSamplerate, mp3FrameInfo.nChans);
					Play_DMA_Init(ptMP3->pI2SBuffer0, ptMP3->pI2SBuffer1, ptMP3->usOutsamples);
					AUDIO_Play();
					break;

				/* 退出音频播放 */
				case MusicTaskAudioReturn_6:
					AUDIO_Stop();
					if(fout != NULL)
					{
						fclose (fout);
					}
					MP3FreeDecoder(mp3Decoder);
					os_free_mem(AppMalloc, ptMP3->pI2STempBuff);
					os_free_mem(AppMalloc, ptMP3->pI2SBuffer1);
					os_free_mem(AppMalloc, ptMP3->pI2SBuffer0);
					os_free_mem(AppMalloc, ptMP3->pInputBuf);
					os_free_mem(AppMalloc, ptMP3);
					return;

				/* 获取歌曲播放时间 */
				case MusicTaskAudioGetTime_7:
					if(ucFileOpenFlag == 1)
						MP3_TimeShow(fout, ptMP3);
					break;

				/* 开始播放，歌曲切换下一曲使用 */
				case MusicTaskAudioStart_8:
					/* 获取要播放mp3文件的相关信息 */
					GetMP3TagInfo((uint8_t *)filename, ptMP3);
					GetMP3DispInfo(ptMP3, &mp3FrameInfo);

					/* mp3文件被打开标志设置 */
					ucFileOpenFlag = 1;

					/* 打开MP3音频文件*/
					fout = fopen ((const char *)filename, "r");
					fseek(fout, ptMP3->uiFrameStart, SEEK_SET);
					MP3_TimeShow(fout, ptMP3);

					/* 初始化部分变量 */
					ptMP3->pReadPos = ptMP3->pInputBuf;
					ptMP3->iBytesleft = fread(ptMP3->pInputBuf, 1, DecoderMP3BufSize, fout);

					/* 初始化Helix MP3解码器 */
					mp3Decoder = MP3InitDecoder();

					/* 获取3帧解码好的数据 */
					GetMP3DecoderData(fout, ptMP3->pI2STempBuff, &mp3FrameInfo, mp3Decoder, ptMP3);
					memcpy(ptMP3->pI2SBuffer0, ptMP3->pI2STempBuff, MaxOutputBufSize);
					GetMP3DecoderData(fout, ptMP3->pI2STempBuff, &mp3FrameInfo, mp3Decoder, ptMP3);
					memcpy(ptMP3->pI2SBuffer1, ptMP3->pI2STempBuff, MaxOutputBufSize);
					GetMP3DecoderData(fout, ptMP3->pI2STempBuff, &mp3FrameInfo, mp3Decoder, ptMP3);

					/* 初始化SAI音频接口进行播放 */
					AUDIO_Init(1, I2S_Standard_Phillips, SAI_DataSize_16b, ptMP3->uiSamplerate, mp3FrameInfo.nChans);
					Play_DMA_Init(ptMP3->pI2SBuffer0, ptMP3->pI2SBuffer1, ptMP3->usOutsamples);
					AUDIO_Play();
					AUDIO_Pause();
					break;

				/* 当前使用的是缓冲0，填充缓冲1，并通过64点FFT实行频谱显示 */
				case MusicTaskAudioFillBuffer1_9:
					VedioSynCount--;
					if(hWinMusic != WM_HWIN_NULL)
					{
						DSP_FFT64(ptMP3->pI2SBuffer0);	
					}
					MP3_FillAudio((uint16_t *)ptMP3->pI2SBuffer1,
								  (uint16_t *)ptMP3->pI2STempBuff,
								  mp3FrameInfo.outputSamps);
					GetMP3DecoderData(fout, ptMP3->pI2STempBuff, &mp3FrameInfo, mp3Decoder, ptMP3);
					break;

				/* 当前使用的是缓冲1，填充缓冲0，并通过64点FFT实行频谱显示 */
				case MusicTaskAudioFillBuffer0_10:
					VedioSynCount--;
					if(hWinMusic != WM_HWIN_NULL)
					{
						DSP_FFT64(ptMP3->pI2SBuffer1);
					}
					MP3_FillAudio((uint16_t *)ptMP3->pI2SBuffer0,
								  (uint16_t *)ptMP3->pI2STempBuff,
								  mp3FrameInfo.outputSamps);
					GetMP3DecoderData(fout, ptMP3->pI2STempBuff, &mp3FrameInfo, mp3Decoder, ptMP3);
					break;

				/* 
				   早期设计的时候，为了防止视频播放任务一直占用文件系统，导致音频的DMA的
				   双缓冲都发消息过来，现在的版本进入这个消息的概率极小了。
				*/
				case MusicTaskWaitBuffer:
					#if 0 /* 这种方式有时候会多计算一次 */
						printf_MP3dbg("count = %d, ddd = %x\r\n", count, xResult);
						xResult = 0;
						while(os_sem_wait (&SemaphoreVedio, 0) == OS_R_OK)
						{
							xResult++;
							GetMP3DecoderData(fout, ptMP3->pI2STempBuff, &mp3FrameInfo, mp3Decoder, ptMP3);
						}
						printf_MP3dbg("count = %d, ddd = %x\r\n", count, xResult);
						count = 0;
					#else
						printf_MP3dbg("count = %d, ddd = %x\r\n", VedioSynCount, xResult);
						uiTempCount = VedioSynCount;
						VedioSynCount = 0;
						
						for(i = 0; i < uiTempCount; i++)
						{
							GetMP3DecoderData(fout, ptMP3->pI2STempBuff, &mp3FrameInfo, mp3Decoder, ptMP3);
						}
						
						/* 防止同步的过程中，count被改变，再进行一次 */
						for(i = 0; i < VedioSynCount; i++)
						{
							GetMP3DecoderData(fout, ptMP3->pI2STempBuff, &mp3FrameInfo, mp3Decoder, ptMP3);
						}
						printf_MP3dbg("count = %d, ddd = %x\r\n", VedioSynCount, xResult);
						VedioSynCount = 0;
					#endif
					break;

            /* 后5位暂未使用 */
            default:
                printf("xResult = %x\r\n", xResult);
                break;
            }
        }

        if(fout != NULL)
        {
            /* 音频文件读取完毕，这一步主要用于歌曲的自动切换 */
            if(feof(fout) != 0)
            {
                /* 等待其中一个缓冲完成 */
                os_evt_wait_or(MusicTaskWaitBuffer, 0xFFFF);
				xResult = os_evt_get ();
				
				/* 缓冲清零，防止播放快结束的时候重复播放存储的音频数据 */
				if(xResult & MusicTaskAudioFillBuffer1_9)
				{
					memset(ptMP3->pI2SBuffer1, 0, MaxOutputBufSize);
				}
				else
				{
					memset(ptMP3->pI2SBuffer0, 0, MaxOutputBufSize);
				}
				
                printf_MP3dbg("等待其中一个缓冲完成\r\n");
				
                /* 等待另一个缓冲完成 */
                os_evt_wait_or(MusicTaskWaitBuffer, 0xFFFF);
				xResult = os_evt_get ();
				if(xResult & MusicTaskAudioFillBuffer1_9)
				{
					memset(ptMP3->pI2SBuffer1, 0, MaxOutputBufSize);
				}
				else
				{
					memset(ptMP3->pI2SBuffer0, 0, MaxOutputBufSize);
				}
				
                printf_MP3dbg("等待其中另一个缓冲完成\r\n");
				
                /* 等待一次播放时间获取，保证音乐播放的进度条完整显示完毕 */
                os_evt_wait_or(MusicTaskAudioGetTime_7, 0xFFFF);
                MP3_TimeShow(fout, ptMP3);
                os_dly_wait(800);

                /* 关闭SAI音频接口和MP3文件 */
                AUDIO_Stop();

                if(fout != NULL)
                {
                    fclose (fout);
                }

                /* 释放相关动态内存 */
                MP3FreeDecoder(mp3Decoder);
                os_free_mem(AppMalloc, ptMP3->pI2STempBuff);
                os_free_mem(AppMalloc, ptMP3->pI2SBuffer1);
                os_free_mem(AppMalloc, ptMP3->pI2SBuffer0);
                os_free_mem(AppMalloc, ptMP3->pInputBuf);
                os_free_mem(AppMalloc, ptMP3);

                /* 给emWin的音乐播放器发消息，切换到下一首歌曲播放 */
				if(hWinMusic != WM_HWIN_NULL)
				{
					WM_SendMessageNoPara(hWinMusic, MSG_NextMusic);
				}
				
				if(hWinVideo != WM_HWIN_NULL)
				{
					WM_SendMessageNoPara(hWinVideo, MSG_NextMusic);
				}
                return;
            }
        }
    }
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

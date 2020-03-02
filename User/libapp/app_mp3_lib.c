/*
*********************************************************************************************************
*
*	ģ������ : MP3����Ӧ��
*	�ļ����� : app_mp3_lib.c
*	��    �� : V1.0
*	˵    �� : MP3������Ҫ����ֲ��Helix Decoder��ʵ����⡣
*              1. ����DecodeID3V1��DecodeID3V2����MP3�ļ���ǩ�Ľ���������GetMP3TagInfo����
*                 �����������Ϣ��¼��
*              2. MP3MusicPlay��ʵ���ǻ���RTX����ϵͳ�ġ�
*              3. ֧����һ������һ��������Ϳ��ˣ����õĲ����ʺ����ʶ�֧�֣���������������Ҳ��֧�֡�
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
	#define printf_MP3dbg printf
#else
	#define printf_MP3dbg(...)
#endif


/*
*********************************************************************************************************
                                          �궨��
*********************************************************************************************************
*/
#define MP3TimeScale        10       /* ��MP3�Ĳ���ʱ��Ŵ�10�����������Ƶ����ͬ�� */
#define DecoderMP3BufSize   5*1024	 /* ����MP3�����С����λ�ֽ� */
#define MaxInputBufSize     1940     /* ���뻺���������MP3֡��С����λ�ֽ� */
#define MaxOutputBufSize    4*1152   /* �����������������������Ƶ����ʱ��������������ֵ����λ�ֽ� */


/*
*********************************************************************************************************
                                          �ļ��ڲ����õĺ���
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
*	�� �� ��: DecodeID3V1
*	����˵��: ����MP3��ID3V1��ǩ������ȡ�������͸�������
*	��    ��: _pBuf  128�ֽڵĻ����ַ
*	�� �� ֵ: TRUE(1) ��ʾ�ɹ��� FALSE(0)��ʾʧ��
*********************************************************************************************************
*/
static uint8_t DecodeID3V1(uint8_t *_pBuf,  MP3Control_T *pMP3Control)
{
    ID3V1_T *ptID3V1_Tag;

    ptID3V1_Tag=(ID3V1_T *)_pBuf;

    /* �ж�ID3V1��ǩ��ǰ������ĸ�Ƿ�ΪTAG */
    if(strncmp("TAG", (char *)ptID3V1_Tag->ucTAG, 3) == 0)
    {
        /* ��ø����� */
        strncpy((char *)pMP3Control->ucTitle, (char *)ptID3V1_Tag->ucSongTitle, 30);

        /* ��ø��������� */
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
*	�� �� ��: DecodeID3V2
*	����˵��: ����MP3��ID3V2��ǩ����ȡ���ݿ�ʼλ�ã���������ߡ�
*             ÿ��ID3V2.3�ı�ǩ����һ����ǩͷ�����ɸ���ǩ֡��һ����չ��ǩͷ��ɡ�
*             ������Ŀ����Ϣ������⡢���ߵȶ�����ڲ�ͬ�ı�ǩ֡�У���չ��ǩͷ�ͱ�ǩ֡
*             �����Ǳ�Ҫ�ģ���ÿ����ǩ����Ҫ��һ����ǩ֡����ǩͷ�ͱ�ǩ֡һ��˳������
*             mp3�ļ����ײ���
*	��    ��: _pBuf        �����ַ
*             _uiSize      �����С
*             pMP3Control
*	�� �� ֵ: TRUE(1) ��ʾ�ɹ��� FALSE(0)��ʾʧ��
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
          1. ���б�ǩ֡�Ĵ�С����������ǩͷ��10���ֽ�
          2. ��ǩ��Сһ��4���ֽڣ���ÿ���ֽ�ֻ����7λ�����λ��ʹ�ú�Ϊ0�������Сʱ
             Ҫ��0ȥ�����õ�һ��28λ��2�����������Ǳ�ǩ��С��
        */
        uiTagSize = ((uint32_t)tID3V2Header->ucSize[0] << 21)
                    |((uint32_t)tID3V2Header->ucSize[1] << 14)
                    |((uint32_t)tID3V2Header->ucSize[2] << 7)
                    |(uint32_t)tID3V2Header->ucSize[3];

        printf_MP3dbg("uiTagSize = %d\r\n", uiTagSize);

        /* ���ݿ�ʼλ�ã���������ǩͷ��10���ֽ� */
        pMP3Control->uiFrameStart = uiTagSize + 10;

        /* ID3V2.3�汾�������� */
        if(tID3V2Header->ucMajorVer >= 3)
        {
            uiFrameHeaderSize = 10;
        }
        /* ID3V2.3�汾���°汾δ֧�� */
        else
        {
            return FALSE;
        }

        /* ��ֹ��ǩ��С���ڻ����С */
        if(uiTagSize > _uiSize)
        {
            uiTagSize = _uiSize;
        }

        /* ͷ��ǩ��10���ֽڣ���10��ʼ��ȡ���� */
        uiCount = 10;

        while(uiCount < uiTagSize)
        {
            tID3V23FrameHead=(ID3V23FrameHead_T*)(_pBuf + uiCount);

            /*  ÿ����ǩ֡����һ��10���ֽڵ�֡ͷ������һ���ֽڵĲ��̶����ȵ�������� */
            uiFrameSize = ((uint32_t)tID3V23FrameHead->ucSize[0] << 24)
                          |((uint32_t)tID3V23FrameHead->ucSize[1] << 16)
                          |((uint32_t)tID3V23FrameHead->ucSize[2] << 8)
                          |(uint32_t)tID3V23FrameHead->ucSize[3];

            /*
                ֡��ʶ��
            		TIT2  ����
            		TPE1  ����
            		TALB  ר��
            		TRCK  ����
            		TYER  ���
            		TCON  ����
            		COMM  ��ע
            */
            /* �ҵ����Ⲣ��ȡ */
            if (strncmp("TT2", (char*)tID3V23FrameHead->ucFrameID, 3) == 0
                    ||strncmp("TIT2",(char*)tID3V23FrameHead->ucFrameID, 4) == 0)
            {
                strncpy((char *)pMP3Control->ucTitle,
                        (char *)(_pBuf + uiCount + uiFrameHeaderSize + 1),
                        MIN(uiFrameSize - 1, 40 - 1));
            }

            /* �ҵ����߲���ȡ */
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
        /* ������ID3V2��ǩ */
        pMP3Control->uiFrameStart = 0;
    }

    return TRUE;
}

/*
*********************************************************************************************************
*	�� �� ��: GetMP3Info
*	����˵��: ����MP3��ID3V1��ID3V2��ǩ���Լ�MP3����ʱ��Ļ�ȡ��
*	��    ��: pFileName    �ļ�·��
*             pMP3Control  MP3Control_T���ͽṹ�����
*	�� �� ֵ: TRUE(1) ��ʾ�ɹ��� FALSE(0)��ʾʧ��
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

    /* ��1��������10KB�Ŀռ䲢����ID3V2��ǩ���� ------------------------------------------*/
    buf = (uint8_t *)os_alloc_mem(AppMalloc, 10*1024);
    fout_mp3 = fopen ((const char *)pFileName, "r");

    if(fout_mp3 == NULL)
    {
        return FALSE;
    }

    /* ����ID3V2����ȡǰ10KB���� */
    uiNumOfReadBytes = fread(buf, 1, 10*1024, fout_mp3);
    DecodeID3V2(buf, uiNumOfReadBytes, pMP3Control);

    /* ��2��������ID3V1��ǩ ------------------------------------------*/
    /*
       ����ID3V1����, ��ȡ�ļ�ĩβ128�ֽڼ���
       ��ЩMP3��Ƶ�ļ�û��ID3V2��ǩ��ID3V1���Բ�ʹ��
    */
    fseek (fout_mp3, -128L, SEEK_END);
    fread(buf, 1, 128, fout_mp3);
    DecodeID3V1(buf, pMP3Control);

    /* ��3����MP3���� -----------------------------------------------*/
    Decoder = MP3InitDecoder();
    fseek(fout_mp3, pMP3Control->uiFrameStart, SEEK_SET);
    uiNumOfReadBytes = fread(buf, 1, 10*1024, fout_mp3);

    /* ����֡ͬ����Ϣ */
    offset = MP3FindSyncWord(buf, uiNumOfReadBytes);

    /* �ҵ�֡ͬ����Ϣ��,����һ����Ϣ��ȡ���� */
    if(offset >= 0
            && MP3GetNextFrameInfo(Decoder, &tFrameInfo, &buf[offset]) == 0)
    {
        /*
          VBRIͷ��ʼλ�� = MPEG��һ֡֡ͷ��ʼλ�� +  ֡ͷ��С + 32��
          ֡ͷ��С = 4����6,��Protection bit==0ʱ��֡ͷ�����16bit=2byte��CRC,bi5����
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
		
		/* VBR��ʽ VBRI֡ */
		if(strncmp("VBRI", (char*)pFrameVBRI->ucVBRI, 4)==0)
		{
			if (tFrameInfo.version == MPEG1)
			{
				/* MPEG1, Layer IIIÿ֡��MPEG��Ƶ���ݵ�������1152 */
				SamplesPerFrame = 1152;			
			}
			else 
			{
				/* MPEG2����MPEG2.5, Layer IIIÿ֡��MPEG��Ƶ���ݵ�������576 */
				SamplesPerFrame = 576; 
			}
			
			uiTotalFrames = ((uint32_t)pFrameVBRI->ucFrames[0] << 24)
			                |((uint32_t)pFrameVBRI->ucFrames[1] << 16)
			                |((uint32_t)pFrameVBRI->ucFrames[2] << 8)
			                |(uint32_t)pFrameVBRI->ucFrames[3];
			
			  /* ��Ƶ������ʱ�� */
            g_tWav.uiTotalTime = uiTotalFrames * SamplesPerFrame * MP3TimeScale / tFrameInfo.samprate;

            /* ��¼��VBR����CBR��ʽ */
            pMP3Control->ucFmtVBRCBR = 1;
			
			printf_MP3dbg("VBRI = VBR\r\n");
			
		}
		/* VBR��ʽXing֡����CBR��ʽ����һ�������ֳ�����VBR��CBR��ʽ������׼ȷ */
		else	
		{  
			if (tFrameInfo.version==MPEG1)	
			{
				/* �߽���Ϣ��С */
				p = tFrameInfo.nChans == 2? 32 : 17;
				
				/* MPEG1, Layer IIIÿ֡��MPEG��Ƶ���ݵ�������1152 */
				SamplesPerFrame = 1152;	
			}
			else
			{
				/* �߽���Ϣ��С */
				p = tFrameInfo.nChans == 2? 17 : 9;
				
				/* MPEG2����MPEG2.5, Layer IIIÿ֡��MPEG��Ƶ���ݵ�������576 */
				SamplesPerFrame = 576;		
			}
			
			/* �˴��������ʣ��о�Ӧ�÷�4��6������� */
			p += offset + 4;
			pFrameXing = (MP3FrameXing_T *)(buf + p);
			
			/* VBR��ʽ,��ѯ�Ƿ���xing֡ ******************************/
			if(strncmp("Xing",(char*)pFrameXing->ucXing, 4) == 0)
			{
				/* ����Frames�� */
				if(pFrameXing->ucFlags[3] & 0x01)
				{
					/* �õ���֡�� */
					uiTotalFrames = ((uint32_t)pFrameXing->ucFrames[0] << 24)
								  |((uint32_t)pFrameXing->ucFrames[1] << 16)
								  |((uint32_t)pFrameXing->ucFrames[2] << 8)
								  |(uint32_t)pFrameXing->ucFrames[3];

					/* ��Ƶ������ʱ�� */
					g_tWav.uiTotalTime = uiTotalFrames*SamplesPerFrame * MP3TimeScale / tFrameInfo.samprate;

					printf_MP3dbg("Xing === VBR \r\n");
				}
				/* ������Frames�� */
				else	
				{
					fseek (fout_mp3, 0L, SEEK_END);
                    g_tWav.uiTotalTime = (ftell(fout_mp3) -pMP3Control->uiFrameStart) * MP3TimeScale / (tFrameInfo.bitrate/8);
					
					printf_MP3dbg("Xing === CBR \r\n");
				} 
				
				/* ��¼��VBR��ʽ */
				pMP3Control->ucFmtVBRCBR = 1;
			}
			/* CBR��ʽ **************************************************/
			else if(strncmp("Info", (char*)pFrameXing->ucXing, 4) == 0)
			{
				/* ����Frames�� */
				if(pFrameXing->ucFlags[3] & 0x01)
				{
					/* �õ���֡�� */
					uiTotalFrames = ((uint32_t)pFrameXing->ucFrames[0] << 24)
								  |((uint32_t)pFrameXing->ucFrames[1] << 16)
								  |((uint32_t)pFrameXing->ucFrames[2] << 8)
								  |(uint32_t)pFrameXing->ucFrames[3];

					/* ��Ƶ������ʱ�� */
                    g_tWav.uiTotalTime = uiTotalFrames*SamplesPerFrame * MP3TimeScale / tFrameInfo.samprate;
					
					printf_MP3dbg("info === VBR \r\n");
				}
				/* ������Frames�� */
				else	
				{
					fseek (fout_mp3, 0L, SEEK_END);
                    g_tWav.uiTotalTime = (ftell(fout_mp3) -pMP3Control->uiFrameStart) * MP3TimeScale / (tFrameInfo.bitrate/8);
					printf_MP3dbg("info === CBR \r\n");
                }
					
				/* ��¼��CBR��ʽ */
				pMP3Control->ucFmtVBRCBR = 2;
			}
			/* CBR��ʽ **************************************************/
			else 		
			{
				fseek (fout_mp3, 0L, SEEK_END);
                g_tWav.uiTotalTime = (ftell(fout_mp3) -pMP3Control->uiFrameStart) * MP3TimeScale / (tFrameInfo.bitrate/8);
				
				/* ��¼��CBR��ʽ */
				pMP3Control->ucFmtVBRCBR = 2;
				
				printf_MP3dbg("LAST === CBR \r\n");
			}
		} 

        /* ��¼���� */
        pMP3Control->uiBitrate = tFrameInfo.bitrate;
        /* ��¼������ */
        pMP3Control->uiSamplerate = tFrameInfo.samprate;

        if(tFrameInfo.nChans == 2)
        {
            /* ��¼������*/
            pMP3Control->usChannels = 2;
            pMP3Control->usOutsamples = tFrameInfo.outputSamps;
        }
        else
        {
          /* ������MP3_FillAudioҪͳһ */
			#if 1
				/* ���������������SAI��Ƶ�ӿڵ�һ��slot */
				pMP3Control->usOutsamples = tFrameInfo.outputSamps;
			#else
				/* ��Ϊ���˫����������SAI��Ƶ�ӿڵ�����slot */			
				pMP3Control->usOutsamples = tFrameInfo.outputSamps * 2;			
			#endif
        }
    }
    else
    {
        return FALSE;
    }

    /* ��4�����ͷŶ�̬�ڴ沢�ر���ش˴򿪵��ļ� -----------------------------------------------*/
    MP3FreeDecoder(Decoder);
    fclose(fout_mp3);
    os_free_mem(AppMalloc, buf);
    return TRUE;
}

/*
*********************************************************************************************************
*	�� �� ��: MP3_TimeShow
*	����˵��: ���ڻ�ȡ��Ƶ��ǰ�Ĳ���ʱ�䣬ע�ⷵ��ʱ��ĵ�λ��0.1s��Ҳ���ǷŴ���10��
*	��    ��: _pFile      FIL�ļ�
*             pMP3Control MP3Control_T���ͽṹ��ָ�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MP3_TimeShow(FILE *_pFile, MP3Control_T *pMP3Control)
{
    uint64_t uiPos;
    uint32_t uiTempPos;

    /* ��ȡ��ǰ����λ�� */
    uiTempPos = ftell(_pFile);
    uiPos = uiTempPos - pMP3Control->uiFrameStart;

    /* ͨ�����ַ�������ļ��Ĵ�С */
    fseek (_pFile, 0L, SEEK_END);
    /* ͨ�����ַ�������ļ��Ĵ�С */
    g_tWav.uiCurTime = (uint64_t)(uiPos * g_tWav.uiTotalTime) / (ftell(_pFile) -  pMP3Control->uiFrameStart);

    /* �ָ���ǰ���ڲ��ŵ�λ�� */
    fseek (_pFile, uiTempPos, SEEK_SET);
}

/*
*********************************************************************************************************
*	�� �� ��: MP3_TimeSet
*	����˵��: ��������MP3����λ��
*	��    ��: _pFile      FIL�ļ�
*             pMP3Control MP3Control_T���ͽṹ��ָ�����
*	�� �� ֵ: ��
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
*	�� �� ��: MP3_FillAudio
*	����˵��: ���������������䵽DMA�Ļ�����
*	��    ��: _ptr      DMA�����ַ
*             _ptrtemp  �������ʵ�ʵ�ַ
*             _uiSize   Ҫ�������ݴ�С
*	�� �� ֵ: ��
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
        /* ˫ͨ�� */
        for(i = 0; i < _uiSize; i++)
        {
            _ptr[i] = _ptrtemp[i];
        }
    }
    else
    {
        /* ��ͨ��ת��Ϊ˫ͨ�� */
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
*	�� �� ��: GetMP3DecoderData
*	����˵��: ����һ֡MP3��Ƶ���ݡ�
*	��    ��: pFile         �ļ�ϵͳ���
*             _pBuf         ��¼����һ֡MP3�������
*             mp3frameinfo  MP3FrameInfo����ָ�����
*             mp3Decoder    HMP3Decoder���ͱ���
*             pMP3Control   MP3Control_T����ָ��������û�����
*	�� �� ֵ: ��
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
        /* �ӵ�ַpReadPos��ʼ��λ�ò���ͬ���ַ������Բ���iBytesleft�ֽڴ�С�ķ�Χ */
        offset = MP3FindSyncWord(pMP3Control->pReadPos, pMP3Control->iBytesleft);

        if(offset < 0)
        {
            /* ���뻺����δ����֡��ʼ�㣬��������ֱ�Ӷ�ȡ��һ֡ */
            pMP3Control->iBytesleft = MaxInputBufSize - 1;
            printf_MP3dbg("Syn Err\r\n");

            /* �˳�while(1)ѭ�� */
            res = 0;
        }
        else
        {
            /* �ҵ�ͬ���ַ��ˣ����¶�ȡλ�õ�ͬ���ַ��� */
            pMP3Control->pReadPos += offset;
            /* ����ʣ���ֽ��� */
            pMP3Control->iBytesleft -= offset;

            /* ��ȡҪ�������һ֡�������Ϣ */
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
					  1. ������Ҫ��ʵ��MP3�������ŵĿ�����˹���ʱҪ�õ����������Ż����ò�����
					  2. ����ζ�� MP3FindSyncWord �������ҵ�ͬ���֣�
						 ��������֡��ʼ�㡣�������������ԭ���������ID3��ǩ���ҵ���ͬ���֡�
					  3. �������ǵ���100���ֽڣ���������Ƿ��д��������ʵ�ִ����õ����ơ�
					*/
					case ERR_MP3_INVALID_FRAMEHEADER:
						if(pMP3Control->iBytesleft < 100)
						{
							/* �˳�while(1)ѭ�� */
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
                 1. ͨ������MP3GetNextFrameInfo��ȡҪ�������һ֡����û�д���ʱ�Ž��н��������
                 2. ����һ֡MP3���ݣ���������pReadPos��iBytesleft
                 3. ��err = -6��ʱ�򣬳��ֵĴ���ʱ���������pMP3Control->pReadPos��pMP3Control->iBytesleft
                */
                MP3Decode(mp3Decoder, &(pMP3Control->pReadPos), &(pMP3Control->iBytesleft), (short *)_pBuf, 0);

                /* ��ʱû���õ���ע�͵� */
				#if 0
					MP3GetLastFrameInfo(mp3Decoder, mp3frameinfo);
					if(pMP3Control->uiBitrate != mp3frameinfo->bitrate)
					{
						pMP3Control->uiBitrate = mp3frameinfo->bitrate;
					}
				#endif

                /* �˳�while(1)ѭ�� */
                res = 0;;
            }
        }
    }


    /* 1940�ֽ������뻺��������MP3֡��С��С��1940�ֽ�ʱ����������䣬�������ΪDecoderMP3BufSize�ֽڴ�С */
    if(pMP3Control->iBytesleft < MaxInputBufSize)
    {
        /* �ƶ�ʣ����ֽڵ�������ײ� */
        memmove(pMP3Control->pInputBuf, pMP3Control->pReadPos, pMP3Control->iBytesleft);

        /* �ٴζ�ȡ�������MP3���뻺�� */
        br = fread(pMP3Control->pInputBuf + pMP3Control->iBytesleft,
                   1,
                   DecoderMP3BufSize - pMP3Control->iBytesleft,
                   pFile);

        /* �����ȡ����С��ָ���Ķ�ȡ��С��˵�������ļ�ĩβ�� */
        if(br < DecoderMP3BufSize - pMP3Control->iBytesleft)
        {
            memset(pMP3Control->pInputBuf + pMP3Control->iBytesleft + br,
                   0,
                   DecoderMP3BufSize - pMP3Control->iBytesleft - br);
        }

        /* ��������iBytesleft��pReadPos */
        pMP3Control->iBytesleft = DecoderMP3BufSize;
        pMP3Control->pReadPos = pMP3Control->pInputBuf;
    }
}

/*
*********************************************************************************************************
*	�� �� ��: GetMP3DispInfo
*	����˵��: ����MP3��Ƶͷ�ļ��������ص���Ƶ��Ϣ��¼������g_tWav.usInfo����
*	��    ��: pMP3Control   MP3Control_T����ָ��������û�����
*             mp3frameinfo  MP3FrameInfo����ָ�����
*	�� �� ֵ: ��
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

    /* 1. ͨ�������ʻ�ò����ʵ��ַ��� */
    for(n = 0; n < (sizeof(g_tDispFSMP3)/sizeof(g_tDispFSMP3[0])); n++)
    {
        if(pMP3Control->uiSamplerate == g_tDispFSMP3[n].Fs)
        {
            break;
        }
    }

    /* 2. ��¼�����ʣ����ʺ�����������ͨ����������g_tWav.usInfo���� */
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

    /* 3. ��¼VBR����CBR��ʽ������g_tWav.usInfo���� */
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
*	�� �� ��: GetMP3Runtime
*	����˵��: ר�����ڻ�ȡMP3��Ƶ������ʱ�䣬����emWin��ʾ�����б�ʱʱ�����ʾ��
*	��    ��: pFileName     �ļ����
*             uiTotalTime   ����ִ��ʱ��
*	�� �� ֵ: TRUE(1) ��ʾ�ɹ��� FALSE(0)��ʾʧ��
*********************************************************************************************************
*/
uint32_t GetMP3Runtime(uint8_t *pFileName, uint32_t *uiTotalTime)
{
    MP3Control_T  MP3Control;

    /* ��ȡMP3�����Ϣ����Ҫ��ִ��ʱ�� */
    GetMP3TagInfo(pFileName, &MP3Control);
    *uiTotalTime = g_tWav.uiTotalTime;

    return TRUE;
}

/*
*********************************************************************************************************
*	�� �� ��: GetMP3Infoo
*	����˵��: ר�����ڻ�ȡMP3��Ƶ�������Ϣ�����㲥��MP3����ʱ�����Ϣ����ʾ��
*	��    ��: _pPath   �ļ����
*	�� �� ֵ: ��
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
*	�� �� ��: MP3MusicPlay
*	����˵��: ����MP3��Ƶ
*	��    ��: filename  ������·��
*	�� �� ֵ: ��
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

    /* ���붯̬�ڴ� */
    ptMP3 = (MP3Control_T *)os_alloc_mem(AppMalloc, sizeof(MP3Control_T));
    memset(ptMP3, 0, sizeof(MP3Control_T));

    /* ����MP3��Ƶ��������Ļ��� */
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
				/* �������emWin������Ļ�����λ�Ǳ�emWin������ʹ�� */
				case MusicTaskReserved_0:
					break;

				/* ʵ�ֿ������ */
				case MusicTaskAudioAdjust_1:
					AUDIO_Pause();
					MP3_TimeSet(fout, ptMP3);

					/* ��ʼ�����ֱ��� */
					ptMP3->pReadPos = ptMP3->pInputBuf;
					ptMP3->iBytesleft = fread(ptMP3->pInputBuf, 1, DecoderMP3BufSize, fout);

					/* ��ȡ3֡����õ����� */
					GetMP3DecoderData(fout, ptMP3->pI2STempBuff, &mp3FrameInfo, mp3Decoder, ptMP3);
					memcpy(ptMP3->pI2SBuffer0, ptMP3->pI2STempBuff, MaxOutputBufSize);
					GetMP3DecoderData(fout, ptMP3->pI2STempBuff, &mp3FrameInfo, mp3Decoder, ptMP3);
					memcpy(ptMP3->pI2SBuffer1, ptMP3->pI2STempBuff, MaxOutputBufSize);
					GetMP3DecoderData(fout, ptMP3->pI2STempBuff, &mp3FrameInfo, mp3Decoder, ptMP3);
					AUDIO_Resume();
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
					/* ��ȡҪ����mp3�ļ��������Ϣ */
					GetMP3TagInfo((uint8_t *)filename, ptMP3);
					GetMP3DispInfo(ptMP3, &mp3FrameInfo);

					/* mp3�ļ����򿪱�־���� */
					ucFileOpenFlag = 1;

					/* ��MP3��Ƶ�ļ�*/
					fout = fopen ((const char *)filename, "r");
					fseek(fout, ptMP3->uiFrameStart, SEEK_SET);
					MP3_TimeShow(fout, ptMP3);

					/* ��ʼ�����ֱ��� */
					ptMP3->pReadPos = ptMP3->pInputBuf;
					ptMP3->iBytesleft = fread(ptMP3->pInputBuf, 1, DecoderMP3BufSize, fout);

					/* ��ʼ��Helix MP3������ */
					mp3Decoder = MP3InitDecoder();

					/* ��ȡ3֡����õ����� */
					GetMP3DecoderData(fout, ptMP3->pI2STempBuff, &mp3FrameInfo, mp3Decoder, ptMP3);
					memcpy(ptMP3->pI2SBuffer0, ptMP3->pI2STempBuff, MaxOutputBufSize);
					GetMP3DecoderData(fout, ptMP3->pI2STempBuff, &mp3FrameInfo, mp3Decoder, ptMP3);
					memcpy(ptMP3->pI2SBuffer1, ptMP3->pI2STempBuff, MaxOutputBufSize);
					GetMP3DecoderData(fout, ptMP3->pI2STempBuff, &mp3FrameInfo, mp3Decoder, ptMP3);

					/* ��ʼ��SAI��Ƶ�ӿڽ��в��� */
					AUDIO_Init(1, I2S_Standard_Phillips, SAI_DataSize_16b, ptMP3->uiSamplerate, mp3FrameInfo.nChans);
					Play_DMA_Init(ptMP3->pI2SBuffer0, ptMP3->pI2SBuffer1, ptMP3->usOutsamples);
					AUDIO_Play();
					break;

				/* �˳���Ƶ���� */
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

				/* ��ȡ��������ʱ�� */
				case MusicTaskAudioGetTime_7:
					if(ucFileOpenFlag == 1)
						MP3_TimeShow(fout, ptMP3);
					break;

				/* ��ʼ���ţ������л���һ��ʹ�� */
				case MusicTaskAudioStart_8:
					/* ��ȡҪ����mp3�ļ��������Ϣ */
					GetMP3TagInfo((uint8_t *)filename, ptMP3);
					GetMP3DispInfo(ptMP3, &mp3FrameInfo);

					/* mp3�ļ����򿪱�־���� */
					ucFileOpenFlag = 1;

					/* ��MP3��Ƶ�ļ�*/
					fout = fopen ((const char *)filename, "r");
					fseek(fout, ptMP3->uiFrameStart, SEEK_SET);
					MP3_TimeShow(fout, ptMP3);

					/* ��ʼ�����ֱ��� */
					ptMP3->pReadPos = ptMP3->pInputBuf;
					ptMP3->iBytesleft = fread(ptMP3->pInputBuf, 1, DecoderMP3BufSize, fout);

					/* ��ʼ��Helix MP3������ */
					mp3Decoder = MP3InitDecoder();

					/* ��ȡ3֡����õ����� */
					GetMP3DecoderData(fout, ptMP3->pI2STempBuff, &mp3FrameInfo, mp3Decoder, ptMP3);
					memcpy(ptMP3->pI2SBuffer0, ptMP3->pI2STempBuff, MaxOutputBufSize);
					GetMP3DecoderData(fout, ptMP3->pI2STempBuff, &mp3FrameInfo, mp3Decoder, ptMP3);
					memcpy(ptMP3->pI2SBuffer1, ptMP3->pI2STempBuff, MaxOutputBufSize);
					GetMP3DecoderData(fout, ptMP3->pI2STempBuff, &mp3FrameInfo, mp3Decoder, ptMP3);

					/* ��ʼ��SAI��Ƶ�ӿڽ��в��� */
					AUDIO_Init(1, I2S_Standard_Phillips, SAI_DataSize_16b, ptMP3->uiSamplerate, mp3FrameInfo.nChans);
					Play_DMA_Init(ptMP3->pI2SBuffer0, ptMP3->pI2SBuffer1, ptMP3->usOutsamples);
					AUDIO_Play();
					AUDIO_Pause();
					break;

				/* ��ǰʹ�õ��ǻ���0����仺��1����ͨ��64��FFTʵ��Ƶ����ʾ */
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

				/* ��ǰʹ�õ��ǻ���1����仺��0����ͨ��64��FFTʵ��Ƶ����ʾ */
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
				   ������Ƶ�ʱ��Ϊ�˷�ֹ��Ƶ��������һֱռ���ļ�ϵͳ��������Ƶ��DMA��
				   ˫���嶼����Ϣ���������ڵİ汾���������Ϣ�ĸ��ʼ�С�ˡ�
				*/
				case MusicTaskWaitBuffer:
					#if 0 /* ���ַ�ʽ��ʱ�������һ�� */
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
						
						/* ��ֹͬ���Ĺ����У�count���ı䣬�ٽ���һ�� */
						for(i = 0; i < VedioSynCount; i++)
						{
							GetMP3DecoderData(fout, ptMP3->pI2STempBuff, &mp3FrameInfo, mp3Decoder, ptMP3);
						}
						printf_MP3dbg("count = %d, ddd = %x\r\n", VedioSynCount, xResult);
						VedioSynCount = 0;
					#endif
					break;

            /* ��5λ��δʹ�� */
            default:
                printf("xResult = %x\r\n", xResult);
                break;
            }
        }

        if(fout != NULL)
        {
            /* ��Ƶ�ļ���ȡ��ϣ���һ����Ҫ���ڸ������Զ��л� */
            if(feof(fout) != 0)
            {
                /* �ȴ�����һ��������� */
                os_evt_wait_or(MusicTaskWaitBuffer, 0xFFFF);
				xResult = os_evt_get ();
				
				/* �������㣬��ֹ���ſ������ʱ���ظ����Ŵ洢����Ƶ���� */
				if(xResult & MusicTaskAudioFillBuffer1_9)
				{
					memset(ptMP3->pI2SBuffer1, 0, MaxOutputBufSize);
				}
				else
				{
					memset(ptMP3->pI2SBuffer0, 0, MaxOutputBufSize);
				}
				
                printf_MP3dbg("�ȴ�����һ���������\r\n");
				
                /* �ȴ���һ��������� */
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
				
                printf_MP3dbg("�ȴ�������һ���������\r\n");
				
                /* �ȴ�һ�β���ʱ���ȡ����֤���ֲ��ŵĽ�����������ʾ��� */
                os_evt_wait_or(MusicTaskAudioGetTime_7, 0xFFFF);
                MP3_TimeShow(fout, ptMP3);
                os_dly_wait(800);

                /* �ر�SAI��Ƶ�ӿں�MP3�ļ� */
                AUDIO_Stop();

                if(fout != NULL)
                {
                    fclose (fout);
                }

                /* �ͷ���ض�̬�ڴ� */
                MP3FreeDecoder(mp3Decoder);
                os_free_mem(AppMalloc, ptMP3->pI2STempBuff);
                os_free_mem(AppMalloc, ptMP3->pI2SBuffer1);
                os_free_mem(AppMalloc, ptMP3->pI2SBuffer0);
                os_free_mem(AppMalloc, ptMP3->pInputBuf);
                os_free_mem(AppMalloc, ptMP3);

                /* ��emWin�����ֲ���������Ϣ���л�����һ�׸������� */
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

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

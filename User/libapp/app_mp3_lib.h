/*
*********************************************************************************************************
*
*	ģ������ : MP3����Ӧ��ͷ�ļ�
*	�ļ����� : app_mp3_lib.h
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
#ifndef _APP_MP3_LIB_H_
#define _APP_MP3_LIB_H_


/*
ID3V1��ǩ, �̶�128�ֽڣ�������ļ�ĩβ��
���Ե���վhttp://id3.org/ID3v1�����˽�
*/
typedef __packed struct 
{
    uint8_t ucTAG[3];	       /* �̶�T A G������ĸ */
    uint8_t ucSongTitle[30];   /* ����              */
	uint8_t ucAlbum[30]; 	   /* ר��              */
    uint8_t ucArtist[30];	   /* ����              */
	uint8_t ucYear[4];		   /* ��Ʒ���          */
	uint8_t ucComment[30];	   /* ��ע              */
	uint8_t ucGenre; 		   /* ���ɣ�������壬ҡ������ʿ�� */ 
}ID3V1_T;

/*
ID3V2��ǩͷ, ��С10�ֽڡ�
http://id3.org/id3v2.3.0
*/
typedef __packed struct 
{
    uint8_t ucID3[3];	  /* �̶�I D 3������ĸ */	   
    uint8_t ucMajorVer;	  /* ���汾�� */	 
    uint8_t ucRevision;	  /* �Ӱ汾�� */	 	
    uint8_t ucFlags;	  /* ��־�ֽ� */	
    uint8_t ucSize[4];	  /* ��ǩ��С�����б�ǩ֡�Ĵ�С����������ǩͷ��10���ֽ� */
}ID3V2Header_T;

/*
ID3V2��ǩ֡֡ͷ, ��С10�ֽڣ��汾V2.3
http://id3.org/id3v2.3.0
*/
typedef __packed struct 
{
    uint8_t  ucFrameID[4];	  /* ���ĸ��ַ���ʶһ��֡   */
    uint8_t  ucSize[4];	      /* ֡���ݴ�С��������֡ͷ */
    uint16_t ucFlags;		  /* ֡��־                 */
}ID3V23FrameHead_T;

/*
MP3 Xing֡
1. �󲿷ֿɱ�����ʱ�����ļ�����������ͷ
2. һ��XINGͷ�������ٰ���ID�ֶκ�Flags�ֶΣ�������ֶ���
   ������Flags�ֶεģ������ǿ�ѡ�ġ���һЩ����£�CBR�ļ�
   ��Ҳ��������ͷ������������£�IDֵһ���á�Info������ʶ
3. http://www.cnblogs.com/gansc23/archive/2010/11/27/1889537.html
*/
typedef __packed struct 
{
    uint8_t ucXing[4];		       /* �̶�λXing����Info�ĸ���ĸ */
    uint8_t ucFlags[4];		       /* ��ű�־��������֡����4�������Ĵ������ */
    uint8_t ucFrames[4];	       /* ��֡������˸�ʽ����ѡ */
	uint8_t ucSize[4];		       /* �ļ���С����˸�ʽ����ѡ */
	uint8_t ucTOC[100];            /* TOC��������ѡ */
	uint8_t ucQualittIndicator[4]; /* TOC��������˸�ʽ����ѡ��0��ʾ��ã�100��ʾ��� */
}MP3FrameXing_T;
 
/*
MP3 VBRI֡
1. ����XINGͷ��ͬ��������ڵ�һ֡��Ƶ֡ͷ֮���32���ֽ�ƫ�ƴ�׼ȷ�ض�λ���ͷ
2. http://www.cnblogs.com/gansc23/archive/2010/11/27/1889537.html
*/
typedef __packed struct 
{
    uint8_t ucVBRI[4];		       /* �̶�ΪVBRI�ĸ���ĸ */ 	
	uint8_t ucVersion[2];		   /* �汾�ţ���˸�ʽ */
	uint8_t ucDelay[2];		       /* �ӳ٣���˸�ʽ*/
	uint8_t ucQualityIndicator[2]; /* ����������0��ʾ��ã�100��ʾ��� */
	uint8_t ucSize[4];		       /* �ļ���С����˸�ʽ */
	uint8_t ucFrames[4];		   /* ��֡������˸�ʽ */ 
	uint8_t ucNumOfTOC[2];         /* TOC�����Ŀ������˸�ʽ */  
	uint8_t ucFactorOfTOC[2];      /* TOC�����Ŀ�Ŵ����ӣ���˸�ʽ */ 
	uint8_t ucSizePerTable[2];     /* ÿ����Ŀ�Ĵ�С�����4�ֽڣ���˸�ʽ */ 
	uint8_t ucFramesPerTable[2];   /* ÿ����Ŀ��֡������˸�ʽ */ 
}MP3FrameVBRI_T;

/* ����MP3��Ƶ���ſ��� */
typedef struct 
{
    uint8_t  ucTitle[40];	 /* ������ */
    uint8_t  ucArtist[40];	 /* ������ */
	uint8_t  ucFmtVBRCBR;    /* ���ڼ�¼VBR����CBR��ʽ */
    uint32_t uiBitrate;	     /* ����   */
	uint32_t uiSamplerate;   /* ������ */
	uint16_t usOutsamples;	 /* PCM�����������С */
	uint32_t uiFrameStart;	 /* ʵ����Ƶ֡����ʼλ�� */
	uint16_t usChannels;     /* ��¼��ͨ������˫ͨ�� */
	
	int32_t  iBytesleft;
	uint8_t  *pInputBuf;
	uint8_t  *pReadPos;	
	uint8_t  *pI2SBuffer0;
	uint8_t  *pI2SBuffer1;
	uint8_t  *pI2STempBuff;
}MP3Control_T;


/* ���ⲿ�ļ����õĺ��� */
uint32_t GetMP3Runtime(uint8_t *pFileName, uint32_t *uiTotalTime);
void GetMP3Info(uint8_t *_pPath);
void MP3MusicPlay(const char *filename);

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

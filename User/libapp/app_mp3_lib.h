/*
*********************************************************************************************************
*
*	模块名称 : MP3播放应用头文件
*	文件名称 : app_mp3_lib.h
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
#ifndef _APP_MP3_LIB_H_
#define _APP_MP3_LIB_H_


/*
ID3V1标签, 固定128字节，存放在文件末尾。
可以到网站http://id3.org/ID3v1进行了解
*/
typedef __packed struct 
{
    uint8_t ucTAG[3];	       /* 固定T A G三个字母 */
    uint8_t ucSongTitle[30];   /* 标题              */
	uint8_t ucAlbum[30]; 	   /* 专辑              */
    uint8_t ucArtist[30];	   /* 作者              */
	uint8_t ucYear[4];		   /* 出品年底          */
	uint8_t ucComment[30];	   /* 备注              */
	uint8_t ucGenre; 		   /* 流派，比如乡村，摇滚，爵士等 */ 
}ID3V1_T;

/*
ID3V2标签头, 大小10字节。
http://id3.org/id3v2.3.0
*/
typedef __packed struct 
{
    uint8_t ucID3[3];	  /* 固定I D 3三个字母 */	   
    uint8_t ucMajorVer;	  /* 主版本号 */	 
    uint8_t ucRevision;	  /* 子版本号 */	 	
    uint8_t ucFlags;	  /* 标志字节 */	
    uint8_t ucSize[4];	  /* 标签大小，所有标签帧的大小，不包含标签头的10个字节 */
}ID3V2Header_T;

/*
ID3V2标签帧帧头, 大小10字节，版本V2.3
http://id3.org/id3v2.3.0
*/
typedef __packed struct 
{
    uint8_t  ucFrameID[4];	  /* 用四个字符标识一个帧   */
    uint8_t  ucSize[4];	      /* 帧内容大小，不包含帧头 */
    uint16_t ucFlags;		  /* 帧标志                 */
}ID3V23FrameHead_T;

/*
MP3 Xing帧
1. 大部分可变比特率编码的文件都会包含这个头
2. 一个XING头必须至少包含ID字段和Flags字段，其余的字段是
   依靠与Flags字段的，并且是可选的。在一些情况下，CBR文件
   中也会包含这个头，在这种情况下，ID值一般用”Info”来标识
3. http://www.cnblogs.com/gansc23/archive/2010/11/27/1889537.html
*/
typedef __packed struct 
{
    uint8_t ucXing[4];		       /* 固定位Xing或者Info四个字母 */
    uint8_t ucFlags[4];		       /* 存放标志，决定此帧后面4个变量的存在与否 */
    uint8_t ucFrames[4];	       /* 总帧数，大端格式，可选 */
	uint8_t ucSize[4];		       /* 文件大小，大端格式，可选 */
	uint8_t ucTOC[100];            /* TOC检索，可选 */
	uint8_t ucQualittIndicator[4]; /* TOC检索，大端格式，可选，0表示最好，100表示最差 */
}MP3FrameXing_T;
 
/*
MP3 VBRI帧
1. 它和XING头不同。你可以在第一帧音频帧头之后的32个字节偏移处准确地定位这个头
2. http://www.cnblogs.com/gansc23/archive/2010/11/27/1889537.html
*/
typedef __packed struct 
{
    uint8_t ucVBRI[4];		       /* 固定为VBRI四个字母 */ 	
	uint8_t ucVersion[2];		   /* 版本号，大端格式 */
	uint8_t ucDelay[2];		       /* 延迟，大端格式*/
	uint8_t ucQualityIndicator[2]; /* 音乐质量，0表示最好，100表示最差 */
	uint8_t ucSize[4];		       /* 文件大小，大端格式 */
	uint8_t ucFrames[4];		   /* 总帧数，大端格式 */ 
	uint8_t ucNumOfTOC[2];         /* TOC表的条目数，大端格式 */  
	uint8_t ucFactorOfTOC[2];      /* TOC表的条目放大因子，大端格式 */ 
	uint8_t ucSizePerTable[2];     /* 每个条目的大小，最大4字节，大端格式 */ 
	uint8_t ucFramesPerTable[2];   /* 每个条目的帧数，大端格式 */ 
}MP3FrameVBRI_T;

/* 用于MP3音频播放控制 */
typedef struct 
{
    uint8_t  ucTitle[40];	 /* 歌曲名 */
    uint8_t  ucArtist[40];	 /* 艺术家 */
	uint8_t  ucFmtVBRCBR;    /* 用于记录VBR或者CBR格式 */
    uint32_t uiBitrate;	     /* 码率   */
	uint32_t uiSamplerate;   /* 采样率 */
	uint16_t usOutsamples;	 /* PCM输出数据量大小 */
	uint32_t uiFrameStart;	 /* 实际音频帧的起始位置 */
	uint16_t usChannels;     /* 记录单通道或者双通道 */
	
	int32_t  iBytesleft;
	uint8_t  *pInputBuf;
	uint8_t  *pReadPos;	
	uint8_t  *pI2SBuffer0;
	uint8_t  *pI2SBuffer1;
	uint8_t  *pI2STempBuff;
}MP3Control_T;


/* 供外部文件调用的函数 */
uint32_t GetMP3Runtime(uint8_t *pFileName, uint32_t *uiTotalTime);
void GetMP3Info(uint8_t *_pPath);
void MP3MusicPlay(const char *filename);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

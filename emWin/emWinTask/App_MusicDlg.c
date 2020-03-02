/*
*********************************************************************************************************
*
*	模块名称 : 音乐播放器应用界面设计
*	文件名称 : App_MucisDlg.c
*	版    本 : V1.0
*	说    明 : 音乐播放器界面设计。
*              1. 支持上一曲，下一曲，快进和快退，常用的采样率和码率都支持，单声道和立体声也都支持。
*              2. emWin任务是低优先级任务，音乐解码任务是高优先级任务，两个任务之间通过任务消息队列和事件
*                 标志进行通信。
*              3. 首次使用先点击歌曲列表，歌曲名会被记录到listview控件里面，然后就可以任意操作了。
*                 如果文件夹中歌曲较多，首次打开会稍慢些，主要是因为要获取每首歌曲的播放时间。以后打开
*                 就比较快了，主要是对歌曲列表对话框做了隐藏和显示处理，而不是重复的创建和删除。
*              4. 歌曲列表对话框做了模态处理，这样用户打开此对话框后只能操作这个对话框，而不能操作主界面。
*
*	修改记录 :
*		版本号    日期         作者         说明
*       V1.0    2019-12      果果小师弟     首发
*
*	Copyright (C), 2020-2025, https://space.bilibili.com/357524621 
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
	#define printf_audiodbg printf
#else
	#define printf_audiodbg(...)
#endif


/*
*********************************************************************************************************
*				                      宏定义
*********************************************************************************************************
*/
#define ID_WINDOW_0 		(GUI_ID_USER + 0x00)
#define ID_IMAGE_0  		(GUI_ID_USER + 0x01)
#define ID_SLIDER_0 		(GUI_ID_USER + 0x02)
#define ID_BUTTON_0 		(GUI_ID_USER + 0x03)
#define ID_BUTTON_1 		(GUI_ID_USER + 0x04)
#define ID_BUTTON_2 		(GUI_ID_USER + 0x05)
#define ID_BUTTON_3 		(GUI_ID_USER + 0x06)
#define ID_BUTTON_4 		(GUI_ID_USER + 0x07)
#define ID_BUTTON_5 		(GUI_ID_USER + 0x08)
#define ID_BUTTON_6 		(GUI_ID_USER + 0x09)
#define ID_SLIDER_1  	    (GUI_ID_USER + 0x0A)
#define ID_TEXT_0 			(GUI_ID_USER + 0x0B)
#define ID_TEXT_1 			(GUI_ID_USER + 0x0C)
#define ID_TEXT_2 			(GUI_ID_USER + 0x0D)
#define ID_TEXT_3 			(GUI_ID_USER + 0x0E)

#define ID_BUTTON_7 		(GUI_ID_USER + 0x0F)

/* 不同定时器的句柄 */
#define ID_TIMER_SPEC       0
#define ID_TIMER_PROCESS    1

/* 歌曲路径字符长度 */
#define MusicPathSzie       100 



/*
*********************************************************************************************************
*				                    引用外部变量和函数
*********************************************************************************************************
*/
/* 声明一个支持5个消息的消息邮箱 */
extern os_mbx_declare (mailbox, 5);
extern MusicMsg_T s_tMusicMsg;  /* 用于给音乐播放任务发消息，发送音乐类型和歌曲路径 */

//static char  *_acbuffer;
//static	FILE *result;
//static	FILE *file;
/*
*********************************************************************************************************
*				                         变量
*********************************************************************************************************
*/
const char s_MusicPathDir[] = {"M0:\\Music\\"};  /* 存储器中歌曲存放的路径 */
static uint8_t s_MusicName[MusicPathSzie] = {0}; /* 用于记录当前播放的歌曲 */
static uint8_t s_ucPlayStatus = 0; 	      		 /* 播放器的开始和暂停按钮状态，0表示暂停，1表示运行 */
WM_HWIN  hWinMusic = WM_HWIN_NULL;               /* 音乐播放对话框句柄 */

//static void ShowJPEG(const char *Filename)
//{
//	WM_HMEM hMem;
//    U8     * pData;
//	
//	
//	/* 申请一块动态内存空间 */
//	hMem = GUI_ALLOC_AllocZero(1024*512);
//	/* 转换动态内存的句柄为指针 */
//	_acbuffer = GUI_ALLOC_h2p(hMem);
//	
//	/* 打开文件 */
//	result =  fopen ((char *)Filename, "r"); 	
//	if ((result != NULL)) 
//	{
//		return;
//	} 
//	result = fread(pData, 1, 512, file);
//	/* 读取图片数据到动态内存中 */
//	if (result != NULL) 
//	{
//		return;
//	}	
//	GUI_JPEG_Draw(_acBuffer,sizeof(_acBuffer),0,0);
//	/* 释放内存 */
//	GUI_ALLOC_Free(hMem);
//	fclose(file);
// }

/*
*********************************************************************************************************
*	函 数 名: DisWave
*	功能说明: 显示频谱
*	形    参: x    X轴坐标
*             y    Y轴坐标
*             clr  0 表示清除显示数据，用于刚打开界面时，放在上次的显示有记录
*                  1 表示不清除。
*	返 回 值: 无
*********************************************************************************************************
*/
static void DisWave(uint16_t x, uint16_t y, uint8_t clr)
{
	static uint16_t s_MP3Spec_TopVal[32] = {0};   /* 频谱顶值表 */
	static uint16_t s_MP3Spec_CurVal[32] = {0};	  /* 频谱当前值表 */
	static uint8_t  s_MP3Spec_Time[32] = {0};	  /* 顶值停留时间表 */
	const  uint16_t usMaxVal = 128;               /* 高度固定为128个像素 */
	uint16_t i;
	uint16_t temp;
	float ufTempValue;	
	if(clr == 0)
	{
		for(i = 0; i< 32; i++)
		{
			s_MP3Spec_TopVal[i] = 0;
			s_MP3Spec_CurVal[i] = 0;
			s_MP3Spec_Time[i] = 0;
			g_tWav.uiFFT[i] = 0;
		}
		
		/* 清空后就退出 */
		return;
	}

	/* 显示32条频谱 */								   
	for(i = 0; i < 32; i++)	
	{	
		if(g_tWav.ucSpectrum == 1)
		{
			/* 对数谱，对幅值取对数 */
			if(s_tMusicMsg.ucType == MusicType_WAV)
			{
				ufTempValue = (float)g_tWav.uiFFT[i] / 128;
				if(ufTempValue < 1) 
				{
					ufTempValue = 0;
				}
				temp = 80 * log10(ufTempValue);	
			}
			else
			{
				ufTempValue = (float)g_tWav.uiFFT[i] / 128;
				if(ufTempValue < 1) 
				{
					ufTempValue = 0;
				}
				temp = 64 * log10(ufTempValue);		
			}
		}
		else
		{
			/* 
			   1. 幅值谱
			   2. 将WAV音频得到的FFT模值除以32，MP3模值除以64，方便界面显示 
			*/
			if(s_tMusicMsg.ucType == MusicType_WAV)
			{
				temp = g_tWav.uiFFT[i] >> 5;
			}
			else
			{
				temp = g_tWav.uiFFT[i] >> 6;			
			}
		}

		/* 2. 更新频谱数值 */
		if(s_MP3Spec_CurVal[i] < temp)
		{
			s_MP3Spec_CurVal[i] = temp;
		}
		else
		{
			if(s_MP3Spec_CurVal[i] > 1)
			{
				s_MP3Spec_CurVal[i] -= 2;
			} 
			else 
			{
				s_MP3Spec_CurVal[i] = 0;
			}
		}

		/* 3. 更新频谱顶值 */
		if(s_MP3Spec_Time[i])
		{
			s_MP3Spec_Time[i]--;
		}
		else 
		{	
			/* 峰值减小1 */
			if(s_MP3Spec_TopVal[i])
			{
				s_MP3Spec_TopVal[i]--;
			}
		}

		/* 4. 重设频谱顶值 */
		if(s_MP3Spec_CurVal[i] > s_MP3Spec_TopVal[i])
		{
			s_MP3Spec_TopVal[i] = s_MP3Spec_CurVal[i];
			
			/* 重设峰值停顿时间 */
			s_MP3Spec_Time[i] = 10;
		}

		/* 5. 防止超出频谱值和顶值范围，高度固定为128个像素 */
		if(s_MP3Spec_CurVal[i] > usMaxVal)
		{
			s_MP3Spec_CurVal[i] = usMaxVal;
		}

		if(s_MP3Spec_TopVal[i] > usMaxVal)
		{
			s_MP3Spec_TopVal[i] = usMaxVal;
		} 	   
	}

	/* 6. 绘制得到的频谱 */
	for(i = 0; i < 32; i++)
	{
		/* 显示频谱 */
		GUI_DrawGradientV(x, 
		                  y + usMaxVal - s_MP3Spec_CurVal[i], 
		                  x + 10, 
		                  y + usMaxVal, 
		                  GUI_YELLOW, 
		                  GUI_GREEN);

		/* 显示顶值 */
		GUI_SetColor(GUI_RED);
		GUI_DrawHLine(y + usMaxVal - s_MP3Spec_TopVal[i] - 1, x, x + 10);
		x += 15;
	}
}

/*
*********************************************************************************************************
*				                         任务对话框初始化选项
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateMusic[] = 
{
	{ WINDOW_CreateIndirect, "Window",  ID_WINDOW_0,   0,   0,  800, 480, 0, 0x0, 0 },
	{ SLIDER_CreateIndirect, "Process", ID_SLIDER_0, 100, 365,  600,  16, 0, 0x0, 0 },

	/* 按钮功能 */
	{ BUTTON_CreateIndirect, "DlgBack",    ID_BUTTON_0,  700,   0,  100, 100, 0, 0, 0 },	
	{ BUTTON_CreateIndirect, "MusicPre",   ID_BUTTON_1,  100, 399,   72,  72, 0, 0, 0 },
	{ BUTTON_CreateIndirect, "MusicPlay",  ID_BUTTON_2,  200, 399,   72,  72, 0, 0, 0 },
	{ BUTTON_CreateIndirect, "MusicNext",  ID_BUTTON_4,  300, 399,   72,  72, 0, 0, 0 },
	{ BUTTON_CreateIndirect, "MusicSpk",   ID_BUTTON_5,  400, 399,   72,  72, 0, 0, 0 },
	{ BUTTON_CreateIndirect, "MusicList",  ID_BUTTON_6,    0,   0,  100, 100, 0, 0, 0 },
	
	{ BUTTON_CreateIndirect, "MusicSet",   ID_BUTTON_7,   0,   380,  100, 100, 0, 0, 0 },
	
	/* 声音大小 */
	{ SLIDER_CreateIndirect, "Speaker",    ID_SLIDER_1, 490, 425, 200, 20, 0, 0x0, 0  },
	
	/* 显示歌曲信息 */
	{ TEXT_CreateIndirect, "--/--",         ID_TEXT_0, 100, 345, 270, 20, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "--/--",         ID_TEXT_1, 390, 345, 120, 20, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "00:00/00:00",   ID_TEXT_2, 610, 345, 120, 20, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "-----------",   ID_TEXT_3, 110, 20,  580, 24, 0, 0x64, 0 },
};

/*
*********************************************************************************************************
*	函 数 名: _cbButtonBack
*	功能说明: 按钮回调函数
*	形    参: pMsg  消息指针
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbButtonBack(WM_MESSAGE * pMsg) 
{
	WM_HWIN  hWin;

	hWin  = pMsg->hWin;
	switch (pMsg->MsgId) 
	{
		case WM_PAINT:
			if (BUTTON_IsPressed(hWin)) 
			{
				GUI_SetColor(GUI_DARKGRAY);
				GUI_AA_FillCircle(100, 0, 72);
				GUI_SetBkColor(GUI_DARKGRAY);
				GUI_SetColor(GUI_WHITE); 
				GUI_DrawBitmap(&bmReturn, 40, 5);
			} 
			else 
			{
				GUI_SetColor(GUI_STCOLOR_LIGHTBLUE);
				GUI_AA_FillCircle(100, 0, 72);
				GUI_SetBkColor(GUI_STCOLOR_LIGHTBLUE);
				GUI_SetColor(GUI_WHITE);  
				GUI_DrawBitmap(&bmReturn, 40, 5);				
			}
			break;
			
		default:
			BUTTON_Callback(pMsg);
	}
}

/*
*********************************************************************************************************
*	函 数 名: _cbButtonList
*	功能说明: 按钮回调函数
*	形    参: pMsg  消息指针
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbButtonList(WM_MESSAGE * pMsg) 
{
	WM_HWIN  hWin;

	hWin  = pMsg->hWin;
	switch (pMsg->MsgId) 
	{
		case WM_PAINT:
			if (BUTTON_IsPressed(hWin)) 
			{
				GUI_SetColor(GUI_DARKGRAY);
				GUI_AA_FillCircle(0, 0, 72);
				GUI_SetBkColor(GUI_DARKGRAY);
				GUI_SetColor(GUI_WHITE); 
				
				GUI_DrawBitmap(&bmMusicList, 0, 5);
			} 
			else 
			{
				GUI_SetColor(GUI_STCOLOR_LIGHTBLUE);
				GUI_AA_FillCircle(0, 0, 72);
				GUI_SetBkColor(GUI_STCOLOR_LIGHTBLUE);
				GUI_SetColor(GUI_WHITE);  

				GUI_DrawBitmap(&bmMusicList, 0, 5);
			}
			break;
			
		default:
			BUTTON_Callback(pMsg);
	}
}

/*
*********************************************************************************************************
*	函 数 名: _cbButtonPre
*	功能说明: 按钮回调函数
*	形    参: pMsg  消息指针
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbButtonPre(WM_MESSAGE * pMsg) 
{
	WM_HWIN  hWin;

	hWin  = pMsg->hWin;
	switch (pMsg->MsgId) 
	{
		case WM_PAINT:
			if (BUTTON_IsPressed(hWin)) 
			{
				GUI_SetBkColor(GUI_WHITE);
				GUI_Clear();
				GUI_SetAlpha(0xb0);
				GUI_DrawBitmap(&bmMusicPre, 8, 8);
				GUI_SetAlpha(0);		
			} 
			else 
			{
				GUI_DrawBitmap(&bmMusicPre, 8, 8);		
			}
			break;
			
		default:
			BUTTON_Callback(pMsg);
	}
}

/*
*********************************************************************************************************
*	函 数 名: _cbButtonPlay
*	功能说明: 按钮回调函数
*	形    参: pMsg  消息指针
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbButtonPlay(WM_MESSAGE * pMsg) 
{
	switch (pMsg->MsgId) 
	{
		case WM_PAINT:
			if(s_ucPlayStatus == 0)
			{
				GUI_DrawBitmap(&bmMusicPlay, 8, 8);
			}
			else 
			{
				GUI_DrawBitmap(&bmMusicPause, 8, 8);
			}
			break;
			
		default:
			BUTTON_Callback(pMsg);
			break;
	}
}

/*
*********************************************************************************************************
*	函 数 名: _cbButtonNext
*	功能说明: 按钮回调函数
*	形    参: pMsg  消息指针
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbButtonNext(WM_MESSAGE * pMsg) 
{
	WM_HWIN  hWin;

	hWin  = pMsg->hWin;
	switch (pMsg->MsgId) 
	{
		case WM_PAINT:
			if (BUTTON_IsPressed(hWin)) 
			{
				GUI_SetBkColor(GUI_WHITE);
				GUI_Clear();
				GUI_SetAlpha(0xb0);
				GUI_DrawBitmap(&bmMusicNext, 8, 8);
				GUI_SetAlpha(0);		
			} 
			else 
			{
				GUI_DrawBitmap(&bmMusicNext, 8, 8);		
			}
			break;
			
		default:
			BUTTON_Callback(pMsg);
	}
}

/*
*********************************************************************************************************
*	函 数 名: _cbButtonSpeaker
*	功能说明: 按钮回调函数
*	形    参: pMsg  消息指针
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbButtonSpeaker(WM_MESSAGE * pMsg) 
{
	switch (pMsg->MsgId) 
	{
		case WM_PAINT:
			if(g_tWav.ucSpeakerStatus == 0)
			{
				GUI_DrawBitmap(&bmSpeakerMute, 8, 8);
			}
			else 
			{
				GUI_DrawBitmap(&bmSpeaker, 8, 8);
			}
			break;
			
		default:
			BUTTON_Callback(pMsg);
	}
}

/*
*********************************************************************************************************
*	函 数 名: _cbButtonSet
*	功能说明: 按钮回调函数
*	形    参: pMsg  消息指针
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbButtonSet(WM_MESSAGE * pMsg) 
{
	WM_HWIN  hWin;

	hWin  = pMsg->hWin;
	switch (pMsg->MsgId) 
	{
		case WM_PAINT:
			if (BUTTON_IsPressed(hWin)) 
			{
				GUI_SetColor(GUI_DARKGRAY);
				GUI_AA_FillCircle(0, 100, 72);
				GUI_SetBkColor(GUI_DARKGRAY);
				GUI_SetColor(GUI_WHITE); 
				GUI_DrawBitmap(&bmSetting, 5, 48);
			} 
			else 
			{
				GUI_SetColor(GUI_STCOLOR_LIGHTBLUE);
				GUI_AA_FillCircle(0, 100, 72);
				GUI_SetBkColor(GUI_STCOLOR_LIGHTBLUE);
				GUI_SetColor(GUI_WHITE);  
				GUI_DrawBitmap(&bmSetting, 5, 48);
			}
			break;
			
		default:
			BUTTON_Callback(pMsg);
	}
}

/*
*********************************************************************************************************
*	函 数 名: _cbMusicButton
*	功能说明: 上一曲，下一曲按钮的回调消息处理，因为处理内容基本一样，所有独立成一个函数。
*	形    参: pMsg    消息指针
*             _uiNum  要播放的歌曲序号
*            _pName   歌曲名
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbMusicButton(WM_MESSAGE *pMsg, int _uiNum, char *_pName) 
{
	WM_HWIN hItem;
	char buf[50];
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
	TEXT_SetText(hItem, _pName);
	
	/* 将歌曲完整的路径复制到缓冲s_MusicName里面 */
	memset(s_MusicName, 0, MusicPathSzie);
	
	strcpy((char *)s_MusicName, s_MusicPathDir);
	strcat((char *)s_MusicName, _pName);
	printf_audiodbg("%s\r\n", s_MusicName);
	
	/* 显示当前歌曲播放序号和总的歌曲数量 */
	hItem = WM_GetDialogItem(hWinMusicList, GUI_ID_LISTVIEW0);
	sprintf(buf, "%d/%d", _uiNum + 1, LISTVIEW_GetNumRows(hItem));
	hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
	TEXT_SetText(hItem, buf);

	/* 发消息，退出当前音乐播放 */	
	os_evt_set (MusicTaskAudioReturn_6, HandleMusicStart);

	s_tMusicMsg.ucName = (uint8_t *)s_MusicName;
	if(strstr((char *)s_MusicName,".WAV")||strstr((char *)s_MusicName,".wav"))
	{
		/* WAV歌曲 */
		s_tMusicMsg.ucType = MusicType_WAV;				
	}
	else
	{
		/* MP3歌曲 */
		s_tMusicMsg.ucType = MusicType_MP3;	
	}

	/* 发消息，播放新歌 */
	if(os_mbx_send (&mailbox, &s_tMusicMsg, 0xFFFF) == OS_R_OK)
	{
		/* 根据按钮按下或者播放的状态发送消息 */
		if(s_ucPlayStatus == 1)
		{
			os_evt_set (MusicTaskAudioPlay_5, HandleMusicStart);					
		}
		else
		{
			os_evt_set (MusicTaskAudioStart_8, HandleMusicStart);				
		}
		
		/* 更新播放时间和歌曲信息 */
		hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0);
		SLIDER_SetRange(hItem, 0, g_tWav.uiTotalTime);
		SLIDER_SetValue(hItem, 0);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
		TEXT_SetText(hItem, (const char *)g_tWav.usInfo);
		
		sprintf(buf, "%02d:%02d/%02d:%02d", 0, 0, g_tWav.uiTotalTime/600, g_tWav.uiTotalTime/10%60);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
		TEXT_SetFont(hItem, &GUI_FontLubalGraph20B);
		TEXT_SetText(hItem, buf);
	}
}

/*
*********************************************************************************************************
*	函 数 名: _cbDialogMusic
*	功能说明: 音乐播放对话框回调消息
*	形    参: pMsg  消息指针
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbDialogMusic(WM_MESSAGE * pMsg) 
{
	const   GUI_RECT  Rect = {162, 200-29, 638, 300}; /* 左上角x，y，右下角x，y*/
	static  WM_HTIMER hTimerSpec;
	static  WM_HTIMER hTimerProcess;
	static  int  MusicPlayNum = 0;  
	WM_HWIN hItem;
	int     NCode;
	int     Id;
	char    buf[MusicPathSzie + 20];
	
	(void)hTimerSpec;
	
	switch (pMsg->MsgId) 
	{
		/* 对话框初始化消息 */
		case WM_INIT_DIALOG:
			hTimerSpec = WM_CreateTimer(pMsg->hWin, ID_TIMER_SPEC, 20, 0);	
			hTimerProcess = WM_CreateTimer(pMsg->hWin, ID_TIMER_PROCESS, 100, 0);			

			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
			WM_SetHasTrans(hItem);
			WM_SetCallback(hItem, _cbButtonBack);
		
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
			WM_SetHasTrans(hItem);
			WM_SetCallback(hItem, _cbButtonPre);
		
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
			WM_SetHasTrans(hItem);
			WM_SetCallback(hItem, _cbButtonPlay);
		
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_4);
			WM_SetHasTrans(hItem);
			WM_SetCallback(hItem, _cbButtonNext);
		
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_5);
			WM_SetHasTrans(hItem);
			WM_SetCallback(hItem, _cbButtonSpeaker);
			
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_6);
			WM_SetHasTrans(hItem);
			WM_SetCallback(hItem, _cbButtonList);
			
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_7);
			WM_SetHasTrans(hItem);
			WM_SetCallback(hItem, _cbButtonSet);
		
			hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0); 
			SLIDER_SetBkColor(hItem, GUI_WHITE);
			SLIDER_SetFocusColor (hItem, GUI_STCOLOR_LIGHTBLUE);
			SLIDER_SetValue(hItem, 0);
			SLIDER_SetWidth(hItem, 0);
			SLIDER_SetSTSkin(hItem);
			
			hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_1); 
			SLIDER_SetBkColor(hItem, GUI_WHITE);
			SLIDER_SetFocusColor (hItem, GUI_STCOLOR_LIGHTBLUE);
			SLIDER_SetRange(hItem, 0, 63);
			SLIDER_SetValue(hItem, g_tWav.ucVolume);
			SLIDER_SetWidth(hItem, 0);
			SLIDER_SetSTSkin(hItem);
			
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
			TEXT_SetFont(hItem, &GUI_FontLubalGraph20B);
			TEXT_SetTextColor(hItem, GUI_STCOLOR_LIGHTBLUE);
			TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
			
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
			TEXT_SetFont(hItem, &GUI_FontLubalGraph20B);
			TEXT_SetTextColor(hItem, GUI_STCOLOR_LIGHTBLUE);
			TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
			
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
			TEXT_SetFont(hItem, &GUI_FontLubalGraph20B);
			TEXT_SetTextColor(hItem, GUI_STCOLOR_LIGHTBLUE);
			TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
			
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
			TEXT_SetFont(hItem, &GUI_FontHZ16);
			TEXT_SetTextColor(hItem, GUI_STCOLOR_LIGHTBLUE);
			TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			break;
			
			

		
		/* 定时器回调消息 */
		case WM_TIMER:
			Id = WM_GetTimerId(pMsg->Data.v);
			switch (Id) 
			{
				/* 时间到了更新频谱 */
				case ID_TIMER_SPEC:
					if(g_tWav.ucDispUpdate == 1)
					{
						WM_InvalidateRect(pMsg->hWin, &Rect);
					}
					WM_RestartTimer(pMsg->Data.v, 20);				
					break;
				
				/* 播放进度和播放时间更新 */
				case ID_TIMER_PROCESS:
					if(g_tWav.ucDispUpdate == 1)
					{
						os_evt_set (MusicTaskAudioGetTime_7, HandleMusicStart);
						sprintf(buf,"%02d:%02d/%02d:%02d", g_tWav.uiCurTime/600, g_tWav.uiCurTime/10%60, g_tWav.uiTotalTime/600, g_tWav.uiTotalTime/10%60);
						hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
						TEXT_SetFont(hItem, &GUI_FontLubalGraph20B);
						TEXT_SetText(hItem, buf);
					
						hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0); 
						SLIDER_SetValue(hItem, g_tWav.uiCurTime);
					}
					WM_RestartTimer(pMsg->Data.v, 100);				
					break;
			}
	        break;
			
		/* 重绘消息处理，使用了多缓冲 */
		case WM_PRE_PAINT:
			GUI_MULTIBUF_Begin();
		    break;
		
		case WM_PAINT:
			
			GUI_SetBkColor(GUI_LIGHTCYAN);
			GUI_Clear();   //修改完桌面背景颜色后必须清清屏，否则修改不成功
		
			DisWave(162, 200-28, 1);
			break;
		
		case WM_POST_PAINT:
			GUI_MULTIBUF_End();
		    break;
		
		/* 切换到下一首歌曲，音乐播放任务是高优先级任务，emWin任务是低优先级任务
		   当音乐播放任务通过函数WM_SendMessageNoPara发送消息给emWin任务时，会立即
		   执行此消息，这是因为emWin多任务的实现机制问题，看GUI_X_RTX.C文件即可。
		*/
		case MSG_NextMusic:
			/*1. 获取下一首要播放歌曲的序号 */
			hItem = WM_GetDialogItem(hWinMusicList, GUI_ID_LISTVIEW0);
			MusicPlayNum++;
			if(MusicPlayNum == LISTVIEW_GetNumRows(hItem)) 
			{
				MusicPlayNum = 0;
			}
			
			/* 2. 获取歌曲名 */
			LISTVIEW_GetItemText(hItem, 0, MusicPlayNum, buf, MusicPathSzie);
			
			/* 3. 显示歌曲名到界面上 */
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
			TEXT_SetText(hItem,  (const char *)buf);

			/* 4. 将歌曲完整的路径复制到缓冲s_MusicName里面 */
			memset(s_MusicName, 0, MusicPathSzie);
			strcpy((char *)s_MusicName, s_MusicPathDir);
			strcat((char *)s_MusicName, buf);
			
			/* 5. 显示当前歌曲播放序号和总的歌曲数量 */
			hItem = WM_GetDialogItem(hWinMusicList, GUI_ID_LISTVIEW0);
			sprintf(buf, "%d/%d", MusicPlayNum + 1, LISTVIEW_GetNumRows(hItem));
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
			TEXT_SetText(hItem,  (const char *)buf);

			/* 6. 播放新歌 */
			s_tMusicMsg.ucName = (uint8_t *)s_MusicName;
			if(strstr((char *)s_MusicName,".WAV")||strstr((char *)s_MusicName,".wav"))
			{
				/* WAV格式 */
				s_tMusicMsg.ucType = MusicType_WAV;				
			}
			else
			{
				/* MP3格式 */
				s_tMusicMsg.ucType = MusicType_MP3;	
			}
			/* 
			   给音乐任务发送歌曲路径，因为emWin多任务的执行，
			   此时音乐任务被挂起，回调消息执行完才会回到音乐任务 。
			*/
			if(os_mbx_send (&mailbox, &s_tMusicMsg, 0xFFFF) == OS_R_OK)
			{
				/* 获取歌曲相关信息进行显示 */
				if(s_tMusicMsg.ucType == 1)
				{
					GetWavInfo(s_MusicName);						
				}
				else
				{
					GetMP3Info(s_MusicName);
				}
				hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0);
				SLIDER_SetRange(hItem, 0, g_tWav.uiTotalTime);

				hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
				TEXT_SetText(hItem, (const char *)g_tWav.usInfo);
				
				sprintf(buf, "%02d:%02d/%02d:%02d", 0, 0, g_tWav.uiTotalTime/600, g_tWav.uiTotalTime/10%60);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
				TEXT_SetFont(hItem, &GUI_FontLubalGraph20B);
				TEXT_SetText(hItem, buf);
				
				/* 继续发消息，启动歌曲播放 */
				os_evt_set (MusicTaskAudioPlay_5, HandleMusicStart);
			}
			break;
		
		/*  开始执行音乐播放 */
		case MSG_MusicStart:
			hItem = WM_GetDialogItem(hWinMusicList, GUI_ID_LISTVIEW0);
			MusicPlayNum = LISTVIEW_GetSel(hItem);
			LISTVIEW_GetItemText(hItem, 0, MusicPlayNum, buf, MusicPathSzie);
			_cbMusicButton(pMsg, MusicPlayNum, buf);					
			break;
		
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch(Id) 
			{
				/* 关闭音乐播放对话框 */
				case ID_BUTTON_0:
					switch(NCode) 
					{
						case WM_NOTIFICATION_RELEASED:
							/* 不管开启没有开启播放，都发消息，退出当前音频播放 */
							os_evt_set (MusicTaskAudioReturn_6, HandleMusicStart);
							hWinMusicList = WM_HWIN_NULL;
							hWinMusic = WM_HWIN_NULL;
							GUI_EndDialog(pMsg->hWin, 0);
							break;
					}
					break;
					
				/* 上一曲 */
				case ID_BUTTON_1:
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						
						case WM_NOTIFICATION_RELEASED:
							if(WM_IsWindow(hWinMusicList))
							{
								/* 调整到上一首歌曲 */
								hItem = WM_GetDialogItem(hWinMusicList, GUI_ID_LISTVIEW0);
								if(MusicPlayNum == 0) 
								{
									MusicPlayNum = LISTVIEW_GetNumRows(hItem);								
								}
								MusicPlayNum--;
								LISTVIEW_GetItemText(hItem, 0, MusicPlayNum, buf, MusicPathSzie);
								
								/* 执行相应操作 */
								_cbMusicButton(pMsg, MusicPlayNum, buf);	
							 }
							break;
					}
					break;
					
				/* 下一曲 */
				case ID_BUTTON_4:
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						
						case WM_NOTIFICATION_RELEASED:
							if(WM_IsWindow(hWinMusicList))
							{
								/* 调整到下一首歌曲 */
								hItem = WM_GetDialogItem(hWinMusicList, GUI_ID_LISTVIEW0);
								MusicPlayNum++;
							    if(MusicPlayNum == LISTVIEW_GetNumRows(hItem)) 
								{
									MusicPlayNum = 0;
								}
								LISTVIEW_GetItemText(hItem, 0, MusicPlayNum, buf, MusicPathSzie);
								
								/* 执行相应操作 */
								_cbMusicButton(pMsg, MusicPlayNum, buf);	
							 }
					}
					break;
					
				/* 歌曲列表 */
				case ID_BUTTON_6:
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						
						case WM_NOTIFICATION_RELEASED:
							App_MusicList(pMsg->hWin);
							break;
					}
					break;
					
				/* 频谱模式设置 */
				case ID_BUTTON_7:
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						
						case WM_NOTIFICATION_RELEASED:
							App_MusicSet(pMsg->hWin);
							break;
					}
					break;
					
				/* 用于实现快进快退 */
				case ID_SLIDER_0:
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							WM_DeleteTimer(hTimerProcess);
							break;
						
						case WM_NOTIFICATION_RELEASED:
							hTimerProcess = WM_CreateTimer(pMsg->hWin, ID_TIMER_PROCESS, 100, 0);		
							if(WM_IsWindow(hWinMusicList))
							{
								hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0);
								g_tWav.uiCurTime = SLIDER_GetValue(hItem);
								os_evt_set (MusicTaskAudioAdjust_1, HandleMusicStart);	
							}
							break;
					}
					break;
					
				/* 用于实现快进快退 */
				case ID_SLIDER_1:
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						
						case WM_NOTIFICATION_RELEASED:	
							if(WM_IsWindow(hWinMusicList))
							{
								hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_1);
								g_tWav.ucVolume  = SLIDER_GetValue(hItem);
								/* 调节音量，左右相同音量 */
								wm8978_SetEarVolume(g_tWav.ucVolume);
								wm8978_SetSpkVolume(g_tWav.ucVolume);
							}
							break;
					}
					break;

				/* 暂停和开始按钮 */
				case ID_BUTTON_2:
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						
						case WM_NOTIFICATION_RELEASED:
							if(WM_IsWindow(hWinMusicList))
							{
								if(s_ucPlayStatus == 1)
								{
									/* 暂停 */	
									s_ucPlayStatus = 0;
									g_tWav.ucDispUpdate = 0;
									os_evt_set (MusicTaskAudioPause_2, HandleMusicStart);
								}
								else
								{
									/* 播放 */	
									s_ucPlayStatus = 1;
									g_tWav.ucDispUpdate = 1;
									os_evt_set (MusicTaskAudioResume_3, HandleMusicStart);
								}
							}
							else
							{
								if(s_ucPlayStatus == 1)
								{
									s_ucPlayStatus = 0;
								}
								else
								{
									s_ucPlayStatus = 1;
								}
							}
							break;
					}
					break;
					
				/* 静音设置 */
				case ID_BUTTON_5:
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						
						case WM_NOTIFICATION_RELEASED:
							if(WM_IsWindow(hWinMusicList))
							{
								if(g_tWav.ucSpeakerStatus == 1)
								{
									/* 静音 */	
									g_tWav.ucSpeakerStatus = 0;
									wm8978_OutMute(1);
								}
								else
								{
									/* 不静音 */	
									g_tWav.ucSpeakerStatus = 1;
									wm8978_OutMute(0);
								}
							}
							else
							{
								if(g_tWav.ucSpeakerStatus == 1)
								{
									g_tWav.ucSpeakerStatus = 0;
								}
								else
								{
									g_tWav.ucSpeakerStatus = 1;
								}
							}
							break;
					}
					break;
			}
			break;

		default:
			WM_DefaultProc(pMsg);
			break;
	}
}

/*
*********************************************************************************************************
*	函 数 名: App_Music
*	功能说明: 创建音乐播放对话框
*	形    参: hWin  父窗口
*	返 回 值: 无
*********************************************************************************************************
*/
void App_Music(WM_HWIN hWin) 
{

	s_ucPlayStatus = 0; 	    /* 播放器的开始和暂停按钮状态，0表示暂停，1表示运行 */
	g_tWav.ucDispUpdate = 0;	
	g_tWav.ucSpectrum = 1;		/* 1表示对数谱，0表示幅值谱 */
	
//	/* 申请一块内存空间 并且将其清零  这里申请6MB的内存 */
//	hMem = GUI_ALLOC_AllocZero(1024*512);
//	/* 将申请到内存的句柄转换成指针类型 */
//	pic_buffer = GUI_ALLOC_h2p(hMem);
//    GUI_ALLOC_Free(hMem); 	
	
	/* 清空频谱显示区的数据 */
	DisWave(162, 200-28, 0);
	hWinMusic = GUI_CreateDialogBox(_aDialogCreateMusic, 
	                                GUI_COUNTOF(_aDialogCreateMusic),
                                  	_cbDialogMusic, 
	                                hWin, 
	                                0, 
	                                0);	
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

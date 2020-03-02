/*
*********************************************************************************************************
*	                                  
*	模块名称 : 视频播放器应用界面设计
*	文件名称 : App_VideoDlg.c
*	版    本 : V1.0
*	说    明 : 视频播放器界面设计。
*              1. 支持上一个，下一个，快进和快退。
*              2. emWin任务是低优先级任务，音频解码任务是高优先级任务，两个任务之间通过消息邮箱和事件
*                 标志进行通信。
*              3. 首次使用先点击视频列表，视频名会被记录到listview控件里面，然后就可以任意操作了。
*                 如果文件夹中视频较多，首次打开会稍慢些，主要是因为要获取每首视频的播放时间。以后打开
*                 就比较快了，主要是对视频列表对话框做了隐藏和显示处理，而不是重复的创建和删除。
*              4. 视频列表对话框做了模态处理，这样用户打开此对话框后只能操作这个对话框，而不能操作主界面。
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
	#define printf_videodbg printf
#else
	#define printf_videodbg(...)
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
#define ID_TEXT_4 			(GUI_ID_USER + 0x0F)

/* 不同定时器的句柄 */
#define ID_TIMER_NEXT       0
#define ID_TIMER_PROCESS    1

/* 视频路径字符长度 */
#define VideoPathSzie       100 


/*
*********************************************************************************************************
*				                    引用外部变量和函数
*********************************************************************************************************
*/
/* 声明一个支持5个消息的消息邮箱 */
extern os_mbx_declare (mailbox, 5);
extern MusicMsg_T s_tMusicMsg;  /* 用于给音乐播放任务发消息，发送音乐类型和歌曲路径 */


/*
*********************************************************************************************************
*				                         变量
*********************************************************************************************************
*/
const char s_VideoPathDir[] = {"M0:\\Video\\"};  /* 存储器中歌曲存放的路径 */
static uint8_t s_VideoName[VideoPathSzie] = {0}; /* 用于记录当前播放的歌曲 */
static uint8_t s_ucPlayStatus = 0; 	      		 /* 播放器的开始和暂停按钮状态，0表示暂停，1表示运行 */           		 
WM_HWIN  hWinVideo = WM_HWIN_NULL;               /* 视频播放对话框句柄 */
U8 *g_pVevio;                                    /* 视频播放缓冲 */
uint8_t g_ucJpegOpti = 0;                        /* 是否优化JPEG，0表示不优化，1表示优化 */

/* 下面三个变量用于视频文件的读取和处理 */
static FILE *ffilm;
static GUI_MOVIE_INFO   Info;
static GUI_MOVIE_HANDLE hMovie = 0;

GUI_HMEM hMemVedio;  /* 视频播放需要用到的动态内存 */


/*
*********************************************************************************************************
*	函 数 名: _cbNotify
*	功能说明: 视频播放回调函数
*	形    参: hMem          存储设备句柄
*             Notification  消息类型
*             CurrentFrame  当前帧 
*	返 回 值: 无
*********************************************************************************************************
*/
void _cbNotify(GUI_HMEM hMem, int Notification, U32 CurrentFrame) 
{
	static uint32_t t0, t1;
	
	(void)t1;
	
	switch (Notification) 
	{
		case GUI_MOVIE_NOTIFICATION_PREDRAW:
			t0 = GUI_GetTime();
			break;
		
		case GUI_MOVIE_NOTIFICATION_POSTDRAW:
			t1 = GUI_GetTime() - t0;
			printf_videodbg("t = %d\r\n", t1);
			break;
		
		case GUI_MOVIE_NOTIFICATION_STOP:
			break;
		
		case GUI_MOVIE_NOTIFICATION_START:
			break;
		
		case GUI_MOVIE_NOTIFICATION_DELETE:
			break;
	}
}

/*
*********************************************************************************************************
*	函 数 名: _GetData
*	功能说明: 被函数GUI_MOVIE_GetInfoEx调用
*	形    参：p             FIL类型数据
*             NumBytesReq   请求读取的字节数
*             ppData        数据指针
*             Off           如果Off = 1，那么将重新从起始位置读取                 
*	返 回 值: 返回读取的字节数
*********************************************************************************************************
*/
int _GetData(void * p, const U8 ** ppData, unsigned NumBytes, U32 Off) 
{
	static int FileAddress = 0;
	FILE *file;
	U32    NumBytesRead;
	U8     * pData;

	pData  = (U8 *)*ppData;
	file = (FILE *)p;
	
	/*
	* 设置读取位置
	*/
	if(Off == 1) 
	{
		FileAddress = 0;		
	}
	else 
	{
		FileAddress = Off;		
	}

	fseek (file, FileAddress, SEEK_SET);
	
	/*
	* 读取数据到缓存
	*/
	NumBytesRead = fread (pData, 1, NumBytes, file);

	/*
	* 返回读取的字节数
	*/
	return NumBytesRead;
}

/*
*********************************************************************************************************
*				                         任务对话框初始化选项
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateVideo[] = 
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
	
	/* 声音大小 */
	{ SLIDER_CreateIndirect, "Speaker",    ID_SLIDER_1, 490, 425, 200, 20, 0, 0x0, 0  },
	
	/* 显示歌曲信息 */
	{ TEXT_CreateIndirect, "--/--",              ID_TEXT_0, 100, 345, 270, 20, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "--/--",              ID_TEXT_1, 480, 345, 100, 20, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "00:00:00/00:00:00",  ID_TEXT_2, 560, 345, 170, 20, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "-----------",        ID_TEXT_3, 250, 20,  300, 24, 0, 0x64, 0 },
	
	{ TEXT_CreateIndirect, "--/--",              ID_TEXT_4, 320, 345, 120, 20, 0, 0x64, 0 },	
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
				
				GUI_DrawBitmap(&bmVideoIcon, 2, 5);
			} 
			else 
			{
				GUI_SetColor(GUI_STCOLOR_LIGHTBLUE);
				GUI_AA_FillCircle(0, 0, 72);
				GUI_SetBkColor(GUI_STCOLOR_LIGHTBLUE);
				GUI_SetColor(GUI_WHITE);  

				GUI_DrawBitmap(&bmVideoIcon, 2, 5);
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
*	函 数 名: _cbMusicButton
*	功能说明: 上一个，下一个按钮的回调消息处理，因为处理内容基本一样，所有独立成一个函数。
*	形    参: pMsg    消息指针
*             _uiNum  要播放的音频，视频序号
*            _pName   音频名，视频名
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbMusicButton(WM_MESSAGE *pMsg, int _uiNum, char *_pName) 
{
	const   GUI_RECT  Rect = {160, 54, 640, 326};
	WM_HWIN hItem;
	char buf[20];
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
	TEXT_SetText(hItem, _pName);
	
	/* 将音频完整的路径复制到缓冲s_VideoName里面 */
	memset(s_VideoName, 0, VideoPathSzie);
	
	strcpy((char *)s_VideoName, s_VideoPathDir);
	strncat((char *)s_VideoName, _pName, strlen(_pName)-3);
	strcat((char *)s_VideoName, "mp3");
	printf_videodbg("%s\r\n", s_VideoName);
	
	/* 显示当前音频播放序号和总的歌曲数量 */
	hItem = WM_GetDialogItem(hWinVideoList, GUI_ID_LISTVIEW0);
	sprintf(buf, "%d/%d", _uiNum + 1, LISTVIEW_GetNumRows(hItem));
	hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
	TEXT_SetText(hItem, buf);

	/* 发消息，退出当前音频播放 */	
	os_evt_set (MusicTaskAudioReturn_6, HandleMusicStart);

	s_tMusicMsg.ucName = (uint8_t *)s_VideoName;
	if(strstr((char *)s_VideoName,".WAV")||strstr((char *)s_VideoName,".wav"))
	{
		/* WAV音频 */
		s_tMusicMsg.ucType = MusicType_WAV;				
	}
	else
	{
		/* MP3音频 */
		s_tMusicMsg.ucType = MusicType_MP3;	
	}

	/* 发消息，播放音频 */
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
		/* 更新播放时间和音频信息 */
		hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0);
		SLIDER_SetRange(hItem, 0, g_tWav.uiTotalTime);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
		TEXT_SetText(hItem, (const char *)g_tWav.usInfo);
	}
	
	memset(s_VideoName, 0, VideoPathSzie);
	
	strcpy((char *)s_VideoName, s_VideoPathDir);
	strncat((char *)s_VideoName, _pName, strlen(_pName)-3);
	strcat((char *)s_VideoName, "emf");
	
	/* 首次打开 */
	if(hMovie == 0)
	{
		/* 打开文件 */	
		ffilm = fopen ((char *)s_VideoName, "r"); 

		if (GUI_MOVIE_GetInfoEx(_GetData, ffilm, &Info) == 0) 
		{
			printf_videodbg("Info.msPerFrame = %d\r\n", Info.msPerFrame);
			hMovie = GUI_MOVIE_CreateEx(_GetData, ffilm, _cbNotify);
			GUI_MOVIE_SetPeriod(hMovie,  Info.msPerFrame);
			if (hMovie) 
			{
				GUI_MOVIE_Show(hMovie, (LCD_GetXSize() - Info.xSize) / 2, (LCD_GetYSize() - Info.ySize) / 2 - 50, 0);
				if(s_ucPlayStatus == 1)
				{		
					GUI_MOVIE_Play(hMovie);
				}
				else
				{
					GUI_MOVIE_Pause(hMovie);
				}
			}
		}
	}
	/* 视频切换 */
	else
	{
		WM_InvalidateRect(pMsg->hWin, &Rect);
		
		GUI_MOVIE_Delete(hMovie);
		if(ffilm != NULL)
		{
			fclose(ffilm);
		}
		/* 打开文件 */	
		ffilm = fopen ((char *)s_VideoName, "r"); 

		if (GUI_MOVIE_GetInfoEx(_GetData, ffilm, &Info) == 0) 
		{
			printf_videodbg("Info.msPerFrame = %d\r\n", Info.msPerFrame);
			hMovie = GUI_MOVIE_CreateEx(_GetData, ffilm, _cbNotify);
			GUI_MOVIE_SetPeriod(hMovie, Info.msPerFrame);
			if (hMovie) 
			{
				GUI_MOVIE_Show(hMovie, (LCD_GetXSize() - Info.xSize) / 2, (LCD_GetYSize() - Info.ySize) / 2 - 50, 0);
				if(s_ucPlayStatus == 1)
				{		
					GUI_MOVIE_Play(hMovie);
				}
				else
				{
					GUI_MOVIE_Pause(hMovie);
				}
			}
		}
	}
	
	sprintf(buf, "%dx%d/%dFPS", Info.xSize, Info.ySize, 1000/Info.msPerFrame);
	hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_4);
	TEXT_SetText(hItem, buf);
	
	/* 更新播放时间，注意时间单位被放大了10倍 */
	sprintf(buf, "%02d:%02d:%02d/%02d:%02d:%02d", 
				  0, 
				  0, 
				  0, 
				  g_tWav.uiTotalTime/36000, 
				  g_tWav.uiTotalTime%36000/600, 
				  g_tWav.uiTotalTime/10%60);
	hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
	TEXT_SetFont(hItem, &GUI_FontLubalGraph20B);
	TEXT_SetText(hItem, buf);
			
	hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0); 
	SLIDER_SetValue(hItem, 0);
}

/*
*********************************************************************************************************
*	函 数 名: _cbDialogVideo
*	功能说明: 视频播放对话框回调消息
*	形    参: pMsg  消息指针
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbDialogVideo(WM_MESSAGE * pMsg) 
{
	const   GUI_RECT  Rect = {160, 54, 640, 326}; /* 左上角x，y，右下角x，y*/
	static  WM_HTIMER hTimerProcess, hTimerNext;
	static  int  MusicPlayNum = 0;  
	WM_HWIN hItem;
	int     NCode;
	int     Id;
	char    buf[VideoPathSzie + 20];
	
	
	switch (pMsg->MsgId) 
	{
		/* 对话框初始化消息 */
		case WM_INIT_DIALOG:
			hTimerProcess = WM_CreateTimer(pMsg->hWin, ID_TIMER_PROCESS, 500, 0);			

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
			TEXT_SetFont(hItem, &GUI_FontLubalGraph20B);
			TEXT_SetTextColor(hItem, GUI_STCOLOR_LIGHTBLUE);
			TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_4);
			TEXT_SetFont(hItem, &GUI_FontLubalGraph20B);
			TEXT_SetTextColor(hItem, GUI_STCOLOR_LIGHTBLUE);
			TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			break;
		
		/* 定时器回调消息 */
		case WM_TIMER:
			Id = WM_GetTimerId(pMsg->Data.v);
			switch (Id) 
			{
				/* 播放进度和播放时间更新 */
				case ID_TIMER_PROCESS:
					if(g_tWav.ucDispUpdate == 1)
					{
						//g_tWav.uiCurTime = GUI_MOVIE_GetFrameIndex(hMovie) / (1000 / Info.msPerFrame);	
						//g_tWav.uiTotalTime = Info.NumFrames / (1000 / Info.msPerFrame);
						os_evt_set (MusicTaskAudioGetTime_7, HandleMusicStart);
						/* 注意时间单位被放大了10倍 */
						sprintf(buf, "%02d:%02d:%02d/%02d:%02d:%02d", 
									  g_tWav.uiCurTime/36000, 
									  g_tWav.uiCurTime%36000/600, 
									  g_tWav.uiCurTime/10%60, 
									  g_tWav.uiTotalTime/36000, 
									  g_tWav.uiTotalTime%36000/600, 
									  g_tWav.uiTotalTime/10%60);
						hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
						TEXT_SetFont(hItem, &GUI_FontLubalGraph20B);
						TEXT_SetText(hItem, buf);
					
						hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0); 
						SLIDER_SetValue(hItem, g_tWav.uiCurTime);
					}
					WM_RestartTimer(pMsg->Data.v, 500);				
					break;
				
				case ID_TIMER_NEXT:
					WM_DeleteTimer(hTimerNext);
				
					if(ffilm != NULL)
					{
						fclose(ffilm);
					}
					
					/* 继续发消息，启动音频播放 */
					os_evt_set (MusicTaskAudioStart_8, HandleMusicStart);
					
					memset(s_VideoName, 0, VideoPathSzie);
			
					hItem = WM_GetDialogItem(hWinVideoList, GUI_ID_LISTVIEW0);
					LISTVIEW_GetItemText(hItem, 0, MusicPlayNum, buf, VideoPathSzie);
					strcpy((char *)s_VideoName, s_VideoPathDir);
					strncat((char *)s_VideoName, buf, strlen(buf)-3);
					strcat((char *)s_VideoName, "emf");
					
					/* 打开文件 */	
					ffilm = fopen ((char *)s_VideoName, "r"); 

					if (GUI_MOVIE_GetInfoEx(_GetData, ffilm, &Info) == 0) 
					{
						sprintf(buf, "%dx%d/%dFPS", Info.xSize, Info.ySize, 1000/Info.msPerFrame);
						hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_4);
						TEXT_SetText(hItem, buf);
						hMovie = GUI_MOVIE_CreateEx(_GetData, ffilm, _cbNotify);
						GUI_MOVIE_SetPeriod(hMovie, Info.msPerFrame);
						if (hMovie) 
						{
							os_evt_set (MusicTaskAudioResume_3, HandleMusicStart);
							GUI_MOVIE_Show(hMovie, (LCD_GetXSize() - Info.xSize) / 2, (LCD_GetYSize() - Info.ySize) / 2 - 50, 0);
							if(s_ucPlayStatus == 1)
							{		
								GUI_MOVIE_Play(hMovie);
							}
							else
							{
								GUI_MOVIE_Pause(hMovie);
							}
						}
					}
					break;
				}
			break;
				
		case WM_PAINT:
			GUI_SetBkColor(GUI_LIGHTCYAN);
			GUI_Clear();
			break;
		
		/* 切换到下一首音频，音频播放任务是高优先级任务，emWin任务是低优先级任务
		   当音频播放任务通过函数WM_SendMessageNoPara发送消息给emWin任务时，会立即
		   执行此消息，这是因为emWin多任务的实现机制问题，看GUI_X_RTX.C文件即可。
		*/
		case MSG_NextMusic:
			WM_InvalidateRect(pMsg->hWin, &Rect);
			GUI_MOVIE_Delete(hMovie);
			/*1. 获取下一首要播放音频的序号 */
			hItem = WM_GetDialogItem(hWinVideoList, GUI_ID_LISTVIEW0);
			MusicPlayNum++;
			if(MusicPlayNum == LISTVIEW_GetNumRows(hItem)) 
			{
				MusicPlayNum = 0;
			}
			
			/* 2. 获取音频名 */
			LISTVIEW_GetItemText(hItem, 0, MusicPlayNum, buf, VideoPathSzie);
			
			/* 3. 显示音频名到界面上 */
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
			TEXT_SetText(hItem,  (const char *)buf);

			/* 4. 将音频完整的路径复制到缓冲s_VideoName里面 */
			memset(s_VideoName, 0, VideoPathSzie);
			
			strcpy((char *)s_VideoName, s_VideoPathDir);
			strncat((char *)s_VideoName, buf, strlen(buf)-3);
			strcat((char *)s_VideoName, "mp3");
			printf_videodbg("%s\r\n", s_VideoName);
			
			/* 5. 显示当前音频播放序号和总的音频数量 */
			hItem = WM_GetDialogItem(hWinVideoList, GUI_ID_LISTVIEW0);
			sprintf(buf, "%d/%d", MusicPlayNum + 1, LISTVIEW_GetNumRows(hItem));
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
			TEXT_SetText(hItem,  (const char *)buf);

			/* 6. 播放音频 */
			s_tMusicMsg.ucName = (uint8_t *)s_VideoName;
			if(strstr((char *)s_VideoName,".WAV")||strstr((char *)s_VideoName,".wav"))
			{
				/* WAV音频 */
				s_tMusicMsg.ucType = MusicType_WAV;				
			}
			else
			{
				/* MP3音频 */
				s_tMusicMsg.ucType = MusicType_MP3;	
			}
			/* 
			   给音频任务发送音频路径，因为emWin多任务的执行，
			   此时音频任务被挂起，回调消息执行完才会回到音频任务 。
			*/
			if(os_mbx_send (&mailbox, &s_tMusicMsg, 0xFFFF) == OS_R_OK)
			{
				/* 获取音频相关信息进行显示 */
				if(s_tMusicMsg.ucType == 1)
				{
					GetWavInfo(s_VideoName);						
				}
				else
				{
					GetMP3Info(s_VideoName);
				}
				g_tWav.uiCurTime = 0;
				hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0);
				SLIDER_SetRange(hItem, 0, g_tWav.uiTotalTime);

				hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
				TEXT_SetText(hItem, (const char *)g_tWav.usInfo);
			}
			
			/* 注意时间单位被放大了10倍 */
			sprintf(buf, "%02d:%02d:%02d/%02d:%02d:%02d", 
						  0, 
						  0, 
						  0, 
						  g_tWav.uiTotalTime/36000, 
						  g_tWav.uiTotalTime%36000/600, 
						  g_tWav.uiTotalTime/10%60);
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
			TEXT_SetFont(hItem, &GUI_FontLubalGraph20B);
			TEXT_SetText(hItem, buf);
		
			hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0); 
			SLIDER_SetValue(hItem, 0);			
			
			hTimerNext = WM_CreateTimer(pMsg->hWin, ID_TIMER_NEXT, 500, 0);	
			break;

		/*  恢复执行音频播放 */
		case MSG_MusicCancel:
			if(WM_IsWindow(hWinVideoList))
			{
				GUI_MOVIE_Play(hMovie);
				os_evt_set (MusicTaskAudioResume_3, HandleMusicStart);
			}
			break;
		
		/*  开始执行音频播放 */
		case MSG_MusicStart:
			hItem = WM_GetDialogItem(hWinVideoList, GUI_ID_LISTVIEW0);
			MusicPlayNum = LISTVIEW_GetSel(hItem);
			LISTVIEW_GetItemText(hItem, 0, MusicPlayNum, buf, VideoPathSzie);
			_cbMusicButton(pMsg, MusicPlayNum, buf);					
			break;
		
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch(Id) 
			{
				/* 关闭视频播放对话框，暂时未用到 */
				case ID_BUTTON_0:
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						
						case WM_NOTIFICATION_RELEASED:
							/* 不管开启没有开启播放，都发消息，退出当前音频播放 */
							os_evt_set (MusicTaskAudioReturn_6, HandleMusicStart);
						
							/* 先删除视频 */
							if (hMovie)
							{
								GUI_MOVIE_Delete(hMovie);
							}
							
							if(ffilm != NULL)
							{
								fclose(ffilm);
							}
							
							hWinVideoList = WM_HWIN_NULL;
							hWinVideo = WM_HWIN_NULL;
							hMovie = 0;
							g_ucJpegOpti = 0;
							/* 释放占用的动态内存，关闭此对话框 */
							GUI_ALLOC_Free(hMemVedio);
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
							if(WM_IsWindow(hWinVideoList))
							{
								/* 调整到上一首歌曲 */
								hItem = WM_GetDialogItem(hWinVideoList, GUI_ID_LISTVIEW0);
								if(MusicPlayNum == 0) 
								{
									MusicPlayNum = LISTVIEW_GetNumRows(hItem);								
								}
								MusicPlayNum--;
								LISTVIEW_GetItemText(hItem, 0, MusicPlayNum, buf, VideoPathSzie);
								
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
							if(WM_IsWindow(hWinVideoList))
							{
								/* 调整到下一首歌曲 */
								hItem = WM_GetDialogItem(hWinVideoList, GUI_ID_LISTVIEW0);
								MusicPlayNum++;
							    if(MusicPlayNum == LISTVIEW_GetNumRows(hItem)) 
								{
									MusicPlayNum = 0;
								}
								LISTVIEW_GetItemText(hItem, 0, MusicPlayNum, buf, VideoPathSzie);
								
								/* 执行相应操作 */
								_cbMusicButton(pMsg, MusicPlayNum, buf);	
							 }
					}
					break;
					
				/* 视频列表 */
				case ID_BUTTON_6:
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						
						case WM_NOTIFICATION_RELEASED:
							if(WM_IsWindow(hWinVideoList))
							{
								GUI_MOVIE_Pause(hMovie);
								os_evt_set (MusicTaskAudioPause_2, HandleMusicStart);								
							}
							App_VideoList(pMsg->hWin);
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
							if(WM_IsWindow(hWinVideoList) && (s_ucPlayStatus == 1))
							{
								uint64_t temp;
								uint32_t comp;
								
								hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0);
								g_tWav.uiCurTime = SLIDER_GetValue(hItem);
								os_evt_set (MusicTaskAudioAdjust_1, HandleMusicStart);
								
								/* 为了方便调试，计数了两次temp变量，以第二次计算为准 */
								comp = Info.NumFrames * 10 / (1000 / Info.msPerFrame);
								
								/* 如果视频的播放时间大于音频的播放时间，以音频播放为基准 */
								if(comp > g_tWav.uiTotalTime)
								{	
									temp = g_tWav.uiCurTime * (1000 / Info.msPerFrame) / 10;
									printf_videodbg("视频大于音频Mothod1 = %lld\r\n", temp);
								
									temp = Info.NumFrames  * g_tWav.uiCurTime / g_tWav.uiTotalTime;
									printf_videodbg("实际使用Mothod2 = %lld\r\n", temp);
								}
								/* 如果视频的播放时间小于音频的播放时间，以视频播放为基准 */
								else
								{
									temp = Info.NumFrames  * g_tWav.uiCurTime / g_tWav.uiTotalTime;
									printf_videodbg("音频大于视频Mothod2 = %lld\r\n", temp);
								
									temp = g_tWav.uiCurTime * (1000 / Info.msPerFrame) / 10;
									printf_videodbg("Mothod1 = %lld\r\n", temp);
								}
								GUI_MOVIE_GotoFrame(hMovie, temp);
							}
							break;
					}
					break;
					
				/* 用于实现声音调节 */
				case ID_SLIDER_1:
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						
						case WM_NOTIFICATION_RELEASED:	
							if(WM_IsWindow(hWinVideoList))
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
							if(WM_IsWindow(hWinVideoList))
							{
								if(s_ucPlayStatus == 1)
								{
									/* 暂停 */	
									s_ucPlayStatus = 0;
									g_tWav.ucDispUpdate = 0;
									GUI_MOVIE_Pause(hMovie);
									os_evt_set (MusicTaskAudioPause_2, HandleMusicStart);
								}
								else
								{
									/* 播放 */	
									s_ucPlayStatus = 1;
									g_tWav.ucDispUpdate = 1;
									GUI_MOVIE_Play(hMovie);
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
							if(WM_IsWindow(hWinVideoList))
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
*	函 数 名: App_Video
*	功能说明: 创建视频播放对话框
*	形    参: hWin  父窗口
*	返 回 值: 无
*********************************************************************************************************
*/
void App_Video(WM_HWIN hWin) 
{
	s_ucPlayStatus = 0; 	      	 /* 播放器的开始和暂停按钮状态，0表示暂停，1表示运行 */
	g_tWav.ucDispUpdate = 0;
	
	/* 申请一块内存空间 并且将其清零  这里申请800*480*4的内存 */
	hMemVedio = GUI_ALLOC_AllocZero(800*480*4);
	
	/* 将申请到内存的句柄转换成指针类型 */
	g_pVevio = GUI_ALLOC_h2p(hMemVedio);
	
	/* 使能JPEG解码优化 */
	g_ucJpegOpti = 1;
	
	hWinVideo = GUI_CreateDialogBox(_aDialogCreateVideo, 
	                                GUI_COUNTOF(_aDialogCreateVideo), 
	                                _cbDialogVideo, 
	                                hWin, 
	                                0, 
	                                0);
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

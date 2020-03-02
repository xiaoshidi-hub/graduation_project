/*
*********************************************************************************************************
*	                                  
*	ģ������ : ��Ƶ������Ӧ�ý������
*	�ļ����� : App_VideoDlg.c
*	��    �� : V1.0
*	˵    �� : ��Ƶ������������ơ�
*              1. ֧����һ������һ��������Ϳ��ˡ�
*              2. emWin�����ǵ����ȼ�������Ƶ���������Ǹ����ȼ�������������֮��ͨ����Ϣ������¼�
*                 ��־����ͨ�š�
*              3. �״�ʹ���ȵ����Ƶ�б���Ƶ���ᱻ��¼��listview�ؼ����棬Ȼ��Ϳ�����������ˡ�
*                 ����ļ�������Ƶ�϶࣬�״δ򿪻�����Щ����Ҫ����ΪҪ��ȡÿ����Ƶ�Ĳ���ʱ�䡣�Ժ��
*                 �ͱȽϿ��ˣ���Ҫ�Ƕ���Ƶ�б�Ի����������غ���ʾ�����������ظ��Ĵ�����ɾ����
*              4. ��Ƶ�б�Ի�������ģ̬���������û��򿪴˶Ի����ֻ�ܲ�������Ի��򣬶����ܲ��������档
*
*	�޸ļ�¼ :
*		�汾��    ����         ����         ˵��
*       V1.0    2019-12      ����Сʦ��     �׷�
*
*	Copyright (C), 2020-2025, https://space.bilibili.com/357524621 
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
	#define printf_videodbg printf
#else
	#define printf_videodbg(...)
#endif

/*
*********************************************************************************************************
*				                      �궨��
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

/* ��ͬ��ʱ���ľ�� */
#define ID_TIMER_NEXT       0
#define ID_TIMER_PROCESS    1

/* ��Ƶ·���ַ����� */
#define VideoPathSzie       100 


/*
*********************************************************************************************************
*				                    �����ⲿ�����ͺ���
*********************************************************************************************************
*/
/* ����һ��֧��5����Ϣ����Ϣ���� */
extern os_mbx_declare (mailbox, 5);
extern MusicMsg_T s_tMusicMsg;  /* ���ڸ����ֲ���������Ϣ�������������ͺ͸���·�� */


/*
*********************************************************************************************************
*				                         ����
*********************************************************************************************************
*/
const char s_VideoPathDir[] = {"M0:\\Video\\"};  /* �洢���и�����ŵ�·�� */
static uint8_t s_VideoName[VideoPathSzie] = {0}; /* ���ڼ�¼��ǰ���ŵĸ��� */
static uint8_t s_ucPlayStatus = 0; 	      		 /* �������Ŀ�ʼ����ͣ��ť״̬��0��ʾ��ͣ��1��ʾ���� */           		 
WM_HWIN  hWinVideo = WM_HWIN_NULL;               /* ��Ƶ���ŶԻ����� */
U8 *g_pVevio;                                    /* ��Ƶ���Ż��� */
uint8_t g_ucJpegOpti = 0;                        /* �Ƿ��Ż�JPEG��0��ʾ���Ż���1��ʾ�Ż� */

/* ������������������Ƶ�ļ��Ķ�ȡ�ʹ��� */
static FILE *ffilm;
static GUI_MOVIE_INFO   Info;
static GUI_MOVIE_HANDLE hMovie = 0;

GUI_HMEM hMemVedio;  /* ��Ƶ������Ҫ�õ��Ķ�̬�ڴ� */


/*
*********************************************************************************************************
*	�� �� ��: _cbNotify
*	����˵��: ��Ƶ���Żص�����
*	��    ��: hMem          �洢�豸���
*             Notification  ��Ϣ����
*             CurrentFrame  ��ǰ֡ 
*	�� �� ֵ: ��
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
*	�� �� ��: _GetData
*	����˵��: ������GUI_MOVIE_GetInfoEx����
*	��    �Σ�p             FIL��������
*             NumBytesReq   �����ȡ���ֽ���
*             ppData        ����ָ��
*             Off           ���Off = 1����ô�����´���ʼλ�ö�ȡ                 
*	�� �� ֵ: ���ض�ȡ���ֽ���
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
	* ���ö�ȡλ��
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
	* ��ȡ���ݵ�����
	*/
	NumBytesRead = fread (pData, 1, NumBytes, file);

	/*
	* ���ض�ȡ���ֽ���
	*/
	return NumBytesRead;
}

/*
*********************************************************************************************************
*				                         ����Ի����ʼ��ѡ��
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateVideo[] = 
{
	{ WINDOW_CreateIndirect, "Window",  ID_WINDOW_0,   0,   0,  800, 480, 0, 0x0, 0 },
	{ SLIDER_CreateIndirect, "Process", ID_SLIDER_0, 100, 365,  600,  16, 0, 0x0, 0 },

	/* ��ť���� */
	{ BUTTON_CreateIndirect, "DlgBack",    ID_BUTTON_0,  700,   0,  100, 100, 0, 0, 0 },	
	{ BUTTON_CreateIndirect, "MusicPre",   ID_BUTTON_1,  100, 399,   72,  72, 0, 0, 0 },
	{ BUTTON_CreateIndirect, "MusicPlay",  ID_BUTTON_2,  200, 399,   72,  72, 0, 0, 0 },
	{ BUTTON_CreateIndirect, "MusicNext",  ID_BUTTON_4,  300, 399,   72,  72, 0, 0, 0 },
	{ BUTTON_CreateIndirect, "MusicSpk",   ID_BUTTON_5,  400, 399,   72,  72, 0, 0, 0 },
	{ BUTTON_CreateIndirect, "MusicList",  ID_BUTTON_6,    0,   0,  100, 100, 0, 0, 0 },
	
	/* ������С */
	{ SLIDER_CreateIndirect, "Speaker",    ID_SLIDER_1, 490, 425, 200, 20, 0, 0x0, 0  },
	
	/* ��ʾ������Ϣ */
	{ TEXT_CreateIndirect, "--/--",              ID_TEXT_0, 100, 345, 270, 20, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "--/--",              ID_TEXT_1, 480, 345, 100, 20, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "00:00:00/00:00:00",  ID_TEXT_2, 560, 345, 170, 20, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "-----------",        ID_TEXT_3, 250, 20,  300, 24, 0, 0x64, 0 },
	
	{ TEXT_CreateIndirect, "--/--",              ID_TEXT_4, 320, 345, 120, 20, 0, 0x64, 0 },	
};

/*
*********************************************************************************************************
*	�� �� ��: _cbButtonBack
*	����˵��: ��ť�ص�����
*	��    ��: pMsg  ��Ϣָ��
*	�� �� ֵ: ��
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
*	�� �� ��: _cbButtonList
*	����˵��: ��ť�ص�����
*	��    ��: pMsg  ��Ϣָ��
*	�� �� ֵ: ��
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
*	�� �� ��: _cbButtonPre
*	����˵��: ��ť�ص�����
*	��    ��: pMsg  ��Ϣָ��
*	�� �� ֵ: ��
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
*	�� �� ��: _cbButtonPlay
*	����˵��: ��ť�ص�����
*	��    ��: pMsg  ��Ϣָ��
*	�� �� ֵ: ��
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
*	�� �� ��: _cbButtonNext
*	����˵��: ��ť�ص�����
*	��    ��: pMsg  ��Ϣָ��
*	�� �� ֵ: ��
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
*	�� �� ��: _cbButtonSpeaker
*	����˵��: ��ť�ص�����
*	��    ��: pMsg  ��Ϣָ��
*	�� �� ֵ: ��
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
*	�� �� ��: _cbMusicButton
*	����˵��: ��һ������һ����ť�Ļص���Ϣ������Ϊ�������ݻ���һ�������ж�����һ��������
*	��    ��: pMsg    ��Ϣָ��
*             _uiNum  Ҫ���ŵ���Ƶ����Ƶ���
*            _pName   ��Ƶ������Ƶ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _cbMusicButton(WM_MESSAGE *pMsg, int _uiNum, char *_pName) 
{
	const   GUI_RECT  Rect = {160, 54, 640, 326};
	WM_HWIN hItem;
	char buf[20];
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
	TEXT_SetText(hItem, _pName);
	
	/* ����Ƶ������·�����Ƶ�����s_VideoName���� */
	memset(s_VideoName, 0, VideoPathSzie);
	
	strcpy((char *)s_VideoName, s_VideoPathDir);
	strncat((char *)s_VideoName, _pName, strlen(_pName)-3);
	strcat((char *)s_VideoName, "mp3");
	printf_videodbg("%s\r\n", s_VideoName);
	
	/* ��ʾ��ǰ��Ƶ������ź��ܵĸ������� */
	hItem = WM_GetDialogItem(hWinVideoList, GUI_ID_LISTVIEW0);
	sprintf(buf, "%d/%d", _uiNum + 1, LISTVIEW_GetNumRows(hItem));
	hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
	TEXT_SetText(hItem, buf);

	/* ����Ϣ���˳���ǰ��Ƶ���� */	
	os_evt_set (MusicTaskAudioReturn_6, HandleMusicStart);

	s_tMusicMsg.ucName = (uint8_t *)s_VideoName;
	if(strstr((char *)s_VideoName,".WAV")||strstr((char *)s_VideoName,".wav"))
	{
		/* WAV��Ƶ */
		s_tMusicMsg.ucType = MusicType_WAV;				
	}
	else
	{
		/* MP3��Ƶ */
		s_tMusicMsg.ucType = MusicType_MP3;	
	}

	/* ����Ϣ��������Ƶ */
	if(os_mbx_send (&mailbox, &s_tMusicMsg, 0xFFFF) == OS_R_OK)
	{
		/* ���ݰ�ť���»��߲��ŵ�״̬������Ϣ */
		if(s_ucPlayStatus == 1)
		{
			os_evt_set (MusicTaskAudioPlay_5, HandleMusicStart);					
		}
		else
		{
			os_evt_set (MusicTaskAudioStart_8, HandleMusicStart);				
		}
		/* ���²���ʱ�����Ƶ��Ϣ */
		hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0);
		SLIDER_SetRange(hItem, 0, g_tWav.uiTotalTime);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
		TEXT_SetText(hItem, (const char *)g_tWav.usInfo);
	}
	
	memset(s_VideoName, 0, VideoPathSzie);
	
	strcpy((char *)s_VideoName, s_VideoPathDir);
	strncat((char *)s_VideoName, _pName, strlen(_pName)-3);
	strcat((char *)s_VideoName, "emf");
	
	/* �״δ� */
	if(hMovie == 0)
	{
		/* ���ļ� */	
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
	/* ��Ƶ�л� */
	else
	{
		WM_InvalidateRect(pMsg->hWin, &Rect);
		
		GUI_MOVIE_Delete(hMovie);
		if(ffilm != NULL)
		{
			fclose(ffilm);
		}
		/* ���ļ� */	
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
	
	/* ���²���ʱ�䣬ע��ʱ�䵥λ���Ŵ���10�� */
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
*	�� �� ��: _cbDialogVideo
*	����˵��: ��Ƶ���ŶԻ���ص���Ϣ
*	��    ��: pMsg  ��Ϣָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _cbDialogVideo(WM_MESSAGE * pMsg) 
{
	const   GUI_RECT  Rect = {160, 54, 640, 326}; /* ���Ͻ�x��y�����½�x��y*/
	static  WM_HTIMER hTimerProcess, hTimerNext;
	static  int  MusicPlayNum = 0;  
	WM_HWIN hItem;
	int     NCode;
	int     Id;
	char    buf[VideoPathSzie + 20];
	
	
	switch (pMsg->MsgId) 
	{
		/* �Ի����ʼ����Ϣ */
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
		
		/* ��ʱ���ص���Ϣ */
		case WM_TIMER:
			Id = WM_GetTimerId(pMsg->Data.v);
			switch (Id) 
			{
				/* ���Ž��ȺͲ���ʱ����� */
				case ID_TIMER_PROCESS:
					if(g_tWav.ucDispUpdate == 1)
					{
						//g_tWav.uiCurTime = GUI_MOVIE_GetFrameIndex(hMovie) / (1000 / Info.msPerFrame);	
						//g_tWav.uiTotalTime = Info.NumFrames / (1000 / Info.msPerFrame);
						os_evt_set (MusicTaskAudioGetTime_7, HandleMusicStart);
						/* ע��ʱ�䵥λ���Ŵ���10�� */
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
					
					/* ��������Ϣ��������Ƶ���� */
					os_evt_set (MusicTaskAudioStart_8, HandleMusicStart);
					
					memset(s_VideoName, 0, VideoPathSzie);
			
					hItem = WM_GetDialogItem(hWinVideoList, GUI_ID_LISTVIEW0);
					LISTVIEW_GetItemText(hItem, 0, MusicPlayNum, buf, VideoPathSzie);
					strcpy((char *)s_VideoName, s_VideoPathDir);
					strncat((char *)s_VideoName, buf, strlen(buf)-3);
					strcat((char *)s_VideoName, "emf");
					
					/* ���ļ� */	
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
		
		/* �л�����һ����Ƶ����Ƶ���������Ǹ����ȼ�����emWin�����ǵ����ȼ�����
		   ����Ƶ��������ͨ������WM_SendMessageNoPara������Ϣ��emWin����ʱ��������
		   ִ�д���Ϣ��������ΪemWin�������ʵ�ֻ������⣬��GUI_X_RTX.C�ļ����ɡ�
		*/
		case MSG_NextMusic:
			WM_InvalidateRect(pMsg->hWin, &Rect);
			GUI_MOVIE_Delete(hMovie);
			/*1. ��ȡ��һ��Ҫ������Ƶ����� */
			hItem = WM_GetDialogItem(hWinVideoList, GUI_ID_LISTVIEW0);
			MusicPlayNum++;
			if(MusicPlayNum == LISTVIEW_GetNumRows(hItem)) 
			{
				MusicPlayNum = 0;
			}
			
			/* 2. ��ȡ��Ƶ�� */
			LISTVIEW_GetItemText(hItem, 0, MusicPlayNum, buf, VideoPathSzie);
			
			/* 3. ��ʾ��Ƶ���������� */
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
			TEXT_SetText(hItem,  (const char *)buf);

			/* 4. ����Ƶ������·�����Ƶ�����s_VideoName���� */
			memset(s_VideoName, 0, VideoPathSzie);
			
			strcpy((char *)s_VideoName, s_VideoPathDir);
			strncat((char *)s_VideoName, buf, strlen(buf)-3);
			strcat((char *)s_VideoName, "mp3");
			printf_videodbg("%s\r\n", s_VideoName);
			
			/* 5. ��ʾ��ǰ��Ƶ������ź��ܵ���Ƶ���� */
			hItem = WM_GetDialogItem(hWinVideoList, GUI_ID_LISTVIEW0);
			sprintf(buf, "%d/%d", MusicPlayNum + 1, LISTVIEW_GetNumRows(hItem));
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
			TEXT_SetText(hItem,  (const char *)buf);

			/* 6. ������Ƶ */
			s_tMusicMsg.ucName = (uint8_t *)s_VideoName;
			if(strstr((char *)s_VideoName,".WAV")||strstr((char *)s_VideoName,".wav"))
			{
				/* WAV��Ƶ */
				s_tMusicMsg.ucType = MusicType_WAV;				
			}
			else
			{
				/* MP3��Ƶ */
				s_tMusicMsg.ucType = MusicType_MP3;	
			}
			/* 
			   ����Ƶ��������Ƶ·������ΪemWin�������ִ�У�
			   ��ʱ��Ƶ���񱻹��𣬻ص���Ϣִ����Ż�ص���Ƶ���� ��
			*/
			if(os_mbx_send (&mailbox, &s_tMusicMsg, 0xFFFF) == OS_R_OK)
			{
				/* ��ȡ��Ƶ�����Ϣ������ʾ */
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
			
			/* ע��ʱ�䵥λ���Ŵ���10�� */
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

		/*  �ָ�ִ����Ƶ���� */
		case MSG_MusicCancel:
			if(WM_IsWindow(hWinVideoList))
			{
				GUI_MOVIE_Play(hMovie);
				os_evt_set (MusicTaskAudioResume_3, HandleMusicStart);
			}
			break;
		
		/*  ��ʼִ����Ƶ���� */
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
				/* �ر���Ƶ���ŶԻ�����ʱδ�õ� */
				case ID_BUTTON_0:
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						
						case WM_NOTIFICATION_RELEASED:
							/* ���ܿ���û�п������ţ�������Ϣ���˳���ǰ��Ƶ���� */
							os_evt_set (MusicTaskAudioReturn_6, HandleMusicStart);
						
							/* ��ɾ����Ƶ */
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
							/* �ͷ�ռ�õĶ�̬�ڴ棬�رմ˶Ի��� */
							GUI_ALLOC_Free(hMemVedio);
							GUI_EndDialog(pMsg->hWin, 0);
							break;
					}
					break;
					
				/* ��һ�� */
				case ID_BUTTON_1:
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						
						case WM_NOTIFICATION_RELEASED:
							if(WM_IsWindow(hWinVideoList))
							{
								/* ��������һ�׸��� */
								hItem = WM_GetDialogItem(hWinVideoList, GUI_ID_LISTVIEW0);
								if(MusicPlayNum == 0) 
								{
									MusicPlayNum = LISTVIEW_GetNumRows(hItem);								
								}
								MusicPlayNum--;
								LISTVIEW_GetItemText(hItem, 0, MusicPlayNum, buf, VideoPathSzie);
								
								/* ִ����Ӧ���� */
								_cbMusicButton(pMsg, MusicPlayNum, buf);	
							 }
							break;
					}
					break;
					
				/* ��һ�� */
				case ID_BUTTON_4:
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						
						case WM_NOTIFICATION_RELEASED:
							if(WM_IsWindow(hWinVideoList))
							{
								/* ��������һ�׸��� */
								hItem = WM_GetDialogItem(hWinVideoList, GUI_ID_LISTVIEW0);
								MusicPlayNum++;
							    if(MusicPlayNum == LISTVIEW_GetNumRows(hItem)) 
								{
									MusicPlayNum = 0;
								}
								LISTVIEW_GetItemText(hItem, 0, MusicPlayNum, buf, VideoPathSzie);
								
								/* ִ����Ӧ���� */
								_cbMusicButton(pMsg, MusicPlayNum, buf);	
							 }
					}
					break;
					
				/* ��Ƶ�б� */
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
					
				/* ����ʵ�ֿ������ */
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
								
								/* Ϊ�˷�����ԣ�����������temp�������Եڶ��μ���Ϊ׼ */
								comp = Info.NumFrames * 10 / (1000 / Info.msPerFrame);
								
								/* �����Ƶ�Ĳ���ʱ�������Ƶ�Ĳ���ʱ�䣬����Ƶ����Ϊ��׼ */
								if(comp > g_tWav.uiTotalTime)
								{	
									temp = g_tWav.uiCurTime * (1000 / Info.msPerFrame) / 10;
									printf_videodbg("��Ƶ������ƵMothod1 = %lld\r\n", temp);
								
									temp = Info.NumFrames  * g_tWav.uiCurTime / g_tWav.uiTotalTime;
									printf_videodbg("ʵ��ʹ��Mothod2 = %lld\r\n", temp);
								}
								/* �����Ƶ�Ĳ���ʱ��С����Ƶ�Ĳ���ʱ�䣬����Ƶ����Ϊ��׼ */
								else
								{
									temp = Info.NumFrames  * g_tWav.uiCurTime / g_tWav.uiTotalTime;
									printf_videodbg("��Ƶ������ƵMothod2 = %lld\r\n", temp);
								
									temp = g_tWav.uiCurTime * (1000 / Info.msPerFrame) / 10;
									printf_videodbg("Mothod1 = %lld\r\n", temp);
								}
								GUI_MOVIE_GotoFrame(hMovie, temp);
							}
							break;
					}
					break;
					
				/* ����ʵ���������� */
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
								/* ����������������ͬ���� */
								wm8978_SetEarVolume(g_tWav.ucVolume);
								wm8978_SetSpkVolume(g_tWav.ucVolume);
							}
							break;
					}
					break;

				/* ��ͣ�Ϳ�ʼ��ť */
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
									/* ��ͣ */	
									s_ucPlayStatus = 0;
									g_tWav.ucDispUpdate = 0;
									GUI_MOVIE_Pause(hMovie);
									os_evt_set (MusicTaskAudioPause_2, HandleMusicStart);
								}
								else
								{
									/* ���� */	
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
					
				/* �������� */
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
									/* ���� */	
									g_tWav.ucSpeakerStatus = 0;
									wm8978_OutMute(1);
								}
								else
								{
									/* ������ */	
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
*	�� �� ��: App_Video
*	����˵��: ������Ƶ���ŶԻ���
*	��    ��: hWin  ������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void App_Video(WM_HWIN hWin) 
{
	s_ucPlayStatus = 0; 	      	 /* �������Ŀ�ʼ����ͣ��ť״̬��0��ʾ��ͣ��1��ʾ���� */
	g_tWav.ucDispUpdate = 0;
	
	/* ����һ���ڴ�ռ� ���ҽ�������  ��������800*480*4���ڴ� */
	hMemVedio = GUI_ALLOC_AllocZero(800*480*4);
	
	/* �����뵽�ڴ�ľ��ת����ָ������ */
	g_pVevio = GUI_ALLOC_h2p(hMemVedio);
	
	/* ʹ��JPEG�����Ż� */
	g_ucJpegOpti = 1;
	
	hWinVideo = GUI_CreateDialogBox(_aDialogCreateVideo, 
	                                GUI_COUNTOF(_aDialogCreateVideo), 
	                                _cbDialogVideo, 
	                                hWin, 
	                                0, 
	                                0);
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

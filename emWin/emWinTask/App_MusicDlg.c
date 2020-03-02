/*
*********************************************************************************************************
*
*	ģ������ : ���ֲ�����Ӧ�ý������
*	�ļ����� : App_MucisDlg.c
*	��    �� : V1.0
*	˵    �� : ���ֲ�����������ơ�
*              1. ֧����һ������һ��������Ϳ��ˣ����õĲ����ʺ����ʶ�֧�֣���������������Ҳ��֧�֡�
*              2. emWin�����ǵ����ȼ��������ֽ��������Ǹ����ȼ�������������֮��ͨ��������Ϣ���к��¼�
*                 ��־����ͨ�š�
*              3. �״�ʹ���ȵ�������б��������ᱻ��¼��listview�ؼ����棬Ȼ��Ϳ�����������ˡ�
*                 ����ļ����и����϶࣬�״δ򿪻�����Щ����Ҫ����ΪҪ��ȡÿ�׸����Ĳ���ʱ�䡣�Ժ��
*                 �ͱȽϿ��ˣ���Ҫ�ǶԸ����б�Ի����������غ���ʾ�����������ظ��Ĵ�����ɾ����
*              4. �����б�Ի�������ģ̬���������û��򿪴˶Ի����ֻ�ܲ�������Ի��򣬶����ܲ��������档
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
	#define printf_audiodbg printf
#else
	#define printf_audiodbg(...)
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

#define ID_BUTTON_7 		(GUI_ID_USER + 0x0F)

/* ��ͬ��ʱ���ľ�� */
#define ID_TIMER_SPEC       0
#define ID_TIMER_PROCESS    1

/* ����·���ַ����� */
#define MusicPathSzie       100 



/*
*********************************************************************************************************
*				                    �����ⲿ�����ͺ���
*********************************************************************************************************
*/
/* ����һ��֧��5����Ϣ����Ϣ���� */
extern os_mbx_declare (mailbox, 5);
extern MusicMsg_T s_tMusicMsg;  /* ���ڸ����ֲ���������Ϣ�������������ͺ͸���·�� */

//static char  *_acbuffer;
//static	FILE *result;
//static	FILE *file;
/*
*********************************************************************************************************
*				                         ����
*********************************************************************************************************
*/
const char s_MusicPathDir[] = {"M0:\\Music\\"};  /* �洢���и�����ŵ�·�� */
static uint8_t s_MusicName[MusicPathSzie] = {0}; /* ���ڼ�¼��ǰ���ŵĸ��� */
static uint8_t s_ucPlayStatus = 0; 	      		 /* �������Ŀ�ʼ����ͣ��ť״̬��0��ʾ��ͣ��1��ʾ���� */
WM_HWIN  hWinMusic = WM_HWIN_NULL;               /* ���ֲ��ŶԻ����� */

//static void ShowJPEG(const char *Filename)
//{
//	WM_HMEM hMem;
//    U8     * pData;
//	
//	
//	/* ����һ�鶯̬�ڴ�ռ� */
//	hMem = GUI_ALLOC_AllocZero(1024*512);
//	/* ת����̬�ڴ�ľ��Ϊָ�� */
//	_acbuffer = GUI_ALLOC_h2p(hMem);
//	
//	/* ���ļ� */
//	result =  fopen ((char *)Filename, "r"); 	
//	if ((result != NULL)) 
//	{
//		return;
//	} 
//	result = fread(pData, 1, 512, file);
//	/* ��ȡͼƬ���ݵ���̬�ڴ��� */
//	if (result != NULL) 
//	{
//		return;
//	}	
//	GUI_JPEG_Draw(_acBuffer,sizeof(_acBuffer),0,0);
//	/* �ͷ��ڴ� */
//	GUI_ALLOC_Free(hMem);
//	fclose(file);
// }

/*
*********************************************************************************************************
*	�� �� ��: DisWave
*	����˵��: ��ʾƵ��
*	��    ��: x    X������
*             y    Y������
*             clr  0 ��ʾ�����ʾ���ݣ����ڸմ򿪽���ʱ�������ϴε���ʾ�м�¼
*                  1 ��ʾ�������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void DisWave(uint16_t x, uint16_t y, uint8_t clr)
{
	static uint16_t s_MP3Spec_TopVal[32] = {0};   /* Ƶ�׶�ֵ�� */
	static uint16_t s_MP3Spec_CurVal[32] = {0};	  /* Ƶ�׵�ǰֵ�� */
	static uint8_t  s_MP3Spec_Time[32] = {0};	  /* ��ֵͣ��ʱ��� */
	const  uint16_t usMaxVal = 128;               /* �߶ȹ̶�Ϊ128������ */
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
		
		/* ��պ���˳� */
		return;
	}

	/* ��ʾ32��Ƶ�� */								   
	for(i = 0; i < 32; i++)	
	{	
		if(g_tWav.ucSpectrum == 1)
		{
			/* �����ף��Է�ֵȡ���� */
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
			   1. ��ֵ��
			   2. ��WAV��Ƶ�õ���FFTģֵ����32��MP3ģֵ����64�����������ʾ 
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

		/* 2. ����Ƶ����ֵ */
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

		/* 3. ����Ƶ�׶�ֵ */
		if(s_MP3Spec_Time[i])
		{
			s_MP3Spec_Time[i]--;
		}
		else 
		{	
			/* ��ֵ��С1 */
			if(s_MP3Spec_TopVal[i])
			{
				s_MP3Spec_TopVal[i]--;
			}
		}

		/* 4. ����Ƶ�׶�ֵ */
		if(s_MP3Spec_CurVal[i] > s_MP3Spec_TopVal[i])
		{
			s_MP3Spec_TopVal[i] = s_MP3Spec_CurVal[i];
			
			/* �����ֵͣ��ʱ�� */
			s_MP3Spec_Time[i] = 10;
		}

		/* 5. ��ֹ����Ƶ��ֵ�Ͷ�ֵ��Χ���߶ȹ̶�Ϊ128������ */
		if(s_MP3Spec_CurVal[i] > usMaxVal)
		{
			s_MP3Spec_CurVal[i] = usMaxVal;
		}

		if(s_MP3Spec_TopVal[i] > usMaxVal)
		{
			s_MP3Spec_TopVal[i] = usMaxVal;
		} 	   
	}

	/* 6. ���Ƶõ���Ƶ�� */
	for(i = 0; i < 32; i++)
	{
		/* ��ʾƵ�� */
		GUI_DrawGradientV(x, 
		                  y + usMaxVal - s_MP3Spec_CurVal[i], 
		                  x + 10, 
		                  y + usMaxVal, 
		                  GUI_YELLOW, 
		                  GUI_GREEN);

		/* ��ʾ��ֵ */
		GUI_SetColor(GUI_RED);
		GUI_DrawHLine(y + usMaxVal - s_MP3Spec_TopVal[i] - 1, x, x + 10);
		x += 15;
	}
}

/*
*********************************************************************************************************
*				                         ����Ի����ʼ��ѡ��
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateMusic[] = 
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
	
	{ BUTTON_CreateIndirect, "MusicSet",   ID_BUTTON_7,   0,   380,  100, 100, 0, 0, 0 },
	
	/* ������С */
	{ SLIDER_CreateIndirect, "Speaker",    ID_SLIDER_1, 490, 425, 200, 20, 0, 0x0, 0  },
	
	/* ��ʾ������Ϣ */
	{ TEXT_CreateIndirect, "--/--",         ID_TEXT_0, 100, 345, 270, 20, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "--/--",         ID_TEXT_1, 390, 345, 120, 20, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "00:00/00:00",   ID_TEXT_2, 610, 345, 120, 20, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "-----------",   ID_TEXT_3, 110, 20,  580, 24, 0, 0x64, 0 },
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
*	�� �� ��: _cbButtonSet
*	����˵��: ��ť�ص�����
*	��    ��: pMsg  ��Ϣָ��
*	�� �� ֵ: ��
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
*	�� �� ��: _cbMusicButton
*	����˵��: ��һ������һ����ť�Ļص���Ϣ������Ϊ�������ݻ���һ�������ж�����һ��������
*	��    ��: pMsg    ��Ϣָ��
*             _uiNum  Ҫ���ŵĸ������
*            _pName   ������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _cbMusicButton(WM_MESSAGE *pMsg, int _uiNum, char *_pName) 
{
	WM_HWIN hItem;
	char buf[50];
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
	TEXT_SetText(hItem, _pName);
	
	/* ������������·�����Ƶ�����s_MusicName���� */
	memset(s_MusicName, 0, MusicPathSzie);
	
	strcpy((char *)s_MusicName, s_MusicPathDir);
	strcat((char *)s_MusicName, _pName);
	printf_audiodbg("%s\r\n", s_MusicName);
	
	/* ��ʾ��ǰ����������ź��ܵĸ������� */
	hItem = WM_GetDialogItem(hWinMusicList, GUI_ID_LISTVIEW0);
	sprintf(buf, "%d/%d", _uiNum + 1, LISTVIEW_GetNumRows(hItem));
	hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
	TEXT_SetText(hItem, buf);

	/* ����Ϣ���˳���ǰ���ֲ��� */	
	os_evt_set (MusicTaskAudioReturn_6, HandleMusicStart);

	s_tMusicMsg.ucName = (uint8_t *)s_MusicName;
	if(strstr((char *)s_MusicName,".WAV")||strstr((char *)s_MusicName,".wav"))
	{
		/* WAV���� */
		s_tMusicMsg.ucType = MusicType_WAV;				
	}
	else
	{
		/* MP3���� */
		s_tMusicMsg.ucType = MusicType_MP3;	
	}

	/* ����Ϣ�������¸� */
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
		
		/* ���²���ʱ��͸�����Ϣ */
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
*	�� �� ��: _cbDialogMusic
*	����˵��: ���ֲ��ŶԻ���ص���Ϣ
*	��    ��: pMsg  ��Ϣָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _cbDialogMusic(WM_MESSAGE * pMsg) 
{
	const   GUI_RECT  Rect = {162, 200-29, 638, 300}; /* ���Ͻ�x��y�����½�x��y*/
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
		/* �Ի����ʼ����Ϣ */
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
			
			

		
		/* ��ʱ���ص���Ϣ */
		case WM_TIMER:
			Id = WM_GetTimerId(pMsg->Data.v);
			switch (Id) 
			{
				/* ʱ�䵽�˸���Ƶ�� */
				case ID_TIMER_SPEC:
					if(g_tWav.ucDispUpdate == 1)
					{
						WM_InvalidateRect(pMsg->hWin, &Rect);
					}
					WM_RestartTimer(pMsg->Data.v, 20);				
					break;
				
				/* ���Ž��ȺͲ���ʱ����� */
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
			
		/* �ػ���Ϣ����ʹ���˶໺�� */
		case WM_PRE_PAINT:
			GUI_MULTIBUF_Begin();
		    break;
		
		case WM_PAINT:
			
			GUI_SetBkColor(GUI_LIGHTCYAN);
			GUI_Clear();   //�޸������汳����ɫ������������������޸Ĳ��ɹ�
		
			DisWave(162, 200-28, 1);
			break;
		
		case WM_POST_PAINT:
			GUI_MULTIBUF_End();
		    break;
		
		/* �л�����һ�׸��������ֲ��������Ǹ����ȼ�����emWin�����ǵ����ȼ�����
		   �����ֲ�������ͨ������WM_SendMessageNoPara������Ϣ��emWin����ʱ��������
		   ִ�д���Ϣ��������ΪemWin�������ʵ�ֻ������⣬��GUI_X_RTX.C�ļ����ɡ�
		*/
		case MSG_NextMusic:
			/*1. ��ȡ��һ��Ҫ���Ÿ�������� */
			hItem = WM_GetDialogItem(hWinMusicList, GUI_ID_LISTVIEW0);
			MusicPlayNum++;
			if(MusicPlayNum == LISTVIEW_GetNumRows(hItem)) 
			{
				MusicPlayNum = 0;
			}
			
			/* 2. ��ȡ������ */
			LISTVIEW_GetItemText(hItem, 0, MusicPlayNum, buf, MusicPathSzie);
			
			/* 3. ��ʾ�������������� */
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
			TEXT_SetText(hItem,  (const char *)buf);

			/* 4. ������������·�����Ƶ�����s_MusicName���� */
			memset(s_MusicName, 0, MusicPathSzie);
			strcpy((char *)s_MusicName, s_MusicPathDir);
			strcat((char *)s_MusicName, buf);
			
			/* 5. ��ʾ��ǰ����������ź��ܵĸ������� */
			hItem = WM_GetDialogItem(hWinMusicList, GUI_ID_LISTVIEW0);
			sprintf(buf, "%d/%d", MusicPlayNum + 1, LISTVIEW_GetNumRows(hItem));
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
			TEXT_SetText(hItem,  (const char *)buf);

			/* 6. �����¸� */
			s_tMusicMsg.ucName = (uint8_t *)s_MusicName;
			if(strstr((char *)s_MusicName,".WAV")||strstr((char *)s_MusicName,".wav"))
			{
				/* WAV��ʽ */
				s_tMusicMsg.ucType = MusicType_WAV;				
			}
			else
			{
				/* MP3��ʽ */
				s_tMusicMsg.ucType = MusicType_MP3;	
			}
			/* 
			   �����������͸���·������ΪemWin�������ִ�У�
			   ��ʱ�������񱻹��𣬻ص���Ϣִ����Ż�ص��������� ��
			*/
			if(os_mbx_send (&mailbox, &s_tMusicMsg, 0xFFFF) == OS_R_OK)
			{
				/* ��ȡ���������Ϣ������ʾ */
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
				
				/* ��������Ϣ�������������� */
				os_evt_set (MusicTaskAudioPlay_5, HandleMusicStart);
			}
			break;
		
		/*  ��ʼִ�����ֲ��� */
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
				/* �ر����ֲ��ŶԻ��� */
				case ID_BUTTON_0:
					switch(NCode) 
					{
						case WM_NOTIFICATION_RELEASED:
							/* ���ܿ���û�п������ţ�������Ϣ���˳���ǰ��Ƶ���� */
							os_evt_set (MusicTaskAudioReturn_6, HandleMusicStart);
							hWinMusicList = WM_HWIN_NULL;
							hWinMusic = WM_HWIN_NULL;
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
							if(WM_IsWindow(hWinMusicList))
							{
								/* ��������һ�׸��� */
								hItem = WM_GetDialogItem(hWinMusicList, GUI_ID_LISTVIEW0);
								if(MusicPlayNum == 0) 
								{
									MusicPlayNum = LISTVIEW_GetNumRows(hItem);								
								}
								MusicPlayNum--;
								LISTVIEW_GetItemText(hItem, 0, MusicPlayNum, buf, MusicPathSzie);
								
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
							if(WM_IsWindow(hWinMusicList))
							{
								/* ��������һ�׸��� */
								hItem = WM_GetDialogItem(hWinMusicList, GUI_ID_LISTVIEW0);
								MusicPlayNum++;
							    if(MusicPlayNum == LISTVIEW_GetNumRows(hItem)) 
								{
									MusicPlayNum = 0;
								}
								LISTVIEW_GetItemText(hItem, 0, MusicPlayNum, buf, MusicPathSzie);
								
								/* ִ����Ӧ���� */
								_cbMusicButton(pMsg, MusicPlayNum, buf);	
							 }
					}
					break;
					
				/* �����б� */
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
					
				/* Ƶ��ģʽ���� */
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
					
				/* ����ʵ�ֿ������ */
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
					
				/* ����ʵ�ֿ������ */
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
							if(WM_IsWindow(hWinMusicList))
							{
								if(s_ucPlayStatus == 1)
								{
									/* ��ͣ */	
									s_ucPlayStatus = 0;
									g_tWav.ucDispUpdate = 0;
									os_evt_set (MusicTaskAudioPause_2, HandleMusicStart);
								}
								else
								{
									/* ���� */	
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
					
				/* �������� */
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
*	�� �� ��: App_Music
*	����˵��: �������ֲ��ŶԻ���
*	��    ��: hWin  ������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void App_Music(WM_HWIN hWin) 
{

	s_ucPlayStatus = 0; 	    /* �������Ŀ�ʼ����ͣ��ť״̬��0��ʾ��ͣ��1��ʾ���� */
	g_tWav.ucDispUpdate = 0;	
	g_tWav.ucSpectrum = 1;		/* 1��ʾ�����ף�0��ʾ��ֵ�� */
	
//	/* ����һ���ڴ�ռ� ���ҽ�������  ��������6MB���ڴ� */
//	hMem = GUI_ALLOC_AllocZero(1024*512);
//	/* �����뵽�ڴ�ľ��ת����ָ������ */
//	pic_buffer = GUI_ALLOC_h2p(hMem);
//    GUI_ALLOC_Free(hMem); 	
	
	/* ���Ƶ����ʾ�������� */
	DisWave(162, 200-28, 0);
	hWinMusic = GUI_CreateDialogBox(_aDialogCreateMusic, 
	                                GUI_COUNTOF(_aDialogCreateMusic),
                                  	_cbDialogMusic, 
	                                hWin, 
	                                0, 
	                                0);	
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

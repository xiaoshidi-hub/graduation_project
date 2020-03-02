/*
*********************************************************************************************************
*	                                  
*	ģ������ : ¼����Ӧ�ý������
*	�ļ����� : App_RecorderDlg.c
*	��    �� : V1.0
*	˵    �� : ¼����������ơ�
*              1. ¼�����Ĺ������ıȽϼ򵥣����ϽǺ����Ͻǵ�������ťδ�õ���
*              2. �����ʹ̶�Ϊ32KHz��16bit����ͨ����
*              3. �����ʼ��ť����ʼ¼����¼��30�룬��ر�֤¼��������ʼ������
*              4. ��ʾ��Ƶ���Ƕ����ף�����FFT�仯���ֵȡ������
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


/*
*********************************************************************************************************
*				                    �����ⲿ�����ͺ���
*********************************************************************************************************
*/
/* ����һ��֧��5����Ϣ����Ϣ���� */
extern os_mbx_declare (mailbox, 5);
extern uint16_t g_uiNum;
extern MusicMsg_T s_tMusicMsg;          /* ���ڸ����ֲ���������Ϣ������ִ��¼������ */


/*
*********************************************************************************************************
*				                         ����
*********************************************************************************************************
*/
static uint8_t s_ucPlayStatus = 0; 	   /* �������Ŀ�ʼ����ͣ��ť״̬��0��ʾ��ͣ��1��ʾ���� */
WM_HWIN  hWinRecorder;                 /* ¼�������ھ�� */
/* ¼�����õ���emWin��̬�ڴ��� */
static GUI_HMEM hMemVedio;                       
uint8_t *g_pRecorder;


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
	static uint16_t s_MP3Spec_CurVal[32] = {0};	  /* Ƶ�׵�ǰֵ�� */
	const  uint16_t usMaxVal = 128;               /* �߶ȹ̶�Ϊ128������ */
	uint16_t i;
	uint16_t temp;
	float ufTempValue;
	
	if(clr == 0)
	{
		for(i = 0; i< 32; i++)
		{
			s_MP3Spec_CurVal[i] = 0;
			g_tWav.uiFFT[i] = 0;
		}
		
		/* ��պ���˳� */
		return;
	}

	/* ��ʾ32��Ƶ�� */								   
	for(i = 0; i < 32; i++)	
	{	
		/* ��ֵ�� */
		//temp = g_tWav.uiFFT[i]>>3;
		
		/* ��ʾ������ */
		ufTempValue = (float)g_tWav.uiFFT[i] / 32;
		if(ufTempValue < 1) 
		{
			ufTempValue = 0;                                                        
		}
		temp = 64 * log10(ufTempValue);	
		
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

		/* 5. ��ֹ����Ƶ��ֵ�Ͷ�ֵ��Χ���߶ȹ̶�Ϊ128������ */
		if(s_MP3Spec_CurVal[i] > usMaxVal)
		{
			s_MP3Spec_CurVal[i] = usMaxVal;
		}	   
	}

	/* 6. ���Ƶõ���Ƶ�� */
	for(i = 0; i < 32; i++)
	{
		/* ��ʾƵ�� */
		if(s_MP3Spec_CurVal[i] == 0)
		{
			GUI_SetColor(0x0000FF);
			GUI_DrawHLine(y + usMaxVal, x, x+10);
		}
		else
		{
			GUI_DrawGradientV(x, 
							  y + usMaxVal - s_MP3Spec_CurVal[i], 
							  x + 10, 
							  y + usMaxVal, 
							  0x0000FF, 
							  0x00FFFF);	
		}

		x += 15;
	}
}

/*
*********************************************************************************************************
*				                         ����Ի����ʼ��ѡ��
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateRecorder[] = 
{
	{ WINDOW_CreateIndirect, "Window",  ID_WINDOW_0,   0,   0,  800, 480, 0, 0x0, 0 },
	{ SLIDER_CreateIndirect, "Process", ID_SLIDER_0, 100, 365,  600,  16, 0, 0x0, 0 },

	/* ��ť���� */
	{ BUTTON_CreateIndirect, "DlgBack",    ID_BUTTON_0,  700,   0,  100, 100, 0, 0, 0 },	
	{ BUTTON_CreateIndirect, "MusicPlay",  ID_BUTTON_1,  200, 399,   72,  72, 0, 0, 0 },
	{ BUTTON_CreateIndirect, "MusicSpk",   ID_BUTTON_5,  300, 399,   72,  72, 0, 0, 0 },
	{ BUTTON_CreateIndirect, "MusicList",  ID_BUTTON_6,    0,   0,  100, 100, 0, 0, 0 },
	
	/* ������С */
	{ SLIDER_CreateIndirect, "Speaker",    ID_SLIDER_1, 400, 425, 200, 20, 0, 0x0, 0  },
	
	/* ��ʾ¼��������Ϣ */
	{ TEXT_CreateIndirect, "32KHz/16bits/Mono",   ID_TEXT_0, 100, 345, 270, 20, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Idle",   			  ID_TEXT_1, 380, 345, 100, 20, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "00:00/00:30",   	  ID_TEXT_2, 610, 345, 120, 20, 0, 0x64, 0 },
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
				
				GUI_DrawBitmap(&bmSetting, 0, 5);
			} 
			else 
			{
				GUI_SetColor(GUI_STCOLOR_LIGHTBLUE);
				GUI_AA_FillCircle(0, 0, 72);
				GUI_SetBkColor(GUI_STCOLOR_LIGHTBLUE);
				GUI_SetColor(GUI_WHITE);  

				GUI_DrawBitmap(&bmSetting, 0, 5);
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
				GUI_DrawBitmap(&bmRecorderIcon, 8, 8);
			}
			else 
			{
				GUI_DrawBitmap(&bmMusicPlay, 8, 8);
			}
			break;
			
		default:
			BUTTON_Callback(pMsg);
			break;
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
*	�� �� ��: Recorder
*	����˵��: ¼�����ŶԻ���ص���Ϣ
*	��    ��: pMsg  ��Ϣָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _cbDialogRecorder(WM_MESSAGE * pMsg) 
{
	const   GUI_RECT  Rect = {162, 200-29, 638, 300}; /* ���Ͻ�x��y�����½�x��y*/
	WM_HWIN hItem;
	int     NCode;
	int     Id;
	char    buf[100];
	

	switch (pMsg->MsgId) 
	{
		/* �Ի����ʼ����Ϣ */
		case WM_INIT_DIALOG:		

			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
			WM_SetHasTrans(hItem);
			WM_SetCallback(hItem, _cbButtonBack);
		
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
			WM_SetHasTrans(hItem);
			WM_SetCallback(hItem, _cbButtonPlay);
		
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_5);
			WM_SetHasTrans(hItem);
			WM_SetCallback(hItem, _cbButtonSpeaker);
			
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_6);
			WM_SetHasTrans(hItem);
			WM_SetCallback(hItem, _cbButtonList);
		
			hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0); 
			SLIDER_SetBkColor(hItem, GUI_WHITE);
			SLIDER_SetFocusColor (hItem, GUI_STCOLOR_LIGHTBLUE);
			SLIDER_SetRange(hItem, 0, 960);
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
			
			s_tMusicMsg.ucType = MusicType_REC;	
			/* ����Ϣ��ִ��¼�������� */
			if(os_mbx_send (&mailbox, &s_tMusicMsg, 0xFFFF) == OS_R_OK)
			{			
				
			}
			break;

		/* �ػ���Ϣ����ʹ���˶໺�� */
		case WM_PRE_PAINT:
			GUI_MULTIBUF_Begin();
		    break;
		
		case WM_PAINT:
			GUI_SetBkColor(GUI_WHITE);
			GUI_Clear();
			GUI_DrawBitmap(&bmMicrophone, (800-72)/2, 30);
			DisWave(162, 200-28, 1);
			break;
		
		case WM_POST_PAINT:
			GUI_MULTIBUF_End();
		    break;
		
		/*  ��ʼִ��¼������� */
		case MSG_MusicStart:
			hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0); 
			SLIDER_SetValue(hItem, g_uiNum);	

			sprintf(buf, "00:%02d/00:30", g_uiNum/32);
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
			TEXT_SetFont(hItem, &GUI_FontLubalGraph20B);
			TEXT_SetText(hItem, buf);
			if(g_uiNum == 960)
			{
				hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
				TEXT_SetText(hItem, "Idle");
			}
			WM_InvalidateRect(pMsg->hWin, &Rect);
			break;
		
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch(Id) 
			{
				/* �ر�¼���Ի��� */
				case ID_BUTTON_0:
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						
						case WM_NOTIFICATION_RELEASED:
							/* ���ܿ���û�п������ţ�������Ϣ���˳���ǰ¼������ */
							os_evt_set (RecTaskAudioReturn_6, HandleMusicStart);
						
							/* �ͷ�ռ�õĶ�̬�ڴ棬�رմ˶Ի��� */
							GUI_ALLOC_Free(hMemVedio);
							GUI_EndDialog(pMsg->hWin, 0);
							break;
					}
					break;

				/* ���ź�¼����ť */
				case ID_BUTTON_1:
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						
						case WM_NOTIFICATION_RELEASED:
							if(s_ucPlayStatus == 1)
							{
								s_ucPlayStatus = 0;
								
								/* ����¼�� */
								hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
								TEXT_SetText(hItem, "Playing");
								os_evt_set (RecTaskAudioPlay_4, HandleMusicStart);				
							}
							else
							{
								s_ucPlayStatus = 1;
								
								/* ��ʼ¼�� */
								hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
								TEXT_SetText(hItem, "Recording");
								os_evt_set (RecTaskAudioRecorde_5, HandleMusicStart);				
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
							break;
					}
					break;
					
				/* �������� */
				case ID_SLIDER_1:
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						
						case WM_NOTIFICATION_RELEASED:	
							hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_1);
							g_tWav.ucVolume  = SLIDER_GetValue(hItem);
							/* ����������������ͬ���� */
							wm8978_SetEarVolume(g_tWav.ucVolume);
							wm8978_SetSpkVolume(g_tWav.ucVolume);
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
*	�� �� ��: App_Recorder78
*	����˵��: ����¼�����Ի���
*	��    ��: hWin  ������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void App_Recorder(WM_HWIN hWin) 
{
	s_ucPlayStatus = 0; 	/* ¼�����Ŀ�ʼ����ͣ��ť״̬��0��ʾ��ͣ��1��ʾ���� */
	
	/* ����һ���ڴ�ռ� ���ҽ�������  ��������2MB���ڴ� */
	hMemVedio = GUI_ALLOC_AllocZero(2*1024*1024);
	
	/* �����뵽�ڴ�ľ��ת����ָ������ */
	g_pRecorder = GUI_ALLOC_h2p(hMemVedio);
	
	/* ���Ƶ����ʾ�������� */
	DisWave(162, 200-28, 0);
	
	hWinRecorder = GUI_CreateDialogBox(_aDialogCreateRecorder, 
	                                    GUI_COUNTOF(_aDialogCreateRecorder), 
	                                    _cbDialogRecorder, 
	                                    hWin, 
	                                    0, 
	                                    0);	
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

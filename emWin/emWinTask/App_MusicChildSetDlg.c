/*
*********************************************************************************************************
*
*	ģ������ : ���ֲ�����Ƶ�����ý���
*	�ļ����� : App_MusicChildSetDlg.c
*	��    �� : V1.0
*	˵    �� : ����Ի��������ֶԻ�����Ӵ��ڣ�����Ƶ��Ч������
*              1. ��Ҫʵ�ַ�ֵ�׺Ͷ����׵��л���ϵͳ�ϵ��Ĭ���Ƿ�ֵ�ס�
*              2. ����������ʽ����Radio�ؼ������ã�����ѡ���У��ĳ����»���ʽѡ�����������ȽϺ��á�
*              3. ����Ƶ�׷����֪ʶ�����������������˽��⣺http://bbs.armfly.com/read.php?tid=25129��
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
#define ID_RADIO_0  (GUI_ID_USER + 0x00)
#define ID_BUTTON_0 (GUI_ID_USER + 0x01)
#define ID_BUTTON_1 (GUI_ID_USER + 0x02)
#define ID_TEXT_0   (GUI_ID_USER + 0x03)

/*
*********************************************************************************************************
*				                    Ƶ�����öԻ����ʼ��ѡ��
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateMusicSet[] =
{
    { FRAMEWIN_CreateIndirect,  "Ƶ������",         0,    120,   0,  559,  329,  0},
	{ RADIO_CreateIndirect,       "Radio", 	ID_RADIO_0,   10, 34, 300, 100, 0, 0x1f02, 0 },
    { BUTTON_CreateIndirect,      "ȷ��",   ID_BUTTON_0,  60, 245,   90,   40,  0, 0, 0},
    { BUTTON_CreateIndirect,       "ȡ��",  ID_BUTTON_1,  380, 245,   90,   40,  0, 0, 0},
	{ TEXT_CreateIndirect,         "Text",  ID_TEXT_0,    10, 150, 520, 80, 0, 0x64, 0 },
};

/*
*********************************************************************************************************
*	�� �� ��: InitDialogMusicSet
*	����˵��: ��ʼ������
*	��    ��: pMsg
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void InitDialogMusicSet(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;

    /* ���ÿ�ܴ��ڵ�Title */
    FRAMEWIN_SetTextColor(pMsg->hWin, 0x0000ff);
    FRAMEWIN_SetFont(pMsg->hWin, &GUI_FontHZ16);
    FRAMEWIN_SetTextAlign(pMsg->hWin, GUI_TA_VCENTER|GUI_TA_CENTER);
    FRAMEWIN_SetTitleHeight(pMsg->hWin, 30);
	
	/* ���õ�ѡ��ť */
    hItem = WM_GetDialogItem(pMsg->hWin, ID_RADIO_0);
    RADIO_SetText(hItem, "Aplitude Spectrum", 0);
    RADIO_SetText(hItem, "Logarithmic Spectrum", 1);
    RADIO_SetFont(hItem, GUI_FONT_16B_1);
	RADIO_SetValue(hItem, g_tWav.ucSpectrum);
	
	/* �����ı��ؼ� */
	hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
    TEXT_SetFont(hItem, &GUI_FontHZ16);
    TEXT_SetText(hItem, "[1]Aplitude Spectrum ��ֵ��\n"
						"[2]Logarithmic Spectrum �����ף���������ķ�ֵȡ������ʹ��ֵ��С\n"
						"   ���źű����ߣ���ʾ���ѣ���Ϊ�˵�������Ӧ��ǿ�ȳɶ�����ϵ��\n"
						"[3]����������Radio�ؼ�Ҫ���»���ʽ��ѡ�񣬷�������ѡ�С�");

    /* ����button�ؼ� */
    BUTTON_SetFont(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0), &GUI_FontHZ16);
    BUTTON_SetFont(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1), &GUI_FontHZ16);
}

/*
*********************************************************************************************************
*	�� �� ��: _cbCallbackMusicSet
*	����˵��: �����б�Ի���ص�����
*	��    �Σ�pMsg
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _cbCallbackMusicSet(WM_MESSAGE * pMsg)
{
    int NCode, Id;
    WM_HWIN hWin = pMsg->hWin;

    switch (pMsg->MsgId)
    {
		/* ��ʼ����Ϣ */
		case WM_INIT_DIALOG:
			InitDialogMusicSet(pMsg);
			break;

		/* ֪ͨ��Ϣ */
		case WM_NOTIFY_PARENT:
			Id = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch (Id)
			{
				/* ���OK��ť����Ƶ����ʾЧ������ */
				case ID_BUTTON_0:
					switch(NCode)
					{
						case WM_NOTIFICATION_CLICKED:
							break;

						case WM_NOTIFICATION_RELEASED:
							/* ����0����ֵ�ף�����1�������� */
							g_tWav.ucSpectrum = RADIO_GetValue( WM_GetDialogItem(pMsg->hWin, ID_RADIO_0));
							GUI_EndDialog(hWin, 0);
							break;
					}
					break;

				/* ���CANCEL��ťȡ���˳� */
				case ID_BUTTON_1:
					switch(NCode)
					{
						case WM_NOTIFICATION_CLICKED:
							break;

						case WM_NOTIFICATION_RELEASED:
							GUI_EndDialog(hWin, 0);
							break;
					}
					break;
			}
			break;

		default:
			WM_DefaultProc(pMsg);
    }
}

/*
*********************************************************************************************************
*	�� �� ��: App_MusicSet
*	����˵��: ���ֲ��ŶԻ�����Ӵ��ڣ�����Ƶ��Ч������
*	��    ��: hWin  ������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void App_MusicSet(WM_HWIN hWin) 
{
	WM_HWIN hWinMusicSet;
	
	/* ����hWinMusicSet */
	hWinMusicSet = GUI_CreateDialogBox(_aDialogCreateMusicSet, 
	                                   GUI_COUNTOF(_aDialogCreateMusicSet), 
	                                   _cbCallbackMusicSet, 
	                                   hWin, 
	                                   0, 
	                                   0);
	
	/* ����Ϊģ̬���� */
	WM_MakeModal(hWinMusicSet);
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

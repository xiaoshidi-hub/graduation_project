/*
*********************************************************************************************************
*	                                  
*	ģ������ : �ҵĵ��Խ������
*	�ļ����� : App_ComputerDlg.c
*	��    �� : V1.0
*	˵    �� : �ҵĵ��Խ�����ơ�
*              1. ��ǰ������SD����֧�֡�
*              2. �ļ�������õ�CHOOSEFILE�Ի��򣬴˿ؼ��㿪�����һ����������Ĺ��̣�
				  ������������󣬴˿ؼ����������ġ�
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
*                                       �궨��
*********************************************************************************************************
*/
#define MAG          3
#define ID_WINDOW_0 	(GUI_ID_USER + 0x00)
#define ID_TEXT_0 	    (GUI_ID_USER + 0x01)
#define ID_TEXT_1 	    (GUI_ID_USER + 0x02)
#define ID_TEXT_2 	    (GUI_ID_USER + 0x03)
#define ID_TEXT_3 	    (GUI_ID_USER + 0x04)
#define ID_TEXT_4 	    (GUI_ID_USER + 0x05)
#define ID_TEXT_5 	    (GUI_ID_USER + 0x06)
#define ID_PROGBAR_0    (GUI_ID_USER + 0x07)
#define ID_PROGBAR_1    (GUI_ID_USER + 0x08)
#define ID_PROGBAR_2    (GUI_ID_USER + 0x09)
#define ID_BUTTON_0     (GUI_ID_USER + 0xA0)
#define ID_BUTTON_1     (GUI_ID_USER + 0xB0)


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
				
				GUI_DrawBitmap(&bmManualSearch, 0, 5);
			} 
			else 
			{
				GUI_SetColor(GUI_STCOLOR_LIGHTBLUE);
				GUI_AA_FillCircle(0, 0, 72);
				GUI_SetBkColor(GUI_STCOLOR_LIGHTBLUE);
				GUI_SetColor(GUI_WHITE);  

				GUI_DrawBitmap(&bmManualSearch, 0, 5);
			}
			break;
			
		default:
			BUTTON_Callback(pMsg);
	}
}

/*
*********************************************************************************************************
*	               �Ի���ؼ��б�
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateComputer[] = {
	{ WINDOW_CreateIndirect,    "Window",   ID_WINDOW_0,   0,   0,  800, 480, 0, 0x0, 0 },
	{ TEXT_CreateIndirect,      "Ӳ��",     ID_TEXT_0,    3, 98, 400,24, 0,0},
    { TEXT_CreateIndirect,      "�п��ƶ��洢���豸",    ID_TEXT_1,  3,  234, 400,24, 0,0},
	
	{ PROGBAR_CreateIndirect,    NULL,      ID_PROGBAR_0,    40, 125+2, 180, 22, 0, 0},
	{ TEXT_CreateIndirect,      " ",  		ID_TEXT_2,       40, 150+2, 200, 24, 0, 0},
	
	{ PROGBAR_CreateIndirect,    NULL,      ID_PROGBAR_1,    268, 125+2, 180, 22, 0, 0},
	{ TEXT_CreateIndirect,      " ",  		ID_TEXT_3,       268, 150+2, 200, 24, 0, 0},
	
	{ PROGBAR_CreateIndirect,    NULL,      ID_PROGBAR_2,    40, 261+2, 180, 22, 0, 0},
	{ TEXT_CreateIndirect,      " ",  		ID_TEXT_4,       40, 286+2, 200, 24, 0, 0},

	{ BUTTON_CreateIndirect, "DlgBack",     ID_BUTTON_0,    700,   0,  100, 100, 0, 0, 0 },	
	{ BUTTON_CreateIndirect, "MusicList",   ID_BUTTON_1,      0,   0,  100, 100, 0, 0, 0 },
	
	{ TEXT_CreateIndirect,      " ",  	     ID_TEXT_5,       3, 440, 797, 40, 0, 0},
};

/*
*********************************************************************************************************
*	�� �� ��: InitDialogComputer
*	����˵��: �Ի���ص������ĳ�ʼ����Ϣ
*	��    �Σ�pMsg   ��Ϣָ�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void InitDialogComputer(WM_MESSAGE * pMsg)
{
    WM_HWIN hWin = pMsg->hWin;
	WM_HWIN hItem;
	char buf[50];

	hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonBack);
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonList);
	
	hItem = WM_GetDialogItem(hWin, ID_TEXT_0);	
    TEXT_SetFont(hItem, &GUI_SysFontHZ16);
	hItem = WM_GetDialogItem(hWin,ID_TEXT_1);
    TEXT_SetFont(hItem, &GUI_SysFontHZ16);
	
	/* ��ʼ��SD������ */
	hItem = WM_GetDialogItem(hWin, ID_PROGBAR_0);
	PROGBAR_SetFont(hItem, &GUI_Font13_1);
	PROGBAR_SetMinMax(hItem, 0, ullSdCapacity>>20);
    PROGBAR_SetValue(hItem, (ullSdCapacity - ullSdUnusedCapacity)>>20);

	hItem = WM_GetDialogItem(hWin, ID_TEXT_2);
	TEXT_SetTextColor(hItem, 0x646f71);
	TEXT_SetFont(hItem, &GUI_SysFontHZ16);
	sprintf(buf, "����%lldMB,�ܹ�:%lldMB", (ullSdCapacity - ullSdUnusedCapacity)>>20, ullSdCapacity>>20);
	TEXT_SetText(hItem, buf);
	
	/* ��ʼ��NAND���� */
	hItem = WM_GetDialogItem(hWin, ID_PROGBAR_1);
	PROGBAR_SetFont(hItem,&GUI_Font13_1);
	PROGBAR_SetMinMax(hItem, 0, 100);
    PROGBAR_SetValue(hItem, 10);

	hItem = WM_GetDialogItem(hWin, ID_TEXT_3);
	TEXT_SetTextColor(hItem, 0x646f71);
	TEXT_SetFont(hItem, &GUI_SysFontHZ16);
	sprintf(buf, "NAND��δ��֧��");
	TEXT_SetText(hItem, buf);
	
	/* ��ʼ��U�̲��� */
	hItem = WM_GetDialogItem(hWin, ID_PROGBAR_2);
	PROGBAR_SetFont(hItem, &GUI_Font13_1);
	PROGBAR_SetMinMax(hItem, 0, 100);
    PROGBAR_SetValue(hItem, 20);

	hItem = WM_GetDialogItem(hWin, ID_TEXT_4);
	TEXT_SetTextColor(hItem, 0x646f71);
	TEXT_SetFont(hItem, &GUI_SysFontHZ16);
	sprintf(buf, "U����δ��֧��");
	TEXT_SetText(hItem, buf);

	hItem = WM_GetDialogItem(hWin, ID_TEXT_5);
	TEXT_SetTextColor(hItem, GUI_BLACK);
	TEXT_SetFont(hItem, &GUI_SysFontHZ16);
    TEXT_SetText(hItem, "[1]ֻ֧��SD���Ķ�ȡ\n"
						"[2]���������Ң2020���ҵ��ơ����ָ��\n");
	
	/* ���ÿ�������� */
	GUI_AA_SetFactor(MAG);
}

/*
*********************************************************************************************************
*	�� �� ��: _cbCallbackComputer
*	����˵��: �ص�����
*	��    ��: pMsg   ��Ϣָ�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _cbCallbackComputer(WM_MESSAGE * pMsg) 
{
    int NCode, Id;
    WM_HWIN hWin = pMsg->hWin;
	
    switch (pMsg->MsgId) 
    {
		case WM_PRE_PAINT:
			GUI_MULTIBUF_Begin();
			break;
		
		case WM_POST_PAINT:
			GUI_MULTIBUF_End();
			break;	
		
		case WM_PAINT:
			GUI_SetBkColor(GUI_LIGHTCYAN);
			GUI_Clear();   //�޸������汳����ɫ������������������޸Ĳ��ɹ�
			GUI_DrawGradientV(0,			   /* ���Ͻ�X λ�� */
							  0,			   /* ���Ͻ�Y λ�� */
							  799,	   	       /* ���½�X λ�� */
							  479,  	   	   /* ���½�Y λ�� */
							  GUI_WHITE,	   /* ���������Ҫ���Ƶ���ɫ */
							  GUI_LIGHTBLUE);  /* �������Ҳ�Ҫ���Ƶ���ɫ */

			GUI_DrawGradientH(0,			   /* ���Ͻ�X λ�� */
							  117,			   /* ���Ͻ�Y λ�� */
							  450,	           /* ���½�X λ�� */
							  118,             /* ���½�Y λ�� */
							  GUI_RED,	       /* ���������Ҫ���Ƶ���ɫ */
							  GUI_YELLOW);     /* �������Ҳ�Ҫ���Ƶ���ɫ */

			GUI_DrawGradientH(0,			   /* ���Ͻ�X λ�� */
							  253,			   /* ���Ͻ�Y λ�� */
							  450,	           /* ���½�X λ�� */
							  254,             /* ���½�Y λ�� */
							  GUI_RED,	       /* ���������Ҫ���Ƶ���ɫ */
							  GUI_YELLOW);     /* �������Ҳ�Ҫ���Ƶ���ɫ */
							  
  			GUI_DrawBitmap(&bmfilesd,   3,  125+2);
			GUI_DrawBitmap(&bmfiledisk, 230, 125+2);
  			GUI_DrawBitmap(&bmfileusb,  3,  261+2);
            break;
		
        case WM_INIT_DIALOG:
            InitDialogComputer(pMsg);
            break;
			
        case WM_NOTIFY_PARENT:
            Id = WM_GetId(pMsg->hWinSrc); 
            NCode = pMsg->Data.v;        
            switch (Id) 
            {
				/* �رնԻ��� */
				case ID_BUTTON_0:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_RELEASED:
							GUI_EndDialog(hWin, 0);
                            break;
                    }
                    break;
				
				/* ���ļ��б�Ի��� */
				case ID_BUTTON_1:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_RELEASED:
							App_ComputerChild();
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
*	�� �� ��: App_Computer
*	����˵��: �ҵĵ��ԶԻ���
*	��    ��: hWin �������Ի��򸸴��� 	
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void App_Computer(WM_HWIN hWin) 
{
	GUI_CreateDialogBox(_aDialogCreateComputer, 
	                    GUI_COUNTOF(_aDialogCreateComputer), 
	                    &_cbCallbackComputer, 
	                    hWin, 
	                    0, 
	                    0);
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

/*
*********************************************************************************************************
*	                                  
*	ģ������ : USB�������
*	�ļ����� : App_USBDlg.c
*	��    �� : V1.0
*	˵    �� : ������Ҫʵ����SD��ģ��U�̵����ӺͶϿ����ܡ�
*	           1. ��ֲ��RL-USB������ʹ�õ�MicroUSB�ӿڣ���С�ڣ�ʵ��SD��ģ��U�̣�����SD����ز��ϡ�
*	           2. ������240s��ʱ��������ӣ����240s���޷������ϣ��Զ��˳�����,һ������£�500ms���Ҿ��������ˡ�
*	           3. �ε�USB��ǰ����ص���Ͽ����Ӱ�ť��
*	           4. Ϊ�˷�ֹ���������USB��ť���û��ظ�������˰�ť�������˽��ܣ�����Ͽ�USB��ť�ɻָ���
*              5. ��������̫�죬Ҫ����Ӧ��������
*              6. �״����µĵ�����������ʱ�����Զ˻���һ���Զ���װ�����Ĺ��̣����ʱ���Գ������Խ�����ʱ������Ϊ120�롣
*              7. �������½�J12��J13������ñ���ã��в��ɽ�PA11��PA12��ͨ������USBͨ�Ų�������ֱ�Ӱε�����ñ���ɡ�
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

#define ID_WINDOW_0    (GUI_ID_USER + 0x00)
#define ID_TEXT_0	   (GUI_ID_USER + 0x01)
#define ID_TEXT_1 	   (GUI_ID_USER + 0x02)
#define ID_MULTIEDIT_0 (GUI_ID_USER + 0x03)
#define ID_BUTTON_0    (GUI_ID_USER + 0x04)
#define ID_BUTTON_1    (GUI_ID_USER + 0x05)
#define ID_BUTTON_2    (GUI_ID_USER + 0x06)
#define ID_BUTTON_3    (GUI_ID_USER + 0x07)
#define ID_BUTTON_4    (GUI_ID_USER + 0x08)
#define ID_BUTTON_5    (GUI_ID_USER + 0x09)
#define ID_BUTTON_6    (GUI_ID_USER + 0x0a)

#define ID_TimerUSB     0
#define USBConnectTime  2400     /* ��������ʱ����120�룬���120���޷������ϣ��Ͳ������� */


/*
*********************************************************************************************************
*                                       ����
*********************************************************************************************************
*/
extern OS_TID HandleTaskUSB;


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
* �� �� ��: _cbButtonCom
* ����˵��: ��ť�ص�����
* ��    ��: pMsg ��Ϣָ��
* �� �� ֵ: ��
*********************************************************************************************************
*/
static void _cbButtonCom(WM_MESSAGE * pMsg)
{
	WM_HWIN hWin;
	GUI_RECT Rect;
	char buf[30];
	
	hWin = pMsg->hWin;
	
	switch (pMsg->MsgId)
	{
		case WM_PAINT:
			WM_GetClientRect(&Rect);
			if (BUTTON_IsPressed(hWin))
			{
				GUI_SetColor(GUI_DARKGRAY);
				GUI_AA_FillCircle((Rect.x1 - Rect.x0)/2, (Rect.y1 - Rect.y0)/2, (Rect.x1 - Rect.x0)/2);
				GUI_SetBkColor(GUI_DARKGRAY);
				GUI_SetColor(GUI_WHITE);
			}
			else
			{
				GUI_SetColor(GUI_LIGHTBLUE);
				GUI_AA_FillCircle((Rect.x1 - Rect.x0)/2, (Rect.y1 - Rect.y0)/2, (Rect.x1 - Rect.x0)/2);
				GUI_SetBkColor(GUI_LIGHTBLUE);
				GUI_SetColor(GUI_WHITE);
			}
			GUI_SetFont(&GUI_FontHZ16);
			BUTTON_GetText(hWin, buf, 20);
			GUI_DispStringInRect(buf, &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
			break;
			
		default:
			BUTTON_Callback(pMsg);
	}
}

/*
*********************************************************************************************************
*	                             �Ի���ؼ��б�
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateUSB[] = 
{
	{ WINDOW_CreateIndirect,  "Window",  ID_WINDOW_0,  0,   0,  800, 480, 0, 0x0, 0 },
	{ TEXT_CreateIndirect,      " ",  	ID_TEXT_0,     319, 139,  160, 20, 0, 0},
	{ TEXT_CreateIndirect,      " ",  	ID_TEXT_1,     3, 395,  790, 85, 0, 0},
	
	{ BUTTON_CreateIndirect,  "Button",    ID_BUTTON_0,   190,  100, 120, 120, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,  "Button",    ID_BUTTON_1,   490,  100, 120, 120, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,  "DlgBack",   ID_BUTTON_5,   700,    0, 100, 100, 0, 0, 0 },	
	{ BUTTON_CreateIndirect,  "Setting",   ID_BUTTON_6,    0,     0, 100, 100, 0, 0, 0 },
};

/*
*********************************************************************************************************
*	�� �� ��: InitDialogUSB
*	����˵��: �Ի���ص������ĳ�ʼ����Ϣ
*	��    ��: pMsg  ��Ϣָ�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void InitDialogUSB(WM_MESSAGE * pMsg)
{
    WM_HWIN hWin = pMsg->hWin;
	WM_HWIN hItem;
	
	/* ���ÿ�������� */
	GUI_AA_SetFactor(MAG);
	
    /* ��ʼ��4����ť */
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
    BUTTON_SetFont(hItem,  &GUI_FontHZ16);
	BUTTON_SetText(hItem, "����USB");
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonCom);
	
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
    BUTTON_SetFont(hItem,  &GUI_FontHZ16);
    BUTTON_SetText(hItem, "�Ͽ�USB");
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonCom);
	
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_5);
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonBack);
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_6);
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonList);
		
	/* ��ʼ���ı� */
	hItem = WM_GetDialogItem(hWin, ID_TEXT_0);
	TEXT_SetTextColor(hItem, GUI_BLACK);
	TEXT_SetFont(hItem, &GUI_FontHZ16);
	TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetText(hItem, "δ����");
	
	hItem = WM_GetDialogItem(hWin, ID_TEXT_1);
	TEXT_SetTextColor(hItem, GUI_BLACK);
	TEXT_SetFont(hItem, &GUI_FontHZ16);
    TEXT_SetText(hItem, "[1]��ֲ��RL-USB��ʹ��MicroUSB�ӿڣ�ʵ��SD��ģ��U�̣������ϵ�ǰ��SD����USB����ز��ϡ�\n"
						"[2]Ϊ�˷�ֹ���������USB��ť���û��ظ�������˰�ť�������˽��ܣ�����Ͽ�USB��ť�ɻָ���\n"
						"[3]�״��ڵ�����������ʱ�����Զ˻���һ���Զ���װ�����Ĺ��̣����ʱ���Գ�����������240��������ӣ�\n"
						"   ���240s���޷������ϣ��Զ��˳����ӡ����⣬�ε�USB��ǰ����ص���Ͽ����Ӱ�ť��\n"
						"[4]�������½�J12��J13������ñ�в��ɷֱ�ѡ��PA11��PA12��ͨ������USBͨ�Ų�������ֱ�Ӱε�����ñ���ɡ�\n"
						);
}

/*
*********************************************************************************************************
*	�� �� ��: _cbCallbackUSB
*	����˵��: �ص�����
*	��    ��: pMsg   ��Ϣָ�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _cbCallbackUSB(WM_MESSAGE * pMsg) 
{
    int NCode, Id;
	WM_HWIN hItem;
    WM_HWIN hWin = pMsg->hWin;
	char buf[50];
	static int lCount1, lCount2;
	static WM_HTIMER hTimerUSB;
	static char ucConFlag = 0;
	
    switch (pMsg->MsgId) 
    {
        case WM_INIT_DIALOG:
			lCount1 = 0;
			lCount2 = 0;
			ucConFlag = 0;
            InitDialogUSB(pMsg);
            break;
		
		case WM_PAINT:
			GUI_DrawGradientV(0,			   /* ���Ͻ�X λ�� */
							  0,			   /* ���Ͻ�Y λ�� */
							  799,	   	       /* ���½�X λ�� */
							  479,  	   	   /* ���½�Y λ�� */
							  GUI_WHITE,	   /* ���������Ҫ���Ƶ���ɫ */
							  GUI_LIGHTBLUE);  /* �������Ҳ�Ҫ���Ƶ���ɫ */

			GUI_DrawGradientH(319,			   /* ���Ͻ�X λ�� */
							  159,			   /* ���Ͻ�Y λ�� */
							  479,	           /* ���½�X λ�� */
							  161,             /* ���½�Y λ�� */
							  GUI_RED,	       /* ���������Ҫ���Ƶ���ɫ */
							  GUI_YELLOW);     /* �������Ҳ�Ҫ���Ƶ���ɫ */

			break;
		
		case WM_TIMER:
			/* ������Ӻ��û��ֶ��ε���USB�ߣ��������û����ʾ�� */
			if(usbd_configured() == __TRUE)
			{
				hItem = WM_GetDialogItem(hWin, ID_TEXT_0);
				sprintf(buf, "������%d��\r\n", lCount2++);
				TEXT_SetText(hItem, buf);
				
				/* ������ʱ�� */
				WM_RestartTimer(pMsg->Data.v, 1000);		
			}
			else
			{
				hItem = WM_GetDialogItem(hWin, ID_TEXT_0);
				sprintf(buf, "������%d...\r\n", lCount1++);
				TEXT_SetText(hItem, buf);

				/* ������ʱ�� */
				WM_RestartTimer(pMsg->Data.v, 100);	

				/* 120���ʱ�䵽��δ�����ϣ�ֹͣ���� */
				if(lCount1 == USBConnectTime)
				{
					/* ɾ���󣬲������ٵ���WM_RestartTimer�������Ӳ���쳣 */
					ucConFlag = 0;
					WM_DeleteTimer(hTimerUSB);
					hItem = WM_GetDialogItem(hWin, ID_BUTTON_0);
					BUTTON_SetText(hItem, "����USB");
					WM_EnableWindow(hItem);
					hItem = WM_GetDialogItem(hWin, ID_TEXT_0);
					TEXT_SetText(hItem, "δ����");
					os_evt_set (0x0002, HandleTaskUSB);
				}				
			}
			break;
			
        case WM_NOTIFY_PARENT:
            Id = WM_GetId(pMsg->hWinSrc); 
            NCode = pMsg->Data.v;        
            switch (Id) 
            {
				case ID_BUTTON_0:
                    switch(NCode)
                    {
						/* ����USB */
                        case WM_NOTIFICATION_CLICKED:
							lCount1 = 0;
							lCount2 = 0;
						    hItem = WM_GetDialogItem(hWin, ID_BUTTON_0);
							WM_DisableWindow(hItem);
							BUTTON_SetText(hItem, "������\n�Ѿ���ֹ����");
							os_evt_set (USBTaskOpen_0, HandleTaskUSB);
							ucConFlag = 1;
						    hTimerUSB = WM_CreateTimer(pMsg->hWin, ID_TimerUSB, 0, 0);	
                            break;
                    }
                    break;
					
				case ID_BUTTON_1:
                    switch(NCode)
                    {
						/* �Ͽ�USB */
                        case WM_NOTIFICATION_CLICKED:
							if(ucConFlag == 1)
							{
								ucConFlag = 0;
								WM_DeleteTimer(hTimerUSB);
							}
							hItem = WM_GetDialogItem(hWin, ID_BUTTON_0);
							WM_EnableWindow(hItem);
							BUTTON_SetText(hItem, "����USB");
							hItem = WM_GetDialogItem(hWin, ID_TEXT_0);
							TEXT_SetText(hItem, "δ����");
							os_evt_set (USBTaskClose_1, HandleTaskUSB);
                            break;
                    }
                    break;

				/* �ر�USB���� */
				case ID_BUTTON_5:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_RELEASED:
							if(ucConFlag == 1)
							{
								os_evt_set (USBTaskClose_1, HandleTaskUSB);
							}
							os_evt_set (USBTaskReMountSD_2, HandleTaskUSB);
							GUI_EndDialog(pMsg->hWin, 0);
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
*	�� �� ��: App_USB
*	����˵��: ����USB���ƶԻ���
*	��    ��: hWin  ������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void App_USB(WM_HWIN hWin) 
{				 
    GUI_CreateDialogBox(_aDialogCreateUSB, 
	                    GUI_COUNTOF(_aDialogCreateUSB), 
	                    &_cbCallbackUSB, 
	                    hWin, 
	                    0, 
	                    0);
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

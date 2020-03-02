/*
*********************************************************************************************************
*	                                  
*	ģ������ : 485���������շ��������
*	�ļ����� : App_ModbusDlg.c
*	��    �� : V1.0
*	˵    �� : 485���������շ�������ơ�
*              1. ��ֲ��Modbus RTU��վ������ʱ��Ҫ����Ԥװ�˴�վ����İ��ӣ���վ��ַΪ0x01
*			   2. ListView�ؼ��ϵ������ǹ��û�ѡ��ģ�ѡ�����������������ɣ�ϵͳ�ϵ��Ĭ��ѡ�������
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


/*
*********************************************************************************************************
*                                       �궨��
*********************************************************************************************************
*/
extern OS_TID HandleTaskModbus;
WM_HWIN  hWinModbus = WM_HWIN_NULL;  	/* Modbus���ھ�� */

static const char *ModbusCmd0[]= {"����Ȧ�Ĵ���     ", " 01", " 01", " 01 01", " 00 04", " 6D 5F"};
static const char *ModbusCmd1[]= {"����ɢ����Ĵ��� ", " 01", " 02", " 02 01", " 00 03", " 68 73"};
static const char *ModbusCmd2[]= {"�����ּĴ���     ", " 01", " 03", " 03 01", " 00 02", " 95 8F"};
static const char *ModbusCmd3[]= {"������Ĵ���     ", " 01", " 04", " 04 01", " 00 01", " 61 3A"};
static const char *ModbusCmd4[]= {"д������Ȧ�Ĵ��� ", " 01", " 05", " 01 01", " 00 01", " 5C 36"};
static const char *ModbusCmd5[]= {"д������Ȧ�Ĵ��� ", " 01", " 05", " 01 01", " 00 00", " 9D F6"};
static const char *ModbusCmd6[]= {"д�������ּĴ��� ", " 01", " 06", " 03 01", " 00 01", " 19 8E"};
static const char *ModbusCmd7[]= {"д�������ּĴ��� ", " 01", " 06", " 03 01", " 00 00", " D8 4E"};
static const char *ModbusCmd8[]= {"д������ּĴ��� ", " 01", " 10", " 03 01", " 00 02 04 01 02 03 04", " 87 9C"};


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
				GUI_AA_FillRoundedRect(Rect.x0, Rect.y0, Rect.x1, Rect.y1, 5);
				GUI_SetBkColor(GUI_DARKGRAY);
				GUI_SetColor(GUI_WHITE);
			}
			else
			{
				GUI_SetColor(GUI_LIGHTBLUE);
				GUI_AA_FillRoundedRect(Rect.x0, Rect.y0, Rect.x1, Rect.y1, 5);
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
*	                                  �Ի���ؼ��б�
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateModbus[] = 
{
	{ WINDOW_CreateIndirect,  "Window",  ID_WINDOW_0,   0,   0,  800, 480, 0, 0x0, 0 },
	
	{ TEXT_CreateIndirect,      " ",  	ID_TEXT_0,     80, 370,  630, 60, 0, 0},
	{ TEXT_CreateIndirect,      " ",  	ID_TEXT_1,     3, 442,  790, 35, 0, 0},
	
	{ LISTVIEW_CreateIndirect, NULL,     GUI_ID_LISTVIEW0,   70, 90, 660, 245,  0, 0},
	
	{ BUTTON_CreateIndirect,  "Button",    ID_BUTTON_0,   70,  30, 100, 45, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,  "Button",    ID_BUTTON_1,  210,  30, 100, 45, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,  "DlgBack",   ID_BUTTON_5,  700,   0, 100, 100, 0, 0, 0 },	
	{ BUTTON_CreateIndirect,  "MusicList", ID_BUTTON_6,    0,   0, 100, 100, 0, 0, 0 },
};

/*
*********************************************************************************************************
*	�� �� ��: InitDialogModbus
*	����˵��: �Ի���ص������ĳ�ʼ����Ϣ
*	��    ��: pMsg  ��Ϣָ�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void InitDialogModbus(WM_MESSAGE * pMsg)
{
    WM_HWIN hWin = pMsg->hWin;
	WM_HWIN hItem;
	HEADER_Handle hHeader;

	/* ���ÿ�������� */
	GUI_AA_SetFactor(MAG);
	
	/* ����ListView�ؼ��ϵ�header */
	hItem = WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0);
    hHeader = LISTVIEW_GetHeader(hItem);
    HEADER_SetFont(hHeader, &GUI_FontHZ16);
    HEADER_SetHeight(hHeader, 25);
	HEADER_SetSTSkin(hHeader);
	
	/* ����ListView�ؼ� */
    LISTVIEW_SetFont(hItem, &GUI_FontHZ16);
    LISTVIEW_SetRowHeight(hItem, 24);
    LISTVIEW_AddColumn(hItem, 60, " ����", GUI_TA_VCENTER|GUI_TA_LEFT);
    LISTVIEW_AddColumn(hItem, 60, " �ӻ���ַ", GUI_TA_VCENTER|GUI_TA_LEFT);
    LISTVIEW_AddColumn(hItem, 60, " ������", GUI_TA_VCENTER|GUI_TA_LEFT);
	LISTVIEW_AddColumn(hItem, 60, " �Ĵ����׵�ַ", GUI_TA_VCENTER|GUI_TA_LEFT);
	LISTVIEW_AddColumn(hItem, 60, " �Ĵ�������/д��ֵ", GUI_TA_VCENTER|GUI_TA_LEFT);
	LISTVIEW_AddColumn(hItem, 60, " У����", GUI_TA_VCENTER|GUI_TA_LEFT);		
    LISTVIEW_SetColumnWidth(hItem, 0, 135);
    LISTVIEW_SetColumnWidth(hItem, 1, 85);
    LISTVIEW_SetColumnWidth(hItem, 2, 70);
	LISTVIEW_SetColumnWidth(hItem, 3, 120);
    LISTVIEW_SetColumnWidth(hItem, 4, 170);
	LISTVIEW_SetColumnWidth(hItem, 5, 80);
	
	LISTVIEW_AddRow(hItem, (GUI_ConstString *)ModbusCmd0);
	LISTVIEW_AddRow(hItem, (GUI_ConstString *)ModbusCmd1);
	LISTVIEW_AddRow(hItem, (GUI_ConstString *)ModbusCmd2);
	LISTVIEW_AddRow(hItem, (GUI_ConstString *)ModbusCmd3);
	LISTVIEW_AddRow(hItem, (GUI_ConstString *)ModbusCmd4);
	LISTVIEW_AddRow(hItem, (GUI_ConstString *)ModbusCmd5);
	LISTVIEW_AddRow(hItem, (GUI_ConstString *)ModbusCmd6);
	LISTVIEW_AddRow(hItem, (GUI_ConstString *)ModbusCmd7);
	LISTVIEW_AddRow(hItem, (GUI_ConstString *)ModbusCmd8);
	
	LISTVIEW_SetSel(hItem, 1); /* �ϵ����ѡ��1 */
	
    /* ��ʼ��5����ť */
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
    BUTTON_SetFont(hItem,  &GUI_FontHZ16);
	BUTTON_SetText(hItem, "��������");
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonCom);
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
    BUTTON_SetFont(hItem,  &GUI_FontHZ16);
	BUTTON_SetText(hItem, "���״̬��");
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonCom);

	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_5);
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonBack);
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_6);
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonList);
	
	hItem = WM_GetDialogItem(hWin, ID_TEXT_0);
	TEXT_SetTextColor(hItem, GUI_BLACK);
	TEXT_SetFont(hItem, &GUI_FontHZ16);
    TEXT_SetText(hItem, "��������: \n"
	                    "��������: \n"
	                    "״    ̬: \n");
		
	/* ��ʼ���ı� */
	hItem = WM_GetDialogItem(hWin, ID_TEXT_1);
	TEXT_SetTextColor(hItem, GUI_BLACK);
	TEXT_SetFont(hItem, &GUI_FontHZ16);
    TEXT_SetText(hItem, "[1]��ֲ��Modbus RTU��վ������ʱ��Ҫ����Ԥװ�˴�վ����İ��ӣ���վ��ַΪ0x01��\n"
						"[2]ListView�ؼ��ϵ������ǹ��û�ѡ��ģ�ѡ�����������������ɣ�ϵͳ�ϵ��Ĭ��ѡ���������\n");
}

/*
*********************************************************************************************************
*	�� �� ��: _cbCallbackModbus
*	����˵��: �ص�����
*	��    ��: pMsg   ��Ϣָ�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _cbCallbackModbus(WM_MESSAGE * pMsg) 
{
    int NCode, Id;
	WM_HWIN hItem;
    WM_HWIN hWin = pMsg->hWin;
	char buf[200];
	int i, j;
	int RowValue;
	char *p;
	
	
    switch (pMsg->MsgId) 
    {
        case WM_INIT_DIALOG:
            InitDialogModbus(pMsg);
            break;
		
		case WM_PAINT:
			GUI_SetBkColor(GUI_WHITE);
			GUI_Clear();
			GUI_SetColor(GUI_RED);
			GUI_AA_DrawRoundedRect(70, 350, 660+70, 80+350, 6);
			GUI_SetFont(&GUI_FontHZ16);
			GUI_DispStringAt("����/����״̬", 90, 345);
			break;
		
		/* ����ICON�ľ۽� */
		case MSG_ModbusSuccess:
			hItem = WM_GetDialogItem(hWin, ID_TEXT_0);
		
			strcpy((char *)buf, "��������:");
			j = strlen(buf);
			p = &buf[j];
			for (i = 0; i < g_tPrint.Txlen; i++)
			{
				sprintf(p, " %02X", g_tPrint.TxBuf[i]);
				p = &buf[j + (i+1)*3];
			}
			
			strcat(buf, "\n��������:");
			j = strlen(buf);
			p = &buf[j];
			for (i = 0; i < g_tPrint.Rxlen; i++)
			{
				sprintf(p, " %02X", g_tPrint.RxBuf[i]);
				p = &buf[j + (i+1)*3];
			}
			
			strcat(buf, "\n״    ̬: �ɹ�");

			TEXT_SetText(hItem, buf);
			break;
			
			/* ����ICON�ľ۽� */
		case MSG_ModbusErr:
			hItem = WM_GetDialogItem(hWin, ID_TEXT_0);
		
			strcpy((char *)buf, "��������:");
			j = strlen(buf);
			p = &buf[j];
			for (i = 0; i < g_tPrint.Txlen; i++)
			{
				sprintf(p, " %02X", g_tPrint.TxBuf[i]);
				p = &buf[j + (i+1)*3];
			}
			
			strcat(buf, "\n��������:");
			j = strlen(buf);
			p = &buf[j];
			for (i = 0; i < g_tPrint.Rxlen; i++)
			{
				sprintf(p, " %02X", g_tPrint.RxBuf[i]);
				p = &buf[j + (i+1)*3];
			}
			
			strcat(buf, "\n״    ̬: ʧ��");

			TEXT_SetText(hItem, buf);
			break;
			
        case WM_NOTIFY_PARENT:
            Id = WM_GetId(pMsg->hWinSrc); 
            NCode = pMsg->Data.v;        
            switch (Id) 
            {
				case ID_BUTTON_0:
                    switch(NCode)
                    {
						/* ����Modbus���� */
                        case WM_NOTIFICATION_CLICKED:
							hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_LISTVIEW0);	
							RowValue = LISTVIEW_GetSel(hItem);
							//printf("-----RowValue = %d\r\n", RowValue);
							if(RowValue >= 0)
							{
								os_evt_set (1<<RowValue, HandleTaskModbus);
							}
                            break;
                    }
                    break;
				
				/* ��շ���/����״̬���� */
				case ID_BUTTON_1:
                    switch(NCode)
                    {
						
                        case WM_NOTIFICATION_CLICKED:
							hItem = WM_GetDialogItem(hWin, ID_TEXT_0);
							TEXT_SetText(hItem, "");
                            break;
                    }
                    break;
					
				/* �ر�Modbus���� */
				case ID_BUTTON_5:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_RELEASED:
						    hWinModbus = WM_HWIN_NULL;
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
*	�� �� ��: App_Modbus
*	����˵��: ����RS485 Modbus���ƶԻ���
*	��    ��: hWin  ������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void App_Modbus(WM_HWIN hWin) 
{					 
    hWinModbus = GUI_CreateDialogBox(_aDialogCreateModbus, 
	                                 GUI_COUNTOF(_aDialogCreateModbus), 
	                                 &_cbCallbackModbus, 
	                                 hWin, 
	                                 0, 
	                                 0);
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

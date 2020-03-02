/*
*********************************************************************************************************
*	                                  
*	ģ������ : CAN���������շ��������
*	�ļ����� : App_CANDlg.c
*	��    �� : V1.0
*	˵    �� : CAN���������շ�������ơ�
*              1. ��ֲ��RL-CAN������ʹ�õ�CAN2���ٶ�500Kbps����׼ID
*			   2. ����ʱ����Ҫʹ��������ӣ�������ʽ����
*			   3. CAN�ı�ʶ�����������ñ�ʶ���б�ģʽ������ÿ�����ӽ����ձ�׼IDΪ33������֡����ң��֡
*              4. ���ڷ��͵�Ĭ��ʱ����500ms��
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

#define ID_TimerCAN     0
#define ID_TimerTime    1


/*
*********************************************************************************************************
*                                       ����
*********************************************************************************************************
*/
WM_HWIN  hWinCAN = WM_HWIN_NULL;  	/* CAN���ھ�� */
extern CAN_msg msg_rece;
static uint32_t s_uiCount = 0;
static SCROLLBAR_Handle hScrollbar;

static const char *can_frametype[] =
{
	"����֡",
	"ң��֡"
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
static const GUI_WIDGET_CREATE_INFO _aDialogCreateCAN[] = 
{
	{ WINDOW_CreateIndirect,  "Window",  ID_WINDOW_0,   0,   0,  800, 480, 0, 0x0, 0 },
	
	{ TEXT_CreateIndirect,      " ",  	ID_TEXT_1,     3, 420,  700, 60, 0, 0},
	{ LISTVIEW_CreateIndirect, NULL,       GUI_ID_LISTVIEW0,   70, 120, 660, 270,  0, 0},
	
	{ BUTTON_CreateIndirect,  "Button",    ID_BUTTON_0,   70,  45, 100, 50, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,  "Button",    ID_BUTTON_1,  210,  45, 100, 50, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,  "Button",    ID_BUTTON_2,  350,  45, 100, 50, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,  "Button",    ID_BUTTON_3,  490,  45, 100, 50, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,  "Button",    ID_BUTTON_4,  630,  45, 100, 50, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,  "DlgBack",   ID_BUTTON_5,  700,   0, 100, 100, 0, 0, 0 },	
	{ BUTTON_CreateIndirect,  "MusicList", ID_BUTTON_6,    0,   0, 100, 100, 0, 0, 0 },
};

/*
*********************************************************************************************************
*	�� �� ��: InitDialogCAN
*	����˵��: �Ի���ص������ĳ�ʼ����Ϣ
*	��    ��: pMsg  ��Ϣָ�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void InitDialogCAN(WM_MESSAGE * pMsg)
{
    WM_HWIN hWin = pMsg->hWin;
	WM_HWIN hItem;
	HEADER_Handle hHeader;

	s_uiCount = 0;
	
	/* ���ÿ�������� */
	GUI_AA_SetFactor(MAG);
	
	/* ����ListView�ؼ��ϵ�header */
	hItem = WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0);
    hHeader = LISTVIEW_GetHeader(hItem);
    HEADER_SetFont(hHeader, &GUI_FontHZ16);
    HEADER_SetHeight(hHeader, 25);
	HEADER_SetSTSkin(hHeader);
	
	/* ����ListView�ؼ��ϵ�SCROLLBAR */
    hScrollbar = SCROLLBAR_CreateAttached(hItem, SCROLLBAR_CF_VERTICAL);
    SCROLLBAR_SetWidth(hScrollbar, 18);
	SCROLLBAR_SetSTSkin(hScrollbar);
	
	/* ����ListView�ؼ� */
    LISTVIEW_SetFont(hItem, &GUI_FontHZ16);
    LISTVIEW_SetRowHeight(hItem, 24);
    LISTVIEW_AddColumn(hItem, 60, " ���",  GUI_TA_VCENTER|GUI_TA_LEFT);
    LISTVIEW_AddColumn(hItem, 60, " ID",    GUI_TA_VCENTER|GUI_TA_LEFT);
    LISTVIEW_AddColumn(hItem, 60, " ����",  GUI_TA_VCENTER|GUI_TA_LEFT);
	LISTVIEW_AddColumn(hItem, 60, " ���ݳ���", GUI_TA_VCENTER|GUI_TA_LEFT);
	LISTVIEW_AddColumn(hItem, 60, " ֡����",   GUI_TA_VCENTER|GUI_TA_LEFT);	
    LISTVIEW_SetColumnWidth(hItem, 0, 50);
    LISTVIEW_SetColumnWidth(hItem, 1, 50);
    LISTVIEW_SetColumnWidth(hItem, 2, 360);
	LISTVIEW_SetColumnWidth(hItem, 3, 90);
    LISTVIEW_SetColumnWidth(hItem, 4, 110);
	
    /* ��ʼ��5����ť */
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
    BUTTON_SetFont(hItem,  &GUI_FontHZ16);
	BUTTON_SetText(hItem, "��������֡");
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonCom);
	
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
    BUTTON_SetFont(hItem,  &GUI_FontHZ16);
    BUTTON_SetText(hItem, "����ң��֡");
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonCom);
	
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
    BUTTON_SetFont(hItem,  &GUI_FontHZ16);
    BUTTON_SetText(hItem, "���ڷ���");
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonCom);
	
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_3);
    BUTTON_SetFont(hItem, &GUI_FontHZ16);
    BUTTON_SetText(hItem, "ֹͣ����");
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonCom);
	
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_4);
    BUTTON_SetFont(hItem, &GUI_FontHZ16);
    BUTTON_SetText(hItem, "��ս�����");
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonCom);
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_5);
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonBack);
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_6);
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonList);
		
	/* ��ʼ���ı� */
	hItem = WM_GetDialogItem(hWin, ID_TEXT_1);
	TEXT_SetTextColor(hItem, GUI_BLACK);
	TEXT_SetFont(hItem, &GUI_FontHZ16);
    TEXT_SetText(hItem, "[1]��ֲ��RL-CAN������ʹ�õ�CAN2���ٶ�500Kbps����׼ID��\n"
						"[2]����ʱ����Ҫʹ��������ӣ�������ʽ���ӡ�\n"
						"[3]CAN�ı�ʶ�����������ñ�ʶ���б�ģʽ������ÿ�����ӽ����ձ�׼IDΪ33������֡����ң��֡��\n");
}

/*
*********************************************************************************************************
*	�� �� ��: _cbCallbackCAN
*	����˵��: �ص�����
*	��    ��: pMsg   ��Ϣָ�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _cbCallbackCAN(WM_MESSAGE * pMsg) 
{
    int NCode, Id;
	WM_HWIN hItem;
	char buf[200];
	int i, j;
	static  WM_HTIMER hTimerCAN;
	static  WM_HTIMER hTimerTime;
	
	/* ������Ϣ      = { ID, {data[0] .. data[7]}, LEN, CHANNEL, FORMAT, TYPE } */
	CAN_msg msg_send = { 33, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   
						  8, 2, STANDARD_FORMAT, DATA_FRAME};
	
    switch (pMsg->MsgId) 
    {
        case WM_INIT_DIALOG:
            InitDialogCAN(pMsg);
            break;
		
		case WM_TIMER:
			if(WM_GetTimerId(pMsg->Data.v) == ID_TimerCAN)
		    {
				WM_DeleteTimer(hTimerCAN);
				hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_LISTVIEW0);	
				LISTVIEW_AddRow(hItem, NULL);
				
				/* ��� */
				sprintf(buf, "%d", s_uiCount);
				LISTVIEW_SetItemText(hItem, 0, s_uiCount, (const char *)buf);
			    
				/* ��׼ID */
				sprintf(buf, "%d", msg_rece.id);
				LISTVIEW_SetItemText(hItem, 1, s_uiCount, (const char *)buf);
			
				/* ���������֡ */
				if(msg_rece.type == DATA_FRAME)
				{
					/* ���յ������� */
					sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d", 
									  msg_rece.data[0],
									  msg_rece.data[1],
									  msg_rece.data[2],
								      msg_rece.data[3],
								      msg_rece.data[4],
								      msg_rece.data[5],
								      msg_rece.data[6],
								      msg_rece.data[7]);
					
					LISTVIEW_SetItemText(hItem, 2, s_uiCount, (const char *)buf);
				
					/* ���ݳ��� */
					sprintf(buf, "%d", msg_rece.len);
					LISTVIEW_SetItemText(hItem, 3, s_uiCount, (const char *)buf);
				}
				/* �����ң��֡ */
				else
				{
					/* ���յ������� */
					sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d", 0,0,0,0,0,0,0,0);
					
					LISTVIEW_SetItemText(hItem, 2, s_uiCount, (const char *)buf);
				
					/* ���ݳ��� */
					sprintf(buf, "%d", 0);
					LISTVIEW_SetItemText(hItem, 3, s_uiCount, (const char *)buf);
				}
				
				/* ֡���� */
				sprintf(buf, "%s", can_frametype[msg_rece.type]);
				LISTVIEW_SetItemText(hItem, 4, s_uiCount++, (const char *)buf);
				
				/* ListView������к󣬱�֤һֱ��ʾ������ */
				SCROLLBAR_SetValue(hScrollbar, SCROLLBAR_GetNumItems(hScrollbar));
			}
			else if(WM_GetTimerId(pMsg->Data.v) == ID_TimerTime)
			{
				msg_send.type = DATA_FRAME;
				msg_send.id = 33;
				msg_send.len = 8;
				msg_send.data[0] = rand()%255;
				msg_send.data[1] = rand()%255;
				msg_send.data[2] = rand()%255;
				msg_send.data[3] = rand()%255;
				msg_send.data[4] = rand()%255;
				msg_send.data[5] = rand()%255;
				msg_send.data[6] = rand()%255;
				msg_send.data[7] = rand()%255;
				CAN_send(2, &msg_send, 1000);
				
				/* ������ʱ�� */
				WM_RestartTimer(pMsg->Data.v, 500);
			}
			break;
		
		/* ���յ�CAN���� */
		case MSG_CANReceive:
			hTimerCAN = WM_CreateTimer(pMsg->hWin, ID_TimerCAN, 0, 0);	
			break;
			
        case WM_NOTIFY_PARENT:
            Id = WM_GetId(pMsg->hWinSrc); 
            NCode = pMsg->Data.v;        
            switch (Id) 
            {
				case ID_BUTTON_0:
                    switch(NCode)
                    {
						/* ��������֡ */
                        case WM_NOTIFICATION_CLICKED:
							msg_send.type = DATA_FRAME;
							msg_send.id = 33;
							msg_send.len = 8;
							msg_send.data[0] = rand()%255;
							msg_send.data[1] = rand()%255;
							msg_send.data[2] = rand()%255;
							msg_send.data[3] = rand()%255;
							msg_send.data[4] = rand()%255;
							msg_send.data[5] = rand()%255;
							msg_send.data[6] = rand()%255;
							msg_send.data[7] = rand()%255;
							CAN_send(2, &msg_send, 1000);
                            break;
                    }
                    break;
					
				case ID_BUTTON_1:
                    switch(NCode)
                    {
						/* ����ң��֡ */
                        case WM_NOTIFICATION_CLICKED:
							msg_send.type = REMOTE_FRAME;
							msg_send.id = 33;
							CAN_request(2, &msg_send, 1000);
                            break;
                    }
                    break;
					
				case ID_BUTTON_2:
                    switch(NCode)
                    {
						/* ���������Է�������֡ */
                        case WM_NOTIFICATION_CLICKED:
							hTimerTime = WM_CreateTimer(pMsg->hWin, ID_TimerTime, 0, 0);
                            break;
                    }
                    break;
					
				case ID_BUTTON_3:
                    switch(NCode)
                    {
						/* ֹͣ�����Է�������֡ */
                        case WM_NOTIFICATION_CLICKED:
							WM_DeleteTimer(hTimerTime);
                            break;
                    }
                    break;
					
				case ID_BUTTON_4:
                    switch(NCode)
                    {
						/* ���ListView�������� */
                        case WM_NOTIFICATION_CLICKED:
							hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_LISTVIEW0);	
							j = LISTVIEW_GetNumRows(hItem);

						    #if 1
								for(i = 0; i < j; i++)
								{
									LISTVIEW_DeleteRow(hItem, 0);
								}
							#else
								for(i = j - 1; i >=0; i--)
								{
									LISTVIEW_DeleteRow(hItem, i);
								}
							#endif
								
							s_uiCount = 0;
                            break;
                    }
                    break;
					
				/* �ر�USB���� */
				case ID_BUTTON_5:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_RELEASED:
						    hWinCAN = WM_HWIN_NULL;
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
*	�� �� ��: App_CAN
*	����˵��: ����CAN���ƶԻ���
*	��    ��: hWin  ������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void App_CAN(WM_HWIN hWin)
{					 
    hWinCAN = GUI_CreateDialogBox(_aDialogCreateCAN, 
	                              GUI_COUNTOF(_aDialogCreateCAN), 
	                              &_cbCallbackCAN, 
	                              hWin, 
	                              0, 
	                              0);
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

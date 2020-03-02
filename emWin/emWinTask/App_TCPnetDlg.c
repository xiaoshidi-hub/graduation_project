/*
*********************************************************************************************************
*
*	ģ������ : TCPnet����Э��ջ���Խ��档
*	�ļ����� : App_TCPnetDlg.c
*	��    �� : V1.0
*	˵    �� : ��Ҫ��ʾ������Ϣ��SNTP�ͻ�����Ϣ��Ƕ��ʽweb��Ϣ��ÿ�����һ�Ρ�
*              1. ��ֲ��RL-TCPnet����Э��ջ�����ڶ�ӦDM9161����ؽ������ӵ���������·�ɻ򽻻����ϣ������
	              �๦���޷��������У����߿��������Σ�֧���Զ�������������Ϣ��SNTP�ͻ��˶���ÿ�����һ�Ρ�
			   2. DHCP�Ѿ�ʹ�ܣ������������8�����޷���·����/���������IP��ַ����ʹ�ù̶�IP:192.168.1.200��
			   3. ��������ȶ���SNTP�����������ˣ���ǰʹ�õ����ʱ��ʱ���������Ҳ��Բ��ɹ��Ļ�Ҳ�������ġ�
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
#define ID_TEXT_2      (GUI_ID_USER + 0x03)
#define ID_TEXT_3      (GUI_ID_USER + 0x04)
#define ID_BUTTON_0    (GUI_ID_USER + 0x05)
#define ID_BUTTON_1    (GUI_ID_USER + 0x06)
#define ID_BUTTON_2    (GUI_ID_USER + 0x07)
#define ID_BUTTON_3    (GUI_ID_USER + 0x08)
#define ID_BUTTON_4    (GUI_ID_USER + 0x09)
#define ID_BUTTON_5    (GUI_ID_USER + 0x0a)
#define ID_BUTTON_6    (GUI_ID_USER + 0x0b)

extern  LOCALM localm[];
#define LocalM   localm[NETIF_ETH]


/*
*********************************************************************************************************
*                                       ����
*********************************************************************************************************
*/
WM_HWIN  hWinNET = WM_HWIN_NULL;  	/* ���細�ھ�� */

char *SNTP_Status[] =
{
	"SNTP�����Ѿ����ͳɹ�",
	"ʧ��, SNTPδ�������߲�������",
	"����, ������δ��Ӧ��������״̬�Ƚϲ�"
};

char *NetCable_Status[] =
{
	"�Ͽ�",
	"����",
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
*	                                 �Ի���ؼ��б�
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateNet[] = 
{
	{ WINDOW_CreateIndirect,  "Window", ID_WINDOW_0,   0,   0,  800, 480, 0, 0x0, 0 },
	{ TEXT_CreateIndirect,      " ",  	ID_TEXT_0,     80, 55,  630, 150, 0, 0},
	{ TEXT_CreateIndirect,      " ",  	ID_TEXT_1,     80, 225,  630, 80, 0, 0},
	{ TEXT_CreateIndirect,      " ",  	ID_TEXT_2,     80, 335,  630, 60, 0, 0},
	{ TEXT_CreateIndirect,      " ",  	ID_TEXT_3,     3, 410,  790, 70, 0, 0},

	{ BUTTON_CreateIndirect,  "DlgBack",   ID_BUTTON_5,  700,   0, 100, 100, 0, 0, 0 },	
	{ BUTTON_CreateIndirect,  "MusicList", ID_BUTTON_6,    0,   0, 100, 100, 0, 0, 0 },
};

/*
*********************************************************************************************************
*	�� �� ��: UpdateDisp
*	����˵��: ������ʾ
*	��    ��: pMsg ָ�����            
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void UpdateDisp(WM_MESSAGE * pMsg)
{
	char buf[200];
	WM_HWIN hWin = pMsg->hWin;
	WM_HWIN hItem;
	int j;
	char *p;

	
	hItem = WM_GetDialogItem(hWin, ID_TEXT_1);

	sprintf(buf, "������ʱ�䣺%02d:%02d:%02d\n", (int)g_time.tm_hour, (int)g_time.tm_min, (int)g_time.tm_sec);

	j = strlen(buf);
	p = &buf[j];
	sprintf(p, 
			"���������ڣ�%04d/%02d/%02d\n", 
			g_time.tm_year, 
			g_time.tm_mon, 
			g_time.tm_mday);

	j = strlen(buf);
	p = &buf[j];
	sprintf(p, "SNTP�ͻ��˻�ȡ״̬: %s\n", SNTP_Status[g_SNTPStatus]);

	j = strlen(buf);
	p = &buf[j];
	sprintf(p, "ͨ������Զ��SNTP������IP: 182.16.3.162 ��ȡʱ������� \n");

	TEXT_SetText(hItem, buf);


	hItem = WM_GetDialogItem(hWin, ID_TEXT_0);

	////////////////////////
	sprintf(buf, "����ͨ��: %s\n", NetCable_Status[g_ucEthLinkStatus]);

	j = strlen(buf);
	p = &buf[j];
	sprintf(p, "DHCP״̬: %s\nNetBIOS���Ʒ�����: Enable\n", DHCP_Status[1]);

	j = strlen(buf);
	p = &buf[j];
	sprintf(p, "IP��ַ: %s\n", DHCP_Status[0]);

	j = strlen(buf);
	p = &buf[j];
	sprintf(p, "��������: %d.%d.%d.%d\n", LocalM.NetMask[0],
										  LocalM.NetMask[1],
										  LocalM.NetMask[2],
										  LocalM.NetMask[3]);

	j = strlen(buf);
	p = &buf[j];
	sprintf(p, "��������: %d.%d.%d.%d\n", LocalM.DefGW[0],
										  LocalM.DefGW[1],
										  LocalM.DefGW[2],
										  LocalM.DefGW[3]);	
										  
	j = strlen(buf);
	p = &buf[j];
	sprintf(p, "��ѡDNS������: %d.%d.%d.%d\n", LocalM.PriDNS[0],
											   LocalM.PriDNS[1],
											   LocalM.PriDNS[2],
											   LocalM.PriDNS[3]);

	j = strlen(buf);
	p = &buf[j];
	sprintf(p, "��ѡDNS������: %d.%d.%d.%d\n", LocalM.SecDNS[0],
											   LocalM.SecDNS[1],
											   LocalM.SecDNS[2],
											   LocalM.SecDNS[3]);	
	
	TEXT_SetText(hItem, buf);
}

/*
*********************************************************************************************************
*	�� �� ��: InitDialogNet
*	����˵��: �Ի���ص������ĳ�ʼ����Ϣ
*	��    ��: pMsg  ��Ϣָ�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void InitDialogNet(WM_MESSAGE * pMsg)
{
    WM_HWIN hWin = pMsg->hWin;
	WM_HWIN hItem;
	

	/* ���ÿ�������� */
	GUI_AA_SetFactor(MAG);

	hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_5);
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonBack);
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_6);
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonList);
	
	hItem = WM_GetDialogItem(hWin, ID_TEXT_0);
	TEXT_SetTextColor(hItem, GUI_BLACK);
	TEXT_SetFont(hItem, &GUI_FontHZ16);
    TEXT_SetText(hItem, "����ͨ��: �Ͽ�\n"
						"DHCP״̬: ʧ��\n"
						"NetBIOS���Ʒ�����: Enable\n"
						"IP��ַ: 192.168.1.200\n"
	                    "��������: 255.255.255.0\n"
						"Ĭ������: 192.168.1.1\n"
						"��ѡDNS������: 233.5.5.5\n"
						"��ѡDNS������: 233.5.5.6\n");
	
	hItem = WM_GetDialogItem(hWin, ID_TEXT_1);
	TEXT_SetTextColor(hItem, GUI_BLACK);
	TEXT_SetFont(hItem, &GUI_FontHZ16);
    TEXT_SetText(hItem, "������ʱ��: 15:04:59\n"
						"����������: 2017/02/22\n"
	                    "SNTP�ͻ��˻�ȡ״̬: ʧ��\n"
						"ͨ������Զ��SNTP������IP: 182.16.3.162 ��ȡʱ������� \n");
						
	
	hItem = WM_GetDialogItem(hWin, ID_TEXT_2);
	TEXT_SetTextColor(hItem, GUI_BLACK);
	TEXT_SetFont(hItem, &GUI_FontHZ16);
    TEXT_SetText(hItem, "Web�������û���: admin ����: 123456\n"
						"�û����������ֱ������IP��ַ����Web��������IP��ַ������������Ϣ�����и�����\n"
	                    "Ҳ����ͨ��NetBIOS����������ֱ�ӷ��ʣ���http://stm32-v6/������������鿴����IP�ˡ�\n");
		
	/* ��ʼ���ı� */
	hItem = WM_GetDialogItem(hWin, ID_TEXT_3);
	TEXT_SetTextColor(hItem, GUI_BLACK);
	TEXT_SetFont(hItem, &GUI_FontHZ16);
    TEXT_SetText(hItem, "[1]��ֲ��RL-TCPnet����Э��ջ�����ڶ�ӦDM9161����ؽ������ӵ���������·�ɻ򽻻����ϣ������ \n"
	                    "   �๦���޷��������У����߿��������Σ�֧���Զ�������������Ϣ��SNTP�ͻ��˶���ÿ�����һ�Ρ�\n"
						"[2]DHCP�Ѿ�ʹ�ܣ������������8�����޷���·����/���������IP��ַ����ʹ�ù̶�IP:192.168.1.200��\n"
						"[3]��������ȶ���SNTP�����������ˣ���ǰʹ�õ����ʱ��ʱ���������Ҳ��Բ��ɹ��Ļ�Ҳ�������ġ�\n");

	WM_CreateTimer(hWin, /* ������Ϣ�Ĵ��ڵľ�� */
				   0, 	 /* �û������Id���������ͬһ����ʹ�ö����ʱ������ֵ��������Ϊ�㡣 */
				   0,    /* ���ڣ������ڹ���ָ������Ӧ�յ���Ϣ*/
				   0);	 /* ��������ʹ�ã�ӦΪ0 */
}

/*
*********************************************************************************************************
*	�� �� ��: _cbCallbackNet
*	����˵��: �ص�����
*	��    ��: pMsg   ��Ϣָ�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _cbCallbackNet(WM_MESSAGE * pMsg) 
{
    int NCode, Id;
	
	
    switch (pMsg->MsgId) 
    {
        case WM_INIT_DIALOG:
            InitDialogNet(pMsg);
            break;
		
		case WM_TIMER:
			/* ��ʾʱ������� */
			UpdateDisp(pMsg);
		
			/* ������ʱ�� */
			WM_RestartTimer(pMsg->Data.v, 1000);
			break;
		
		case WM_PAINT:
			GUI_SetColor(GUI_RED);
			GUI_AA_DrawRoundedRect(70, 40, 660+70, 150+40, 6);
			GUI_SetFont(&GUI_FontHZ16);
			GUI_DispStringAt("������Ϣ", 90, 32);
		
			GUI_AA_DrawRoundedRect(70, 210, 660+70, 90+210, 6);
			GUI_SetFont(&GUI_FontHZ16);
			GUI_DispStringAt("SNTP�ͻ���", 90, 202);
		
			GUI_AA_DrawRoundedRect(70, 320, 660+70, 70+320, 6);
			GUI_SetFont(&GUI_FontHZ16);
			GUI_DispStringAt("Ƕ��ʽWeb������", 90, 312);
			break;
			
        case WM_NOTIFY_PARENT:
            Id = WM_GetId(pMsg->hWinSrc); 
            NCode = pMsg->Data.v;        
            switch (Id) 
            {
				/* �ر�TCPnet���� */
				case ID_BUTTON_5:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_RELEASED:
						    hWinNET = WM_HWIN_NULL;
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
*	�� �� ��: App_TCPnet
*	����˵��: ��������Ի���
*	��    ��: hWin  ������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void App_TCPnet(WM_HWIN hWin) 
{				 
    hWinNET = GUI_CreateDialogBox(_aDialogCreateNet,
								  GUI_COUNTOF(_aDialogCreateNet), 
								  &_cbCallbackNet, 
								  hWin, 
								  0, 
								  0);
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

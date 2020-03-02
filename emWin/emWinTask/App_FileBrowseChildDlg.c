/*
*********************************************************************************************************
*	                                  
*	ģ������ : TXT��ʽ�ı��Ķ�����
*	�ļ����� : App_FileBrowseChildDlg.c
*	��    �� : V1.0
*	˵    �� : ����Ի������ı��Ķ��Ի�����Ӵ��ڣ�������ʾ�ı��б�
*              1. ֧�����������ı�����50����
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
#define ID_BUTTON_0 (GUI_ID_USER + 0x01)
#define ID_BUTTON_1 (GUI_ID_USER + 0x02)


/*
*********************************************************************************************************
*				                      ����
*********************************************************************************************************
*/
WM_HWIN hWinTextList;                  /* �ı��б�Ի����� */
extern const char s_TextPathDir[];     /* �ı��ڴ洢���еĴ��·�� */
extern const uint16_t usTextMaxNum;
extern uint32_t TXT_Size[];            /* ��¼ÿ���ļ��Ĵ�С */

/*
*********************************************************************************************************
*				                    ����Ի����ʼ��ѡ��
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateText[] = 
{
    { FRAMEWIN_CreateIndirect,    "�ı��б�",        0,             120,   0,  559,  329,  0},
    { LISTVIEW_CreateIndirect,     NULL,          GUI_ID_LISTVIEW0,   0,   0,  549,  240,  0, 0},
    { BUTTON_CreateIndirect,       "ȷ��",        ID_BUTTON_0,      60, 245,   90,   40,  0, 0, 0},
    { BUTTON_CreateIndirect,       "ȡ��",        ID_BUTTON_1,     380, 245,   90,   40,  0, 0, 0}
};

/*
*********************************************************************************************************
*	�� �� ��: InitDialogText
*	����˵��: ��ʼ������ 
*	��    ��: pMsg   ��Ϣָ�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void InitDialogText(WM_MESSAGE * pMsg)
{
	WM_HWIN hWin = pMsg->hWin;
	WM_HWIN hItem;
	HEADER_Handle hHeader;
	FINFO info;
	char buf[100];
	char searchbuf[100];	
	unsigned char i = 0;
	SCROLLBAR_Handle hScrollbar;
	

    /* ���ÿ�ܴ��ڵ�Title */
    FRAMEWIN_SetTextColor(hWin, 0x0000ff);
    FRAMEWIN_SetFont(hWin, &GUI_FontHZ16);
    FRAMEWIN_SetTextAlign(hWin, GUI_TA_VCENTER|GUI_TA_CENTER);
    FRAMEWIN_SetTitleHeight(hWin, 30);

    /* ����listview�ؼ��ϵ�header */
    hHeader = LISTVIEW_GetHeader(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0));
    HEADER_SetFont(hHeader, &GUI_FontHZ16);
    HEADER_SetHeight(hHeader, 25);

    /* ����button�ؼ� */
    BUTTON_SetFont(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0), &GUI_FontHZ16);
    BUTTON_SetFont(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1), &GUI_FontHZ16);

    /* ����listview�ؼ��ϵ�SCROLLBAR */
	hItem = WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0);
    hScrollbar = SCROLLBAR_CreateAttached(hItem, SCROLLBAR_CF_VERTICAL);
    SCROLLBAR_SetWidth(hScrollbar, 18);

    /* ����LISTVIEW�ؼ� */
    LISTVIEW_SetFont(hItem, &GUI_FontHZ16);
    LISTVIEW_SetRowHeight(hItem, 24);
    LISTVIEW_AddColumn(hItem, 60, "�ı������ļ�����֧��ASCII�ַ���", GUI_TA_VCENTER|GUI_TA_LEFT);
    LISTVIEW_AddColumn(hItem, 60, "�ı���С", GUI_TA_VCENTER|GUI_TA_LEFT);
    LISTVIEW_SetColumnWidth(hItem, 0, 310);
    LISTVIEW_SetColumnWidth(hItem, 1, 230);
	
	/* 
	   ����Ŀ¼�µ������ļ��г�����
	   1. "*" ���� "*.*" ����ָ��·���µ������ļ�
	   2. "abc*"         ����ָ��·������abc��ͷ�������ļ�
	   3. "*.jpg"        ����ָ��·������.jpg��β�������ļ�
	   4. "abc*.jpg"     ����ָ��·������abc��ͷ��.jpg��β�������ļ�
	
	   ������ʵ��������Ŀ¼�������ļ�
	*/
	info.fileID = 0;   /* ÿ��ʹ��ffind����ǰ��info.fileID�����ʼ��Ϊ0 */
	
	/* �����ı���ӵ�listview�ؼ��� */
	strcpy((char *)searchbuf, s_TextPathDir);
	strcat((char *)searchbuf, "*.txt");
	while(ffind ((const char *)searchbuf, &info) == 0)  
	{ 
		LISTVIEW_AddRow(hItem, NULL);
		LISTVIEW_SetItemText(hItem, 0, i, (const char *)info.name);
		
		TXT_Size[i] = info.size;
		if(info.size < 1024)
		{
			sprintf((char *)buf, "%dB", info.size);
		}
		else if(info.size < 1024*1024)
		{
			sprintf((char *)buf, "%.2fKB", (float)info.size/1024);
		}
		else
		{
			sprintf((char *)buf, "%.2fMB", (float)info.size/1024/1024);
		}
		LISTVIEW_SetItemText(hItem, 1, i++, (const char *)buf);	
		
		/* ���ÿɴ洢���ļ������Ϊ50�� */
		if(i == usTextMaxNum) break;
    }		  
}

/*
*********************************************************************************************************
*	�� �� ��: _cbCallbackText
*	����˵��: �ı��б�Ի���ص����� 
*	��    ��: pMsg   ��Ϣָ�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _cbCallbackText(WM_MESSAGE * pMsg) 
{
    int NCode, Id;
    WM_HWIN hWin = pMsg->hWin;
	
    switch (pMsg->MsgId) 
    {
		/* ��ʼ����Ϣ */
        case WM_INIT_DIALOG:
            InitDialogText(pMsg);
            break;
		
		/* ֪ͨ��Ϣ */
        case WM_NOTIFY_PARENT:
            Id = WM_GetId(pMsg->hWinSrc); 
            NCode = pMsg->Data.v;        
            switch (Id) 
            {
				/* ���ȷ����ť�������Ӧѡ����ı� */
				case ID_BUTTON_0:
					switch(NCode) 
					{
						case WM_NOTIFICATION_RELEASED:
						    WM_SendMessageNoPara(hWinText, MSG_TextStart);
						    /* ȡ��ģ̬ */
						    WM_MakeModal(0);
						    WM_HideWindow(hWin);
							break;
					}
					break;
				
				/* ���ȡ����ť��ȡ���˳� */					
				case ID_BUTTON_1:
					switch(NCode) 
					{
						case WM_NOTIFICATION_RELEASED:
						    WM_MakeModal(0);
						    WM_HideWindow(hWin);
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
*	�� �� ��: App_FileBrowseChild
*	����˵��: �ı��Ի�����Ӵ��ڣ�������ʾ�ı��б�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void App_FileBrowseChild(WM_HWIN hWin) 
{
	/* ���û�д���hWinMusicList�����д��� */
	if(!WM_IsWindow(hWinTextList))
	{
		hWinTextList = GUI_CreateDialogBox(_aDialogCreateText, 
		                                    GUI_COUNTOF(_aDialogCreateText), 
		                                    _cbCallbackText, 
		                                    hWin, 
		                                    0, 
		                                    0);
		WM_SetStayOnTop(hWinTextList, 1);
		/* ����Ϊģ̬���� */
		WM_MakeModal(hWinTextList);
	}
	else
	{
		/* ����Ϊģ̬���� */
		WM_MakeModal(hWinTextList);
		WM_ShowWindow(hWinTextList);
	}
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

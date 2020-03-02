/*
*********************************************************************************************************
*	                                  
*	ģ������ : ���ֲ����������б����
*	�ļ����� : App_MusicChildListDlg.c
*	��    �� : V1.0
*	˵    �� : ����Ի��������ֶԻ�����Ӵ��ڣ�������ʾ�����б�
*              1. ֧����һ������һ��������Ϳ��ˣ����õĲ����ʺ����ʶ�֧�֣���������������Ҳ��֧�֡�
*              2. emWin�����ǵ����ȼ��������ֽ��������Ǹ����ȼ�������������֮��ͨ����Ϣ������¼�
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
*				                      �궨��
*********************************************************************************************************
*/
#define ID_BUTTON_0 (GUI_ID_USER + 0x00)
#define ID_BUTTON_1 (GUI_ID_USER + 0x01)


/*
*********************************************************************************************************
*				                      ����
*********************************************************************************************************
*/
WM_HWIN hWinMusicList = WM_HWIN_NULL;             /* �����б�Ի����� */
extern const char s_MusicPathDir[];               /* �����ڴ洢���еĴ��·�� */

/*
*********************************************************************************************************
*				                    ����Ի����ʼ��ѡ��
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateMusicList[] = 
{
    { FRAMEWIN_CreateIndirect,    "�����б�",        0,             120,   10,  559,  329,  0},
    { LISTVIEW_CreateIndirect,     NULL,          GUI_ID_LISTVIEW0,   0,   0,  549,  240,  0, 0},
    { BUTTON_CreateIndirect,       "ȷ��",        ID_BUTTON_0,      60, 245,   90,   40,  0, 0, 0},
    { BUTTON_CreateIndirect,       "ȡ��",        ID_BUTTON_1,     380, 245,   90,   40,  0, 0, 0}
};

/*
*********************************************************************************************************
*	�� �� ��: InitDialogMusicList
*	����˵��: ��ʼ������ 
*	��    �Σ�pMsg
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void InitDialogMusicList(WM_MESSAGE * pMsg)
{
	WM_HWIN hWin = pMsg->hWin;
	WM_HWIN hItem;
	HEADER_Handle hHeader;
	FINFO info;
	char buf[100];
	char searchbuf[100];	
	unsigned char i = 0;
	SCROLLBAR_Handle hScrollbar;
	uint32_t uiTotalTime;  

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
    hScrollbar = SCROLLBAR_CreateAttached(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0), SCROLLBAR_CF_VERTICAL);
    SCROLLBAR_SetWidth(hScrollbar, 18);

    /* ����LISTVIEW�ؼ� */
	hItem = WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0);
    LISTVIEW_SetFont(hItem, &GUI_FontHZ16);
    LISTVIEW_SetRowHeight(hItem, 24);
    LISTVIEW_AddColumn(hItem, 60, "���������ļ�����֧��ASCII�ַ���", GUI_TA_VCENTER|GUI_TA_LEFT);//���һ������
    LISTVIEW_AddColumn(hItem, 60, "����ʱ��", GUI_TA_VCENTER|GUI_TA_LEFT);//���һ������
    LISTVIEW_AddColumn(hItem, 60, "�ļ���С", GUI_TA_VCENTER|GUI_TA_LEFT);//���һ������
    LISTVIEW_SetColumnWidth(hItem, 0, 330);//�����п�
    LISTVIEW_SetColumnWidth(hItem, 1, 105);//�����п�
    LISTVIEW_SetColumnWidth(hItem, 2, 105);//�����п�
	
	/* 
	   ����Ŀ¼�µ������ļ��г�����
	   1. "*" ���� "*.*" ����ָ��·���µ������ļ�
	   2. "abc*"         ����ָ��·������abc��ͷ�������ļ�
	   3. "*.jpg"        ����ָ��·������.jpg��β�������ļ�
	   4. "abc*.jpg"     ����ָ��·������abc��ͷ��.jpg��β�������ļ�
	
	   ������ʵ��������Ŀ¼�������ļ�
	*/
	info.fileID = 0;   /* ÿ��ʹ��ffind����ǰ��info.fileID�����ʼ��Ϊ0 */
	
	/* ����MP3������ӵ�listview�ؼ��� */
	strcpy((char *)searchbuf, s_MusicPathDir);
	strcat((char *)searchbuf, "*.mp3");
	while(ffind ((const char *)searchbuf, &info) == 0)  
	{ 
		LISTVIEW_AddRow(hItem, NULL);
		LISTVIEW_SetItemText(hItem, 0, i, (const char *)info.name);
		strcpy((char *)buf, s_MusicPathDir);
		strcat((char *)buf, (char *)info.name);
		
		GetMP3Runtime((uint8_t *)buf, &uiTotalTime);
		sprintf((char *)buf,"%02d:%02d", uiTotalTime/600, uiTotalTime/10%60);
		LISTVIEW_SetItemText(hItem, 1, i, (const char *)buf);
		
		sprintf((char *)buf, "%.2fMB", (float)info.size/1024/1024);
		LISTVIEW_SetItemText(hItem, 2, i++, (const char *)buf);	
    }		  
	
	
	info.fileID = 0;   /* ÿ��ʹ��ffind����ǰ��info.fileID�����ʼ��Ϊ0 */

	/* ����WAV������ӵ�listview�ؼ��� */	
	strcpy(searchbuf, s_MusicPathDir);
	strcat(searchbuf, "*.wav");
	while(ffind (searchbuf, &info) == 0)  
	{ 
		LISTVIEW_AddRow(hItem, NULL);
		LISTVIEW_SetItemText(hItem, 0, i, (const char *)info.name);
		strcpy(buf, s_MusicPathDir);
		strcat(buf, (char *)info.name);

		GetWavRuntime((uint8_t *)buf, &uiTotalTime);
		sprintf(buf,"%02d:%02d", uiTotalTime/600, uiTotalTime/10%60);
		LISTVIEW_SetItemText(hItem, 1, i, (const char *)buf);
		
		sprintf(buf, "%.2fMB", (float)info.size/1024/1024);
		LISTVIEW_SetItemText(hItem, 2, i++, (const char *)buf);
    }
}

/*
*********************************************************************************************************
*	�� �� ��: _cbCallbackMusicList
*	����˵��: �����б�Ի���ص����� 
*	��    �Σ�pMsg
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _cbCallbackMusicList(WM_MESSAGE * pMsg) 
{
    int NCode, Id;
    WM_HWIN hWin = pMsg->hWin;
	
    switch (pMsg->MsgId) 
    {
		/* ��ʼ����Ϣ */
        case WM_INIT_DIALOG:
            InitDialogMusicList(pMsg);
            break;
		
		/* ֪ͨ��Ϣ */
        case WM_NOTIFY_PARENT:
            Id = WM_GetId(pMsg->hWinSrc); 
            NCode = pMsg->Data.v;        
            switch (Id) 
            {
				/* ���OK��ť����ѡ��ĸ��� */
				case ID_BUTTON_0:
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						
						case WM_NOTIFICATION_RELEASED:
						    WM_SendMessageNoPara(hWinMusic, MSG_MusicStart);
						    /* ȡ��ģ̬ */
						    WM_MakeModal(0);
						    WM_HideWindow(hWin);
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
*	�� �� ��: App_MusicList
*	����˵��: ���ֲ��ŶԻ�����Ӵ��ڣ�������ʾ�����б�
*	��    ��: hWin  ������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void App_MusicList(WM_HWIN hWin) 
{
	/* ���û�д���hWinMusicList�����д��� */
	if(!WM_IsWindow(hWinMusicList))
	{
		hWinMusicList = GUI_CreateDialogBox(_aDialogCreateMusicList, 
		                                    GUI_COUNTOF(_aDialogCreateMusicList), 
		                                    _cbCallbackMusicList, 
		                                    hWin, 
		                                    0, 
		                                    0);
		/* ����Ϊģ̬���� */
		WM_MakeModal(hWinMusicList);
	}
	else
	{
		/* ����Ϊģ̬���� */
		WM_MakeModal(hWinMusicList);
		WM_ShowWindow(hWinMusicList);
	}
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

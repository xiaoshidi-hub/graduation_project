/*
*********************************************************************************************************
*	                                  
*	ģ������ : ��Ƶ�б�
*	�ļ����� : App_VideoListDlg.c
*	��    �� : V1.0
*	˵    �� : ��Ƶ�б������ơ�
*              1. ֧����һ������һ��������Ϳ��ˡ�
*              2. emWin�����ǵ����ȼ�������Ƶ���������Ǹ����ȼ�������������֮��ͨ����Ϣ������¼�
*                 ��־����ͨ�š�
*              3. �״�ʹ���ȵ����Ƶ�б���Ƶ���ᱻ��¼��listview�ؼ����棬Ȼ��Ϳ�����������ˡ�
*                 ����ļ�������Ƶ�϶࣬�״δ򿪻�����Щ����Ҫ����ΪҪ��ȡÿ����Ƶ�Ĳ���ʱ�䡣�Ժ��
*                 �ͱȽϿ��ˣ���Ҫ�Ƕ���Ƶ�б�Ի����������غ���ʾ�����������ظ��Ĵ�����ɾ����
*              4. ��Ƶ�б�Ի�������ģ̬���������û��򿪴˶Ի����ֻ�ܲ�������Ի��򣬶����ܲ��������档
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
	#define printf_videolistdbg printf
#else
	#define printf_videolistdbg(...)
#endif


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
WM_HWIN hWinVideoList = WM_HWIN_NULL;             /* ��Ƶ�б�Ի����� */
extern const char s_VideoPathDir[];               /* ��Ƶ�ڴ洢���еĴ��·�� */


/*
*********************************************************************************************************
*				                    ����Ի����ʼ��ѡ��
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateVideoList[] = 
{
    { FRAMEWIN_CreateIndirect,    "��Ƶ�б�",        0,             120,   0,  559,  329,  0},
    { LISTVIEW_CreateIndirect,     NULL,          GUI_ID_LISTVIEW0,   0,   0,  549,  240,  0, 0},
    { BUTTON_CreateIndirect,       "ȷ��",        ID_BUTTON_0,      60, 245,   90,   40,  0, 0, 0},
    { BUTTON_CreateIndirect,       "ȡ��",        ID_BUTTON_1,     380, 245,   90,   40,  0, 0, 0}
};

/*
*********************************************************************************************************
*	�� �� ��: InitDialogVideoList
*	����˵��: ��ʼ������ 
*	��    ��: pMsg
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void InitDialogVideoList(WM_MESSAGE * pMsg)
{
	WM_HWIN hWin = pMsg->hWin;
	WM_HWIN hItem;
	HEADER_Handle hHeader;
	FINFO info;
	char buf[100];
	char searchbuf[100];	
	int i = 0;
	SCROLLBAR_Handle hScrollbar;
	
	uint32_t uiTotalTime;

    /* ���ÿ�ܴ��ڵ�Title */
    FRAMEWIN_SetTextColor(hWin,0x0000ff);
    FRAMEWIN_SetFont(hWin,&GUI_FontHZ16);
    FRAMEWIN_SetTextAlign(hWin,GUI_TA_VCENTER|GUI_TA_CENTER);
    FRAMEWIN_SetTitleHeight(hWin,30);

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
    LISTVIEW_AddColumn(hItem, 60, "��Ƶ�����ļ�����֧��ASCII�ַ���", GUI_TA_VCENTER|GUI_TA_LEFT);
    LISTVIEW_AddColumn(hItem, 60, "����ʱ��", GUI_TA_VCENTER|GUI_TA_LEFT);
    LISTVIEW_AddColumn(hItem, 60, "�ļ���С", GUI_TA_VCENTER|GUI_TA_LEFT);
    LISTVIEW_SetColumnWidth(hItem, 0, 310);
    LISTVIEW_SetColumnWidth(hItem, 1, 115);
    LISTVIEW_SetColumnWidth(hItem, 2, 115);
	
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
	strcpy((char *)searchbuf, s_VideoPathDir);
	strcat((char *)searchbuf, "*.emf");
	while(ffind ((const char *)searchbuf, &info) == 0)  
	{ 
		LISTVIEW_AddRow(hItem, NULL);
		LISTVIEW_SetItemText(hItem, 0, i, (const char *)info.name);

		/* ����emf��׺Ϊmp3 */
		#if 1
			strcpy((char *)buf, s_VideoPathDir);
			strncat((char *)buf, (char *)info.name, strlen((char *)info.name)-3);
			strcat((char *)buf, "mp3");
		#else
			buf[strlen(buf) - 3] = 'm';
			buf[strlen(buf) - 2] = 'p';
			buf[strlen(buf) - 1] = '3';
			buf[strlen(buf) - 0] = '\0';
		#endif

		printf_videolistdbg("videolist = %s, %d\r\n", buf, strlen(buf));

		GetMP3Runtime((uint8_t *)buf, &uiTotalTime);
		sprintf(buf,"%02d:%02d:%02d", uiTotalTime/36000, uiTotalTime%36000/600, uiTotalTime/10%60);
		LISTVIEW_SetItemText(hItem, 1, i, (const char *)buf);
		
		sprintf((char *)buf, "%.2fMB", (float)info.size/1024/1024);
		LISTVIEW_SetItemText(hItem, 2, i++, (const char *)buf);	
    }		  
}

/*
*********************************************************************************************************
*	�� �� ��: _cbCallbackVideoList
*	����˵��: ��Ƶ�б�Ի���ص����� 
*	��    �Σ�pMsg
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _cbCallbackVideoList(WM_MESSAGE * pMsg) 
{
    int NCode, Id;
    WM_HWIN hWin = pMsg->hWin;
	
    switch (pMsg->MsgId) 
    {
		/* ��ʼ����Ϣ */
        case WM_INIT_DIALOG:
            InitDialogVideoList(pMsg);
            break;
		
		/* ֪ͨ��Ϣ */
        case WM_NOTIFY_PARENT:
            Id = WM_GetId(pMsg->hWinSrc); 
            NCode = pMsg->Data.v;        
            switch (Id) 
            {
				/* ���OK��ť����ѡ�����Ƶ */
				case ID_BUTTON_0:
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						
						case WM_NOTIFICATION_RELEASED:
						    WM_SendMessageNoPara(hWinVideo, MSG_MusicStart);
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
							WM_SendMessageNoPara(hWinVideo, MSG_MusicCancel);
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
*	�� �� ��: App_VideoList
*	����˵��: ��Ƶ���ŶԻ�����Ӵ��ڣ�������ʾ��Ƶ�б�
*	��    ��: hWin ������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void App_VideoList(WM_HWIN hWin) 
{
	/* ���û�д���hWinVideoList�����д��� */
	if(!WM_IsWindow(hWinVideoList))
	{
		hWinVideoList = GUI_CreateDialogBox(_aDialogCreateVideoList, 
		                                    GUI_COUNTOF(_aDialogCreateVideoList), 
		                                    _cbCallbackVideoList, 
		                                    hWinVideo, 
		                                    0, 
		                                    0);
		/* ����Ϊģ̬���� */
		WM_MakeModal(hWinVideoList);
	}
	else
	{
		/* ����Ϊģ̬���� */
		WM_MakeModal(hWinVideoList);
		WM_ShowWindow(hWinVideoList);
	}
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

/*
*********************************************************************************************************
*	                                  
*	模块名称 : TXT格式文本阅读界面
*	文件名称 : App_FileBrowseChildDlg.c
*	版    本 : V1.0
*	说    明 : 这个对话框是文本阅读对话框的子窗口，用于显示文本列表
*              1. 支持浏览的最大文本数是50个。
*
*	修改记录 :
*		版本号    日期         作者         说明
*       V1.0    2019-12      果果小师弟     首发
*
*	Copyright (C), 2020-2025, https://space.bilibili.com/357524621 
*                                     
*********************************************************************************************************
*/

#include "includes.h"
#include "MainTask.h"


/*
*********************************************************************************************************
*				                      宏定义
*********************************************************************************************************
*/
#define ID_BUTTON_0 (GUI_ID_USER + 0x01)
#define ID_BUTTON_1 (GUI_ID_USER + 0x02)


/*
*********************************************************************************************************
*				                      变量
*********************************************************************************************************
*/
WM_HWIN hWinTextList;                  /* 文本列表对话框句柄 */
extern const char s_TextPathDir[];     /* 文本在存储器中的存放路径 */
extern const uint16_t usTextMaxNum;
extern uint32_t TXT_Size[];            /* 记录每个文件的大小 */

/*
*********************************************************************************************************
*				                    任务对话框初始化选项
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateText[] = 
{
    { FRAMEWIN_CreateIndirect,    "文本列表",        0,             120,   0,  559,  329,  0},
    { LISTVIEW_CreateIndirect,     NULL,          GUI_ID_LISTVIEW0,   0,   0,  549,  240,  0, 0},
    { BUTTON_CreateIndirect,       "确定",        ID_BUTTON_0,      60, 245,   90,   40,  0, 0, 0},
    { BUTTON_CreateIndirect,       "取消",        ID_BUTTON_1,     380, 245,   90,   40,  0, 0, 0}
};

/*
*********************************************************************************************************
*	函 数 名: InitDialogText
*	功能说明: 初始化函数 
*	形    参: pMsg   消息指针变量
*	返 回 值: 无
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
	

    /* 设置框架窗口的Title */
    FRAMEWIN_SetTextColor(hWin, 0x0000ff);
    FRAMEWIN_SetFont(hWin, &GUI_FontHZ16);
    FRAMEWIN_SetTextAlign(hWin, GUI_TA_VCENTER|GUI_TA_CENTER);
    FRAMEWIN_SetTitleHeight(hWin, 30);

    /* 设置listview控件上的header */
    hHeader = LISTVIEW_GetHeader(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0));
    HEADER_SetFont(hHeader, &GUI_FontHZ16);
    HEADER_SetHeight(hHeader, 25);

    /* 设置button控件 */
    BUTTON_SetFont(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0), &GUI_FontHZ16);
    BUTTON_SetFont(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1), &GUI_FontHZ16);

    /* 设置listview控件上的SCROLLBAR */
	hItem = WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0);
    hScrollbar = SCROLLBAR_CreateAttached(hItem, SCROLLBAR_CF_VERTICAL);
    SCROLLBAR_SetWidth(hScrollbar, 18);

    /* 设置LISTVIEW控件 */
    LISTVIEW_SetFont(hItem, &GUI_FontHZ16);
    LISTVIEW_SetRowHeight(hItem, 24);
    LISTVIEW_AddColumn(hItem, 60, "文本名（文件名仅支持ASCII字符）", GUI_TA_VCENTER|GUI_TA_LEFT);
    LISTVIEW_AddColumn(hItem, 60, "文本大小", GUI_TA_VCENTER|GUI_TA_LEFT);
    LISTVIEW_SetColumnWidth(hItem, 0, 310);
    LISTVIEW_SetColumnWidth(hItem, 1, 230);
	
	/* 
	   将根目录下的所有文件列出来。
	   1. "*" 或者 "*.*" 搜索指定路径下的所有文件
	   2. "abc*"         搜索指定路径下以abc开头的所有文件
	   3. "*.jpg"        搜索指定路径下以.jpg结尾的所有文件
	   4. "abc*.jpg"     搜索指定路径下以abc开头和.jpg结尾的所有文件
	
	   以下是实现搜索根目录下所有文件
	*/
	info.fileID = 0;   /* 每次使用ffind函数前，info.fileID必须初始化为0 */
	
	/* 搜索文本添加到listview控件上 */
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
		
		/* 设置可存储的文件数最大为50个 */
		if(i == usTextMaxNum) break;
    }		  
}

/*
*********************************************************************************************************
*	函 数 名: _cbCallbackText
*	功能说明: 文本列表对话框回调函数 
*	形    参: pMsg   消息指针变量
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbCallbackText(WM_MESSAGE * pMsg) 
{
    int NCode, Id;
    WM_HWIN hWin = pMsg->hWin;
	
    switch (pMsg->MsgId) 
    {
		/* 初始化消息 */
        case WM_INIT_DIALOG:
            InitDialogText(pMsg);
            break;
		
		/* 通知消息 */
        case WM_NOTIFY_PARENT:
            Id = WM_GetId(pMsg->hWinSrc); 
            NCode = pMsg->Data.v;        
            switch (Id) 
            {
				/* 点击确定按钮，浏览相应选择的文本 */
				case ID_BUTTON_0:
					switch(NCode) 
					{
						case WM_NOTIFICATION_RELEASED:
						    WM_SendMessageNoPara(hWinText, MSG_TextStart);
						    /* 取消模态 */
						    WM_MakeModal(0);
						    WM_HideWindow(hWin);
							break;
					}
					break;
				
				/* 点击取消按钮，取消退出 */					
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
*	函 数 名: App_FileBrowseChild
*	功能说明: 文本对话框的子窗口，用于显示文本列表
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void App_FileBrowseChild(WM_HWIN hWin) 
{
	/* 如果没有创建hWinMusicList，进行创建 */
	if(!WM_IsWindow(hWinTextList))
	{
		hWinTextList = GUI_CreateDialogBox(_aDialogCreateText, 
		                                    GUI_COUNTOF(_aDialogCreateText), 
		                                    _cbCallbackText, 
		                                    hWin, 
		                                    0, 
		                                    0);
		WM_SetStayOnTop(hWinTextList, 1);
		/* 设置为模态窗口 */
		WM_MakeModal(hWinTextList);
	}
	else
	{
		/* 设置为模态窗口 */
		WM_MakeModal(hWinTextList);
		WM_ShowWindow(hWinTextList);
	}
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

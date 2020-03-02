/*
*********************************************************************************************************
*	                                  
*	模块名称 : 视频列表
*	文件名称 : App_VideoListDlg.c
*	版    本 : V1.0
*	说    明 : 视频列表界面设计。
*              1. 支持上一个，下一个，快进和快退。
*              2. emWin任务是低优先级任务，音频解码任务是高优先级任务，两个任务之间通过消息邮箱和事件
*                 标志进行通信。
*              3. 首次使用先点击视频列表，视频名会被记录到listview控件里面，然后就可以任意操作了。
*                 如果文件夹中视频较多，首次打开会稍慢些，主要是因为要获取每首视频的播放时间。以后打开
*                 就比较快了，主要是对视频列表对话框做了隐藏和显示处理，而不是重复的创建和删除。
*              4. 视频列表对话框做了模态处理，这样用户打开此对话框后只能操作这个对话框，而不能操作主界面。
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
*	                                  用于本文件的调试
*********************************************************************************************************
*/
#if 0
	#define printf_videolistdbg printf
#else
	#define printf_videolistdbg(...)
#endif


/*
*********************************************************************************************************
*				                      宏定义
*********************************************************************************************************
*/
#define ID_BUTTON_0 (GUI_ID_USER + 0x00)
#define ID_BUTTON_1 (GUI_ID_USER + 0x01)


/*
*********************************************************************************************************
*				                      变量
*********************************************************************************************************
*/
WM_HWIN hWinVideoList = WM_HWIN_NULL;             /* 视频列表对话框句柄 */
extern const char s_VideoPathDir[];               /* 视频在存储器中的存放路径 */


/*
*********************************************************************************************************
*				                    任务对话框初始化选项
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateVideoList[] = 
{
    { FRAMEWIN_CreateIndirect,    "视频列表",        0,             120,   0,  559,  329,  0},
    { LISTVIEW_CreateIndirect,     NULL,          GUI_ID_LISTVIEW0,   0,   0,  549,  240,  0, 0},
    { BUTTON_CreateIndirect,       "确定",        ID_BUTTON_0,      60, 245,   90,   40,  0, 0, 0},
    { BUTTON_CreateIndirect,       "取消",        ID_BUTTON_1,     380, 245,   90,   40,  0, 0, 0}
};

/*
*********************************************************************************************************
*	函 数 名: InitDialogVideoList
*	功能说明: 初始化函数 
*	形    参: pMsg
*	返 回 值: 无
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

    /* 设置框架窗口的Title */
    FRAMEWIN_SetTextColor(hWin,0x0000ff);
    FRAMEWIN_SetFont(hWin,&GUI_FontHZ16);
    FRAMEWIN_SetTextAlign(hWin,GUI_TA_VCENTER|GUI_TA_CENTER);
    FRAMEWIN_SetTitleHeight(hWin,30);

    /* 设置listview控件上的header */
    hHeader = LISTVIEW_GetHeader(WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0));
    HEADER_SetFont(hHeader, &GUI_FontHZ16);
    HEADER_SetHeight(hHeader, 25);

    /* 设置button控件 */
    BUTTON_SetFont(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0), &GUI_FontHZ16);
    BUTTON_SetFont(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1), &GUI_FontHZ16);

    /* 设置listview控件上的SCROLLBAR */
    hScrollbar = SCROLLBAR_CreateAttached(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0), SCROLLBAR_CF_VERTICAL);
    SCROLLBAR_SetWidth(hScrollbar, 18);

    /* 设置LISTVIEW控件 */
	hItem = WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0);
    LISTVIEW_SetFont(hItem, &GUI_FontHZ16);
    LISTVIEW_SetRowHeight(hItem, 24);
    LISTVIEW_AddColumn(hItem, 60, "视频名（文件名仅支持ASCII字符）", GUI_TA_VCENTER|GUI_TA_LEFT);
    LISTVIEW_AddColumn(hItem, 60, "播放时间", GUI_TA_VCENTER|GUI_TA_LEFT);
    LISTVIEW_AddColumn(hItem, 60, "文件大小", GUI_TA_VCENTER|GUI_TA_LEFT);
    LISTVIEW_SetColumnWidth(hItem, 0, 310);
    LISTVIEW_SetColumnWidth(hItem, 1, 115);
    LISTVIEW_SetColumnWidth(hItem, 2, 115);
	
	/* 
	   将根目录下的所有文件列出来。
	   1. "*" 或者 "*.*" 搜索指定路径下的所有文件
	   2. "abc*"         搜索指定路径下以abc开头的所有文件
	   3. "*.jpg"        搜索指定路径下以.jpg结尾的所有文件
	   4. "abc*.jpg"     搜索指定路径下以abc开头和.jpg结尾的所有文件
	
	   以下是实现搜索根目录下所有文件
	*/
	info.fileID = 0;   /* 每次使用ffind函数前，info.fileID必须初始化为0 */
	
	/* 搜索MP3歌曲添加到listview控件上 */
	strcpy((char *)searchbuf, s_VideoPathDir);
	strcat((char *)searchbuf, "*.emf");
	while(ffind ((const char *)searchbuf, &info) == 0)  
	{ 
		LISTVIEW_AddRow(hItem, NULL);
		LISTVIEW_SetItemText(hItem, 0, i, (const char *)info.name);

		/* 更改emf后缀为mp3 */
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
*	函 数 名: _cbCallbackVideoList
*	功能说明: 视频列表对话框回调函数 
*	形    参：pMsg
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbCallbackVideoList(WM_MESSAGE * pMsg) 
{
    int NCode, Id;
    WM_HWIN hWin = pMsg->hWin;
	
    switch (pMsg->MsgId) 
    {
		/* 初始化消息 */
        case WM_INIT_DIALOG:
            InitDialogVideoList(pMsg);
            break;
		
		/* 通知消息 */
        case WM_NOTIFY_PARENT:
            Id = WM_GetId(pMsg->hWinSrc); 
            NCode = pMsg->Data.v;        
            switch (Id) 
            {
				/* 点击OK按钮播放选择的视频 */
				case ID_BUTTON_0:
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						
						case WM_NOTIFICATION_RELEASED:
						    WM_SendMessageNoPara(hWinVideo, MSG_MusicStart);
						    /* 取消模态 */
						    WM_MakeModal(0);
						    WM_HideWindow(hWin);
							break;
					}
					break;
				
				/* 点击CANCEL按钮取消退出 */					
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
*	函 数 名: App_VideoList
*	功能说明: 视频播放对话框的子窗口，用于显示视频列表
*	形    参: hWin 父窗口
*	返 回 值: 无
*********************************************************************************************************
*/
void App_VideoList(WM_HWIN hWin) 
{
	/* 如果没有创建hWinVideoList，进行创建 */
	if(!WM_IsWindow(hWinVideoList))
	{
		hWinVideoList = GUI_CreateDialogBox(_aDialogCreateVideoList, 
		                                    GUI_COUNTOF(_aDialogCreateVideoList), 
		                                    _cbCallbackVideoList, 
		                                    hWinVideo, 
		                                    0, 
		                                    0);
		/* 设置为模态窗口 */
		WM_MakeModal(hWinVideoList);
	}
	else
	{
		/* 设置为模态窗口 */
		WM_MakeModal(hWinVideoList);
		WM_ShowWindow(hWinVideoList);
	}
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

/*
*********************************************************************************************************
*
*	模块名称 : 音乐播放器频谱设置界面
*	文件名称 : App_MusicChildSetDlg.c
*	版    本 : V1.0
*	说    明 : 这个对话框是音乐对话框的子窗口，用于频谱效果设置
*              1. 主要实现幅值谱和对数谱的切换，系统上电后默认是幅值谱。
*              2. 电容屏单击式操作Radio控件不好用，容易选不中，改成上下滑动式选择，这样操作比较好用。
*              3. 音乐频谱方面的知识在这个帖子里面进行了讲解：http://bbs.armfly.com/read.php?tid=25129。
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
#define ID_RADIO_0  (GUI_ID_USER + 0x00)
#define ID_BUTTON_0 (GUI_ID_USER + 0x01)
#define ID_BUTTON_1 (GUI_ID_USER + 0x02)
#define ID_TEXT_0   (GUI_ID_USER + 0x03)

/*
*********************************************************************************************************
*				                    频谱设置对话框初始化选项
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateMusicSet[] =
{
    { FRAMEWIN_CreateIndirect,  "频谱设置",         0,    120,   0,  559,  329,  0},
	{ RADIO_CreateIndirect,       "Radio", 	ID_RADIO_0,   10, 34, 300, 100, 0, 0x1f02, 0 },
    { BUTTON_CreateIndirect,      "确定",   ID_BUTTON_0,  60, 245,   90,   40,  0, 0, 0},
    { BUTTON_CreateIndirect,       "取消",  ID_BUTTON_1,  380, 245,   90,   40,  0, 0, 0},
	{ TEXT_CreateIndirect,         "Text",  ID_TEXT_0,    10, 150, 520, 80, 0, 0x64, 0 },
};

/*
*********************************************************************************************************
*	函 数 名: InitDialogMusicSet
*	功能说明: 初始化函数
*	形    参: pMsg
*	返 回 值: 无
*********************************************************************************************************
*/
static void InitDialogMusicSet(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;

    /* 设置框架窗口的Title */
    FRAMEWIN_SetTextColor(pMsg->hWin, 0x0000ff);
    FRAMEWIN_SetFont(pMsg->hWin, &GUI_FontHZ16);
    FRAMEWIN_SetTextAlign(pMsg->hWin, GUI_TA_VCENTER|GUI_TA_CENTER);
    FRAMEWIN_SetTitleHeight(pMsg->hWin, 30);
	
	/* 设置单选按钮 */
    hItem = WM_GetDialogItem(pMsg->hWin, ID_RADIO_0);
    RADIO_SetText(hItem, "Aplitude Spectrum", 0);
    RADIO_SetText(hItem, "Logarithmic Spectrum", 1);
    RADIO_SetFont(hItem, GUI_FONT_16B_1);
	RADIO_SetValue(hItem, g_tWav.ucSpectrum);
	
	/* 设置文本控件 */
	hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
    TEXT_SetFont(hItem, &GUI_FontHZ16);
    TEXT_SetText(hItem, "[1]Aplitude Spectrum 幅值谱\n"
						"[2]Logarithmic Spectrum 对数谱，将纵坐标的幅值取对数，使幅值较小\n"
						"   的信号被拉高，显示更佳，因为人的听觉响应与强度成对数关系。\n"
						"[3]电容屏操作Radio控件要上下滑动式的选择，否则不容易选中。");

    /* 设置button控件 */
    BUTTON_SetFont(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0), &GUI_FontHZ16);
    BUTTON_SetFont(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1), &GUI_FontHZ16);
}

/*
*********************************************************************************************************
*	函 数 名: _cbCallbackMusicSet
*	功能说明: 歌曲列表对话框回调函数
*	形    参：pMsg
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbCallbackMusicSet(WM_MESSAGE * pMsg)
{
    int NCode, Id;
    WM_HWIN hWin = pMsg->hWin;

    switch (pMsg->MsgId)
    {
		/* 初始化消息 */
		case WM_INIT_DIALOG:
			InitDialogMusicSet(pMsg);
			break;

		/* 通知消息 */
		case WM_NOTIFY_PARENT:
			Id = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch (Id)
			{
				/* 点击OK按钮设置频谱显示效果变量 */
				case ID_BUTTON_0:
					switch(NCode)
					{
						case WM_NOTIFICATION_CLICKED:
							break;

						case WM_NOTIFICATION_RELEASED:
							/* 返回0，幅值谱，返回1，对数谱 */
							g_tWav.ucSpectrum = RADIO_GetValue( WM_GetDialogItem(pMsg->hWin, ID_RADIO_0));
							GUI_EndDialog(hWin, 0);
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
							GUI_EndDialog(hWin, 0);
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
*	函 数 名: App_MusicSet
*	功能说明: 音乐播放对话框的子窗口，用于频谱效果设置
*	形    参: hWin  父窗口
*	返 回 值: 无
*********************************************************************************************************
*/
void App_MusicSet(WM_HWIN hWin) 
{
	WM_HWIN hWinMusicSet;
	
	/* 创建hWinMusicSet */
	hWinMusicSet = GUI_CreateDialogBox(_aDialogCreateMusicSet, 
	                                   GUI_COUNTOF(_aDialogCreateMusicSet), 
	                                   _cbCallbackMusicSet, 
	                                   hWin, 
	                                   0, 
	                                   0);
	
	/* 设置为模态窗口 */
	WM_MakeModal(hWinMusicSet);
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

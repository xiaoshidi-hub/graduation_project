/*
*********************************************************************************************************
*	                                  
*	模块名称 : 我的电脑界面设计
*	文件名称 : App_ComputerDlg.c
*	版    本 : V1.0
*	说    明 : 我的电脑界面设计。
*              1. 当前仅作了SD卡的支持。
*              2. 文件浏览是用的CHOOSEFILE对话框，此控件点开后会有一个网格调整的过程，
				  这个属正常现象，此控件就是这样的。
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
*                                       宏定义
*********************************************************************************************************
*/
#define MAG          3
#define ID_WINDOW_0 	(GUI_ID_USER + 0x00)
#define ID_TEXT_0 	    (GUI_ID_USER + 0x01)
#define ID_TEXT_1 	    (GUI_ID_USER + 0x02)
#define ID_TEXT_2 	    (GUI_ID_USER + 0x03)
#define ID_TEXT_3 	    (GUI_ID_USER + 0x04)
#define ID_TEXT_4 	    (GUI_ID_USER + 0x05)
#define ID_TEXT_5 	    (GUI_ID_USER + 0x06)
#define ID_PROGBAR_0    (GUI_ID_USER + 0x07)
#define ID_PROGBAR_1    (GUI_ID_USER + 0x08)
#define ID_PROGBAR_2    (GUI_ID_USER + 0x09)
#define ID_BUTTON_0     (GUI_ID_USER + 0xA0)
#define ID_BUTTON_1     (GUI_ID_USER + 0xB0)


/*
*********************************************************************************************************
*	函 数 名: _cbButtonBack
*	功能说明: 按钮回调函数
*	形    参: pMsg  消息指针
*	返 回 值: 无
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
*	函 数 名: _cbButtonList
*	功能说明: 按钮回调函数
*	形    参: pMsg  消息指针
*	返 回 值: 无
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
				
				GUI_DrawBitmap(&bmManualSearch, 0, 5);
			} 
			else 
			{
				GUI_SetColor(GUI_STCOLOR_LIGHTBLUE);
				GUI_AA_FillCircle(0, 0, 72);
				GUI_SetBkColor(GUI_STCOLOR_LIGHTBLUE);
				GUI_SetColor(GUI_WHITE);  

				GUI_DrawBitmap(&bmManualSearch, 0, 5);
			}
			break;
			
		default:
			BUTTON_Callback(pMsg);
	}
}

/*
*********************************************************************************************************
*	               对话框控件列表
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateComputer[] = {
	{ WINDOW_CreateIndirect,    "Window",   ID_WINDOW_0,   0,   0,  800, 480, 0, 0x0, 0 },
	{ TEXT_CreateIndirect,      "硬盘",     ID_TEXT_0,    3, 98, 400,24, 0,0},
    { TEXT_CreateIndirect,      "有可移动存储的设备",    ID_TEXT_1,  3,  234, 400,24, 0,0},
	
	{ PROGBAR_CreateIndirect,    NULL,      ID_PROGBAR_0,    40, 125+2, 180, 22, 0, 0},
	{ TEXT_CreateIndirect,      " ",  		ID_TEXT_2,       40, 150+2, 200, 24, 0, 0},
	
	{ PROGBAR_CreateIndirect,    NULL,      ID_PROGBAR_1,    268, 125+2, 180, 22, 0, 0},
	{ TEXT_CreateIndirect,      " ",  		ID_TEXT_3,       268, 150+2, 200, 24, 0, 0},
	
	{ PROGBAR_CreateIndirect,    NULL,      ID_PROGBAR_2,    40, 261+2, 180, 22, 0, 0},
	{ TEXT_CreateIndirect,      " ",  		ID_TEXT_4,       40, 286+2, 200, 24, 0, 0},

	{ BUTTON_CreateIndirect, "DlgBack",     ID_BUTTON_0,    700,   0,  100, 100, 0, 0, 0 },	
	{ BUTTON_CreateIndirect, "MusicList",   ID_BUTTON_1,      0,   0,  100, 100, 0, 0, 0 },
	
	{ TEXT_CreateIndirect,      " ",  	     ID_TEXT_5,       3, 440, 797, 40, 0, 0},
};

/*
*********************************************************************************************************
*	函 数 名: InitDialogComputer
*	功能说明: 对话框回调函数的初始化消息
*	形    参：pMsg   消息指针变量
*	返 回 值: 无
*********************************************************************************************************
*/
static void InitDialogComputer(WM_MESSAGE * pMsg)
{
    WM_HWIN hWin = pMsg->hWin;
	WM_HWIN hItem;
	char buf[50];

	hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonBack);
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonList);
	
	hItem = WM_GetDialogItem(hWin, ID_TEXT_0);	
    TEXT_SetFont(hItem, &GUI_SysFontHZ16);
	hItem = WM_GetDialogItem(hWin,ID_TEXT_1);
    TEXT_SetFont(hItem, &GUI_SysFontHZ16);
	
	/* 初始化SD卡部分 */
	hItem = WM_GetDialogItem(hWin, ID_PROGBAR_0);
	PROGBAR_SetFont(hItem, &GUI_Font13_1);
	PROGBAR_SetMinMax(hItem, 0, ullSdCapacity>>20);
    PROGBAR_SetValue(hItem, (ullSdCapacity - ullSdUnusedCapacity)>>20);

	hItem = WM_GetDialogItem(hWin, ID_TEXT_2);
	TEXT_SetTextColor(hItem, 0x646f71);
	TEXT_SetFont(hItem, &GUI_SysFontHZ16);
	sprintf(buf, "已用%lldMB,总共:%lldMB", (ullSdCapacity - ullSdUnusedCapacity)>>20, ullSdCapacity>>20);
	TEXT_SetText(hItem, buf);
	
	/* 初始化NAND部分 */
	hItem = WM_GetDialogItem(hWin, ID_PROGBAR_1);
	PROGBAR_SetFont(hItem,&GUI_Font13_1);
	PROGBAR_SetMinMax(hItem, 0, 100);
    PROGBAR_SetValue(hItem, 10);

	hItem = WM_GetDialogItem(hWin, ID_TEXT_3);
	TEXT_SetTextColor(hItem, 0x646f71);
	TEXT_SetFont(hItem, &GUI_SysFontHZ16);
	sprintf(buf, "NAND暂未做支持");
	TEXT_SetText(hItem, buf);
	
	/* 初始化U盘部分 */
	hItem = WM_GetDialogItem(hWin, ID_PROGBAR_2);
	PROGBAR_SetFont(hItem, &GUI_Font13_1);
	PROGBAR_SetMinMax(hItem, 0, 100);
    PROGBAR_SetValue(hItem, 20);

	hItem = WM_GetDialogItem(hWin, ID_TEXT_4);
	TEXT_SetTextColor(hItem, 0x646f71);
	TEXT_SetFont(hItem, &GUI_SysFontHZ16);
	sprintf(buf, "U盘暂未做支持");
	TEXT_SetText(hItem, buf);

	hItem = WM_GetDialogItem(hWin, ID_TEXT_5);
	TEXT_SetTextColor(hItem, GUI_BLACK);
	TEXT_SetFont(hItem, &GUI_SysFontHZ16);
    TEXT_SetText(hItem, "[1]只支持SD卡的读取\n"
						"[2]本设计是刘尧2020年毕业设计。请多指教\n");
	
	/* 设置抗锯齿因数 */
	GUI_AA_SetFactor(MAG);
}

/*
*********************************************************************************************************
*	函 数 名: _cbCallbackComputer
*	功能说明: 回调函数
*	形    参: pMsg   消息指针变量
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbCallbackComputer(WM_MESSAGE * pMsg) 
{
    int NCode, Id;
    WM_HWIN hWin = pMsg->hWin;
	
    switch (pMsg->MsgId) 
    {
		case WM_PRE_PAINT:
			GUI_MULTIBUF_Begin();
			break;
		
		case WM_POST_PAINT:
			GUI_MULTIBUF_End();
			break;	
		
		case WM_PAINT:
			GUI_SetBkColor(GUI_LIGHTCYAN);
			GUI_Clear();   //修改完桌面背景颜色后必须清清屏，否则修改不成功
			GUI_DrawGradientV(0,			   /* 左上角X 位置 */
							  0,			   /* 左上角Y 位置 */
							  799,	   	       /* 右下角X 位置 */
							  479,  	   	   /* 右下角Y 位置 */
							  GUI_WHITE,	   /* 矩形最左侧要绘制的颜色 */
							  GUI_LIGHTBLUE);  /* 矩形最右侧要绘制的颜色 */

			GUI_DrawGradientH(0,			   /* 左上角X 位置 */
							  117,			   /* 左上角Y 位置 */
							  450,	           /* 右下角X 位置 */
							  118,             /* 右下角Y 位置 */
							  GUI_RED,	       /* 矩形最左侧要绘制的颜色 */
							  GUI_YELLOW);     /* 矩形最右侧要绘制的颜色 */

			GUI_DrawGradientH(0,			   /* 左上角X 位置 */
							  253,			   /* 左上角Y 位置 */
							  450,	           /* 右下角X 位置 */
							  254,             /* 右下角Y 位置 */
							  GUI_RED,	       /* 矩形最左侧要绘制的颜色 */
							  GUI_YELLOW);     /* 矩形最右侧要绘制的颜色 */
							  
  			GUI_DrawBitmap(&bmfilesd,   3,  125+2);
			GUI_DrawBitmap(&bmfiledisk, 230, 125+2);
  			GUI_DrawBitmap(&bmfileusb,  3,  261+2);
            break;
		
        case WM_INIT_DIALOG:
            InitDialogComputer(pMsg);
            break;
			
        case WM_NOTIFY_PARENT:
            Id = WM_GetId(pMsg->hWinSrc); 
            NCode = pMsg->Data.v;        
            switch (Id) 
            {
				/* 关闭对话框 */
				case ID_BUTTON_0:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_RELEASED:
							GUI_EndDialog(hWin, 0);
                            break;
                    }
                    break;
				
				/* 打开文件列表对话框 */
				case ID_BUTTON_1:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_RELEASED:
							App_ComputerChild();
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
*	函 数 名: App_Computer
*	功能说明: 我的电脑对话框
*	形    参: hWin 所创建对话框父窗口 	
*	返 回 值: 无
*********************************************************************************************************
*/
void App_Computer(WM_HWIN hWin) 
{
	GUI_CreateDialogBox(_aDialogCreateComputer, 
	                    GUI_COUNTOF(_aDialogCreateComputer), 
	                    &_cbCallbackComputer, 
	                    hWin, 
	                    0, 
	                    0);
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

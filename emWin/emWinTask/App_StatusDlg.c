/*
*********************************************************************************************************
*	                                  
*	模块名称 : 主界面状态栏
*	文件名称 : App_StatusDlg.c
*	版    本 : V1.0
*	说    明 : 主要是实现以下功能：
*              1. 日期和时间的更新。
*              2. 网线插拔标志显示。
*              3. SD卡插拔标志显示。
*              4. 闹钟是默认支持的，所以一直显示闹钟标志。
*              5. 用于模拟U盘功能的存储设备不需要更新图标，因为已经设置仅在USB界面里面才可以设置。
*              6. 其余的暂不使用。
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
*                                       变量
*********************************************************************************************************
*/
static uint8_t s_ucSDDetect = 0;
uint8_t g_ucNETDetect = 0;
WM_HWIN hWinStatus;

/* 用于星期的显示 */
static const char ucWeekDay[7][3] =
{
	"Mon",
	"Tue",
	"Wed",
	"Thu",
	"Fri",
	"Sat",
	"Sun"
};

/*
*********************************************************************************************************
*				                         对话框初始化选项
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateSysInfo[] = 
{
    { WINDOW_CreateIndirect,  "",   0,   0,  444,  800,36,0},
	{ TEXT_CreateIndirect,    "",   GUI_ID_TEXT0, 715,2, 85, 16, 0,0},
    { TEXT_CreateIndirect,    "",   GUI_ID_TEXT1, 715,18,85, 16, 0,0},
};

/*
*********************************************************************************************************
*	函 数 名: Caculate_RTC
*	功能说明: 显示RTC时间
*	形    参: pMsg  WM_MESSAGE类型指针变量           
*	返 回 值: 无
*********************************************************************************************************
*/
extern RTC_TimeTypeDef  RTC_TimeStructure;
extern RTC_DateTypeDef  RTC_DateStructure;
static void Caculate_RTC(WM_MESSAGE * pMsg)
{
	  char buf[30];
	  WM_HWIN hWin = pMsg->hWin;

	  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	  RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);

	  sprintf(buf, 
	          "%0.2d:%0.2d:%0.2d %0.3s", 
			  RTC_TimeStructure.RTC_Hours, 
			  RTC_TimeStructure.RTC_Minutes,
			  RTC_TimeStructure.RTC_Seconds, 
			  ucWeekDay[RTC_DateStructure.RTC_WeekDay-1]);
	 TEXT_SetText(WM_GetDialogItem(hWin,GUI_ID_TEXT1), buf);
 
	  sprintf(buf, 
	          "20%0.2d/%0.2d/%0.2d", 
			  RTC_DateStructure.RTC_Year, 
			  RTC_DateStructure.RTC_Month, 
			  RTC_DateStructure.RTC_Date);
	  TEXT_SetText(WM_GetDialogItem(hWin,GUI_ID_TEXT0), buf); 	

}

/*
*********************************************************************************************************
*	函 数 名: PaintDialogSysInfo
*	功能说明: 重回函数 
*	形    参: pMsg  WM_MESSAGE类型指针变量  
*	返 回 值: 无
*********************************************************************************************************
*/
static void PaintDialogSysInfo(WM_MESSAGE * pMsg)
{

	int        xSize;
	int        ySize;
	static int Reception = 3; 
	int        xPos;
	int        i;

    xSize = WM_GetWindowSizeX(pMsg->hWin);
    ySize = WM_GetWindowSizeY(pMsg->hWin);	
	
	/* 电池标志 */
	GUI_SetColor(0x303030);
    GUI_FillRect(0, 2, xSize - 1, ySize - 1);
	
    GUI_SetColor(0x808080);
    GUI_DrawHLine(0, 0, xSize - 1);
	
    GUI_SetColor(0x404040);
    GUI_DrawHLine(1, 0, xSize - 1);
	
	GUI_DrawBitmap(&_bmBatteryEmpty_27x14, 680,10);
    GUI_DrawBitmap(_apbmCharge[4], 680, 10);
	
	/* WIFI信号绘制 */
	GUI_SetColor(GUI_WHITE);
	for (xPos = 650, i = 0; i < Reception; i++) 
	{
		GUI_DrawVLine(xPos + i * 4 + 0, 18 - i * 2, 20);
		GUI_DrawVLine(xPos + i * 4 + 1, 18 - i * 2, 20);
	}
	
	GUI_SetColor(GUI_GRAY);
	for (; i < 5; i++) {
		GUI_DrawVLine(xPos + i * 4 + 0, 18 - i * 2, 20);
		GUI_DrawVLine(xPos + i * 4 + 1, 18 - i * 2, 20);
	}
	
	/* 闹钟绘制 */
	GUI_DrawBitmap(&_bmAlarm_16x16, 620, 10);
	
	/* 网线插拔标志 */
	if(g_ucNETDetect == 1)
	{
		GUI_DrawBitmap(&bmNetStatus, 3, 2);		
	}
	else
	{
		GUI_DrawBitmap(&bmNetStatus, 3, 2);
		GUI_DrawBitmap(&bmLinkError, 3+16, 2+16);		
	}

	/* SD卡插拔标志 */
	if(s_ucSDDetect == 1)
	{
		GUI_DrawBitmap(&bmSdcard, 40, 2);		
	}
	else
	{
		GUI_DrawBitmap(&bmSdcard, 40, 2);	
		GUI_DrawBitmap(&bmLinkError, 40+16, 2+16);	
	}

	/* U盘插拔标志 */
	GUI_DrawBitmap(&bmusbdisk, 77, 2);
	GUI_DrawBitmap(&bmLinkError, 77+16, 2+16);

	/* USB连接线插拔标志 */
	GUI_DrawBitmap(&bmusbtrans, 77+37, 2);
	GUI_DrawBitmap(&bmLinkError, 77+37+16, 2+16);	
}

/*
*********************************************************************************************************
*	函 数 名: InitDialogSysInfo
*	功能说明: 对话框初始化
*	形    参: pMsg  WM_MESSAGE类型指针变量  
*	返 回 值: 无
*********************************************************************************************************
*/
static void InitDialogSysInfo(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	WM_HWIN hWin = pMsg->hWin; 

	/* 创建定时器 */
	WM_CreateTimer(hWin, 0, 1000, 0);	        

	//
    //初始化文本GUI_ID_TEXT0
    //
	hItem = WM_GetDialogItem(hWin,GUI_ID_TEXT0);
    TEXT_SetBkColor(hItem, 0x303030);
	TEXT_SetTextColor(hItem, GUI_WHITE);
    TEXT_SetFont(hItem, &GUI_Font16B_ASCII);
    TEXT_SetTextAlign(hItem, GUI_TA_VCENTER|GUI_TA_CENTER);
	
    //
    //初始化文本GUI_ID_TEXT1
    //
	hItem = WM_GetDialogItem(hWin,GUI_ID_TEXT1);	
    TEXT_SetBkColor(hItem, 0x303030);
	TEXT_SetTextColor(hItem, GUI_WHITE);
    TEXT_SetFont(hItem, &GUI_Font16B_ASCII);
    TEXT_SetTextAlign(hItem, GUI_TA_VCENTER|GUI_TA_CENTER);

	Caculate_RTC(pMsg);
}

/*
*********************************************************************************************************
*	函 数 名: _cbCallbackSysInfo
*	功能说明: 回调函数 
*	形    参: pMsg  WM_MESSAGE类型指针变量  
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbCallbackSysInfo(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	
	switch (pMsg->MsgId) 
	{
		case WM_INIT_DIALOG:
			InitDialogSysInfo(pMsg);
			break;
		
		/* 定时器更新 */
		case WM_TIMER:
			Caculate_RTC(pMsg);
			WM_RestartTimer(pMsg->Data.v, 1000);
			break;

		case WM_PAINT:
			PaintDialogSysInfo(pMsg);
			break;

		/* SD卡插入时，界面更新 */
		case MSG_SDPlugIn:
			s_ucSDDetect = 1;
			WM_InvalidateWindow(hWin);
			break;

		/* SD卡拔出后，界面更新 */
		case MSG_SDPlugOut:
			s_ucSDDetect = 0;
			WM_InvalidateWindow(hWin);
			break;
		
		/* 网线插入时，界面更新 */
		case MSG_NETPlugIn:
			g_ucNETDetect = 1;
			WM_InvalidateWindow(hWin);
			break;

		/* 网线拔出后，界面更新 */
		case MSG_NETPlugOut:
			g_ucNETDetect = 0;
			WM_InvalidateWindow(hWin);
			break;

		default:
			WM_DefaultProc(pMsg);
	}
}

/*
*********************************************************************************************************
*	函 数 名: CreateSysInfoDlg
*	功能说明: 系统信息对话框
*	形    参: 无   	
*	返 回 值: 窗口句柄
*********************************************************************************************************
*/
void CreateSysInfoDlg(void) 
{
	
	/* 创建一个对话框, 类型XP系统的任务栏 */
	hWinStatus = GUI_CreateDialogBox(_aDialogCreateSysInfo, 
	                                 GUI_COUNTOF(_aDialogCreateSysInfo), 
								     &_cbCallbackSysInfo, 
								     0, 
								     0, 
								     0);
	
	/* 系统信息界面创建后给任务HandlePinDetectStart发消息 */
	if(HandlePinDetectStart)
	{
		os_evt_set(GUIDetectBIT_15, HandlePinDetectStart);
	}
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

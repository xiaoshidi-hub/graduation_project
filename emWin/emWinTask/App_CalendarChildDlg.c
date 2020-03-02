/*
*********************************************************************************************************
*	                                  
*	模块名称 : 时间，日期和闹钟设置界面
*	文件名称 : App_CalendarChildDlg.c
*	版    本 : V1.0
*	说    明 : 实现STM32F429自带RTC的时间，日期和闹钟设置
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
*                                        宏定义
*********************************************************************************************************
*/
#define ID_SPINBOX_0  (GUI_ID_USER + 0x01)
#define ID_SPINBOX_1  (GUI_ID_USER + 0x02)
#define ID_SPINBOX_2  (GUI_ID_USER + 0x03)
#define ID_SPINBOX_3  (GUI_ID_USER + 0x04)
#define ID_SPINBOX_4  (GUI_ID_USER + 0x05)
#define ID_SPINBOX_5  (GUI_ID_USER + 0x06)


/*
*********************************************************************************************************
*                                        变量
*********************************************************************************************************
*/
const char *apDays[] =
{
	"六",
	"日",
	"一",
	"二",
	"三",
	"四",
	"五",
	"六",
};

const char *apMonths[]=
{
	"1月",
	"2月",
	"3月",
	"4月",
	"5月",
	"6月",
	"7月",
	"8月",
	"9月",
	"10月",
	"11月",
	"12月",
};


/*
*********************************************************************************************************
*	                               对话框控件列表
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateSetTimeAlarm[] = {
    { WINDOW_CreateIndirect,  "Time Settings",        	0,                     0,  0,  800,480,0,0},
    { TEXT_CreateIndirect,      "时间设置",          GUI_ID_TEXT0,            114,22, 73, 20, 0,0},
    { TEXT_CreateIndirect,      "闹钟设置",          GUI_ID_TEXT1,            314,22, 73, 20, 0,0},
    { CHECKBOX_CreateIndirect,  "保存时间设置",  	 GUI_ID_CHECK0,           60,270,210,24, 0,0},
	{ CHECKBOX_CreateIndirect,  "保存闹钟设置",   	 GUI_ID_CHECK1,           280,270,210,24, 0,0},
	{ CHECKBOX_CreateIndirect,  "保存年月日设置",    GUI_ID_CHECK2,           500,270,210,24, 0,0},
    { TEXT_CreateIndirect,      "时:",               GUI_ID_TEXT2,            40, 59, 29, 20, 0,0},
    { TEXT_CreateIndirect,      "分:",               GUI_ID_TEXT3,            40, 106+15,29, 20, 0,0},
    { TEXT_CreateIndirect,      "秒:",               GUI_ID_TEXT4,            40, 156+30,29, 20, 0,0},
	
	{ BUTTON_CreateIndirect, "Button", GUI_ID_BUTTON0, 200, 400, 100, 45, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "Button", GUI_ID_BUTTON1, 500, 400, 100, 45, 0, 0x0, 0 },

	{ SPINBOX_CreateIndirect, "Spinbox", ID_SPINBOX_0, 80, 56, 130, 35, 0, 0x0, 0 },
	{ SPINBOX_CreateIndirect, "Spinbox", ID_SPINBOX_1, 80, 103 + 15, 130, 35, 0, 0x0, 0 },
	{ SPINBOX_CreateIndirect, "Spinbox", ID_SPINBOX_2, 80, 149 + 30, 130, 35, 0, 0x0, 0 },
	{ SPINBOX_CreateIndirect, "Spinbox", ID_SPINBOX_3, 280, 56, 130, 35, 0, 0x0, 0 },
	{ SPINBOX_CreateIndirect, "Spinbox", ID_SPINBOX_4, 280, 103 + 15, 130, 35, 0, 0x0, 0 },
	{ SPINBOX_CreateIndirect, "Spinbox", ID_SPINBOX_5, 280, 153 + 30, 130, 35, 0, 0x0, 0 },
};

/*
*********************************************************************************************************
*	函 数 名: InitDialogSetTimeAlarm
*	功能说明: 回调函数初始化消息 
*	形    参: pMsg  消息指针变量
*	返 回 值: 无
*********************************************************************************************************
*/
extern RTC_TimeTypeDef  RTC_TimeStructure;
extern RTC_DateTypeDef  RTC_DateStructure;
extern RTC_AlarmTypeDef RTC_AlarmStructure;
void InitDialogSetTimeAlarm(WM_MESSAGE * pMsg)
{
    WM_HWIN hWin = pMsg->hWin;
	
	/* 日历控件初始化 */
	CALENDAR_SetDefaultSize(CALENDAR_SI_HEADER, 35 );
	CALENDAR_SetDefaultSize(CALENDAR_SI_CELL_X, 40 );
	CALENDAR_SetDefaultSize(CALENDAR_SI_CELL_Y, 28 );

	CALENDAR_SetDefaultFont(CALENDAR_FI_CONTENT, &GUI_FontHZ16);
	CALENDAR_SetDefaultFont(CALENDAR_FI_HEADER, &GUI_FontHZ16); 
	CALENDAR_SetDefaultBkColor(CALENDAR_CI_WEEKDAY, 0xFF00FF);
	
	CALENDAR_SetDefaultDays(apDays);
	CALENDAR_SetDefaultMonths(apMonths);

	RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
	CALENDAR_Create(hWin, 
	                 450, 
	                 22, 
	                 RTC_DateStructure.RTC_Year+2000, 
	                 RTC_DateStructure.RTC_Month, 
					 RTC_DateStructure.RTC_Date, 
					 2, 
					 GUI_ID_CALENDAR0, 
					 WM_CF_SHOW);

	/* 文本控件初始化 */
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT0), &GUI_FontHZ16);
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT1), &GUI_FontHZ16);
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT2), &GUI_FontHZ16);
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT3), &GUI_FontHZ16);
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT4), &GUI_FontHZ16);

	/* 复选框初始化 */
    CHECKBOX_SetText(WM_GetDialogItem(hWin,GUI_ID_CHECK0), "保存时间设置");
    CHECKBOX_SetFont(WM_GetDialogItem(hWin,GUI_ID_CHECK0), &GUI_FontHZ16);
    CHECKBOX_SetText(WM_GetDialogItem(hWin,GUI_ID_CHECK1), "保存闹钟设置");
    CHECKBOX_SetFont(WM_GetDialogItem(hWin,GUI_ID_CHECK1), &GUI_FontHZ16);
    CHECKBOX_SetText(WM_GetDialogItem(hWin,GUI_ID_CHECK2), "保存年月日设置");
    CHECKBOX_SetFont(WM_GetDialogItem(hWin,GUI_ID_CHECK2), &GUI_FontHZ16);
	
	/* 按钮初始化 */
    BUTTON_SetFont(WM_GetDialogItem(hWin,GUI_ID_BUTTON0), &GUI_FontHZ16);
    BUTTON_SetText(WM_GetDialogItem(hWin,GUI_ID_BUTTON0), "确定");
	BUTTON_SetFont(WM_GetDialogItem(hWin,GUI_ID_BUTTON1), &GUI_FontHZ16);
    BUTTON_SetText(WM_GetDialogItem(hWin,GUI_ID_BUTTON1), "取消");
	
	/* 编辑框初始化 */
	EDIT_SetTextAlign(SPINBOX_GetEditHandle(WM_GetDialogItem(hWin,ID_SPINBOX_0)), GUI_TA_HCENTER | GUI_TA_VCENTER);
	EDIT_SetTextAlign(SPINBOX_GetEditHandle(WM_GetDialogItem(hWin,ID_SPINBOX_1)), GUI_TA_HCENTER | GUI_TA_VCENTER);
	EDIT_SetTextAlign(SPINBOX_GetEditHandle(WM_GetDialogItem(hWin,ID_SPINBOX_2)), GUI_TA_HCENTER | GUI_TA_VCENTER);
	EDIT_SetTextAlign(SPINBOX_GetEditHandle(WM_GetDialogItem(hWin,ID_SPINBOX_3)), GUI_TA_HCENTER | GUI_TA_VCENTER);
	EDIT_SetTextAlign(SPINBOX_GetEditHandle(WM_GetDialogItem(hWin,ID_SPINBOX_4)), GUI_TA_HCENTER | GUI_TA_VCENTER);
	EDIT_SetTextAlign(SPINBOX_GetEditHandle(WM_GetDialogItem(hWin,ID_SPINBOX_5)), GUI_TA_HCENTER | GUI_TA_VCENTER);

	/* SPINBOX初始化 */
	SPINBOX_SetEdge(WM_GetDialogItem(hWin,ID_SPINBOX_0), SPINBOX_EDGE_CENTER);
	SPINBOX_SetEdge(WM_GetDialogItem(hWin,ID_SPINBOX_1), SPINBOX_EDGE_CENTER);
	SPINBOX_SetEdge(WM_GetDialogItem(hWin,ID_SPINBOX_2), SPINBOX_EDGE_CENTER);
	SPINBOX_SetEdge(WM_GetDialogItem(hWin,ID_SPINBOX_3), SPINBOX_EDGE_CENTER);
	SPINBOX_SetEdge(WM_GetDialogItem(hWin,ID_SPINBOX_4), SPINBOX_EDGE_CENTER);
	SPINBOX_SetEdge(WM_GetDialogItem(hWin,ID_SPINBOX_5), SPINBOX_EDGE_CENTER);
	
	SPINBOX_SetFont(WM_GetDialogItem(hWin,ID_SPINBOX_0), &GUI_FontHZ16);
	SPINBOX_SetFont(WM_GetDialogItem(hWin,ID_SPINBOX_1), &GUI_FontHZ16);
	SPINBOX_SetFont(WM_GetDialogItem(hWin,ID_SPINBOX_2), &GUI_FontHZ16);
	SPINBOX_SetFont(WM_GetDialogItem(hWin,ID_SPINBOX_3), &GUI_FontHZ16);
	SPINBOX_SetFont(WM_GetDialogItem(hWin,ID_SPINBOX_4), &GUI_FontHZ16);
	SPINBOX_SetFont(WM_GetDialogItem(hWin,ID_SPINBOX_5), &GUI_FontHZ16);
	
	SPINBOX_SetRange(WM_GetDialogItem(hWin,ID_SPINBOX_0), 0, 23);  /* 小时 */
	SPINBOX_SetRange(WM_GetDialogItem(hWin,ID_SPINBOX_1), 0, 59);  /* 分钟 */
	SPINBOX_SetRange(WM_GetDialogItem(hWin,ID_SPINBOX_2), 0, 59);  /* 秒钟 */
	
	SPINBOX_SetRange(WM_GetDialogItem(hWin,ID_SPINBOX_3), 0, 23);  /* 闹钟小时 */
	SPINBOX_SetRange(WM_GetDialogItem(hWin,ID_SPINBOX_4), 0, 59);  /* 闹钟分钟 */
	SPINBOX_SetRange(WM_GetDialogItem(hWin,ID_SPINBOX_5), 0, 59);  /* 闹钟秒钟 */
	
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	RTC_GetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);
	
	SPINBOX_SetValue(WM_GetDialogItem(hWin,ID_SPINBOX_0), RTC_TimeStructure.RTC_Hours);   /* 时 */								   
	SPINBOX_SetValue(WM_GetDialogItem(hWin,ID_SPINBOX_1), RTC_TimeStructure.RTC_Minutes); /* 分 */							   
	SPINBOX_SetValue(WM_GetDialogItem(hWin,ID_SPINBOX_2), RTC_TimeStructure.RTC_Seconds); /* 秒 */
	
	SPINBOX_SetValue(WM_GetDialogItem(hWin,ID_SPINBOX_3), RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours);   /* 闹钟时 */			 
	SPINBOX_SetValue(WM_GetDialogItem(hWin,ID_SPINBOX_4), RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes); /* 闹钟分 */				 
	SPINBOX_SetValue(WM_GetDialogItem(hWin,ID_SPINBOX_5), RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds); /* 闹钟秒 */
}

/*
*********************************************************************************************************
*	函 数 名: _cbCallbackSetTimeAlarm
*	功能说明: 回调函数 
*	形    参: pMsg  消息指针变量
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbCallbackSetTimeAlarm(WM_MESSAGE * pMsg) 
{
    int NCode, Id;
    WM_HWIN hWin = pMsg->hWin;
	CALENDAR_DATE CALENDAR_Date;
	
    switch (pMsg->MsgId) 
    {
        case WM_INIT_DIALOG:
            InitDialogSetTimeAlarm(pMsg);
            break;
		
        case WM_KEY:
            switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
            {
                case GUI_KEY_ESCAPE:
                    GUI_EndDialog(hWin, 1);
                    break;
				
                case GUI_KEY_ENTER:
                    GUI_EndDialog(hWin, 0);
                    break;
            }
            break;
			
        case WM_NOTIFY_PARENT:
            Id = WM_GetId(pMsg->hWinSrc); 
            NCode = pMsg->Data.v;        
            switch (Id) 
            {
				/* 点击确定 */
				case GUI_ID_BUTTON0:
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						
						case WM_NOTIFICATION_RELEASED:
							
							/* 是否保存时间设置 */
							if(CHECKBOX_GetState(WM_GetDialogItem(hWin,GUI_ID_CHECK0)) == 1)
							{
								RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
								RTC_TimeStructure.RTC_Hours   = SPINBOX_GetValue(WM_GetDialogItem(hWin,ID_SPINBOX_0));
								RTC_TimeStructure.RTC_Minutes = SPINBOX_GetValue(WM_GetDialogItem(hWin,ID_SPINBOX_1));
								RTC_TimeStructure.RTC_Seconds = SPINBOX_GetValue(WM_GetDialogItem(hWin,ID_SPINBOX_2));
								RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);
							}
							
							/* 是否保存闹钟时间设置 */
							if(CHECKBOX_GetState(WM_GetDialogItem(hWin,GUI_ID_CHECK1)) == 1)
							{
								/* 禁能 Alarm A */
								RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
								RTC_AlarmStructure.RTC_AlarmTime.RTC_H12     = RTC_H12_AM;
								RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours   = SPINBOX_GetValue(WM_GetDialogItem(hWin,ID_SPINBOX_3));
								RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = SPINBOX_GetValue(WM_GetDialogItem(hWin,ID_SPINBOX_4));
								RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = SPINBOX_GetValue(WM_GetDialogItem(hWin,ID_SPINBOX_5));
				
								/* 设置 Alarm A */
								RTC_AlarmStructure.RTC_AlarmDateWeekDay = 0x31;
								RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
								RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;

								/* 配置 RTC Alarm A 寄存器 */
								/* 根据需要，这里可以加上是否配置成功的状态监测 */
								RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);

								/* 使能 RTC Alarm A 中断 */
								RTC_ITConfig(RTC_IT_ALRA, ENABLE);

								/* 使能 alarm  A */
								RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
							}
							
							/* 是否保存年月日设置 */
							if(CHECKBOX_GetState(WM_GetDialogItem(hWin,GUI_ID_CHECK2)) == 1)
							{
								CALENDAR_GetSel(WM_GetDialogItem(hWin,GUI_ID_CALENDAR0), &CALENDAR_Date);
								
								RTC_DateStructure.RTC_Year = CALENDAR_Date.Year%100;
								RTC_DateStructure.RTC_Month = CALENDAR_Date.Month;
								RTC_DateStructure.RTC_Date = CALENDAR_Date.Day;
								RTC_DateStructure.RTC_WeekDay = RTC_CalcWeek(RTC_DateStructure.RTC_Year, 
																			 RTC_DateStructure.RTC_Month,
																			 RTC_DateStructure.RTC_Date);
								
								RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);
								//CALENDAR_SetDate(WM_GetDialogItem(hWin,GUI_ID_CALENDAR0), &CALENDAR_Date);
							}
							
						    GUI_EndDialog(hWin, 0);
							break;
					}
					break;
				
				/* 点击取消按钮 */
				case GUI_ID_BUTTON1:
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
*	函 数 名: App_CalendarChild
*	功能说明: 闹钟，时间，日期设置对话框 
*	形    参: hWin 父窗口句柄
*	返 回 值: 无
*********************************************************************************************************
*/
void App_CalendarChild(WM_HWIN hWin) 
{
	WM_HWIN hWinAlarm; 
    
	hWinAlarm = GUI_CreateDialogBox(_aDialogCreateSetTimeAlarm, 
	                                GUI_COUNTOF(_aDialogCreateSetTimeAlarm), 
	                                &_cbCallbackSetTimeAlarm, 
	                                hWin, 
	                                0, 
	                                0);
	
	/* 设置为模态窗口 */
	WM_MakeModal(hWinAlarm);
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

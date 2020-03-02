/*
*********************************************************************************************************
*	                                  
*	模块名称 : USB界面设计
*	文件名称 : App_USBDlg.c
*	版    本 : V1.0
*	说    明 : 这里主要实现了SD卡模拟U盘的连接和断开功能。
*	           1. 移植了RL-USB，板子使用的MicroUSB接口，即小口，实现SD卡模拟U盘，所以SD卡务必插上。
*	           2. 设置了240s的时间进行连接，如果240s内无法连接上，自动退出连接,一般情况下，500ms左右就连接上了。
*	           3. 拔掉USB线前，务必点击断开连接按钮。
*	           4. 为了防止点击了连接USB按钮后用户重复点击，此按钮被设置了禁能，点击断开USB按钮可恢复。
*              5. 操作不可太快，要不反应不过来。
*              6. 首次在新的电脑上面连接时，电脑端会有一个自动安装驱动的过程，这个时间稍长。所以将连接时间设置为120秒。
*              7. 板子左下角J12和J13的跳线帽设置，切不可将PA11和PA12导通，否则USB通信不正常，直接拔掉跳线帽即可。
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

#define ID_WINDOW_0    (GUI_ID_USER + 0x00)
#define ID_TEXT_0	   (GUI_ID_USER + 0x01)
#define ID_TEXT_1 	   (GUI_ID_USER + 0x02)
#define ID_MULTIEDIT_0 (GUI_ID_USER + 0x03)
#define ID_BUTTON_0    (GUI_ID_USER + 0x04)
#define ID_BUTTON_1    (GUI_ID_USER + 0x05)
#define ID_BUTTON_2    (GUI_ID_USER + 0x06)
#define ID_BUTTON_3    (GUI_ID_USER + 0x07)
#define ID_BUTTON_4    (GUI_ID_USER + 0x08)
#define ID_BUTTON_5    (GUI_ID_USER + 0x09)
#define ID_BUTTON_6    (GUI_ID_USER + 0x0a)

#define ID_TimerUSB     0
#define USBConnectTime  2400     /* 设置连接时间是120秒，如果120秒无法连接上，就不再连接 */


/*
*********************************************************************************************************
*                                       变量
*********************************************************************************************************
*/
extern OS_TID HandleTaskUSB;


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
* 函 数 名: _cbButtonCom
* 功能说明: 按钮回调函数
* 形    参: pMsg 消息指针
* 返 回 值: 无
*********************************************************************************************************
*/
static void _cbButtonCom(WM_MESSAGE * pMsg)
{
	WM_HWIN hWin;
	GUI_RECT Rect;
	char buf[30];
	
	hWin = pMsg->hWin;
	
	switch (pMsg->MsgId)
	{
		case WM_PAINT:
			WM_GetClientRect(&Rect);
			if (BUTTON_IsPressed(hWin))
			{
				GUI_SetColor(GUI_DARKGRAY);
				GUI_AA_FillCircle((Rect.x1 - Rect.x0)/2, (Rect.y1 - Rect.y0)/2, (Rect.x1 - Rect.x0)/2);
				GUI_SetBkColor(GUI_DARKGRAY);
				GUI_SetColor(GUI_WHITE);
			}
			else
			{
				GUI_SetColor(GUI_LIGHTBLUE);
				GUI_AA_FillCircle((Rect.x1 - Rect.x0)/2, (Rect.y1 - Rect.y0)/2, (Rect.x1 - Rect.x0)/2);
				GUI_SetBkColor(GUI_LIGHTBLUE);
				GUI_SetColor(GUI_WHITE);
			}
			GUI_SetFont(&GUI_FontHZ16);
			BUTTON_GetText(hWin, buf, 20);
			GUI_DispStringInRect(buf, &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
			break;
			
		default:
			BUTTON_Callback(pMsg);
	}
}

/*
*********************************************************************************************************
*	                             对话框控件列表
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateUSB[] = 
{
	{ WINDOW_CreateIndirect,  "Window",  ID_WINDOW_0,  0,   0,  800, 480, 0, 0x0, 0 },
	{ TEXT_CreateIndirect,      " ",  	ID_TEXT_0,     319, 139,  160, 20, 0, 0},
	{ TEXT_CreateIndirect,      " ",  	ID_TEXT_1,     3, 395,  790, 85, 0, 0},
	
	{ BUTTON_CreateIndirect,  "Button",    ID_BUTTON_0,   190,  100, 120, 120, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,  "Button",    ID_BUTTON_1,   490,  100, 120, 120, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,  "DlgBack",   ID_BUTTON_5,   700,    0, 100, 100, 0, 0, 0 },	
	{ BUTTON_CreateIndirect,  "Setting",   ID_BUTTON_6,    0,     0, 100, 100, 0, 0, 0 },
};

/*
*********************************************************************************************************
*	函 数 名: InitDialogUSB
*	功能说明: 对话框回调函数的初始化消息
*	形    参: pMsg  消息指针变量
*	返 回 值: 无
*********************************************************************************************************
*/
static void InitDialogUSB(WM_MESSAGE * pMsg)
{
    WM_HWIN hWin = pMsg->hWin;
	WM_HWIN hItem;
	
	/* 设置抗锯齿因数 */
	GUI_AA_SetFactor(MAG);
	
    /* 初始化4个按钮 */
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
    BUTTON_SetFont(hItem,  &GUI_FontHZ16);
	BUTTON_SetText(hItem, "连接USB");
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonCom);
	
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
    BUTTON_SetFont(hItem,  &GUI_FontHZ16);
    BUTTON_SetText(hItem, "断开USB");
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonCom);
	
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_5);
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonBack);
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_6);
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonList);
		
	/* 初始化文本 */
	hItem = WM_GetDialogItem(hWin, ID_TEXT_0);
	TEXT_SetTextColor(hItem, GUI_BLACK);
	TEXT_SetFont(hItem, &GUI_FontHZ16);
	TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetText(hItem, "未连接");
	
	hItem = WM_GetDialogItem(hWin, ID_TEXT_1);
	TEXT_SetTextColor(hItem, GUI_BLACK);
	TEXT_SetFont(hItem, &GUI_FontHZ16);
    TEXT_SetText(hItem, "[1]移植了RL-USB，使用MicroUSB接口，实现SD卡模拟U盘，所以上电前，SD卡和USB线务必插上。\n"
						"[2]为了防止点击了连接USB按钮后用户重复点击，此按钮被设置了禁能，点击断开USB按钮可恢复。\n"
						"[3]首次在电脑上面连接时，电脑端会有一个自动安装驱动的过程，这个时间稍长，所以设置240秒进行连接，\n"
						"   如果240s内无法连接上，自动退出连接。另外，拔掉USB线前，务必点击断开连接按钮。\n"
						"[4]板子左下角J12和J13的跳线帽切不可分别选择PA11和PA12导通，否则USB通信不正常，直接拔掉跳线帽即可。\n"
						);
}

/*
*********************************************************************************************************
*	函 数 名: _cbCallbackUSB
*	功能说明: 回调函数
*	形    参: pMsg   消息指针变量
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbCallbackUSB(WM_MESSAGE * pMsg) 
{
    int NCode, Id;
	WM_HWIN hItem;
    WM_HWIN hWin = pMsg->hWin;
	char buf[50];
	static int lCount1, lCount2;
	static WM_HTIMER hTimerUSB;
	static char ucConFlag = 0;
	
    switch (pMsg->MsgId) 
    {
        case WM_INIT_DIALOG:
			lCount1 = 0;
			lCount2 = 0;
			ucConFlag = 0;
            InitDialogUSB(pMsg);
            break;
		
		case WM_PAINT:
			GUI_DrawGradientV(0,			   /* 左上角X 位置 */
							  0,			   /* 左上角Y 位置 */
							  799,	   	       /* 右下角X 位置 */
							  479,  	   	   /* 右下角Y 位置 */
							  GUI_WHITE,	   /* 矩形最左侧要绘制的颜色 */
							  GUI_LIGHTBLUE);  /* 矩形最右侧要绘制的颜色 */

			GUI_DrawGradientH(319,			   /* 左上角X 位置 */
							  159,			   /* 左上角Y 位置 */
							  479,	           /* 右下角X 位置 */
							  161,             /* 右下角Y 位置 */
							  GUI_RED,	       /* 矩形最左侧要绘制的颜色 */
							  GUI_YELLOW);     /* 矩形最右侧要绘制的颜色 */

			break;
		
		case WM_TIMER:
			/* 如果连接后，用户手动拔掉了USB线，这个函数没有提示的 */
			if(usbd_configured() == __TRUE)
			{
				hItem = WM_GetDialogItem(hWin, ID_TEXT_0);
				sprintf(buf, "已连接%d秒\r\n", lCount2++);
				TEXT_SetText(hItem, buf);
				
				/* 重启定时器 */
				WM_RestartTimer(pMsg->Data.v, 1000);		
			}
			else
			{
				hItem = WM_GetDialogItem(hWin, ID_TEXT_0);
				sprintf(buf, "连接中%d...\r\n", lCount1++);
				TEXT_SetText(hItem, buf);

				/* 重启定时器 */
				WM_RestartTimer(pMsg->Data.v, 100);	

				/* 120秒的时间到后未连接上，停止连接 */
				if(lCount1 == USBConnectTime)
				{
					/* 删除后，不可以再调用WM_RestartTimer，会进入硬件异常 */
					ucConFlag = 0;
					WM_DeleteTimer(hTimerUSB);
					hItem = WM_GetDialogItem(hWin, ID_BUTTON_0);
					BUTTON_SetText(hItem, "连接USB");
					WM_EnableWindow(hItem);
					hItem = WM_GetDialogItem(hWin, ID_TEXT_0);
					TEXT_SetText(hItem, "未连接");
					os_evt_set (0x0002, HandleTaskUSB);
				}				
			}
			break;
			
        case WM_NOTIFY_PARENT:
            Id = WM_GetId(pMsg->hWinSrc); 
            NCode = pMsg->Data.v;        
            switch (Id) 
            {
				case ID_BUTTON_0:
                    switch(NCode)
                    {
						/* 连接USB */
                        case WM_NOTIFICATION_CLICKED:
							lCount1 = 0;
							lCount2 = 0;
						    hItem = WM_GetDialogItem(hWin, ID_BUTTON_0);
							WM_DisableWindow(hItem);
							BUTTON_SetText(hItem, "连接中\n已经禁止重连");
							os_evt_set (USBTaskOpen_0, HandleTaskUSB);
							ucConFlag = 1;
						    hTimerUSB = WM_CreateTimer(pMsg->hWin, ID_TimerUSB, 0, 0);	
                            break;
                    }
                    break;
					
				case ID_BUTTON_1:
                    switch(NCode)
                    {
						/* 断开USB */
                        case WM_NOTIFICATION_CLICKED:
							if(ucConFlag == 1)
							{
								ucConFlag = 0;
								WM_DeleteTimer(hTimerUSB);
							}
							hItem = WM_GetDialogItem(hWin, ID_BUTTON_0);
							WM_EnableWindow(hItem);
							BUTTON_SetText(hItem, "连接USB");
							hItem = WM_GetDialogItem(hWin, ID_TEXT_0);
							TEXT_SetText(hItem, "未连接");
							os_evt_set (USBTaskClose_1, HandleTaskUSB);
                            break;
                    }
                    break;

				/* 关闭USB窗口 */
				case ID_BUTTON_5:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_RELEASED:
							if(ucConFlag == 1)
							{
								os_evt_set (USBTaskClose_1, HandleTaskUSB);
							}
							os_evt_set (USBTaskReMountSD_2, HandleTaskUSB);
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
*	函 数 名: App_USB
*	功能说明: 创建USB控制对话框
*	形    参: hWin  父窗口
*	返 回 值: 无
*********************************************************************************************************
*/
void App_USB(WM_HWIN hWin) 
{				 
    GUI_CreateDialogBox(_aDialogCreateUSB, 
	                    GUI_COUNTOF(_aDialogCreateUSB), 
	                    &_cbCallbackUSB, 
	                    hWin, 
	                    0, 
	                    0);
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

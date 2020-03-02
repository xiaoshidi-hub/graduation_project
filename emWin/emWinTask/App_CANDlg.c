/*
*********************************************************************************************************
*	                                  
*	模块名称 : CAN总线数据收发界面设计
*	文件名称 : App_CANDlg.c
*	版    本 : V1.0
*	说    明 : CAN总线数据收发界面设计。
*              1. 移植了RL-CAN，板子使用的CAN2，速度500Kbps，标准ID
*			   2. 测试时至少要使用两块板子，手拉手式连接
*			   3. CAN的标识符过滤器采用标识符列表模式，配置每个板子仅接收标准ID为33的数据帧或者遥控帧
*              4. 周期发送的默认时间是500ms。
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

#define ID_TimerCAN     0
#define ID_TimerTime    1


/*
*********************************************************************************************************
*                                       变量
*********************************************************************************************************
*/
WM_HWIN  hWinCAN = WM_HWIN_NULL;  	/* CAN窗口句柄 */
extern CAN_msg msg_rece;
static uint32_t s_uiCount = 0;
static SCROLLBAR_Handle hScrollbar;

static const char *can_frametype[] =
{
	"数据帧",
	"遥控帧"
};


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
				GUI_AA_FillRoundedRect(Rect.x0, Rect.y0, Rect.x1, Rect.y1, 5);
				GUI_SetBkColor(GUI_DARKGRAY);
				GUI_SetColor(GUI_WHITE);
			}
			else
			{
				GUI_SetColor(GUI_LIGHTBLUE);
				GUI_AA_FillRoundedRect(Rect.x0, Rect.y0, Rect.x1, Rect.y1, 5);
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
*	                                  对话框控件列表
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateCAN[] = 
{
	{ WINDOW_CreateIndirect,  "Window",  ID_WINDOW_0,   0,   0,  800, 480, 0, 0x0, 0 },
	
	{ TEXT_CreateIndirect,      " ",  	ID_TEXT_1,     3, 420,  700, 60, 0, 0},
	{ LISTVIEW_CreateIndirect, NULL,       GUI_ID_LISTVIEW0,   70, 120, 660, 270,  0, 0},
	
	{ BUTTON_CreateIndirect,  "Button",    ID_BUTTON_0,   70,  45, 100, 50, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,  "Button",    ID_BUTTON_1,  210,  45, 100, 50, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,  "Button",    ID_BUTTON_2,  350,  45, 100, 50, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,  "Button",    ID_BUTTON_3,  490,  45, 100, 50, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,  "Button",    ID_BUTTON_4,  630,  45, 100, 50, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,  "DlgBack",   ID_BUTTON_5,  700,   0, 100, 100, 0, 0, 0 },	
	{ BUTTON_CreateIndirect,  "MusicList", ID_BUTTON_6,    0,   0, 100, 100, 0, 0, 0 },
};

/*
*********************************************************************************************************
*	函 数 名: InitDialogCAN
*	功能说明: 对话框回调函数的初始化消息
*	形    参: pMsg  消息指针变量
*	返 回 值: 无
*********************************************************************************************************
*/
static void InitDialogCAN(WM_MESSAGE * pMsg)
{
    WM_HWIN hWin = pMsg->hWin;
	WM_HWIN hItem;
	HEADER_Handle hHeader;

	s_uiCount = 0;
	
	/* 设置抗锯齿因数 */
	GUI_AA_SetFactor(MAG);
	
	/* 设置ListView控件上的header */
	hItem = WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0);
    hHeader = LISTVIEW_GetHeader(hItem);
    HEADER_SetFont(hHeader, &GUI_FontHZ16);
    HEADER_SetHeight(hHeader, 25);
	HEADER_SetSTSkin(hHeader);
	
	/* 设置ListView控件上的SCROLLBAR */
    hScrollbar = SCROLLBAR_CreateAttached(hItem, SCROLLBAR_CF_VERTICAL);
    SCROLLBAR_SetWidth(hScrollbar, 18);
	SCROLLBAR_SetSTSkin(hScrollbar);
	
	/* 设置ListView控件 */
    LISTVIEW_SetFont(hItem, &GUI_FontHZ16);
    LISTVIEW_SetRowHeight(hItem, 24);
    LISTVIEW_AddColumn(hItem, 60, " 序号",  GUI_TA_VCENTER|GUI_TA_LEFT);
    LISTVIEW_AddColumn(hItem, 60, " ID",    GUI_TA_VCENTER|GUI_TA_LEFT);
    LISTVIEW_AddColumn(hItem, 60, " 数据",  GUI_TA_VCENTER|GUI_TA_LEFT);
	LISTVIEW_AddColumn(hItem, 60, " 数据长度", GUI_TA_VCENTER|GUI_TA_LEFT);
	LISTVIEW_AddColumn(hItem, 60, " 帧类型",   GUI_TA_VCENTER|GUI_TA_LEFT);	
    LISTVIEW_SetColumnWidth(hItem, 0, 50);
    LISTVIEW_SetColumnWidth(hItem, 1, 50);
    LISTVIEW_SetColumnWidth(hItem, 2, 360);
	LISTVIEW_SetColumnWidth(hItem, 3, 90);
    LISTVIEW_SetColumnWidth(hItem, 4, 110);
	
    /* 初始化5个按钮 */
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
    BUTTON_SetFont(hItem,  &GUI_FontHZ16);
	BUTTON_SetText(hItem, "发送数据帧");
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonCom);
	
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
    BUTTON_SetFont(hItem,  &GUI_FontHZ16);
    BUTTON_SetText(hItem, "发送遥控帧");
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonCom);
	
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
    BUTTON_SetFont(hItem,  &GUI_FontHZ16);
    BUTTON_SetText(hItem, "周期发送");
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonCom);
	
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_3);
    BUTTON_SetFont(hItem, &GUI_FontHZ16);
    BUTTON_SetText(hItem, "停止发送");
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonCom);
	
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_4);
    BUTTON_SetFont(hItem, &GUI_FontHZ16);
    BUTTON_SetText(hItem, "清空接收区");
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonCom);
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_5);
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonBack);
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_6);
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonList);
		
	/* 初始化文本 */
	hItem = WM_GetDialogItem(hWin, ID_TEXT_1);
	TEXT_SetTextColor(hItem, GUI_BLACK);
	TEXT_SetFont(hItem, &GUI_FontHZ16);
    TEXT_SetText(hItem, "[1]移植了RL-CAN，板子使用的CAN2，速度500Kbps，标准ID。\n"
						"[2]测试时至少要使用两块板子，手拉手式连接。\n"
						"[3]CAN的标识符过滤器采用标识符列表模式，配置每个板子仅接收标准ID为33的数据帧或者遥控帧。\n");
}

/*
*********************************************************************************************************
*	函 数 名: _cbCallbackCAN
*	功能说明: 回调函数
*	形    参: pMsg   消息指针变量
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbCallbackCAN(WM_MESSAGE * pMsg) 
{
    int NCode, Id;
	WM_HWIN hItem;
	char buf[200];
	int i, j;
	static  WM_HTIMER hTimerCAN;
	static  WM_HTIMER hTimerTime;
	
	/* 发送消息      = { ID, {data[0] .. data[7]}, LEN, CHANNEL, FORMAT, TYPE } */
	CAN_msg msg_send = { 33, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   
						  8, 2, STANDARD_FORMAT, DATA_FRAME};
	
    switch (pMsg->MsgId) 
    {
        case WM_INIT_DIALOG:
            InitDialogCAN(pMsg);
            break;
		
		case WM_TIMER:
			if(WM_GetTimerId(pMsg->Data.v) == ID_TimerCAN)
		    {
				WM_DeleteTimer(hTimerCAN);
				hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_LISTVIEW0);	
				LISTVIEW_AddRow(hItem, NULL);
				
				/* 序号 */
				sprintf(buf, "%d", s_uiCount);
				LISTVIEW_SetItemText(hItem, 0, s_uiCount, (const char *)buf);
			    
				/* 标准ID */
				sprintf(buf, "%d", msg_rece.id);
				LISTVIEW_SetItemText(hItem, 1, s_uiCount, (const char *)buf);
			
				/* 如果是数据帧 */
				if(msg_rece.type == DATA_FRAME)
				{
					/* 接收到的数据 */
					sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d", 
									  msg_rece.data[0],
									  msg_rece.data[1],
									  msg_rece.data[2],
								      msg_rece.data[3],
								      msg_rece.data[4],
								      msg_rece.data[5],
								      msg_rece.data[6],
								      msg_rece.data[7]);
					
					LISTVIEW_SetItemText(hItem, 2, s_uiCount, (const char *)buf);
				
					/* 数据长度 */
					sprintf(buf, "%d", msg_rece.len);
					LISTVIEW_SetItemText(hItem, 3, s_uiCount, (const char *)buf);
				}
				/* 如果是遥控帧 */
				else
				{
					/* 接收到的数据 */
					sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d", 0,0,0,0,0,0,0,0);
					
					LISTVIEW_SetItemText(hItem, 2, s_uiCount, (const char *)buf);
				
					/* 数据长度 */
					sprintf(buf, "%d", 0);
					LISTVIEW_SetItemText(hItem, 3, s_uiCount, (const char *)buf);
				}
				
				/* 帧类型 */
				sprintf(buf, "%s", can_frametype[msg_rece.type]);
				LISTVIEW_SetItemText(hItem, 4, s_uiCount++, (const char *)buf);
				
				/* ListView添加新行后，保证一直显示最新行 */
				SCROLLBAR_SetValue(hScrollbar, SCROLLBAR_GetNumItems(hScrollbar));
			}
			else if(WM_GetTimerId(pMsg->Data.v) == ID_TimerTime)
			{
				msg_send.type = DATA_FRAME;
				msg_send.id = 33;
				msg_send.len = 8;
				msg_send.data[0] = rand()%255;
				msg_send.data[1] = rand()%255;
				msg_send.data[2] = rand()%255;
				msg_send.data[3] = rand()%255;
				msg_send.data[4] = rand()%255;
				msg_send.data[5] = rand()%255;
				msg_send.data[6] = rand()%255;
				msg_send.data[7] = rand()%255;
				CAN_send(2, &msg_send, 1000);
				
				/* 重启定时器 */
				WM_RestartTimer(pMsg->Data.v, 500);
			}
			break;
		
		/* 接收到CAN数据 */
		case MSG_CANReceive:
			hTimerCAN = WM_CreateTimer(pMsg->hWin, ID_TimerCAN, 0, 0);	
			break;
			
        case WM_NOTIFY_PARENT:
            Id = WM_GetId(pMsg->hWinSrc); 
            NCode = pMsg->Data.v;        
            switch (Id) 
            {
				case ID_BUTTON_0:
                    switch(NCode)
                    {
						/* 发送数据帧 */
                        case WM_NOTIFICATION_CLICKED:
							msg_send.type = DATA_FRAME;
							msg_send.id = 33;
							msg_send.len = 8;
							msg_send.data[0] = rand()%255;
							msg_send.data[1] = rand()%255;
							msg_send.data[2] = rand()%255;
							msg_send.data[3] = rand()%255;
							msg_send.data[4] = rand()%255;
							msg_send.data[5] = rand()%255;
							msg_send.data[6] = rand()%255;
							msg_send.data[7] = rand()%255;
							CAN_send(2, &msg_send, 1000);
                            break;
                    }
                    break;
					
				case ID_BUTTON_1:
                    switch(NCode)
                    {
						/* 发送遥控帧 */
                        case WM_NOTIFICATION_CLICKED:
							msg_send.type = REMOTE_FRAME;
							msg_send.id = 33;
							CAN_request(2, &msg_send, 1000);
                            break;
                    }
                    break;
					
				case ID_BUTTON_2:
                    switch(NCode)
                    {
						/* 启动周期性发送数据帧 */
                        case WM_NOTIFICATION_CLICKED:
							hTimerTime = WM_CreateTimer(pMsg->hWin, ID_TimerTime, 0, 0);
                            break;
                    }
                    break;
					
				case ID_BUTTON_3:
                    switch(NCode)
                    {
						/* 停止周期性发送数据帧 */
                        case WM_NOTIFICATION_CLICKED:
							WM_DeleteTimer(hTimerTime);
                            break;
                    }
                    break;
					
				case ID_BUTTON_4:
                    switch(NCode)
                    {
						/* 清空ListView的所有行 */
                        case WM_NOTIFICATION_CLICKED:
							hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_LISTVIEW0);	
							j = LISTVIEW_GetNumRows(hItem);

						    #if 1
								for(i = 0; i < j; i++)
								{
									LISTVIEW_DeleteRow(hItem, 0);
								}
							#else
								for(i = j - 1; i >=0; i--)
								{
									LISTVIEW_DeleteRow(hItem, i);
								}
							#endif
								
							s_uiCount = 0;
                            break;
                    }
                    break;
					
				/* 关闭USB窗口 */
				case ID_BUTTON_5:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_RELEASED:
						    hWinCAN = WM_HWIN_NULL;
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
*	函 数 名: App_CAN
*	功能说明: 创建CAN控制对话框
*	形    参: hWin  父窗口
*	返 回 值: 无
*********************************************************************************************************
*/
void App_CAN(WM_HWIN hWin)
{					 
    hWinCAN = GUI_CreateDialogBox(_aDialogCreateCAN, 
	                              GUI_COUNTOF(_aDialogCreateCAN), 
	                              &_cbCallbackCAN, 
	                              hWin, 
	                              0, 
	                              0);
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

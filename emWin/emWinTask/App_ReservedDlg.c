/*
*********************************************************************************************************
*	                                  
*	模块名称 : 保留对话框界面设计
*	文件名称 : App_ReservedDlg.c
*	版    本 : V1.0
*	说    明 : 未使用的应用全部使用此对话框
*
*	修改记录 :
*		版本号    日期         作者         说明
*       V1.0    2019-12      果果小师弟     首发
*
*	Copyright (C), 2020-2025, https://space.bilibili.com/357524621 
*                                     
*********************************************************************************************************
*/

#include "MainTask.h"




/*
*********************************************************************************************************
*                                       宏定义
*********************************************************************************************************
*/
#define MAG          3
#define ID_WINDOW_0 	(GUI_ID_USER + 0x00)
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
*	               对话框控件列表
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateReserved[] = {
	{ WINDOW_CreateIndirect, "Window",      ID_WINDOW_0,    0,    0,   800, 480, 0, 0x0, 0},
	{ BUTTON_CreateIndirect, "DlgBack",     ID_BUTTON_0,   700,    0,  100, 100, 0, 0, 0},	
	{ BUTTON_CreateIndirect, "MusicList",   ID_BUTTON_1,     0,    0,  100, 100, 0, 0, 0},
};

/*
*********************************************************************************************************
*	函 数 名: InitDialogSys
*	功能说明: 对话框回调函数的初始化消息
*	形    参：pMsg   消息指针变量
*	返 回 值: 无
*********************************************************************************************************
*/
static void InitDialogReserved(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
	WM_SetCallback(hItem, _cbButtonBack);
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
	WM_SetCallback(hItem, _cbButtonList);

}

/*
*********************************************************************************************************
*	函 数 名: _cbCallbackReserved
*	功能说明: 回调函数
*	形    参: pMsg   消息指针变量
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbCallbackReserved(WM_MESSAGE * pMsg) 
{

	WM_HWIN hWin = pMsg->hWin;
		 int NCode, Id;
    switch (pMsg->MsgId) 
    {
		case WM_INIT_DIALOG:
            InitDialogReserved(pMsg);
            break;
		
		case WM_PAINT:
			GUI_SetBkColor(GUI_WHITE);
			GUI_Clear();
			/* 绘制破折号直线 */
			GUI_SetColor(GUI_BLACK);
			GUI_SetLineStyle(GUI_LS_DASH);
			GUI_DrawLine(70, 10, 170, 110);
			/* 绘制点直线 */
			GUI_SetLineStyle(GUI_LS_DOT);
			GUI_DrawLine(50, 10, 170, 130);
			/* 绘制实心直线 */
			GUI_SetLineStyle(GUI_LS_SOLID);
			GUI_DrawLine(30, 10, 170, 150);
			GUI_SetPenSize(4);
			GUI_DrawLine(10, 10, 170, 170);
			/* 绘制矩形 */
			GUI_SetColor(GUI_BLUE);
			GUI_DrawRect(210, 10, 290, 90);
			GUI_FillRect(310, 10, 390, 90);
			/* 绘制圆角矩形 */
			GUI_SetColor(GUI_ORANGE);
			GUI_DrawRoundedFrame(210, 110, 290, 190, 20, 8);
			GUI_FillRoundedRect(310, 110, 390, 190, 20);
			/* 绘制渐变色圆角矩形 */
			GUI_DrawGradientRoundedV(410, 10, 490, 190, 20, GUI_LIGHTMAGENTA,GUI_LIGHTCYAN);
			
			case WM_NOTIFY_PARENT://这个消息是对话框回调函数的重点，所有对话框子控件的具体行为逻辑都
			                     //在此消息中设置和处理
            Id = WM_GetId(pMsg->hWinSrc); 
            NCode = pMsg->Data.v;        
            switch (Id) 
            {
				/* 关闭对话框 */
				case ID_BUTTON_0:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_RELEASED://当被单击的控件被释放时，将发送此通知消息
							  GUI_EndDialog(hWin, 0);//结束一个对话框 就是点击返回那一个按钮 返回到主界面
                             break;
                    }
                    break;
					
				case ID_BUTTON_1:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED://按钮已被点击 就是点击右上方那一个按钮 没反应
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
*	函 数 名: App_Reserved
*	功能说明: 未使用的应用全部使用此对话框
*	形    参: hWin 所创建对话框 	
*	返 回 值: 无
*********************************************************************************************************
*/


void App_Reserved(WM_HWIN hWin) 
{
	GUI_CreateDialogBox(_aDialogCreateReserved, 
	                    GUI_COUNTOF(_aDialogCreateReserved), 
	                    &_cbCallbackReserved, 
	                    hWin, 
	                    0, 
	                    0);
}



/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

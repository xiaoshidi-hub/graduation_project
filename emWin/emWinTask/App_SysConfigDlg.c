/*
*********************************************************************************************************
*	                                  
*	模块名称 : 系统信息界面设计
*	文件名称 : App_SysConfig.c
*	版    本 : V1.0
*	说    明 : 系统信息界面设计。
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

#define MAG          3
#define ID_WINDOW_0 	  (GUI_ID_USER + 0x00)
#define ID_BUTTON_0       (GUI_ID_USER + 0x01)
#define ID_BUTTON_1       (GUI_ID_USER + 0x02)
#define ID_TEXT_2         (GUI_ID_USER + 0xA0)
#define ID_TEXT_3         (GUI_ID_USER + 0xB0)


GUI_HMEM hMemQR;

extern GUI_CONST_STORAGE GUI_BITMAP bmLOGO_PNG;
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
*	功能说明: 按钮设置回调函数 左上方那一个按钮，是自定义按钮
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
			if (BUTTON_IsPressed(hWin)) //返回按钮是否被按下
			{
				GUI_SetColor(GUI_DARKGRAY);
				GUI_AA_FillCircle(0, 0, 72);//画一个圆
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
			BUTTON_Callback(pMsg);//除了 WM_PAINT 消息以外，其它没用上的所有消息都需要还给按钮控件默认的回调
                                  //函数 BUTTON_Callback 进行处理，否则整个按钮功能就得乱套了。
	}
}
/*
*********************************************************************************************************
*	               对话框控件列表
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateSys[] = 
{

  { WINDOW_CreateIndirect, "Window",      ID_WINDOW_0,    0,    0,   800, 480, 0, 0x0, 0},
  
  { BUTTON_CreateIndirect, "DlgBack",     ID_BUTTON_0,   700,    0,  100, 100, 0, 0, 0},	
  { BUTTON_CreateIndirect, "MusicList",   ID_BUTTON_1,     0,    0,  100, 100, 0, 0, 0},
  
  { TEXT_CreateIndirect,      "",          ID_TEXT_2,       12,   80, 400,24, 0,0},  
  { TEXT_CreateIndirect,      " ",  	   ID_TEXT_3,        12,   440, 797, 100, 0, 0},


};
/*
*********************************************************************************************************
*	函 数 名: InitDialogSys
*	功能说明: 对话框回调函数的初始化消息
*	形    参：pMsg   消息指针变量
*	返 回 值: 无
*********************************************************************************************************
*/
static void InitDialogSys(WM_MESSAGE * pMsg) 
{	
	WM_HWIN hWin = pMsg->hWin;
	WM_HWIN hItem;

	hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);//通过 WM_GetDialogItem 函数来自动建立并获取按钮的句柄
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonBack);//修改控件回调函数的方式自定义 Button
                                         //的外观，用 WM_SetCallback 函数进行重定向
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonList);
	
	
	hItem = WM_GetDialogItem(hWin, ID_TEXT_2);	
	TEXT_SetTextColor(hItem, GUI_RED);
	TEXT_SetFont(hItem, &GUI_SysFontHZ16);
	TEXT_SetText(hItem, "固件版本");

	hItem = WM_GetDialogItem(hWin, ID_TEXT_3);
	TEXT_SetTextColor(hItem, GUI_RED);
	TEXT_SetFont(hItem, &GUI_FontHZ16);
	TEXT_SetText(hItem, "毕业设计");	
//	TEXT_SetText(hItem, "[1]世上无难事，只怕有心人\n"
//						"[2]本设计是刘尧2020年毕业设计。请多指教\n");
	
	/* 设置抗锯齿因数 */
	GUI_AA_SetFactor(MAG);
	hMemQR = GUI_QR_Create("https://space.bilibili.com/357524621", 8, GUI_QR_ECLEVEL_H, 0);/* 创建二维码对象 */
	
 
}
/*
*********************************************************************************************************
*	函 数 名: _cbCallbackSys
*	功能说明: 回调函数
*	形    参: pMsg   消息指针变量
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbCallbackSys(WM_MESSAGE * pMsg) 
{
    GUI_QR_INFO Info;
	 int NCode, Id;
    WM_HWIN hWin = pMsg->hWin;
	
    switch (pMsg->MsgId) 
    {
	  case WM_INIT_DIALOG:
            InitDialogSys(pMsg);
		break;
	  case WM_PAINT:  //窗口重绘消息,这个比较难说明白,反正在Framewin或Window窗口之中我们一般是用控
	                  //件,如果要在Framewin或Window窗口之中显示文字或绘制直线、矩形、圆等在这里实现
		
			GUI_SetBkColor(GUI_LIGHTCYAN);
			GUI_Clear();   //修改完桌面背景颜色后必须清清屏，否则修改不成功
			GUI_SetColor(GUI_BLUE);
			GUI_SetFont(GUI_FONT_24B_1);
			GUI_DispStringAt("Board:STM32F429BIT6",       12, 106);
			GUI_DispStringAt("Core:Cortex-M4",            12, 138);
			GUI_DispStringAt("CPU Speed:186MHz",          12, 170);
			GUI_DispStringAt("RTOS:RL_RTX",               12, 203);
			GUI_DispStringAt("GUI:emWin V5.40",           12, 235);
			GUI_DispStringAt("File System:RL-FlashFS",    12, 267);
			GUI_DispStringAt("Firmware version:1.0.0",    12, 300);
			GUI_DispStringAt("Modbus:Designed by liuyao", 12, 332);
	 
	
	        GUI_SetColor(GUI_BLUE);
	  	    GUI_DrawBitmap(&bmLOGO_PNG,700,400);     //显示自己的QQ头像

	  	    GUI_QR_GetInfo(hMemQR, &Info);
			GUI_QR_Draw(hMemQR, 450, 100);  
	
	       break;
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
							  GUI_QR_Delete(hMemQR);
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
            WM_DefaultProc(pMsg);//所有我们不关心或者没有用到的系统消息都可以调用默认消息处理函数
                                //WM_DefaultProc 进行处理

    }
}


/*
*********************************************************************************************************
*	函 数 名: App_SysConfig
*	功能说明: 系统信息对话框
*	形    参: hWin 所创建对话框 	
*	返 回 值: 无
*********************************************************************************************************
*/
void App_SysConfig(WM_HWIN hWin) 
{
	GUI_CreateDialogBox(_aDialogCreateSys, 
	                    GUI_COUNTOF(_aDialogCreateSys), 
	                    &_cbCallbackSys, 
	                    hWin, 
	                    0, 
	                    0);
}


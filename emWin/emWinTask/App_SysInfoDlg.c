/*
*********************************************************************************************************
*	                                  
*	模块名称 : 主界面系统信息对话框
*	文件名称 : App_SysInfo.c
*	版    本 : V1.0
*	说    明 : 本对话框悬浮在主界面上。
*              1. 显示当前IP地址，未插入网线时显示192.168.X.X
*              2. 显示emWin动态内存的剩余，总共是分配了12MB。
*              3. 显示CPU利用率，上电后有段时间是显示的0.0%，因为网络初始化占用了一段时间CPU，不插网线的话
*                 时间稍长，过会就正常了。
*              4. 电压和电流的测试功能未加上。
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
*                                        宏定义和外部变量应用
*********************************************************************************************************
*/ 
#define MAG          3
extern uint8_t g_ucNETDetect;


/*
*********************************************************************************************************
*	函 数 名: _cbDialogSysInfo
*	功能说明: 回调函数
*	形    参: pMsg  WM_MESSAGE类型指针变量           
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbDialogSysInfo(WM_MESSAGE * pMsg) 
{
	GUI_RECT pRect;
	char buf[40];

	switch (pMsg->MsgId) 
	{
		case WM_TIMER:
			WM_InvalidateWindow(pMsg->hWin);
			WM_RestartTimer(pMsg->Data.v, 1000);
			break;

		case WM_PAINT:
			GUI_SetColor(0x007C18CC);
			WM_GetClientRect(&pRect);
			GUI_AA_FillRoundedRect(pRect.x0, pRect.y0, pRect.x1, pRect.y1, 6);
			GUI_SetTextMode(GUI_TEXTMODE_TRANS);
			GUI_SetColor(GUI_WHITE);
			GUI_SetFont(GUI_FONT_13_1);
			if(g_ucNETDetect == 1)
			{

				GUI_DispStringAt(buf, 3, 3);				
			}
			else
			{
				sprintf(buf, "IP: 192.168.X.X");
				GUI_DispStringAt(buf, 3, 3);					
			}
			
			#if 0
				sprintf(buf, "emWin: %uKB/8MB", GUI_ALLOC_GetNumFreeBytes()>>10);
			#else
				sprintf(buf, "emWin: %dB", (int)GUI_ALLOC_GetNumFreeBytes());
			#endif
			
			GUI_DispStringAt(buf, 3, 17);
			
			sprintf(buf, "CPU: %5.2f%%", OSCPUUsage);
			GUI_DispStringAt(buf, 3, 31);
			GUI_DispStringAt("V: 12.2V", 3, 45);
			GUI_DispStringAt("A: 312mA", 57, 45);
			break;

		case WM_CREATE:
			/* 设置抗锯齿因数 */
			GUI_AA_SetFactor(MAG);
			WM_CreateTimer(pMsg->hWin, 0, 1000, 0);	         
			break;

		default:
			WM_DefaultProc(pMsg);
			break;
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
void CreateStatusDlg(void) 
{
	WM_CreateWindow(690, 
	                345, 
	                109, 
	                62, 
	                WM_CF_SHOW | WM_CF_HASTRANS,
                     _cbDialogSysInfo, 
	                0);
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

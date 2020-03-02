/*
*********************************************************************************************************
*
*	模块名称 : TCPnet网络协议栈测试界面。
*	文件名称 : App_TCPnetDlg.c
*	版    本 : V1.0
*	说    明 : 主要显示网络信息，SNTP客户端信息和嵌入式web信息，每秒更新一次。
*              1. 移植了RL-TCPnet网络协议栈，网口对应DM9161，务必将其连接到能联网的路由或交换机上，否则很
	              多功能无法正常运行，网线可以随意插拔，支持自动重连。网络信息和SNTP客户端都是每秒更新一次。
			   2. DHCP已经使能，如果插上网线8秒内无法从路由器/交换机获得IP地址，将使用固定IP:192.168.1.200。
			   3. 国内免费稳定的SNTP服务器很少了，当前使用的这个时好时坏，如果大家测试不成功的话也是正常的。
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
#define ID_TEXT_2      (GUI_ID_USER + 0x03)
#define ID_TEXT_3      (GUI_ID_USER + 0x04)
#define ID_BUTTON_0    (GUI_ID_USER + 0x05)
#define ID_BUTTON_1    (GUI_ID_USER + 0x06)
#define ID_BUTTON_2    (GUI_ID_USER + 0x07)
#define ID_BUTTON_3    (GUI_ID_USER + 0x08)
#define ID_BUTTON_4    (GUI_ID_USER + 0x09)
#define ID_BUTTON_5    (GUI_ID_USER + 0x0a)
#define ID_BUTTON_6    (GUI_ID_USER + 0x0b)

extern  LOCALM localm[];
#define LocalM   localm[NETIF_ETH]


/*
*********************************************************************************************************
*                                       变量
*********************************************************************************************************
*/
WM_HWIN  hWinNET = WM_HWIN_NULL;  	/* 网络窗口句柄 */

char *SNTP_Status[] =
{
	"SNTP请求已经发送成功",
	"失败, SNTP未就绪或者参数错误",
	"错误, 服务器未响应或者网络状态比较差"
};

char *NetCable_Status[] =
{
	"断开",
	"连接",
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
*	                                 对话框控件列表
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateNet[] = 
{
	{ WINDOW_CreateIndirect,  "Window", ID_WINDOW_0,   0,   0,  800, 480, 0, 0x0, 0 },
	{ TEXT_CreateIndirect,      " ",  	ID_TEXT_0,     80, 55,  630, 150, 0, 0},
	{ TEXT_CreateIndirect,      " ",  	ID_TEXT_1,     80, 225,  630, 80, 0, 0},
	{ TEXT_CreateIndirect,      " ",  	ID_TEXT_2,     80, 335,  630, 60, 0, 0},
	{ TEXT_CreateIndirect,      " ",  	ID_TEXT_3,     3, 410,  790, 70, 0, 0},

	{ BUTTON_CreateIndirect,  "DlgBack",   ID_BUTTON_5,  700,   0, 100, 100, 0, 0, 0 },	
	{ BUTTON_CreateIndirect,  "MusicList", ID_BUTTON_6,    0,   0, 100, 100, 0, 0, 0 },
};

/*
*********************************************************************************************************
*	函 数 名: UpdateDisp
*	功能说明: 更新显示
*	形    参: pMsg 指针参数            
*	返 回 值: 无
*********************************************************************************************************
*/
static void UpdateDisp(WM_MESSAGE * pMsg)
{
	char buf[200];
	WM_HWIN hWin = pMsg->hWin;
	WM_HWIN hItem;
	int j;
	char *p;

	
	hItem = WM_GetDialogItem(hWin, ID_TEXT_1);

	sprintf(buf, "服务器时间：%02d:%02d:%02d\n", (int)g_time.tm_hour, (int)g_time.tm_min, (int)g_time.tm_sec);

	j = strlen(buf);
	p = &buf[j];
	sprintf(p, 
			"服务器日期：%04d/%02d/%02d\n", 
			g_time.tm_year, 
			g_time.tm_mon, 
			g_time.tm_mday);

	j = strlen(buf);
	p = &buf[j];
	sprintf(p, "SNTP客户端获取状态: %s\n", SNTP_Status[g_SNTPStatus]);

	j = strlen(buf);
	p = &buf[j];
	sprintf(p, "通过访问远程SNTP服务器IP: 182.16.3.162 获取时间和日期 \n");

	TEXT_SetText(hItem, buf);


	hItem = WM_GetDialogItem(hWin, ID_TEXT_0);

	////////////////////////
	sprintf(buf, "网线通断: %s\n", NetCable_Status[g_ucEthLinkStatus]);

	j = strlen(buf);
	p = &buf[j];
	sprintf(p, "DHCP状态: %s\nNetBIOS名称服务器: Enable\n", DHCP_Status[1]);

	j = strlen(buf);
	p = &buf[j];
	sprintf(p, "IP地址: %s\n", DHCP_Status[0]);

	j = strlen(buf);
	p = &buf[j];
	sprintf(p, "子网掩码: %d.%d.%d.%d\n", LocalM.NetMask[0],
										  LocalM.NetMask[1],
										  LocalM.NetMask[2],
										  LocalM.NetMask[3]);

	j = strlen(buf);
	p = &buf[j];
	sprintf(p, "子网掩码: %d.%d.%d.%d\n", LocalM.DefGW[0],
										  LocalM.DefGW[1],
										  LocalM.DefGW[2],
										  LocalM.DefGW[3]);	
										  
	j = strlen(buf);
	p = &buf[j];
	sprintf(p, "首选DNS服务器: %d.%d.%d.%d\n", LocalM.PriDNS[0],
											   LocalM.PriDNS[1],
											   LocalM.PriDNS[2],
											   LocalM.PriDNS[3]);

	j = strlen(buf);
	p = &buf[j];
	sprintf(p, "备选DNS服务器: %d.%d.%d.%d\n", LocalM.SecDNS[0],
											   LocalM.SecDNS[1],
											   LocalM.SecDNS[2],
											   LocalM.SecDNS[3]);	
	
	TEXT_SetText(hItem, buf);
}

/*
*********************************************************************************************************
*	函 数 名: InitDialogNet
*	功能说明: 对话框回调函数的初始化消息
*	形    参: pMsg  消息指针变量
*	返 回 值: 无
*********************************************************************************************************
*/
static void InitDialogNet(WM_MESSAGE * pMsg)
{
    WM_HWIN hWin = pMsg->hWin;
	WM_HWIN hItem;
	

	/* 设置抗锯齿因数 */
	GUI_AA_SetFactor(MAG);

	hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_5);
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonBack);
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_6);
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonList);
	
	hItem = WM_GetDialogItem(hWin, ID_TEXT_0);
	TEXT_SetTextColor(hItem, GUI_BLACK);
	TEXT_SetFont(hItem, &GUI_FontHZ16);
    TEXT_SetText(hItem, "网线通断: 断开\n"
						"DHCP状态: 失败\n"
						"NetBIOS名称服务器: Enable\n"
						"IP地址: 192.168.1.200\n"
	                    "子网掩码: 255.255.255.0\n"
						"默认网关: 192.168.1.1\n"
						"首选DNS服务器: 233.5.5.5\n"
						"备选DNS服务器: 233.5.5.6\n");
	
	hItem = WM_GetDialogItem(hWin, ID_TEXT_1);
	TEXT_SetTextColor(hItem, GUI_BLACK);
	TEXT_SetFont(hItem, &GUI_FontHZ16);
    TEXT_SetText(hItem, "服务器时间: 15:04:59\n"
						"服务器日期: 2017/02/22\n"
	                    "SNTP客户端获取状态: 失败\n"
						"通过访问远程SNTP服务器IP: 182.16.3.162 获取时间和日期 \n");
						
	
	hItem = WM_GetDialogItem(hWin, ID_TEXT_2);
	TEXT_SetTextColor(hItem, GUI_BLACK);
	TEXT_SetFont(hItem, &GUI_FontHZ16);
    TEXT_SetText(hItem, "Web服务器用户名: admin 密码: 123456\n"
						"用户可浏览器中直接输入IP地址访问Web服务器，IP地址在上面网络信息栏中有给出，\n"
	                    "也可以通过NetBIOS局域网域名直接访问，即http://stm32-v6/，这样就无需查看具体IP了。\n");
		
	/* 初始化文本 */
	hItem = WM_GetDialogItem(hWin, ID_TEXT_3);
	TEXT_SetTextColor(hItem, GUI_BLACK);
	TEXT_SetFont(hItem, &GUI_FontHZ16);
    TEXT_SetText(hItem, "[1]移植了RL-TCPnet网络协议栈，网口对应DM9161，务必将其连接到能联网的路由或交换机上，否则很 \n"
	                    "   多功能无法正常运行，网线可以随意插拔，支持自动重连。网络信息和SNTP客户端都是每秒更新一次。\n"
						"[2]DHCP已经使能，如果插上网线8秒内无法从路由器/交换机获得IP地址，将使用固定IP:192.168.1.200。\n"
						"[3]国内免费稳定的SNTP服务器很少了，当前使用的这个时好时坏，如果大家测试不成功的话也是正常的。\n");

	WM_CreateTimer(hWin, /* 接受信息的窗口的句柄 */
				   0, 	 /* 用户定义的Id。如果不对同一窗口使用多个定时器，此值可以设置为零。 */
				   0,    /* 周期，此周期过后指定窗口应收到消息*/
				   0);	 /* 留待将来使用，应为0 */
}

/*
*********************************************************************************************************
*	函 数 名: _cbCallbackNet
*	功能说明: 回调函数
*	形    参: pMsg   消息指针变量
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbCallbackNet(WM_MESSAGE * pMsg) 
{
    int NCode, Id;
	
	
    switch (pMsg->MsgId) 
    {
        case WM_INIT_DIALOG:
            InitDialogNet(pMsg);
            break;
		
		case WM_TIMER:
			/* 显示时间和日期 */
			UpdateDisp(pMsg);
		
			/* 重启定时器 */
			WM_RestartTimer(pMsg->Data.v, 1000);
			break;
		
		case WM_PAINT:
			GUI_SetColor(GUI_RED);
			GUI_AA_DrawRoundedRect(70, 40, 660+70, 150+40, 6);
			GUI_SetFont(&GUI_FontHZ16);
			GUI_DispStringAt("网络信息", 90, 32);
		
			GUI_AA_DrawRoundedRect(70, 210, 660+70, 90+210, 6);
			GUI_SetFont(&GUI_FontHZ16);
			GUI_DispStringAt("SNTP客户端", 90, 202);
		
			GUI_AA_DrawRoundedRect(70, 320, 660+70, 70+320, 6);
			GUI_SetFont(&GUI_FontHZ16);
			GUI_DispStringAt("嵌入式Web服务器", 90, 312);
			break;
			
        case WM_NOTIFY_PARENT:
            Id = WM_GetId(pMsg->hWinSrc); 
            NCode = pMsg->Data.v;        
            switch (Id) 
            {
				/* 关闭TCPnet窗口 */
				case ID_BUTTON_5:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_RELEASED:
						    hWinNET = WM_HWIN_NULL;
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
*	函 数 名: App_TCPnet
*	功能说明: 创建网络对话框
*	形    参: hWin  父窗口
*	返 回 值: 无
*********************************************************************************************************
*/
void App_TCPnet(WM_HWIN hWin) 
{				 
    hWinNET = GUI_CreateDialogBox(_aDialogCreateNet,
								  GUI_COUNTOF(_aDialogCreateNet), 
								  &_cbCallbackNet, 
								  hWin, 
								  0, 
								  0);
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

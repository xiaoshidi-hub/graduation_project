/*
*********************************************************************************************************
*	                                  
*	模块名称 : 收音机界面设计
*	文件名称 : App_RadioDlg.c
*	版    本 : V1.0
*	说    明 : 收音机界面设计。
*              1. 为了更好搜索电台，务必给耳机接口插上耳机当做收音机天线用。
*              2. 开机上电后，要先点击界面上的开启键，默认是自动搜索模式，开启后点击左下角的搜索键，可以
*                 自动搜索电台。通过左键和右键实现搜到的电台切换。界面上面的6个颜色块是搜索到电台显示，
*                 也可以点击切换。
*              3. 界面左上角的按钮是切换到手动搜索，然后通过左键和右键分别实现每次递减0.1MHz和递增0.1MHz。
*                 要切换回自动搜索，点击左下角按钮即可。手动搜索模式下，滚动条上面的文本会显示Manual，自动
*                 搜索模式下，滚动条上面的文本会显示Auto，作为两种模式的区分。
*              4. 调整滚动条也可以选择电台，范围87.5MHz到108.0MHz。
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
#define ID_WINDOW_0 		(GUI_ID_USER + 0x00)
#define ID_IMAGE_0  		(GUI_ID_USER + 0x01)
#define ID_SLIDER_0 		(GUI_ID_USER + 0x02)
#define ID_BUTTON_0 		(GUI_ID_USER + 0x03)
#define ID_BUTTON_1 		(GUI_ID_USER + 0x04)
#define ID_BUTTON_2 		(GUI_ID_USER + 0x05)
#define ID_BUTTON_3 		(GUI_ID_USER + 0x06)
#define ID_BUTTON_4 		(GUI_ID_USER + 0x07)
#define ID_BUTTON_5 		(GUI_ID_USER + 0x08)
#define ID_BUTTON_6 		(GUI_ID_USER + 0x09)
#define ID_SLIDER_1  	    (GUI_ID_USER + 0x0A)
#define ID_TEXT_0 			(GUI_ID_USER + 0x0B)
#define ID_TEXT_1 			(GUI_ID_USER + 0x0C)
#define ID_TEXT_2 			(GUI_ID_USER + 0x0D)
#define ID_TEXT_3 			(GUI_ID_USER + 0x0E)
#define ID_TEXT_4 			(GUI_ID_USER + 0x0F)
#define ID_TEXT_5 			(GUI_ID_USER + 0x10)

#define ID_EDIT_0 			(GUI_ID_USER + 0x11)
#define ID_EDIT_1 			(GUI_ID_USER + 0x12)
#define ID_EDIT_2 			(GUI_ID_USER + 0x13)
#define ID_EDIT_3 			(GUI_ID_USER + 0x14)
#define ID_EDIT_4 			(GUI_ID_USER + 0x15)
#define ID_EDIT_5 			(GUI_ID_USER + 0x16)

#define ID_BUTTON_7 		(GUI_ID_USER + 0x17)
#define ID_TEXT_6			(GUI_ID_USER + 0x18)

/* 不同定时器的句柄 */
#define ID_TIMER_PROCESS    1


/*
*********************************************************************************************************
*				                    引用外部变量和函数
*********************************************************************************************************
*/
/* 声明一个支持5个消息的消息邮箱 */
extern os_mbx_declare (mailbox, 5);
extern MusicMsg_T s_tMusicMsg;         /* 用于给音乐播放任务发消息来控制收音机 */


/*
*********************************************************************************************************
*				                         变量
*********************************************************************************************************
*/
static uint8_t s_ucPlayStatus = 0; 	    /* 收音机的开始和暂停按钮状态，0表示暂停，1表示运行 */
static uint8_t s_ucSeekMode = 1; 	    /* 1 表示自动搜索模式，0 表示手动搜索模式 */    

/* EDIT控件的颜色 */
static const GUI_COLOR EditColors[] =
{
	0x0061ff, 0xE16941, 0xF020A0, 0xA020F0, 0x40913D, 0x00D7FF
};


/*
*********************************************************************************************************
*				                         任务对话框初始化选项
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateRadio[] = 
{
	{ WINDOW_CreateIndirect, "Window",  ID_WINDOW_0,   0,   0,  800, 480, 0, 0x0, 0 },
	{ SLIDER_CreateIndirect, "Process", ID_SLIDER_0, 100, 365,  600,  16, 0, 0x0, 0 },

	/* 按钮功能 */
	{ BUTTON_CreateIndirect, "DlgBack",    ID_BUTTON_0,  700,   0,  100, 100, 0, 0, 0 },	
	{ BUTTON_CreateIndirect, "MusicPre",   ID_BUTTON_1,  100, 399,   72,  72, 0, 0, 0 },
	{ BUTTON_CreateIndirect, "MusicPlay",  ID_BUTTON_2,  200, 399,   72,  72, 0, 0, 0 },
	{ BUTTON_CreateIndirect, "MusicNext",  ID_BUTTON_4,  300, 399,   72,  72, 0, 0, 0 },
	{ BUTTON_CreateIndirect, "MusicSpk",   ID_BUTTON_5,  400, 399,   72,  72, 0, 0, 0 },
	{ BUTTON_CreateIndirect, "MusicList",  ID_BUTTON_6,   0,   0,  100, 100, 0, 0, 0 },
	
	{ BUTTON_CreateIndirect, "MusicSeek",  ID_BUTTON_7,   0,   380,  100, 100, 0, 0, 0 },
	
	/* 声音大小 */
	{ SLIDER_CreateIndirect, "Speaker",    ID_SLIDER_1, 490, 425, 200, 20, 0, 0x0, 0  },
	
	/* 显示歌曲信息 */
	{ TEXT_CreateIndirect, "87.5MHz",        ID_TEXT_0, 100, 345, 270, 20, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "--/--",          ID_TEXT_1, 480, 345, 50, 20, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "108.0MHz",       ID_TEXT_2, 620, 345, 100, 20, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "RSSI = --dB",    ID_TEXT_3, 200, 345, 100, 20, 0, 0x64, 0 },
	
	{ TEXT_CreateIndirect, "SNR = --dB",     ID_TEXT_4, 320, 345, 120, 20, 0, 0x64, 0 },	

	{ TEXT_CreateIndirect, "87.5",           ID_TEXT_5, 250, 120, 300, 100, 0, 0x64, 0 },
	
	{ TEXT_CreateIndirect, "Auto",          ID_TEXT_6, 530, 345, 90, 20, 0, 0x64, 0 },

	{ EDIT_CreateIndirect, "Edit", ID_EDIT_0, 110, 22, 80, 60, 0, 0x64, 0 },
	{ EDIT_CreateIndirect, "Edit", ID_EDIT_1, 210, 22, 80, 60, 0, 0x64, 0 },
	{ EDIT_CreateIndirect, "Edit", ID_EDIT_2, 310, 22, 80, 60, 0, 0x64, 0 },
	{ EDIT_CreateIndirect, "Edit", ID_EDIT_3, 410, 22, 80, 60, 0, 0x64, 0 },
	{ EDIT_CreateIndirect, "Edit", ID_EDIT_4, 510, 22, 80, 60, 0, 0x64, 0 },
	{ EDIT_CreateIndirect, "Edit", ID_EDIT_5, 610, 22, 80, 60, 0, 0x64, 0 },
};

/*
*********************************************************************************************************
*	函 数 名: _cbButtonSeek
*	功能说明: 按钮回调函数
*	形    参: pMsg  消息指针
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbButtonSeek(WM_MESSAGE * pMsg) 
{
	WM_HWIN  hWin;

	hWin  = pMsg->hWin;
	switch (pMsg->MsgId) 
	{
		case WM_PAINT:
			if (BUTTON_IsPressed(hWin)) 
			{
				GUI_SetColor(GUI_DARKGRAY);
				GUI_AA_FillCircle(0, 100, 72);
				GUI_SetBkColor(GUI_DARKGRAY);
				GUI_SetColor(GUI_WHITE); 
				GUI_DrawBitmap(&bmRadioSearch, 8, 50);
			} 
			else 
			{
				GUI_SetColor(GUI_STCOLOR_LIGHTBLUE);
				GUI_AA_FillCircle(0, 100, 72);
				GUI_SetBkColor(GUI_STCOLOR_LIGHTBLUE);
				GUI_SetColor(GUI_WHITE);  
				GUI_DrawBitmap(&bmRadioSearch, 8, 50);
			}
			
			break;
			
		default:
			BUTTON_Callback(pMsg);
	}
}


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
*	函 数 名: _cbButtonPre
*	功能说明: 按钮回调函数
*	形    参: pMsg  消息指针
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbButtonPre(WM_MESSAGE * pMsg) 
{
	WM_HWIN  hWin;

	hWin  = pMsg->hWin;
	switch (pMsg->MsgId) 
	{
		case WM_PAINT:
			if (BUTTON_IsPressed(hWin)) 
			{
				GUI_SetBkColor(GUI_WHITE);
				GUI_Clear();
				GUI_SetAlpha(0xb0);
				GUI_DrawBitmap(&bmMusicPre, 8, 8);
				GUI_SetAlpha(0);		
			} 
			else 
			{
				GUI_DrawBitmap(&bmMusicPre, 8, 8);		
			}
			break;
			
		default:
			BUTTON_Callback(pMsg);
	}
}

/*
*********************************************************************************************************
*	函 数 名: _cbButtonPlay
*	功能说明: 按钮回调函数
*	形    参: pMsg  消息指针
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbButtonPlay(WM_MESSAGE * pMsg) 
{
	switch (pMsg->MsgId) 
	{
		case WM_PAINT:
			if(s_ucPlayStatus == 0)
			{
				GUI_DrawBitmap(&bmMusicPlay, 8, 8);
			}
			else 
			{
				GUI_DrawBitmap(&bmMusicPause, 8, 8);
			}
			break;
			
		default:
			BUTTON_Callback(pMsg);
			break;
	}
}

/*
*********************************************************************************************************
*	函 数 名: _cbButtonNext
*	功能说明: 按钮回调函数
*	形    参: pMsg  消息指针
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbButtonNext(WM_MESSAGE * pMsg) 
{
	WM_HWIN  hWin;

	hWin  = pMsg->hWin;
	switch (pMsg->MsgId) 
	{
		case WM_PAINT:
			if (BUTTON_IsPressed(hWin)) 
			{
				GUI_SetBkColor(GUI_WHITE);
				GUI_Clear();
				GUI_SetAlpha(0xb0);
				GUI_DrawBitmap(&bmMusicNext, 8, 8);
				GUI_SetAlpha(0);		
			} 
			else 
			{
				GUI_DrawBitmap(&bmMusicNext, 8, 8);		
			}
			break;
			
		default:
			BUTTON_Callback(pMsg);
	}
}

/*
*********************************************************************************************************
*	函 数 名: _cbButtonSpeaker
*	功能说明: 按钮回调函数
*	形    参: pMsg  消息指针
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbButtonSpeaker(WM_MESSAGE * pMsg) 
{
	switch (pMsg->MsgId) 
	{
		case WM_PAINT:
			if(g_tWav.ucSpeakerStatus == 0)
			{
				GUI_DrawBitmap(&bmSpeakerMute, 8, 8);
			}
			else 
			{
				GUI_DrawBitmap(&bmSpeaker, 8, 8);
			}
			break;
			
		default:
			BUTTON_Callback(pMsg);
	}
}

/*
*********************************************************************************************************
*	函 数 名: _cbDialogRadio
*	功能说明: 收音机对话框回调消息
*	形    参: pMsg  消息指针
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbDialogRadio(WM_MESSAGE * pMsg) 
{
	static  WM_HTIMER hTimerProcess;
	WM_HWIN hItem;
	int     NCode;
	int     Id;
	char    buf[50];
	uint16_t i;
	
	
	switch (pMsg->MsgId) 
	{
		/* 对话框初始化消息 */
		case WM_INIT_DIALOG:		
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
			WM_SetHasTrans(hItem);
			WM_SetCallback(hItem, _cbButtonBack);
		
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
			WM_SetHasTrans(hItem);
			WM_SetCallback(hItem, _cbButtonPre);
		
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
			WM_SetHasTrans(hItem);
			WM_SetCallback(hItem, _cbButtonPlay);
		
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_4);
			WM_SetHasTrans(hItem);
			WM_SetCallback(hItem, _cbButtonNext);
		
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_5);
			WM_SetHasTrans(hItem);
			WM_SetCallback(hItem, _cbButtonSpeaker);
			
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_6);
			WM_SetHasTrans(hItem);
			WM_SetCallback(hItem, _cbButtonList);
			
			
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_7);
			WM_SetHasTrans(hItem);
			WM_SetCallback(hItem, _cbButtonSeek);
		
			hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0); 
			SLIDER_SetBkColor(hItem, GUI_WHITE);
			SLIDER_SetFocusColor (hItem, GUI_STCOLOR_LIGHTBLUE);
			SLIDER_SetValue(hItem, 0);
			SLIDER_SetWidth(hItem, 0);
			SLIDER_SetSTSkin(hItem);
			SLIDER_SetRange(hItem, 0, 2050);
			
			hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_1); 
			SLIDER_SetBkColor(hItem, GUI_WHITE);
			SLIDER_SetFocusColor (hItem, GUI_STCOLOR_LIGHTBLUE);
			SLIDER_SetRange(hItem, 0, 63);
			
			/* 收音机的声音大小跟WM8978的声音大小设置成一样 g_tWav.ucVolume = g_tRadio.ucVolume*/
			SLIDER_SetValue(hItem, g_tWav.ucVolume);
			SLIDER_SetWidth(hItem, 0);
			SLIDER_SetSTSkin(hItem);
			
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
			TEXT_SetFont(hItem, &GUI_FontLubalGraph20B);
			TEXT_SetTextColor(hItem, GUI_STCOLOR_LIGHTBLUE);
			TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
			
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
			TEXT_SetFont(hItem, &GUI_FontLubalGraph20B);
			TEXT_SetTextColor(hItem, GUI_STCOLOR_LIGHTBLUE);
			TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
			
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
			TEXT_SetFont(hItem, &GUI_FontLubalGraph20B);
			TEXT_SetTextColor(hItem, GUI_STCOLOR_LIGHTBLUE);
			TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
			
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
			TEXT_SetFont(hItem, &GUI_FontLubalGraph20B);
			TEXT_SetTextColor(hItem, GUI_STCOLOR_LIGHTBLUE);
			TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_4);
			TEXT_SetFont(hItem, &GUI_FontLubalGraph20B);
			TEXT_SetTextColor(hItem, GUI_STCOLOR_LIGHTBLUE);
			TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_5);
			TEXT_SetFont(hItem, &GUI_FontD80);
			TEXT_SetTextColor(hItem, GUI_STCOLOR_LIGHTBLUE);
			TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			
			
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_6);
			TEXT_SetFont(hItem, &GUI_FontLubalGraph20B);
			TEXT_SetTextColor(hItem, GUI_STCOLOR_LIGHTBLUE);
			TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			
			hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_0);
			EDIT_SetText(hItem, "87.5MHz");
			EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			EDIT_SetBkColor(hItem, EDIT_CI_ENABLED, 0x0061ff);
			EDIT_SetTextColor(hItem, EDIT_CI_ENABLED, GUI_WHITE);
			EDIT_SetFont(hItem, &GUI_FontLubalGraph20B);
			EDIT_SetFocussable(hItem, 0);
			
			hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_1);
			EDIT_SetText(hItem, "87.5MHz");
			EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			EDIT_SetBkColor(hItem, EDIT_CI_ENABLED, 0xE16941);
			EDIT_SetTextColor(hItem, EDIT_CI_ENABLED, GUI_WHITE);
			EDIT_SetFont(hItem, &GUI_FontLubalGraph20B);
			EDIT_SetFocussable(hItem, 0);
			
			hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_2);
			EDIT_SetText(hItem, "87.5MHz");
			EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			EDIT_SetBkColor(hItem, EDIT_CI_ENABLED, 0xF020A0);
			EDIT_SetTextColor(hItem, EDIT_CI_ENABLED, GUI_WHITE);
			EDIT_SetFont(hItem, &GUI_FontLubalGraph20B);
			EDIT_SetFocussable(hItem, 0);
			
			hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_3);
			EDIT_SetText(hItem, "87.5MHz");
			EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			EDIT_SetBkColor(hItem, EDIT_CI_ENABLED, 0xA020F0);
			EDIT_SetTextColor(hItem, EDIT_CI_ENABLED, GUI_WHITE);
			EDIT_SetFont(hItem, &GUI_FontLubalGraph20B);
			EDIT_SetFocussable(hItem, 0);
			
			hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_4);
			EDIT_SetText(hItem, "87.5MHz");
			EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			EDIT_SetBkColor(hItem, EDIT_CI_ENABLED, 0x40913D);
			EDIT_SetTextColor(hItem, EDIT_CI_ENABLED, GUI_WHITE);
			EDIT_SetFont(hItem, &GUI_FontLubalGraph20B);
			EDIT_SetFocussable(hItem, 0);
			
			hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_5);
			EDIT_SetText(hItem, "87.5MHz");
			EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			EDIT_SetBkColor(hItem, EDIT_CI_ENABLED, 0x00D7FF);
			EDIT_SetTextColor(hItem, EDIT_CI_ENABLED, GUI_WHITE);
			EDIT_SetFont(hItem, &GUI_FontLubalGraph20B);
			EDIT_SetFocussable(hItem, 0);
			break;
		
		/* 定时器回调消息 */
		case WM_TIMER:
			Id = WM_GetTimerId(pMsg->Data.v);
			switch (Id) 
			{
				/* 周期性更新RSSI和SNR */
				case ID_TIMER_PROCESS:
					os_evt_set (RadioTaskGetRSSI_1, HandleMusicStart);
					sprintf(buf, "RSSI = %ddB",g_tRadio.ucRSSI);			
					hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
					TEXT_SetFont(hItem, &GUI_FontLubalGraph20B);
					TEXT_SetText(hItem, buf);
				
					sprintf(buf, "SNR = %ddB",g_tRadio.ucSNR);	
					hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_4);
					TEXT_SetFont(hItem, &GUI_FontLubalGraph20B);
					TEXT_SetText(hItem, buf);
					WM_RestartTimer(pMsg->Data.v, 1000);	
					break;
				}
			break;
				
		case WM_PAINT:
			GUI_SetBkColor(GUI_WHITE);
			GUI_Clear();
			break;
		
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch(Id) 
			{
				/* 关闭音乐播放对话框 */
				case ID_BUTTON_0:
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						
						/* 关闭收音机对话框 */
						case WM_NOTIFICATION_RELEASED:
							/* 发现消息，退出收音机 */
							os_evt_set (RadioTaskClose_3, HandleMusicStart);
							GUI_EndDialog(pMsg->hWin, 0);
							break;
					}
					break;
					
				/* 上一个频道 */
				case ID_BUTTON_1:
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						
						case WM_NOTIFICATION_RELEASED:
							if(s_ucPlayStatus == 1)	
							{	
								/* 自动搜索模式下进入上一个频道 */
								if(s_ucSeekMode == 1)
								{
									if (g_tRadio.ucIndexFM > 0)
									{
										g_tRadio.ucIndexFM--;
										g_tRadio.usFreq = g_tRadio.usFMList[g_tRadio.ucIndexFM];
										os_evt_set (RadioTaskSetFM_2, HandleMusicStart);	

										sprintf(buf, "%.1f", (float)g_tRadio.usFreq/100);
										hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_5);
										TEXT_SetText(hItem,  (const char *)buf);
										
										hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0); 
										SLIDER_SetValue(hItem, g_tRadio.usFreq - 8750);
									}
										sprintf(buf, "%d/%d", g_tRadio.ucIndexFM + 1, g_tRadio.ucFMCount);
										hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
										TEXT_SetText(hItem,  (const char *)buf);
								}
								/* 手动搜索模式下进入上一个频道，每次递减0.1MHz */
								else
								{
									if (g_tRadio.usFreq > 8750)
									{
										g_tRadio.usFreq -= 10;
										os_evt_set (RadioTaskSetFM_2, HandleMusicStart);	

										sprintf(buf, "%.1f", (float)g_tRadio.usFreq/100);
										hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_5);
										TEXT_SetText(hItem,  (const char *)buf);
										
										hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0); 
										SLIDER_SetValue(hItem, g_tRadio.usFreq - 8750);										
									}
								}
							}
							break;
					}
					break;
					
				/* 下一个频道 */
				case ID_BUTTON_4:
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						
						case WM_NOTIFICATION_RELEASED:
							if(s_ucPlayStatus == 1)	
							{
								/* 自动搜索模式下进入下一个频道 */
								if(s_ucSeekMode == 1)
								{
									if (g_tRadio.ucIndexFM < g_tRadio.ucFMCount - 1)
									{
										g_tRadio.ucIndexFM++;
										g_tRadio.usFreq = g_tRadio.usFMList[g_tRadio.ucIndexFM];
										os_evt_set (RadioTaskSetFM_2, HandleMusicStart);	
										
										sprintf(buf, "%.1f", (float)g_tRadio.usFreq/100);
										hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_5);
										TEXT_SetText(hItem,  (const char *)buf);
										
										hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0); 
										SLIDER_SetValue(hItem, g_tRadio.usFreq - 8750);
										
									}
										sprintf(buf, "%d/%d", g_tRadio.ucIndexFM + 1, g_tRadio.ucFMCount);
										hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
										TEXT_SetText(hItem,  (const char *)buf);
								}
								/* 手动搜索模式下进入下一个频道，每次递增0.1MHz */
								else
								{
									if (g_tRadio.usFreq <= 10800)
									{
										g_tRadio.usFreq += 10;
										os_evt_set (RadioTaskSetFM_2, HandleMusicStart);	

										sprintf(buf, "%.1f", (float)g_tRadio.usFreq/100);
										hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_5);
										TEXT_SetText(hItem,  (const char *)buf);
										
										hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0); 
										SLIDER_SetValue(hItem, g_tRadio.usFreq - 8750);										
									}
								}
							}
							break;
					}
					break;
					
				/* 切换到手动搜索 */
				case ID_BUTTON_6:
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							if(s_ucPlayStatus == 1)	
							{							
								s_ucSeekMode = 0;
								hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_6);
								TEXT_SetText(hItem,  "Manual");
							}
							break;
						
						case WM_NOTIFICATION_RELEASED:
							break;
					}
					break;
					
				/* 切换进入自动搜索并进行搜索*/
				case ID_BUTTON_7:
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							if(s_ucPlayStatus == 1)	
							{
								s_ucSeekMode = 1;
								hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_6);
								TEXT_SetText(hItem,  "Auto");

								hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_5);
								TEXT_SetText(hItem, ".....");
								
								/* 可以添加在这里，让TEXT控件先更新 */
								WM_Exec();

								os_evt_set (RadioTaskSearch_0, HandleMusicStart);				
								
								sprintf(buf, "%.1f", (float)g_tRadio.usFreq/100);
								hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_5);
								TEXT_SetText(hItem,  (const char *)buf);
								
								sprintf(buf, "%d/%d", g_tRadio.ucIndexFM + 1, g_tRadio.ucFMCount);
								hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
								TEXT_SetText(hItem,  (const char *)buf);
								
								hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0); 
								SLIDER_SetValue(hItem, g_tRadio.usFreq - 8750);
								
								for(i = 0; i < g_tRadio.ucFMCount; i++)
								{
									if(i == 6) break;
									hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_0 + i); 
									sprintf(buf, "%.1fMHz", (float)g_tRadio.usFMList[i]/100);
									EDIT_SetText(hItem,  (const char *)buf);
								}
								
								for(i = g_tRadio.ucFMCount; i < 6; i++)
								{
									hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_0 + i); 
									g_tRadio.usFMList[i] = 8750;
									EDIT_SetText(hItem,  "87.5MHz");
								}
							}
							break;
						
						case WM_NOTIFICATION_RELEASED:
							break;
					}
					break;
					
				/* 滚动条调整频道 */
				case ID_SLIDER_0:
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						
						case WM_NOTIFICATION_RELEASED:
							hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0);
							g_tRadio.usFreq = SLIDER_GetValue(hItem)  + 8750;
							os_evt_set (RadioTaskSetFM_2, HandleMusicStart);	
						
							sprintf(buf, "%.1f", (float)g_tRadio.usFreq/100);
							hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_5);
							TEXT_SetText(hItem,  (const char *)buf);
							break;
					}
					break;
					
				/* 调整音量 */
				case ID_SLIDER_1:
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						
						case WM_NOTIFICATION_RELEASED:
							hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_1);
							g_tRadio.ucVolume  = SLIDER_GetValue(hItem);
							/* 调节音量，左右相同音量 */
							wm8978_SetEarVolume(g_tRadio.ucVolume);
							wm8978_SetSpkVolume(g_tRadio.ucVolume);
							SI4730_SetOutVolume(g_tRadio.ucVolume);	
							break;
					}
					break;

				/* 暂停和开始按钮 */
				case ID_BUTTON_2:
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						
						case WM_NOTIFICATION_RELEASED:
							if(s_ucPlayStatus == 1)
							{
								/* 暂停 */	
								s_ucPlayStatus = 0;
								WM_DeleteTimer(hTimerProcess);
								os_evt_set (RadioTaskClose_3, HandleMusicStart);
							}
							else
							{							
								/* 播放 */	
								s_ucPlayStatus = 1;
								s_tMusicMsg.ucType = MusicType_Radio;	

								/* 发消息，执行收音机功能 */
								if(os_mbx_send (&mailbox, &s_tMusicMsg, 0xFFFF) == OS_R_OK)
								{
									
								}
								
								hTimerProcess = WM_CreateTimer(pMsg->hWin, ID_TIMER_PROCESS, 10, 0);	
							}
							break;
					}
					break;
					
				/* 静音设置 */
				case ID_BUTTON_5:
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						
						case WM_NOTIFICATION_RELEASED:
							if(g_tWav.ucSpeakerStatus == 1)
							{
								/* 静音 */	
								g_tWav.ucSpeakerStatus = 0;
								wm8978_OutMute(1);
							}
							else
							{
								/* 不静音 */	
								g_tWav.ucSpeakerStatus = 1;
								wm8978_OutMute(0);
							}
							break;
					}
					break;
					
				/* 编辑框中选择频道，自动搜索FM后，搜索到的频道会填入这些编辑框 */
				case ID_EDIT_0:
				case ID_EDIT_1:
				case ID_EDIT_2:
				case ID_EDIT_3:
				case ID_EDIT_4:
				case ID_EDIT_5:
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							hItem = WM_GetDialogItem(pMsg->hWin, Id);
							EDIT_SetBkColor(hItem, EDIT_CI_ENABLED, GUI_DARKGRAY);
							break;
						
						case WM_NOTIFICATION_RELEASED:
						case WM_NOTIFICATION_MOVED_OUT:
							hItem = WM_GetDialogItem(pMsg->hWin, Id);
							EDIT_SetBkColor(hItem, EDIT_CI_ENABLED, EditColors[Id - ID_EDIT_0]);
							if(s_ucPlayStatus == 1)	
							{
								g_tRadio.usFreq = g_tRadio.usFMList[Id - ID_EDIT_0];
								os_evt_set (RadioTaskSetFM_2, HandleMusicStart);
								
								hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0); 
								SLIDER_SetValue(hItem, g_tRadio.usFreq - 8750);
								
								sprintf(buf, "%.1f", (float)g_tRadio.usFreq/100);
								hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_5);
								TEXT_SetText(hItem,  (const char *)buf);
							}
							break;
					}
					break;
			}
			break;

		default:
			WM_DefaultProc(pMsg);
			break;
	}
}

/*
*********************************************************************************************************
*	函 数 名: App_Radio
*	功能说明: 创建收音机对话框
*	形    参: hWin  父窗口
*	返 回 值: 无
*********************************************************************************************************
*/
void App_Radio(WM_HWIN hWin) 
{
	s_ucPlayStatus = 0; 	        
    s_ucSeekMode = 1; 	   /* 1 表示自动搜索模式，0 表示手动搜索模式 */            
	GUI_CreateDialogBox(_aDialogCreateRadio, 
	                    GUI_COUNTOF(_aDialogCreateRadio), 
	                    _cbDialogRadio, 
	                    hWin,
                        0, 
	                    0);
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

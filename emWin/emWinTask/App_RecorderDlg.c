/*
*********************************************************************************************************
*	                                  
*	模块名称 : 录音机应用界面设计
*	文件名称 : App_RecorderDlg.c
*	版    本 : V1.0
*	说    明 : 录音机界面设计。
*              1. 录音机的功能做的比较简单，左上角和右上角的两个按钮未用到。
*              2. 采样率固定为32KHz，16bit，单通道。
*              3. 点击开始按钮，开始录音，录音30秒，务必保证录音结束后开始放音。
*              4. 显示的频谱是对数谱，即对FFT变化后幅值取对数。
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


/*
*********************************************************************************************************
*				                    引用外部变量和函数
*********************************************************************************************************
*/
/* 声明一个支持5个消息的消息邮箱 */
extern os_mbx_declare (mailbox, 5);
extern uint16_t g_uiNum;
extern MusicMsg_T s_tMusicMsg;          /* 用于给音乐播放任务发消息，发送执行录音任务 */


/*
*********************************************************************************************************
*				                         变量
*********************************************************************************************************
*/
static uint8_t s_ucPlayStatus = 0; 	   /* 播放器的开始和暂停按钮状态，0表示暂停，1表示运行 */
WM_HWIN  hWinRecorder;                 /* 录音机串口句柄 */
/* 录音机用到的emWin动态内存句柄 */
static GUI_HMEM hMemVedio;                       
uint8_t *g_pRecorder;


/*
*********************************************************************************************************
*	函 数 名: DisWave
*	功能说明: 显示频谱
*	形    参: x    X轴坐标
*             y    Y轴坐标
*             clr  0 表示清除显示数据，用于刚打开界面时，放在上次的显示有记录
*                  1 表示不清除。
*	返 回 值: 无
*********************************************************************************************************
*/
static void DisWave(uint16_t x, uint16_t y, uint8_t clr)
{
	static uint16_t s_MP3Spec_CurVal[32] = {0};	  /* 频谱当前值表 */
	const  uint16_t usMaxVal = 128;               /* 高度固定为128个像素 */
	uint16_t i;
	uint16_t temp;
	float ufTempValue;
	
	if(clr == 0)
	{
		for(i = 0; i< 32; i++)
		{
			s_MP3Spec_CurVal[i] = 0;
			g_tWav.uiFFT[i] = 0;
		}
		
		/* 清空后就退出 */
		return;
	}

	/* 显示32条频谱 */								   
	for(i = 0; i < 32; i++)	
	{	
		/* 幅值谱 */
		//temp = g_tWav.uiFFT[i]>>3;
		
		/* 显示对数谱 */
		ufTempValue = (float)g_tWav.uiFFT[i] / 32;
		if(ufTempValue < 1) 
		{
			ufTempValue = 0;                                                        
		}
		temp = 64 * log10(ufTempValue);	
		
		/* 2. 更新频谱数值 */
		if(s_MP3Spec_CurVal[i] < temp)
		{
			s_MP3Spec_CurVal[i] = temp;
		}
		else
		{
			if(s_MP3Spec_CurVal[i] > 1)
			{
				s_MP3Spec_CurVal[i] -= 2;
			} 
			else 
			{
				s_MP3Spec_CurVal[i] = 0;
			}
		}

		/* 5. 防止超出频谱值和顶值范围，高度固定为128个像素 */
		if(s_MP3Spec_CurVal[i] > usMaxVal)
		{
			s_MP3Spec_CurVal[i] = usMaxVal;
		}	   
	}

	/* 6. 绘制得到的频谱 */
	for(i = 0; i < 32; i++)
	{
		/* 显示频谱 */
		if(s_MP3Spec_CurVal[i] == 0)
		{
			GUI_SetColor(0x0000FF);
			GUI_DrawHLine(y + usMaxVal, x, x+10);
		}
		else
		{
			GUI_DrawGradientV(x, 
							  y + usMaxVal - s_MP3Spec_CurVal[i], 
							  x + 10, 
							  y + usMaxVal, 
							  0x0000FF, 
							  0x00FFFF);	
		}

		x += 15;
	}
}

/*
*********************************************************************************************************
*				                         任务对话框初始化选项
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateRecorder[] = 
{
	{ WINDOW_CreateIndirect, "Window",  ID_WINDOW_0,   0,   0,  800, 480, 0, 0x0, 0 },
	{ SLIDER_CreateIndirect, "Process", ID_SLIDER_0, 100, 365,  600,  16, 0, 0x0, 0 },

	/* 按钮功能 */
	{ BUTTON_CreateIndirect, "DlgBack",    ID_BUTTON_0,  700,   0,  100, 100, 0, 0, 0 },	
	{ BUTTON_CreateIndirect, "MusicPlay",  ID_BUTTON_1,  200, 399,   72,  72, 0, 0, 0 },
	{ BUTTON_CreateIndirect, "MusicSpk",   ID_BUTTON_5,  300, 399,   72,  72, 0, 0, 0 },
	{ BUTTON_CreateIndirect, "MusicList",  ID_BUTTON_6,    0,   0,  100, 100, 0, 0, 0 },
	
	/* 声音大小 */
	{ SLIDER_CreateIndirect, "Speaker",    ID_SLIDER_1, 400, 425, 200, 20, 0, 0x0, 0  },
	
	/* 显示录音放音信息 */
	{ TEXT_CreateIndirect, "32KHz/16bits/Mono",   ID_TEXT_0, 100, 345, 270, 20, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Idle",   			  ID_TEXT_1, 380, 345, 100, 20, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "00:00/00:30",   	  ID_TEXT_2, 610, 345, 120, 20, 0, 0x64, 0 },
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
				GUI_DrawBitmap(&bmRecorderIcon, 8, 8);
			}
			else 
			{
				GUI_DrawBitmap(&bmMusicPlay, 8, 8);
			}
			break;
			
		default:
			BUTTON_Callback(pMsg);
			break;
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
*	函 数 名: Recorder
*	功能说明: 录音机放对话框回调消息
*	形    参: pMsg  消息指针
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbDialogRecorder(WM_MESSAGE * pMsg) 
{
	const   GUI_RECT  Rect = {162, 200-29, 638, 300}; /* 左上角x，y，右下角x，y*/
	WM_HWIN hItem;
	int     NCode;
	int     Id;
	char    buf[100];
	

	switch (pMsg->MsgId) 
	{
		/* 对话框初始化消息 */
		case WM_INIT_DIALOG:		

			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
			WM_SetHasTrans(hItem);
			WM_SetCallback(hItem, _cbButtonBack);
		
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
			WM_SetHasTrans(hItem);
			WM_SetCallback(hItem, _cbButtonPlay);
		
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_5);
			WM_SetHasTrans(hItem);
			WM_SetCallback(hItem, _cbButtonSpeaker);
			
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_6);
			WM_SetHasTrans(hItem);
			WM_SetCallback(hItem, _cbButtonList);
		
			hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0); 
			SLIDER_SetBkColor(hItem, GUI_WHITE);
			SLIDER_SetFocusColor (hItem, GUI_STCOLOR_LIGHTBLUE);
			SLIDER_SetRange(hItem, 0, 960);
			SLIDER_SetValue(hItem, 0);
			SLIDER_SetWidth(hItem, 0);
			SLIDER_SetSTSkin(hItem);
			
			hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_1); 
			SLIDER_SetBkColor(hItem, GUI_WHITE);
			SLIDER_SetFocusColor (hItem, GUI_STCOLOR_LIGHTBLUE);
			SLIDER_SetRange(hItem, 0, 63);
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
			
			s_tMusicMsg.ucType = MusicType_REC;	
			/* 发消息，执行录音机功能 */
			if(os_mbx_send (&mailbox, &s_tMusicMsg, 0xFFFF) == OS_R_OK)
			{			
				
			}
			break;

		/* 重绘消息处理，使用了多缓冲 */
		case WM_PRE_PAINT:
			GUI_MULTIBUF_Begin();
		    break;
		
		case WM_PAINT:
			GUI_SetBkColor(GUI_WHITE);
			GUI_Clear();
			GUI_DrawBitmap(&bmMicrophone, (800-72)/2, 30);
			DisWave(162, 200-28, 1);
			break;
		
		case WM_POST_PAINT:
			GUI_MULTIBUF_End();
		    break;
		
		/*  开始执行录音或放音 */
		case MSG_MusicStart:
			hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0); 
			SLIDER_SetValue(hItem, g_uiNum);	

			sprintf(buf, "00:%02d/00:30", g_uiNum/32);
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
			TEXT_SetFont(hItem, &GUI_FontLubalGraph20B);
			TEXT_SetText(hItem, buf);
			if(g_uiNum == 960)
			{
				hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
				TEXT_SetText(hItem, "Idle");
			}
			WM_InvalidateRect(pMsg->hWin, &Rect);
			break;
		
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch(Id) 
			{
				/* 关闭录音对话框 */
				case ID_BUTTON_0:
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						
						case WM_NOTIFICATION_RELEASED:
							/* 不管开启没有开启播放，都发消息，退出当前录音界面 */
							os_evt_set (RecTaskAudioReturn_6, HandleMusicStart);
						
							/* 释放占用的动态内存，关闭此对话框 */
							GUI_ALLOC_Free(hMemVedio);
							GUI_EndDialog(pMsg->hWin, 0);
							break;
					}
					break;

				/* 播放和录音按钮 */
				case ID_BUTTON_1:
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						
						case WM_NOTIFICATION_RELEASED:
							if(s_ucPlayStatus == 1)
							{
								s_ucPlayStatus = 0;
								
								/* 播放录音 */
								hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
								TEXT_SetText(hItem, "Playing");
								os_evt_set (RecTaskAudioPlay_4, HandleMusicStart);				
							}
							else
							{
								s_ucPlayStatus = 1;
								
								/* 开始录音 */
								hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
								TEXT_SetText(hItem, "Recording");
								os_evt_set (RecTaskAudioRecorde_5, HandleMusicStart);				
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
					
				/* 控制音量 */
				case ID_SLIDER_1:
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						
						case WM_NOTIFICATION_RELEASED:	
							hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_1);
							g_tWav.ucVolume  = SLIDER_GetValue(hItem);
							/* 调节音量，左右相同音量 */
							wm8978_SetEarVolume(g_tWav.ucVolume);
							wm8978_SetSpkVolume(g_tWav.ucVolume);
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
*	函 数 名: App_Recorder78
*	功能说明: 创建录音机对话框
*	形    参: hWin  父窗口
*	返 回 值: 无
*********************************************************************************************************
*/
void App_Recorder(WM_HWIN hWin) 
{
	s_ucPlayStatus = 0; 	/* 录音机的开始和暂停按钮状态，0表示暂停，1表示运行 */
	
	/* 申请一块内存空间 并且将其清零  这里申请2MB的内存 */
	hMemVedio = GUI_ALLOC_AllocZero(2*1024*1024);
	
	/* 将申请到内存的句柄转换成指针类型 */
	g_pRecorder = GUI_ALLOC_h2p(hMemVedio);
	
	/* 清空频谱显示区的数据 */
	DisWave(162, 200-28, 0);
	
	hWinRecorder = GUI_CreateDialogBox(_aDialogCreateRecorder, 
	                                    GUI_COUNTOF(_aDialogCreateRecorder), 
	                                    _cbDialogRecorder, 
	                                    hWin, 
	                                    0, 
	                                    0);	
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

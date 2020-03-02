/*
*********************************************************************************************************
*	                                  
*	模块名称 : TXT格式文本阅读界面
*	文件名称 : App_FileBrowseDlg.c
*	版    本 : V1.0
*	说    明 : 这个对话框用于实现TXT格式文本阅读。
*              1. 支持浏览的最大文本数是50个, 通过宏定义TextMaxNum进行设置。
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
*			                      宏定义 
*********************************************************************************************************
*/
#define ReadBuffSize   1024

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


#define TextPathSize       100   /* 文本路径字符长度 */
#define TextMaxNum         50    /* 定义支持的最大文本数 */

/*
*********************************************************************************************************
*			                     变量，结构体定义
*********************************************************************************************************
*/
const uint16_t usTextMaxNum = TextMaxNum;
const char s_TextPathDir[] = {"M0:\\txt\\"};    /* 存储器中文本存放的路径 */
static uint8_t s_TextName[TextPathSize] = {0};  /* 用于记录当前阅读的文本 */

typedef struct
{
	uint32_t ulPageNum;    /* 记录当前翻看的页码号 */
	uint32_t ulHalfCount;  /* 记录当前页板字符个数 */
	uint8_t  ucHalfHead;   /* 头部半字符标识 */
	uint8_t  ucHalfBoot;   /* 尾部半字符标识 */
}TextControl_T;

static TextControl_T s_tText;
static int8_t TXT_Sel = 0;
static uint8_t g_ucReadBuff[ReadBuffSize + 1];                           
static FILE *fouttxt;

uint32_t TXT_Size[TextMaxNum];
WM_HWIN  hWinText;             /* 文本阅读放对话框句柄 */

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
*	函 数 名: _cbButtonSeek
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
				GUI_SetColor(GUI_DARKGRAY);
				GUI_AA_FillCircle(0, 100, 72);
				GUI_SetBkColor(GUI_DARKGRAY);
				GUI_SetColor(GUI_WHITE); 
				GUI_DrawBitmap(&bmleft, 3, 50);
			} 
			else 
			{
				GUI_SetColor(GUI_STCOLOR_LIGHTBLUE);
				GUI_AA_FillCircle(0, 100, 72);
				GUI_SetBkColor(GUI_STCOLOR_LIGHTBLUE);
				GUI_SetColor(GUI_WHITE);  
				GUI_DrawBitmap(&bmleft, 3, 50);
			}
			break;
			
		default:
			BUTTON_Callback(pMsg);
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
				GUI_SetColor(GUI_DARKGRAY);
				GUI_AA_FillCircle(100, 100, 72);
				GUI_SetBkColor(GUI_DARKGRAY);
				GUI_SetColor(GUI_WHITE); 
				GUI_DrawBitmap(&bmright, 48, 50);
			} 
			else 
			{
				GUI_SetColor(GUI_STCOLOR_LIGHTBLUE);
				GUI_AA_FillCircle(100, 100, 72);
				GUI_SetBkColor(GUI_STCOLOR_LIGHTBLUE);
				GUI_SetColor(GUI_WHITE);  
				GUI_DrawBitmap(&bmright, 48, 50);
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
*			                      对话框创建选项 
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateEbook[] = {
	{ WINDOW_CreateIndirect, "Window",  ID_WINDOW_0,   0,   0,  800, 480, 0, 0x0, 0 },
    { MULTIEDIT_CreateIndirect, "MULTIEDIT",     GUI_ID_MULTIEDIT0,     150,  40, 500, 400,0,0},
	
	{ SLIDER_CreateIndirect, "Process", ID_SLIDER_0, 150, 450,  500,  16, 0, 0x0, 0 },
	
	/* 按钮功能 */
	{ BUTTON_CreateIndirect, "DlgBack",    ID_BUTTON_0,  700,   0,  100, 100, 0, 0, 0 },	
	{ BUTTON_CreateIndirect, "MusicList",  ID_BUTTON_1,    0,   0,  100, 100, 0, 0, 0 },
	
	{ BUTTON_CreateIndirect, "上一页",  ID_BUTTON_2,   0,   380,  100, 100, 0, 0, 0 },
	{ BUTTON_CreateIndirect, "下一页",  ID_BUTTON_3,   700,   380,  100, 100, 0, 0, 0 },
	{ TEXT_CreateIndirect, "-----------",        ID_TEXT_3, 250, 10,  300, 24, 0, 0x64, 0 },
};

/*
*********************************************************************************************************
*	函 数 名: InitDialogEbook
*	功能说明: 初始化对话框控件项
*	形    参：pMsg
*	返 回 值: 无		        
*********************************************************************************************************
*/
void InitDialogEbook(WM_MESSAGE * pMsg)
{
    WM_HWIN hWin = pMsg->hWin;
	SCROLLBAR_Handle hScrollbar;
	WM_HWIN hItem;

	hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonBack);
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonPre);
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_3);
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonNext);
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonList);
	
	/* 初始化BUTTON */
	BUTTON_SetFont(WM_GetDialogItem(hWin,ID_BUTTON_2),&GUI_FontHZ16);
	BUTTON_SetFont(WM_GetDialogItem(hWin,ID_BUTTON_1),&GUI_FontHZ16);
    BUTTON_SetFont(WM_GetDialogItem(hWin,ID_BUTTON_3),&GUI_FontHZ16);

	/* 初始化GUI_ID_MULTIEDIT0 */ 
	hItem = WM_GetDialogItem(hWin,GUI_ID_MULTIEDIT0);
	MULTIEDIT_SetBkColor(hItem, 1, GUI_WHITE);
    MULTIEDIT_SetTextColor(hItem, 1, GUI_BLACK);
    MULTIEDIT_SetFont(hItem, &GUI_FontHZ16);
	
	MULTIEDIT_SetWrapWord(hItem);
	MULTIEDIT_SetReadOnly(hItem, 1);
	MULTIEDIT_SetFocussable(hItem, 0);
	
    hScrollbar = SCROLLBAR_CreateAttached(hItem, SCROLLBAR_CF_VERTICAL);
	SCROLLBAR_SetWidth(hScrollbar, 18);
	SCROLLBAR_SetSTSkin(hScrollbar);
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
	TEXT_SetFont(hItem, &GUI_FontLubalGraph20B);
	TEXT_SetTextColor(hItem, GUI_STCOLOR_LIGHTBLUE);
	TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0); 
	SLIDER_SetBkColor(hItem, GUI_WHITE);
	SLIDER_SetFocusColor (hItem, GUI_STCOLOR_LIGHTBLUE);
	SLIDER_SetValue(hItem, 0);
	SLIDER_SetWidth(hItem, 0);
	SLIDER_SetSTSkin(hItem);
}

/*
*********************************************************************************************************
*	函 数 名: _cbCallbackEbook
*	功能说明: 电子书回调函数
*	形    参: pMsg  消息指针
*	返 回 值: 无		        
*********************************************************************************************************
*/
static void _cbCallbackEbook(WM_MESSAGE * pMsg) 
{	
	WM_HWIN hItem;
    int NCode, Id, i;
	char  buf[TextPathSize + 20];
	uint32_t ulContexDeep;
    WM_HWIN hWin = pMsg->hWin;
	
    switch (pMsg->MsgId) 
    {
        case WM_INIT_DIALOG:
            InitDialogEbook(pMsg);
            break;
			
		case MSG_TextStart:
			hItem = WM_GetDialogItem(hWinTextList, GUI_ID_LISTVIEW0);
			TXT_Sel = LISTVIEW_GetSel(hItem);
			LISTVIEW_GetItemText(hItem, 0, TXT_Sel, buf, TextPathSize);
		
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
			TEXT_SetText(hItem,  (const char *)buf);
		
			hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0);
			SLIDER_SetRange(hItem, 0, TXT_Size[TXT_Sel]);
	
			memset(s_TextName, 0, TextPathSize);
	
			strcpy((char *)s_TextName, s_TextPathDir);
			strcat((char *)s_TextName, buf);

			/* 页面头部半字符标识 */
			s_tText.ucHalfHead = 0; 
			/* 页面尾部半字符标识 */
			s_tText.ucHalfBoot = 0;
			 
			/* 打开文件 */
			fouttxt = fopen ((char *)s_TextName, "r"); 
			
			/* 因为可以一次读出ReadBuffSize字节，先清空数据缓冲区 */ 
			memset(g_ucReadBuff, 0, ReadBuffSize + 1);
			 
			s_tText.ulHalfCount = 0;

			/* 将文件内容读出到数据缓冲区 */ 
			fread (g_ucReadBuff, 1, ReadBuffSize, fouttxt);
										
			/* 
			 因为每次读取的是1024字节，检测ascii字符的个数为为偶数
			 就说明其余的汉字可以全部正常显示。否则要再读取一个字节
			*/
			for(i = 0; i < ReadBuffSize; i++)
			{ 
				if(g_ucReadBuff[i] < 0x80)
				{
					s_tText.ulHalfCount++;
				}
				
				/* 在MULTIEDIT里面 回车没有使用，暂时将其用空格代替*/
				if( g_ucReadBuff[i] == 13 )
				{
					g_ucReadBuff[i] = ' ';	
				}
			}

			/* 判断半字符数是否是2的倍数，不是的话还要再读出一个字节 */
			if(s_tText.ulHalfCount % 2)
			{
				fread(&(g_ucReadBuff[ReadBuffSize]), 1, 1, fouttxt);		
				s_tText.ucHalfBoot = 1;			
			}
			else 
			{
				g_ucReadBuff[ReadBuffSize] = 0;
			}

			/* 关闭文件 */
			fclose(fouttxt);		
				
			/*目前显示的是第一页*/
			s_tText.ulPageNum = 1;

			/* 将内容显示到文本框里 */
			MULTIEDIT_SetText(WM_GetDialogItem(hWin, GUI_ID_MULTIEDIT0), (char *)g_ucReadBuff);
			
			hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0); 
			SLIDER_SetValue(hItem, ReadBuffSize);
			break;
			
        case WM_NOTIFY_PARENT:
            Id = WM_GetId(pMsg->hWinSrc); 
            NCode = pMsg->Data.v;        
            switch (Id) 
            {
				/* 关闭对话框 */
				case ID_BUTTON_0:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_RELEASED:
							#if 0
								if(WM_IsWindow(hWinTextList))
								{
									GUI_EndDialog(hWinMusicList, 0);
									hWinTextList = WM_HWIN_NULL;
								}
							#else
								hWinTextList = WM_HWIN_NULL;
							#endif
						    
							GUI_EndDialog(hWin, 0);
                            break;
                    }
                    break;
					
				/* 打开文本列表 */
				case ID_BUTTON_1:
                    switch(NCode)
					{
						case WM_NOTIFICATION_RELEASED:
							App_FileBrowseChild(hWin);
							break;
					}
                    break;	
					
				/* 上一页 */
				case ID_BUTTON_2:
					switch(NCode)
					{
						case WM_NOTIFICATION_RELEASED:
						/*　如果是第一页的话不需要进行任何操作　*/
						if(s_tText.ulPageNum > 1)
					    {	
						    /* 页码减去1 */
							s_tText.ulPageNum--;
							fouttxt = fopen ((char *)s_TextName, "r"); 
							ulContexDeep = (s_tText.ulPageNum - 1) * ReadBuffSize;
							
							hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0); 
							SLIDER_SetValue(hItem, ulContexDeep);
							
							/* 如果成功定位了，定位不成功表示文件到尾部了 */
							if(ulContexDeep <= TXT_Size[TXT_Sel]) 
							{	
							    /* 找到首地址 */ 
								fseek (fouttxt, ulContexDeep, SEEK_SET);                     
								s_tText.ulHalfCount = 0;

								/* 如果头部半符号标志位有效，表示当前页的尾部肯定存在半字符，需要再读取一个字节 */
								fread (g_ucReadBuff, 1, ReadBuffSize, fouttxt);
								if(s_tText.ucHalfHead)
								{
									s_tText.ucHalfBoot = 1;
									g_ucReadBuff[ReadBuffSize] = 0;
									
									/* 循环检查半个字符的数量有多少 */
									for(i = 0; i < ReadBuffSize; i++)
									{ 
										if(g_ucReadBuff[i] < 0x80)
										{
											s_tText.ulHalfCount++;
										}
										
										/* 在MULTIEDIT里面 回车没有使用，暂时将其用空格代替*/
										if( g_ucReadBuff[i] == 13 )
										{
											g_ucReadBuff[i] = ' ';	
										}
									}
									/* 如果是偶数的话则表示头部也存在半字符 */
									if(s_tText.ulHalfCount%2)
									{
										/* 这里是奇数 */
										s_tText.ucHalfHead = 0;
										fread (&(g_ucReadBuff[ReadBuffSize]), 1, 1, fouttxt);	
										MULTIEDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_MULTIEDIT0),(const char *)g_ucReadBuff);//将内容显示到文本框里
									}
									else
									{
										/* 偶数，头部半字符 */
										s_tText.ucHalfHead = 1;
                                        fread (&(g_ucReadBuff[ReadBuffSize]), 1, 1, fouttxt);											
										MULTIEDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_MULTIEDIT0),(const char *)(&g_ucReadBuff[1]));//将内容显示到文本框里
									}
								}
								/* 头部不是半字符，那么当前页的尾部肯定也不是半字符 */
								else
								{	
									s_tText.ucHalfBoot = 0;
									g_ucReadBuff[ReadBuffSize] = 0;
									/* 循环检查半个字符的数量有多少 */
									for(i = 0; i < ReadBuffSize; i++)
									{ 
										if(g_ucReadBuff[i] < 0x80)
										{
											s_tText.ulHalfCount++;
										}
										
										/* 在MULTIEDIT里面 回车没有使用，暂时将其用空格代替*/
										if( g_ucReadBuff[i] == 13 )
										{
											g_ucReadBuff[i] = ' ';	
										}
									}								
									if(s_tText.ulHalfCount%2)
									{
										/* 奇数，头部存在半字符 */
										s_tText.ucHalfHead = 1;
										g_ucReadBuff[ReadBuffSize]=0;
										MULTIEDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_MULTIEDIT0),(const char *)(&g_ucReadBuff[1]));
									}
									else
									{
										/* 偶数，头部不存在半字符 */
										s_tText.ucHalfHead = 0;
										g_ucReadBuff[ReadBuffSize]=0;
										MULTIEDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_MULTIEDIT0),(const char *)(&g_ucReadBuff[0]));
									}
								}
							}
							
							fclose(fouttxt);	
							break;
					   }
					}
						  
				break;
		
				/* 翻到下一页 */
				case ID_BUTTON_3:
					switch(NCode)
					{
						case WM_NOTIFICATION_RELEASED:						   
						    /* 因为有可能读不出ReadBuffSize个字节，先清空数据缓冲区 */
							memset(g_ucReadBuff, 0, ReadBuffSize + 1);	     
							s_tText.ulHalfCount=0;
					
							/* 打开文件 */
							fouttxt = fopen ((char *)s_TextName, "r"); 
							
						    ulContexDeep = s_tText.ulPageNum * ReadBuffSize;
							
							hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0); 
							SLIDER_SetValue(hItem, ulContexDeep);
							
							/* 如果成功定位了，定位不成功表示文件到尾部了 */
							if(ulContexDeep <= TXT_Size[TXT_Sel]) 
							{	
							    /* 找到首地址 */
								fseek (fouttxt, ulContexDeep, SEEK_SET); 
								s_tText.ulPageNum++;
								
								hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0); 
								SLIDER_SetValue(hItem, s_tText.ulPageNum * ReadBuffSize);
								
								/* 读取ReadBuffSize个数据 */
								fread (g_ucReadBuff, 1, ReadBuffSize, fouttxt);
								/* 如果尾部半符号标志位有效，表示当前页的头部肯定存在半字符 */ 
								if(s_tText.ucHalfBoot)
								{
									s_tText.ucHalfHead = 1;	
									/* 循环检查半个字符的数量有多少 */
									for(i = 0; i < ReadBuffSize; i++)
									{ 
										if(g_ucReadBuff[i] < 0x80)
										{
											s_tText.ulHalfCount++;
										}
										
										/* 在MULTIEDIT里面 回车没有使用，暂时将其用空格代替*/
										if( g_ucReadBuff[i] == 13 )
										{
											g_ucReadBuff[i] = ' ';	
										}
									}
									/* 如果是偶数的话则表示尾部也存在半字符 */
									if(s_tText.ulHalfCount%2)
									{
										/* 奇数，尾部不存在半字符 */
										s_tText.ucHalfBoot = 0;
										g_ucReadBuff[ReadBuffSize]=0;
										MULTIEDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_MULTIEDIT0),(const char *)(&g_ucReadBuff[1]));//将内容显示到文本框里
									}
									else
									{
										/* 偶数，尾部存在半字符 */
										s_tText.ucHalfBoot = 1;
										fread (&(g_ucReadBuff[ReadBuffSize]), 1, 1, fouttxt);	
										MULTIEDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_MULTIEDIT0),(const char *)(&g_ucReadBuff[1]));//将内容显示到文本框里
									}
								}
								/* 尾部不是半字符，所以当前页的头部肯定不是半字符 */
								else
								{
									s_tText.ucHalfHead = 0;
									/* 循环检查半个字符的数量有多少 */
									for(i = 0; i < ReadBuffSize; i++)
									{ 
										if(g_ucReadBuff[i] < 0x80)
										{
											s_tText.ulHalfCount++;
										}
										
										/* 在MULTIEDIT里面 回车没有使用，暂时将其用空格代替*/
										if( g_ucReadBuff[i] == 13 )
										{
											g_ucReadBuff[i] = ' ';	
										}
									}
									/* 奇数，尾部存在半字符 */
									if(s_tText.ulHalfCount%2)
									{	
										s_tText.ucHalfBoot = 1;
										fread (&(g_ucReadBuff[ReadBuffSize]), 1, 1, fouttxt);	
										MULTIEDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_MULTIEDIT0),(const char *)g_ucReadBuff);
									}
									/* 偶数，尾部不存在半字符 */
									else
									{
										s_tText.ucHalfBoot = 0;
										g_ucReadBuff[ReadBuffSize]=0;
										MULTIEDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_MULTIEDIT0),(const char *)g_ucReadBuff);
									}
								}
							}
							fclose(fouttxt);
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
*	函 数 名: App_FileBrowse
*	功能说明: 文件阅读对话框
*	形    参: hWin 所创建对话框父窗口 	
*	返 回 值: 无
*********************************************************************************************************
*/
void App_FileBrowse(WM_HWIN hWin) 
{
	hWinText = GUI_CreateDialogBox(_aDialogCreateEbook, 
	                                GUI_COUNTOF(_aDialogCreateEbook), 
	                                &_cbCallbackEbook, 
	                                hWin, 
	                                0, 
	                                0);
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

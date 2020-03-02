/*
*********************************************************************************************************
*	                                  
*	ģ������ : TXT��ʽ�ı��Ķ�����
*	�ļ����� : App_FileBrowseDlg.c
*	��    �� : V1.0
*	˵    �� : ����Ի�������ʵ��TXT��ʽ�ı��Ķ���
*              1. ֧�����������ı�����50��, ͨ���궨��TextMaxNum�������á�
*
*	�޸ļ�¼ :
*		�汾��    ����         ����         ˵��
*       V1.0    2019-12      ����Сʦ��     �׷�
*
*	Copyright (C), 2020-2025, https://space.bilibili.com/357524621 
*                                     
*********************************************************************************************************
*/
#include "includes.h"
#include "MainTask.h"



/*
*********************************************************************************************************
*			                      �궨�� 
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


#define TextPathSize       100   /* �ı�·���ַ����� */
#define TextMaxNum         50    /* ����֧�ֵ�����ı��� */

/*
*********************************************************************************************************
*			                     �������ṹ�嶨��
*********************************************************************************************************
*/
const uint16_t usTextMaxNum = TextMaxNum;
const char s_TextPathDir[] = {"M0:\\txt\\"};    /* �洢�����ı���ŵ�·�� */
static uint8_t s_TextName[TextPathSize] = {0};  /* ���ڼ�¼��ǰ�Ķ����ı� */

typedef struct
{
	uint32_t ulPageNum;    /* ��¼��ǰ������ҳ��� */
	uint32_t ulHalfCount;  /* ��¼��ǰҳ���ַ����� */
	uint8_t  ucHalfHead;   /* ͷ�����ַ���ʶ */
	uint8_t  ucHalfBoot;   /* β�����ַ���ʶ */
}TextControl_T;

static TextControl_T s_tText;
static int8_t TXT_Sel = 0;
static uint8_t g_ucReadBuff[ReadBuffSize + 1];                           
static FILE *fouttxt;

uint32_t TXT_Size[TextMaxNum];
WM_HWIN  hWinText;             /* �ı��Ķ��ŶԻ����� */

/*
*********************************************************************************************************
*	�� �� ��: _cbButtonBack
*	����˵��: ��ť�ص�����
*	��    ��: pMsg  ��Ϣָ��
*	�� �� ֵ: ��
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
*	�� �� ��: _cbButtonSeek
*	����˵��: ��ť�ص�����
*	��    ��: pMsg  ��Ϣָ��
*	�� �� ֵ: ��
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
*	�� �� ��: _cbButtonNext
*	����˵��: ��ť�ص�����
*	��    ��: pMsg  ��Ϣָ��
*	�� �� ֵ: ��
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
*	�� �� ��: _cbButtonList
*	����˵��: ��ť�ص�����
*	��    ��: pMsg  ��Ϣָ��
*	�� �� ֵ: ��
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
*			                      �Ի��򴴽�ѡ�� 
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateEbook[] = {
	{ WINDOW_CreateIndirect, "Window",  ID_WINDOW_0,   0,   0,  800, 480, 0, 0x0, 0 },
    { MULTIEDIT_CreateIndirect, "MULTIEDIT",     GUI_ID_MULTIEDIT0,     150,  40, 500, 400,0,0},
	
	{ SLIDER_CreateIndirect, "Process", ID_SLIDER_0, 150, 450,  500,  16, 0, 0x0, 0 },
	
	/* ��ť���� */
	{ BUTTON_CreateIndirect, "DlgBack",    ID_BUTTON_0,  700,   0,  100, 100, 0, 0, 0 },	
	{ BUTTON_CreateIndirect, "MusicList",  ID_BUTTON_1,    0,   0,  100, 100, 0, 0, 0 },
	
	{ BUTTON_CreateIndirect, "��һҳ",  ID_BUTTON_2,   0,   380,  100, 100, 0, 0, 0 },
	{ BUTTON_CreateIndirect, "��һҳ",  ID_BUTTON_3,   700,   380,  100, 100, 0, 0, 0 },
	{ TEXT_CreateIndirect, "-----------",        ID_TEXT_3, 250, 10,  300, 24, 0, 0x64, 0 },
};

/*
*********************************************************************************************************
*	�� �� ��: InitDialogEbook
*	����˵��: ��ʼ���Ի���ؼ���
*	��    �Σ�pMsg
*	�� �� ֵ: ��		        
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
	
	/* ��ʼ��BUTTON */
	BUTTON_SetFont(WM_GetDialogItem(hWin,ID_BUTTON_2),&GUI_FontHZ16);
	BUTTON_SetFont(WM_GetDialogItem(hWin,ID_BUTTON_1),&GUI_FontHZ16);
    BUTTON_SetFont(WM_GetDialogItem(hWin,ID_BUTTON_3),&GUI_FontHZ16);

	/* ��ʼ��GUI_ID_MULTIEDIT0 */ 
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
*	�� �� ��: _cbCallbackEbook
*	����˵��: ������ص�����
*	��    ��: pMsg  ��Ϣָ��
*	�� �� ֵ: ��		        
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

			/* ҳ��ͷ�����ַ���ʶ */
			s_tText.ucHalfHead = 0; 
			/* ҳ��β�����ַ���ʶ */
			s_tText.ucHalfBoot = 0;
			 
			/* ���ļ� */
			fouttxt = fopen ((char *)s_TextName, "r"); 
			
			/* ��Ϊ����һ�ζ���ReadBuffSize�ֽڣ���������ݻ����� */ 
			memset(g_ucReadBuff, 0, ReadBuffSize + 1);
			 
			s_tText.ulHalfCount = 0;

			/* ���ļ����ݶ��������ݻ����� */ 
			fread (g_ucReadBuff, 1, ReadBuffSize, fouttxt);
										
			/* 
			 ��Ϊÿ�ζ�ȡ����1024�ֽڣ����ascii�ַ��ĸ���ΪΪż��
			 ��˵������ĺ��ֿ���ȫ��������ʾ������Ҫ�ٶ�ȡһ���ֽ�
			*/
			for(i = 0; i < ReadBuffSize; i++)
			{ 
				if(g_ucReadBuff[i] < 0x80)
				{
					s_tText.ulHalfCount++;
				}
				
				/* ��MULTIEDIT���� �س�û��ʹ�ã���ʱ�����ÿո����*/
				if( g_ucReadBuff[i] == 13 )
				{
					g_ucReadBuff[i] = ' ';	
				}
			}

			/* �жϰ��ַ����Ƿ���2�ı��������ǵĻ���Ҫ�ٶ���һ���ֽ� */
			if(s_tText.ulHalfCount % 2)
			{
				fread(&(g_ucReadBuff[ReadBuffSize]), 1, 1, fouttxt);		
				s_tText.ucHalfBoot = 1;			
			}
			else 
			{
				g_ucReadBuff[ReadBuffSize] = 0;
			}

			/* �ر��ļ� */
			fclose(fouttxt);		
				
			/*Ŀǰ��ʾ���ǵ�һҳ*/
			s_tText.ulPageNum = 1;

			/* ��������ʾ���ı����� */
			MULTIEDIT_SetText(WM_GetDialogItem(hWin, GUI_ID_MULTIEDIT0), (char *)g_ucReadBuff);
			
			hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0); 
			SLIDER_SetValue(hItem, ReadBuffSize);
			break;
			
        case WM_NOTIFY_PARENT:
            Id = WM_GetId(pMsg->hWinSrc); 
            NCode = pMsg->Data.v;        
            switch (Id) 
            {
				/* �رնԻ��� */
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
					
				/* ���ı��б� */
				case ID_BUTTON_1:
                    switch(NCode)
					{
						case WM_NOTIFICATION_RELEASED:
							App_FileBrowseChild(hWin);
							break;
					}
                    break;	
					
				/* ��һҳ */
				case ID_BUTTON_2:
					switch(NCode)
					{
						case WM_NOTIFICATION_RELEASED:
						/*������ǵ�һҳ�Ļ�����Ҫ�����κβ�����*/
						if(s_tText.ulPageNum > 1)
					    {	
						    /* ҳ���ȥ1 */
							s_tText.ulPageNum--;
							fouttxt = fopen ((char *)s_TextName, "r"); 
							ulContexDeep = (s_tText.ulPageNum - 1) * ReadBuffSize;
							
							hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0); 
							SLIDER_SetValue(hItem, ulContexDeep);
							
							/* ����ɹ���λ�ˣ���λ���ɹ���ʾ�ļ���β���� */
							if(ulContexDeep <= TXT_Size[TXT_Sel]) 
							{	
							    /* �ҵ��׵�ַ */ 
								fseek (fouttxt, ulContexDeep, SEEK_SET);                     
								s_tText.ulHalfCount = 0;

								/* ���ͷ������ű�־λ��Ч����ʾ��ǰҳ��β���϶����ڰ��ַ�����Ҫ�ٶ�ȡһ���ֽ� */
								fread (g_ucReadBuff, 1, ReadBuffSize, fouttxt);
								if(s_tText.ucHalfHead)
								{
									s_tText.ucHalfBoot = 1;
									g_ucReadBuff[ReadBuffSize] = 0;
									
									/* ѭ��������ַ��������ж��� */
									for(i = 0; i < ReadBuffSize; i++)
									{ 
										if(g_ucReadBuff[i] < 0x80)
										{
											s_tText.ulHalfCount++;
										}
										
										/* ��MULTIEDIT���� �س�û��ʹ�ã���ʱ�����ÿո����*/
										if( g_ucReadBuff[i] == 13 )
										{
											g_ucReadBuff[i] = ' ';	
										}
									}
									/* �����ż���Ļ����ʾͷ��Ҳ���ڰ��ַ� */
									if(s_tText.ulHalfCount%2)
									{
										/* ���������� */
										s_tText.ucHalfHead = 0;
										fread (&(g_ucReadBuff[ReadBuffSize]), 1, 1, fouttxt);	
										MULTIEDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_MULTIEDIT0),(const char *)g_ucReadBuff);//��������ʾ���ı�����
									}
									else
									{
										/* ż����ͷ�����ַ� */
										s_tText.ucHalfHead = 1;
                                        fread (&(g_ucReadBuff[ReadBuffSize]), 1, 1, fouttxt);											
										MULTIEDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_MULTIEDIT0),(const char *)(&g_ucReadBuff[1]));//��������ʾ���ı�����
									}
								}
								/* ͷ�����ǰ��ַ�����ô��ǰҳ��β���϶�Ҳ���ǰ��ַ� */
								else
								{	
									s_tText.ucHalfBoot = 0;
									g_ucReadBuff[ReadBuffSize] = 0;
									/* ѭ��������ַ��������ж��� */
									for(i = 0; i < ReadBuffSize; i++)
									{ 
										if(g_ucReadBuff[i] < 0x80)
										{
											s_tText.ulHalfCount++;
										}
										
										/* ��MULTIEDIT���� �س�û��ʹ�ã���ʱ�����ÿո����*/
										if( g_ucReadBuff[i] == 13 )
										{
											g_ucReadBuff[i] = ' ';	
										}
									}								
									if(s_tText.ulHalfCount%2)
									{
										/* ������ͷ�����ڰ��ַ� */
										s_tText.ucHalfHead = 1;
										g_ucReadBuff[ReadBuffSize]=0;
										MULTIEDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_MULTIEDIT0),(const char *)(&g_ucReadBuff[1]));
									}
									else
									{
										/* ż����ͷ�������ڰ��ַ� */
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
		
				/* ������һҳ */
				case ID_BUTTON_3:
					switch(NCode)
					{
						case WM_NOTIFICATION_RELEASED:						   
						    /* ��Ϊ�п��ܶ�����ReadBuffSize���ֽڣ���������ݻ����� */
							memset(g_ucReadBuff, 0, ReadBuffSize + 1);	     
							s_tText.ulHalfCount=0;
					
							/* ���ļ� */
							fouttxt = fopen ((char *)s_TextName, "r"); 
							
						    ulContexDeep = s_tText.ulPageNum * ReadBuffSize;
							
							hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0); 
							SLIDER_SetValue(hItem, ulContexDeep);
							
							/* ����ɹ���λ�ˣ���λ���ɹ���ʾ�ļ���β���� */
							if(ulContexDeep <= TXT_Size[TXT_Sel]) 
							{	
							    /* �ҵ��׵�ַ */
								fseek (fouttxt, ulContexDeep, SEEK_SET); 
								s_tText.ulPageNum++;
								
								hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0); 
								SLIDER_SetValue(hItem, s_tText.ulPageNum * ReadBuffSize);
								
								/* ��ȡReadBuffSize������ */
								fread (g_ucReadBuff, 1, ReadBuffSize, fouttxt);
								/* ���β������ű�־λ��Ч����ʾ��ǰҳ��ͷ���϶����ڰ��ַ� */ 
								if(s_tText.ucHalfBoot)
								{
									s_tText.ucHalfHead = 1;	
									/* ѭ��������ַ��������ж��� */
									for(i = 0; i < ReadBuffSize; i++)
									{ 
										if(g_ucReadBuff[i] < 0x80)
										{
											s_tText.ulHalfCount++;
										}
										
										/* ��MULTIEDIT���� �س�û��ʹ�ã���ʱ�����ÿո����*/
										if( g_ucReadBuff[i] == 13 )
										{
											g_ucReadBuff[i] = ' ';	
										}
									}
									/* �����ż���Ļ����ʾβ��Ҳ���ڰ��ַ� */
									if(s_tText.ulHalfCount%2)
									{
										/* ������β�������ڰ��ַ� */
										s_tText.ucHalfBoot = 0;
										g_ucReadBuff[ReadBuffSize]=0;
										MULTIEDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_MULTIEDIT0),(const char *)(&g_ucReadBuff[1]));//��������ʾ���ı�����
									}
									else
									{
										/* ż����β�����ڰ��ַ� */
										s_tText.ucHalfBoot = 1;
										fread (&(g_ucReadBuff[ReadBuffSize]), 1, 1, fouttxt);	
										MULTIEDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_MULTIEDIT0),(const char *)(&g_ucReadBuff[1]));//��������ʾ���ı�����
									}
								}
								/* β�����ǰ��ַ������Ե�ǰҳ��ͷ���϶����ǰ��ַ� */
								else
								{
									s_tText.ucHalfHead = 0;
									/* ѭ��������ַ��������ж��� */
									for(i = 0; i < ReadBuffSize; i++)
									{ 
										if(g_ucReadBuff[i] < 0x80)
										{
											s_tText.ulHalfCount++;
										}
										
										/* ��MULTIEDIT���� �س�û��ʹ�ã���ʱ�����ÿո����*/
										if( g_ucReadBuff[i] == 13 )
										{
											g_ucReadBuff[i] = ' ';	
										}
									}
									/* ������β�����ڰ��ַ� */
									if(s_tText.ulHalfCount%2)
									{	
										s_tText.ucHalfBoot = 1;
										fread (&(g_ucReadBuff[ReadBuffSize]), 1, 1, fouttxt);	
										MULTIEDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_MULTIEDIT0),(const char *)g_ucReadBuff);
									}
									/* ż����β�������ڰ��ַ� */
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
*	�� �� ��: App_FileBrowse
*	����˵��: �ļ��Ķ��Ի���
*	��    ��: hWin �������Ի��򸸴��� 	
*	�� �� ֵ: ��
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

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

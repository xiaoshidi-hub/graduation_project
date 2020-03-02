/*
*********************************************************************************************************
*	                                  
*	ģ������ : �����Ի���������
*	�ļ����� : App_ReservedDlg.c
*	��    �� : V1.0
*	˵    �� : δʹ�õ�Ӧ��ȫ��ʹ�ô˶Ի���
*
*	�޸ļ�¼ :
*		�汾��    ����         ����         ˵��
*       V1.0    2019-12      ����Сʦ��     �׷�
*
*	Copyright (C), 2020-2025, https://space.bilibili.com/357524621 
*                                     
*********************************************************************************************************
*/

#include "MainTask.h"




/*
*********************************************************************************************************
*                                       �궨��
*********************************************************************************************************
*/
#define MAG          3
#define ID_WINDOW_0 	(GUI_ID_USER + 0x00)
#define ID_BUTTON_0     (GUI_ID_USER + 0xA0)
#define ID_BUTTON_1     (GUI_ID_USER + 0xB0)

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
*	               �Ի���ؼ��б�
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateReserved[] = {
	{ WINDOW_CreateIndirect, "Window",      ID_WINDOW_0,    0,    0,   800, 480, 0, 0x0, 0},
	{ BUTTON_CreateIndirect, "DlgBack",     ID_BUTTON_0,   700,    0,  100, 100, 0, 0, 0},	
	{ BUTTON_CreateIndirect, "MusicList",   ID_BUTTON_1,     0,    0,  100, 100, 0, 0, 0},
};

/*
*********************************************************************************************************
*	�� �� ��: InitDialogSys
*	����˵��: �Ի���ص������ĳ�ʼ����Ϣ
*	��    �Σ�pMsg   ��Ϣָ�����
*	�� �� ֵ: ��
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
*	�� �� ��: _cbCallbackReserved
*	����˵��: �ص�����
*	��    ��: pMsg   ��Ϣָ�����
*	�� �� ֵ: ��
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
			/* �������ۺ�ֱ�� */
			GUI_SetColor(GUI_BLACK);
			GUI_SetLineStyle(GUI_LS_DASH);
			GUI_DrawLine(70, 10, 170, 110);
			/* ���Ƶ�ֱ�� */
			GUI_SetLineStyle(GUI_LS_DOT);
			GUI_DrawLine(50, 10, 170, 130);
			/* ����ʵ��ֱ�� */
			GUI_SetLineStyle(GUI_LS_SOLID);
			GUI_DrawLine(30, 10, 170, 150);
			GUI_SetPenSize(4);
			GUI_DrawLine(10, 10, 170, 170);
			/* ���ƾ��� */
			GUI_SetColor(GUI_BLUE);
			GUI_DrawRect(210, 10, 290, 90);
			GUI_FillRect(310, 10, 390, 90);
			/* ����Բ�Ǿ��� */
			GUI_SetColor(GUI_ORANGE);
			GUI_DrawRoundedFrame(210, 110, 290, 190, 20, 8);
			GUI_FillRoundedRect(310, 110, 390, 190, 20);
			/* ���ƽ���ɫԲ�Ǿ��� */
			GUI_DrawGradientRoundedV(410, 10, 490, 190, 20, GUI_LIGHTMAGENTA,GUI_LIGHTCYAN);
			
			case WM_NOTIFY_PARENT://�����Ϣ�ǶԻ���ص��������ص㣬���жԻ����ӿؼ��ľ�����Ϊ�߼���
			                     //�ڴ���Ϣ�����úʹ���
            Id = WM_GetId(pMsg->hWinSrc); 
            NCode = pMsg->Data.v;        
            switch (Id) 
            {
				/* �رնԻ��� */
				case ID_BUTTON_0:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_RELEASED://���������Ŀؼ����ͷ�ʱ�������ʹ�֪ͨ��Ϣ
							  GUI_EndDialog(hWin, 0);//����һ���Ի��� ���ǵ��������һ����ť ���ص�������
                             break;
                    }
                    break;
					
				case ID_BUTTON_1:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED://��ť�ѱ���� ���ǵ�����Ϸ���һ����ť û��Ӧ
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
*	�� �� ��: App_Reserved
*	����˵��: δʹ�õ�Ӧ��ȫ��ʹ�ô˶Ի���
*	��    ��: hWin �������Ի��� 	
*	�� �� ֵ: ��
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



/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

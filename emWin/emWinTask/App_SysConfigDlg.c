/*
*********************************************************************************************************
*	                                  
*	ģ������ : ϵͳ��Ϣ�������
*	�ļ����� : App_SysConfig.c
*	��    �� : V1.0
*	˵    �� : ϵͳ��Ϣ������ơ�
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
*	����˵��: ��ť���ûص����� ���Ϸ���һ����ť�����Զ��尴ť
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
			if (BUTTON_IsPressed(hWin)) //���ذ�ť�Ƿ񱻰���
			{
				GUI_SetColor(GUI_DARKGRAY);
				GUI_AA_FillCircle(0, 0, 72);//��һ��Բ
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
			BUTTON_Callback(pMsg);//���� WM_PAINT ��Ϣ���⣬����û���ϵ�������Ϣ����Ҫ������ť�ؼ�Ĭ�ϵĻص�
                                  //���� BUTTON_Callback ���д�������������ť���ܾ͵������ˡ�
	}
}
/*
*********************************************************************************************************
*	               �Ի���ؼ��б�
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
*	�� �� ��: InitDialogSys
*	����˵��: �Ի���ص������ĳ�ʼ����Ϣ
*	��    �Σ�pMsg   ��Ϣָ�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void InitDialogSys(WM_MESSAGE * pMsg) 
{	
	WM_HWIN hWin = pMsg->hWin;
	WM_HWIN hItem;

	hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);//ͨ�� WM_GetDialogItem �������Զ���������ȡ��ť�ľ��
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonBack);//�޸Ŀؼ��ص������ķ�ʽ�Զ��� Button
                                         //����ۣ��� WM_SetCallback ���������ض���
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonList);
	
	
	hItem = WM_GetDialogItem(hWin, ID_TEXT_2);	
	TEXT_SetTextColor(hItem, GUI_RED);
	TEXT_SetFont(hItem, &GUI_SysFontHZ16);
	TEXT_SetText(hItem, "�̼��汾");

	hItem = WM_GetDialogItem(hWin, ID_TEXT_3);
	TEXT_SetTextColor(hItem, GUI_RED);
	TEXT_SetFont(hItem, &GUI_FontHZ16);
	TEXT_SetText(hItem, "��ҵ���");	
//	TEXT_SetText(hItem, "[1]���������£�ֻ��������\n"
//						"[2]���������Ң2020���ҵ��ơ����ָ��\n");
	
	/* ���ÿ�������� */
	GUI_AA_SetFactor(MAG);
	hMemQR = GUI_QR_Create("https://space.bilibili.com/357524621", 8, GUI_QR_ECLEVEL_H, 0);/* ������ά����� */
	
 
}
/*
*********************************************************************************************************
*	�� �� ��: _cbCallbackSys
*	����˵��: �ص�����
*	��    ��: pMsg   ��Ϣָ�����
*	�� �� ֵ: ��
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
	  case WM_PAINT:  //�����ػ���Ϣ,����Ƚ���˵����,������Framewin��Window����֮������һ�����ÿ�
	                  //��,���Ҫ��Framewin��Window����֮����ʾ���ֻ����ֱ�ߡ����Ρ�Բ��������ʵ��
		
			GUI_SetBkColor(GUI_LIGHTCYAN);
			GUI_Clear();   //�޸������汳����ɫ������������������޸Ĳ��ɹ�
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
	  	    GUI_DrawBitmap(&bmLOGO_PNG,700,400);     //��ʾ�Լ���QQͷ��

	  	    GUI_QR_GetInfo(hMemQR, &Info);
			GUI_QR_Draw(hMemQR, 450, 100);  
	
	       break;
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
							  GUI_QR_Delete(hMemQR);
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
            WM_DefaultProc(pMsg);//�������ǲ����Ļ���û���õ���ϵͳ��Ϣ�����Ե���Ĭ����Ϣ������
                                //WM_DefaultProc ���д���

    }
}


/*
*********************************************************************************************************
*	�� �� ��: App_SysConfig
*	����˵��: ϵͳ��Ϣ�Ի���
*	��    ��: hWin �������Ի��� 	
*	�� �� ֵ: ��
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


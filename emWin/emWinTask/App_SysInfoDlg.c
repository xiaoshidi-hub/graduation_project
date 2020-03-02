/*
*********************************************************************************************************
*	                                  
*	ģ������ : ������ϵͳ��Ϣ�Ի���
*	�ļ����� : App_SysInfo.c
*	��    �� : V1.0
*	˵    �� : ���Ի����������������ϡ�
*              1. ��ʾ��ǰIP��ַ��δ��������ʱ��ʾ192.168.X.X
*              2. ��ʾemWin��̬�ڴ��ʣ�࣬�ܹ��Ƿ�����12MB��
*              3. ��ʾCPU�����ʣ��ϵ���ж�ʱ������ʾ��0.0%����Ϊ�����ʼ��ռ����һ��ʱ��CPU���������ߵĻ�
*                 ʱ���Գ�������������ˡ�
*              4. ��ѹ�͵����Ĳ��Թ���δ���ϡ�
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
*                                        �궨����ⲿ����Ӧ��
*********************************************************************************************************
*/ 
#define MAG          3
extern uint8_t g_ucNETDetect;


/*
*********************************************************************************************************
*	�� �� ��: _cbDialogSysInfo
*	����˵��: �ص�����
*	��    ��: pMsg  WM_MESSAGE����ָ�����           
*	�� �� ֵ: ��
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
			/* ���ÿ�������� */
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
*	�� �� ��: CreateSysInfoDlg
*	����˵��: ϵͳ��Ϣ�Ի���
*	��    ��: ��   	
*	�� �� ֵ: ���ھ��
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

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

/*
*********************************************************************************************************
*	                                  
*	ģ������ : ʱ�䣬���ں��������ý���
*	�ļ����� : App_CalendarChildDlg.c
*	��    �� : V1.0
*	˵    �� : ʵ��STM32F429�Դ�RTC��ʱ�䣬���ں���������
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
*                                        �궨��
*********************************************************************************************************
*/
#define ID_SPINBOX_0  (GUI_ID_USER + 0x01)
#define ID_SPINBOX_1  (GUI_ID_USER + 0x02)
#define ID_SPINBOX_2  (GUI_ID_USER + 0x03)
#define ID_SPINBOX_3  (GUI_ID_USER + 0x04)
#define ID_SPINBOX_4  (GUI_ID_USER + 0x05)
#define ID_SPINBOX_5  (GUI_ID_USER + 0x06)


/*
*********************************************************************************************************
*                                        ����
*********************************************************************************************************
*/
const char *apDays[] =
{
	"��",
	"��",
	"һ",
	"��",
	"��",
	"��",
	"��",
	"��",
};

const char *apMonths[]=
{
	"1��",
	"2��",
	"3��",
	"4��",
	"5��",
	"6��",
	"7��",
	"8��",
	"9��",
	"10��",
	"11��",
	"12��",
};


/*
*********************************************************************************************************
*	                               �Ի���ؼ��б�
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateSetTimeAlarm[] = {
    { WINDOW_CreateIndirect,  "Time Settings",        	0,                     0,  0,  800,480,0,0},
    { TEXT_CreateIndirect,      "ʱ������",          GUI_ID_TEXT0,            114,22, 73, 20, 0,0},
    { TEXT_CreateIndirect,      "��������",          GUI_ID_TEXT1,            314,22, 73, 20, 0,0},
    { CHECKBOX_CreateIndirect,  "����ʱ������",  	 GUI_ID_CHECK0,           60,270,210,24, 0,0},
	{ CHECKBOX_CreateIndirect,  "������������",   	 GUI_ID_CHECK1,           280,270,210,24, 0,0},
	{ CHECKBOX_CreateIndirect,  "��������������",    GUI_ID_CHECK2,           500,270,210,24, 0,0},
    { TEXT_CreateIndirect,      "ʱ:",               GUI_ID_TEXT2,            40, 59, 29, 20, 0,0},
    { TEXT_CreateIndirect,      "��:",               GUI_ID_TEXT3,            40, 106+15,29, 20, 0,0},
    { TEXT_CreateIndirect,      "��:",               GUI_ID_TEXT4,            40, 156+30,29, 20, 0,0},
	
	{ BUTTON_CreateIndirect, "Button", GUI_ID_BUTTON0, 200, 400, 100, 45, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "Button", GUI_ID_BUTTON1, 500, 400, 100, 45, 0, 0x0, 0 },

	{ SPINBOX_CreateIndirect, "Spinbox", ID_SPINBOX_0, 80, 56, 130, 35, 0, 0x0, 0 },
	{ SPINBOX_CreateIndirect, "Spinbox", ID_SPINBOX_1, 80, 103 + 15, 130, 35, 0, 0x0, 0 },
	{ SPINBOX_CreateIndirect, "Spinbox", ID_SPINBOX_2, 80, 149 + 30, 130, 35, 0, 0x0, 0 },
	{ SPINBOX_CreateIndirect, "Spinbox", ID_SPINBOX_3, 280, 56, 130, 35, 0, 0x0, 0 },
	{ SPINBOX_CreateIndirect, "Spinbox", ID_SPINBOX_4, 280, 103 + 15, 130, 35, 0, 0x0, 0 },
	{ SPINBOX_CreateIndirect, "Spinbox", ID_SPINBOX_5, 280, 153 + 30, 130, 35, 0, 0x0, 0 },
};

/*
*********************************************************************************************************
*	�� �� ��: InitDialogSetTimeAlarm
*	����˵��: �ص�������ʼ����Ϣ 
*	��    ��: pMsg  ��Ϣָ�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
extern RTC_TimeTypeDef  RTC_TimeStructure;
extern RTC_DateTypeDef  RTC_DateStructure;
extern RTC_AlarmTypeDef RTC_AlarmStructure;
void InitDialogSetTimeAlarm(WM_MESSAGE * pMsg)
{
    WM_HWIN hWin = pMsg->hWin;
	
	/* �����ؼ���ʼ�� */
	CALENDAR_SetDefaultSize(CALENDAR_SI_HEADER, 35 );
	CALENDAR_SetDefaultSize(CALENDAR_SI_CELL_X, 40 );
	CALENDAR_SetDefaultSize(CALENDAR_SI_CELL_Y, 28 );

	CALENDAR_SetDefaultFont(CALENDAR_FI_CONTENT, &GUI_FontHZ16);
	CALENDAR_SetDefaultFont(CALENDAR_FI_HEADER, &GUI_FontHZ16); 
	CALENDAR_SetDefaultBkColor(CALENDAR_CI_WEEKDAY, 0xFF00FF);
	
	CALENDAR_SetDefaultDays(apDays);
	CALENDAR_SetDefaultMonths(apMonths);

	RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
	CALENDAR_Create(hWin, 
	                 450, 
	                 22, 
	                 RTC_DateStructure.RTC_Year+2000, 
	                 RTC_DateStructure.RTC_Month, 
					 RTC_DateStructure.RTC_Date, 
					 2, 
					 GUI_ID_CALENDAR0, 
					 WM_CF_SHOW);

	/* �ı��ؼ���ʼ�� */
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT0), &GUI_FontHZ16);
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT1), &GUI_FontHZ16);
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT2), &GUI_FontHZ16);
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT3), &GUI_FontHZ16);
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT4), &GUI_FontHZ16);

	/* ��ѡ���ʼ�� */
    CHECKBOX_SetText(WM_GetDialogItem(hWin,GUI_ID_CHECK0), "����ʱ������");
    CHECKBOX_SetFont(WM_GetDialogItem(hWin,GUI_ID_CHECK0), &GUI_FontHZ16);
    CHECKBOX_SetText(WM_GetDialogItem(hWin,GUI_ID_CHECK1), "������������");
    CHECKBOX_SetFont(WM_GetDialogItem(hWin,GUI_ID_CHECK1), &GUI_FontHZ16);
    CHECKBOX_SetText(WM_GetDialogItem(hWin,GUI_ID_CHECK2), "��������������");
    CHECKBOX_SetFont(WM_GetDialogItem(hWin,GUI_ID_CHECK2), &GUI_FontHZ16);
	
	/* ��ť��ʼ�� */
    BUTTON_SetFont(WM_GetDialogItem(hWin,GUI_ID_BUTTON0), &GUI_FontHZ16);
    BUTTON_SetText(WM_GetDialogItem(hWin,GUI_ID_BUTTON0), "ȷ��");
	BUTTON_SetFont(WM_GetDialogItem(hWin,GUI_ID_BUTTON1), &GUI_FontHZ16);
    BUTTON_SetText(WM_GetDialogItem(hWin,GUI_ID_BUTTON1), "ȡ��");
	
	/* �༭���ʼ�� */
	EDIT_SetTextAlign(SPINBOX_GetEditHandle(WM_GetDialogItem(hWin,ID_SPINBOX_0)), GUI_TA_HCENTER | GUI_TA_VCENTER);
	EDIT_SetTextAlign(SPINBOX_GetEditHandle(WM_GetDialogItem(hWin,ID_SPINBOX_1)), GUI_TA_HCENTER | GUI_TA_VCENTER);
	EDIT_SetTextAlign(SPINBOX_GetEditHandle(WM_GetDialogItem(hWin,ID_SPINBOX_2)), GUI_TA_HCENTER | GUI_TA_VCENTER);
	EDIT_SetTextAlign(SPINBOX_GetEditHandle(WM_GetDialogItem(hWin,ID_SPINBOX_3)), GUI_TA_HCENTER | GUI_TA_VCENTER);
	EDIT_SetTextAlign(SPINBOX_GetEditHandle(WM_GetDialogItem(hWin,ID_SPINBOX_4)), GUI_TA_HCENTER | GUI_TA_VCENTER);
	EDIT_SetTextAlign(SPINBOX_GetEditHandle(WM_GetDialogItem(hWin,ID_SPINBOX_5)), GUI_TA_HCENTER | GUI_TA_VCENTER);

	/* SPINBOX��ʼ�� */
	SPINBOX_SetEdge(WM_GetDialogItem(hWin,ID_SPINBOX_0), SPINBOX_EDGE_CENTER);
	SPINBOX_SetEdge(WM_GetDialogItem(hWin,ID_SPINBOX_1), SPINBOX_EDGE_CENTER);
	SPINBOX_SetEdge(WM_GetDialogItem(hWin,ID_SPINBOX_2), SPINBOX_EDGE_CENTER);
	SPINBOX_SetEdge(WM_GetDialogItem(hWin,ID_SPINBOX_3), SPINBOX_EDGE_CENTER);
	SPINBOX_SetEdge(WM_GetDialogItem(hWin,ID_SPINBOX_4), SPINBOX_EDGE_CENTER);
	SPINBOX_SetEdge(WM_GetDialogItem(hWin,ID_SPINBOX_5), SPINBOX_EDGE_CENTER);
	
	SPINBOX_SetFont(WM_GetDialogItem(hWin,ID_SPINBOX_0), &GUI_FontHZ16);
	SPINBOX_SetFont(WM_GetDialogItem(hWin,ID_SPINBOX_1), &GUI_FontHZ16);
	SPINBOX_SetFont(WM_GetDialogItem(hWin,ID_SPINBOX_2), &GUI_FontHZ16);
	SPINBOX_SetFont(WM_GetDialogItem(hWin,ID_SPINBOX_3), &GUI_FontHZ16);
	SPINBOX_SetFont(WM_GetDialogItem(hWin,ID_SPINBOX_4), &GUI_FontHZ16);
	SPINBOX_SetFont(WM_GetDialogItem(hWin,ID_SPINBOX_5), &GUI_FontHZ16);
	
	SPINBOX_SetRange(WM_GetDialogItem(hWin,ID_SPINBOX_0), 0, 23);  /* Сʱ */
	SPINBOX_SetRange(WM_GetDialogItem(hWin,ID_SPINBOX_1), 0, 59);  /* ���� */
	SPINBOX_SetRange(WM_GetDialogItem(hWin,ID_SPINBOX_2), 0, 59);  /* ���� */
	
	SPINBOX_SetRange(WM_GetDialogItem(hWin,ID_SPINBOX_3), 0, 23);  /* ����Сʱ */
	SPINBOX_SetRange(WM_GetDialogItem(hWin,ID_SPINBOX_4), 0, 59);  /* ���ӷ��� */
	SPINBOX_SetRange(WM_GetDialogItem(hWin,ID_SPINBOX_5), 0, 59);  /* �������� */
	
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	RTC_GetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);
	
	SPINBOX_SetValue(WM_GetDialogItem(hWin,ID_SPINBOX_0), RTC_TimeStructure.RTC_Hours);   /* ʱ */								   
	SPINBOX_SetValue(WM_GetDialogItem(hWin,ID_SPINBOX_1), RTC_TimeStructure.RTC_Minutes); /* �� */							   
	SPINBOX_SetValue(WM_GetDialogItem(hWin,ID_SPINBOX_2), RTC_TimeStructure.RTC_Seconds); /* �� */
	
	SPINBOX_SetValue(WM_GetDialogItem(hWin,ID_SPINBOX_3), RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours);   /* ����ʱ */			 
	SPINBOX_SetValue(WM_GetDialogItem(hWin,ID_SPINBOX_4), RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes); /* ���ӷ� */				 
	SPINBOX_SetValue(WM_GetDialogItem(hWin,ID_SPINBOX_5), RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds); /* ������ */
}

/*
*********************************************************************************************************
*	�� �� ��: _cbCallbackSetTimeAlarm
*	����˵��: �ص����� 
*	��    ��: pMsg  ��Ϣָ�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _cbCallbackSetTimeAlarm(WM_MESSAGE * pMsg) 
{
    int NCode, Id;
    WM_HWIN hWin = pMsg->hWin;
	CALENDAR_DATE CALENDAR_Date;
	
    switch (pMsg->MsgId) 
    {
        case WM_INIT_DIALOG:
            InitDialogSetTimeAlarm(pMsg);
            break;
		
        case WM_KEY:
            switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
            {
                case GUI_KEY_ESCAPE:
                    GUI_EndDialog(hWin, 1);
                    break;
				
                case GUI_KEY_ENTER:
                    GUI_EndDialog(hWin, 0);
                    break;
            }
            break;
			
        case WM_NOTIFY_PARENT:
            Id = WM_GetId(pMsg->hWinSrc); 
            NCode = pMsg->Data.v;        
            switch (Id) 
            {
				/* ���ȷ�� */
				case GUI_ID_BUTTON0:
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						
						case WM_NOTIFICATION_RELEASED:
							
							/* �Ƿ񱣴�ʱ������ */
							if(CHECKBOX_GetState(WM_GetDialogItem(hWin,GUI_ID_CHECK0)) == 1)
							{
								RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
								RTC_TimeStructure.RTC_Hours   = SPINBOX_GetValue(WM_GetDialogItem(hWin,ID_SPINBOX_0));
								RTC_TimeStructure.RTC_Minutes = SPINBOX_GetValue(WM_GetDialogItem(hWin,ID_SPINBOX_1));
								RTC_TimeStructure.RTC_Seconds = SPINBOX_GetValue(WM_GetDialogItem(hWin,ID_SPINBOX_2));
								RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);
							}
							
							/* �Ƿ񱣴�����ʱ������ */
							if(CHECKBOX_GetState(WM_GetDialogItem(hWin,GUI_ID_CHECK1)) == 1)
							{
								/* ���� Alarm A */
								RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
								RTC_AlarmStructure.RTC_AlarmTime.RTC_H12     = RTC_H12_AM;
								RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours   = SPINBOX_GetValue(WM_GetDialogItem(hWin,ID_SPINBOX_3));
								RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = SPINBOX_GetValue(WM_GetDialogItem(hWin,ID_SPINBOX_4));
								RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = SPINBOX_GetValue(WM_GetDialogItem(hWin,ID_SPINBOX_5));
				
								/* ���� Alarm A */
								RTC_AlarmStructure.RTC_AlarmDateWeekDay = 0x31;
								RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
								RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;

								/* ���� RTC Alarm A �Ĵ��� */
								/* ������Ҫ��������Լ����Ƿ����óɹ���״̬��� */
								RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);

								/* ʹ�� RTC Alarm A �ж� */
								RTC_ITConfig(RTC_IT_ALRA, ENABLE);

								/* ʹ�� alarm  A */
								RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
							}
							
							/* �Ƿ񱣴����������� */
							if(CHECKBOX_GetState(WM_GetDialogItem(hWin,GUI_ID_CHECK2)) == 1)
							{
								CALENDAR_GetSel(WM_GetDialogItem(hWin,GUI_ID_CALENDAR0), &CALENDAR_Date);
								
								RTC_DateStructure.RTC_Year = CALENDAR_Date.Year%100;
								RTC_DateStructure.RTC_Month = CALENDAR_Date.Month;
								RTC_DateStructure.RTC_Date = CALENDAR_Date.Day;
								RTC_DateStructure.RTC_WeekDay = RTC_CalcWeek(RTC_DateStructure.RTC_Year, 
																			 RTC_DateStructure.RTC_Month,
																			 RTC_DateStructure.RTC_Date);
								
								RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);
								//CALENDAR_SetDate(WM_GetDialogItem(hWin,GUI_ID_CALENDAR0), &CALENDAR_Date);
							}
							
						    GUI_EndDialog(hWin, 0);
							break;
					}
					break;
				
				/* ���ȡ����ť */
				case GUI_ID_BUTTON1:
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						
						case WM_NOTIFICATION_RELEASED:
							GUI_EndDialog(hWin, 0);
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
*	�� �� ��: App_CalendarChild
*	����˵��: ���ӣ�ʱ�䣬�������öԻ��� 
*	��    ��: hWin �����ھ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void App_CalendarChild(WM_HWIN hWin) 
{
	WM_HWIN hWinAlarm; 
    
	hWinAlarm = GUI_CreateDialogBox(_aDialogCreateSetTimeAlarm, 
	                                GUI_COUNTOF(_aDialogCreateSetTimeAlarm), 
	                                &_cbCallbackSetTimeAlarm, 
	                                hWin, 
	                                0, 
	                                0);
	
	/* ����Ϊģ̬���� */
	WM_MakeModal(hWinAlarm);
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

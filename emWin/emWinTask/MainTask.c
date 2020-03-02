/*
*********************************************************************************************************
*	                                  
*	ģ������ : GUI��������
*	�ļ����� : MainTask.c
*	��    �� : V1.0
*	˵    �� : ���洰�����潨��JPG����ͼƬ������һ��֧����������С��motion���ڣ�����ʵ��������������л���
*              ÿ������С��λ�ô���һ��ICONVIEW�ؼ���������������
*              1. ��������10��Ӧ��ͼ�ꡣ
*              2. ������ʱ�������ͼ����������򻬶�����Ȼ���Ե��ͼ���������л������������Ļ������á�
*              3. ״̬�����ļ�App_StatusDlg.c��ͷ��˵����ϵͳ��Ϣ�����ļ�App_SysInfoDlg.c��ͷ��˵����
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

extern GUI_CONST_STORAGE GUI_BITMAP bmLOGO;
//extern GUI_CONST_STORAGE GUI_BITMAP bmLOGO_PNG;

/*
*********************************************************************************************************
*	                                  ���ڱ��ļ��ĵ���
*********************************************************************************************************
*/
#if 0
	#define printf_main printf
#else
	#define printf_main(...)
#endif


/*
*********************************************************************************************************
*                                         �궨�� 
*  ICONVIEW�����沼�� 800*480��ÿ��ͼ��̫С��72*72
*  1. ͼ��Ŀ��λ100���ء�
*  2. ICONVIEW�ؼ������������߽�ľ��붼��38�����ء�
*  3. ICONVIEW�ؼ��ඥ�߽���10�����ء�
*  4. ͼ����ͼ��֮���X������4�����ء�
*  5. ͼ����ͼ��֮���Y������5�����ء�
* 
*  |-----------------------------------800------------------------------|---
*  |    | 100|   | 100|   | 100|   | 100|   | 100|   | 100|   | 100|    | 10                                                          |
*  |-38-|-ͼ-|-4-|-ͼ-|-4-|-ͼ-|-4-|-ͼ-|-4-|-ͼ-|-4-|-ͼ-|-4-|-ͼ-|-38-|-----��95 ͼ��߶�
*  |                                                                    | 5 
*  |-38-|-ͼ-|-4-|-ͼ-|-4-|-ͼ-|-4-|-ͼ-|-4-|-ͼ-|-4-|-ͼ-|-4-|-ͼ-|-38-|-----��95 ͼ��߶�
*  |                                                                    |
* 480                                                                   |
*  |                                                                    |
*  |                                                                    |
*  |                                                                    |
*  |                                                                    |
*  |                                                                    |
*  |--------------------------------------------------------------------|
*
*********************************************************************************************************
*/ 


#define ICONVIEW_TBorder   10   /* �ؼ�ICONVIEW���ϱ߾� */
#define ICONVIEW_LBorder   40   /* �ؼ�ICONVIEW����߾� */  
#define ICONVIEW_Width     724  /* �ؼ�ICONVIEW�Ŀ� */  
#define ICONVIEW_Height    100  /* �ؼ�ICONVIEW�ĸ� */  
#define ICONVIEW_YSpace    5    /* �ؼ�ICONVIEW��Y������ */
#define ICONVIEW_XSpace    4    /* �ؼ�ICONVIEW��Y������ */

#define ICON_Width         100      /* �ؼ�ICONVIEW��ͼ��Ŀ�� */  
#define ICON_Height        95       /* �ؼ�ICONVIEW��ͼ��ĸ߶�, ��ͼ��������ı���һ��ĸ߶� */ 

#define SCREEN_Width    800     /* ��ʾ���Ŀ�� */  
#define SCREEN_Height   480     /* ��ʾ���߶�   */ 
#define LineCap         16      /* �ַ�����ĸ���16 */

/*
*********************************************************************************************************
*                                         ����
*********************************************************************************************************
*/ 
static WM_HWIN hIcon1;
static GUI_MEMDEV_Handle   hMempic;
WM_HWIN hMotion;


/*
*********************************************************************************************************
*                               ����ICONVIEW�ؼ��������
*********************************************************************************************************
*/ 
typedef struct 
{
	const GUI_BITMAP * pBitmap;
	const char       * pTextEn;
	const char       * pTextCn;
} BITMAP_ITEM;

typedef struct 
{
  int xPos;
  int yPos;
  int Index;
} ITEM_INFO;

typedef struct 
{
  int          Pos;
  int          FinalMove;
  ITEM_INFO    * pItemInfo;
} PARA;


/* ���ڵ�һ��������ICONVIEWͼ��Ĵ��� */
static const BITMAP_ITEM _aBitmapItem1[] = 
{
	{&bmComputer, "Computer",   "�ҵĵ���"},
	{&bmSettings, "Settings",   "ϵͳ��Ϣ"},
	{&bmMusic,    "Music",      "���ֲ���"},
	{&bmVideo,    "Vedio",      "��Ƶ����"},
	{&bmRecorder, "Recorder",   "¼����"},
	{&bmRadio,    "FM/AM",      "������"},
	{&bmReader,   "Reader",     "�ļ��Ķ�"},
	
	{&bmPicture,   "Picture",     "ͼƬ���"},
	{&bmADC,       "ADC",         "ģ��ת��"},
	{&bmClock,     "Clock",          "ʱ��"},
	{&bmCalculator,"Calculator",  "������"}, 
};



/*
*********************************************************************************************************
*                                  Ӧ�ó�����ں���
*********************************************************************************************************
*/ 
static void (* _apModules0[])( WM_HWIN hWin) = 
{
	App_Computer,
	App_SysConfig,
	App_Music,
	App_Video,
	App_Recorder,
	App_Radio,
	App_FileBrowse,
	
	App_PicBrowse,
	App_Reserved,
	App_Calendar,
	App_Calculator,
};



/*
*********************************************************************************************************
*	�� �� ��: _cbBkWindow
*	����˵��: ���洰�ڵĻص�����,������Ҫ�ǻ��Ʊ������ںͽ����л�ʱ���л���־�Ļ��� 
*	��    ��: pMsg  WM_MESSAGE����ָ�����   
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _cbBkWindow(WM_MESSAGE * pMsg) 
{

	switch (pMsg->MsgId) 
	{
		/* �ػ���Ϣ*/
		case WM_PAINT:
			GUI_MEMDEV_WriteAt(hMempic, 0, 0);//���ڴ��豸������д�뵱ǰѡ���豸��ָ��λ�ã���alpha ͨ��
			break;
	
		default:
			WM_DefaultProc(pMsg);
			break;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: _cbMotion
*	����˵��: Motion���ڵĻص���������Ҫ������ͼ��Ļ�������
*	��    ��: pMsg  WM_MESSAGE����ָ�����   
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _cbMotion(WM_MESSAGE * pMsg) 
{

	static uint32_t  tStart, tEnd;
	int NCode, Id;
	
	switch (pMsg->MsgId) 
	{
		case WM_PRE_PAINT:
			GUI_MULTIBUF_Begin();
			break;
		
		case WM_POST_PAINT:
			GUI_MULTIBUF_End();
			break;
		
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);     
			NCode = pMsg->Data.v;                 
			switch (Id) 
			{
				/* ��һ�������ϵ�ͼ�� */
				case GUI_ID_ICONVIEW0:
					switch (NCode) 
					{
						/* ICON�ؼ������Ϣ */
						case WM_NOTIFICATION_CLICKED:
							tStart = GUI_GetTime();
							break;

						/* ICON�ؼ��ͷ���Ϣ */
						case WM_NOTIFICATION_RELEASED: 
							tEnd = GUI_GetTime() - tStart;
							if(tEnd > 800)
							{
								WM_SetFocus(WM_HBKWIN);
								break;							
							}
							_apModules0[ICONVIEW_GetSel(pMsg->hWinSrc)](WM_HBKWIN);
							break;
					}
					break;
				}			

  }
}

/*
*********************************************************************************************************
*	�� �� ��: _CreateICONVIEW
*	����˵��: ����ICONVIEW
*	��    �Σ�hParent   ������
*             pBm       ICONVIEW�ϵ�λͼ
*             BitmapNum ICONVIEW��ͼ�����       
*             x         x������
*             y         y������
*             w         ICONVIEW��
*             h         ICONVIEW��   
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static WM_HWIN _CreateICONVIEW(WM_HWIN hParent, const BITMAP_ITEM *pBm, int BitmapNum, int Id, int x, int y, int w, int h) 
{
	WM_HWIN hIcon;
	int i;
	
	/*��ָ��λ�ô���ָ���ߴ��ICONVIEW С����*/
	hIcon = ICONVIEW_CreateEx(x, 					/* С���ߵ��������أ��ڸ������У�*/
						     y, 					/* С���ߵ��������أ��ڸ������У�*/
							 w,                     /* С���ߵ�ˮƽ�ߴ磨��λ�����أ�*/
							 h, 	                /* С���ߵĴ�ֱ�ߴ磨��λ�����أ�*/
	                         hParent, 				            /* �����ڵľ�������Ϊ0 ������С���߽���Ϊ���棨�������ڣ����Ӵ��� */
							 WM_CF_SHOW | WM_CF_HASTRANS,       /* ���ڴ�����ǡ�ΪʹС���������ɼ���ͨ��ʹ�� WM_CF_SHOW */ 
	                         0,                                 /* Ĭ����0�����������ʵ������������ֱ������ */
							 Id, 			                    /* С���ߵĴ���ID */
							 ICON_Width, 				        /* ͼ���ˮƽ�ߴ�100 */
							 ICON_Height);						/* 95ͼ��Ĵ�ֱ�ߴ�,ͼ����ļ������������棬��Ҫ����ICONVIEW�ĸ߶ȣ�����Text��ʾ������*/
	
	
	/* ��ICONVIEW С���������ͼ�� */
	for (i = 0; i < BitmapNum; i++) 
	{
		ICONVIEW_AddBitmapItem(hIcon, pBm[i].pBitmap, pBm[i].pTextCn);
	}	
	ICONVIEW_SetFont(hIcon, &GUI_SysFontHZ16);	
	/* ����С���ߵı���ɫ 32 λ��ɫֵ��ǰ8 λ������alpha��ϴ���Ч��*/
	ICONVIEW_SetBkColor(hIcon, ICONVIEW_CI_SEL, GUI_WHITE | 0x80000000);	
	/* ����X����ı߽�ֵΪ0��Ĭ�ϲ���0, Y����Ĭ����0����������Ҳ����һ�����ã������Ժ��޸� */
	ICONVIEW_SetFrame(hIcon, GUI_COORD_X, 0);
	ICONVIEW_SetFrame(hIcon, GUI_COORD_Y, 0);	
	/* ����ͼ����x ��y �����ϵļ�ࡣ*/
	ICONVIEW_SetSpace(hIcon, GUI_COORD_X, ICONVIEW_XSpace);
	ICONVIEW_SetSpace(hIcon, GUI_COORD_Y, ICONVIEW_YSpace);	
	/* ���ö��뷽ʽ ��5.22�汾�����¼���� */
	ICONVIEW_SetIconAlign(hIcon, ICONVIEW_IA_HCENTER|ICONVIEW_IA_TOP);
	
	return hIcon;
}

/*
*********************************************************************************************************
*	�� �� ��: MainTask
*	����˵��: GUI������
*	��    ��: ��   	
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void MainTask(void) 
{ 
	
	PARA Para;
	PARA *pPara;
	
	/* ���ݳ�ʼ�� */
	pPara = &Para;
    pPara->Pos   = 0;
	
    GUI_Init();  /* ��ʼ�� */
	
	/*
	 ���ڶ໺��ʹ����ڴ��豸������˵��
	   1. ʹ�ܶ໺���ǵ��õ����º������û�Ҫ��LCDConf_Lin_Template.c�ļ��������˶໺�壬���ô˺�������Ч��
		  WM_MULTIBUF_Enable(1);
	   2. ����ʹ��ʹ���ڴ��豸�ǵ��ú�����WM_SetCreateFlags(WM_CF_MEMDEV);
	   3. ���emWin�����ö໺��ʹ����ڴ��豸��֧�֣���ѡһ���ɣ����������ѡ��ʹ�ö໺�壬ʵ��ʹ��
		  STM32F429BIT6 + 32λSDRAM + RGB565/RGB888ƽ̨���ԣ��໺�������Ч�Ľ��ʹ����ƶ����߻���ʱ��˺��
		  �У�����Ч����������ԣ�ͨ��ʹ�ܴ���ʹ���ڴ��豸���������ġ�
	   4. ����emWin����Ĭ���ǿ��������塣
	*/
	WM_MULTIBUF_Enable(1);
	/*
       ����У׼����Ĭ����ע�͵��ģ���������ҪУ׼������������У׼������û���ҪУ׼�������Ļ���ִ��
	   �˺������ɣ��Ὣ����У׼�������浽EEPROM���棬�Ժ�ϵͳ�ϵ���Զ���EEPROM������ء�
	*/	
	WM_MOTION_Enable(0);    /* ���ܻ��� ���Ҫ����д1*/ 
	/* Ĭ����500ms�����ｫ���޸�Ϊ50ms���������һ���̶��Ͼ����������ȣ��ܾ��������ȣ���Ҫ����
	   ��ΪF429���������ޡ� 
	*/
	WM_MOTION_SetDefaultPeriod(50);
	
	/* ��0����ϵͳ���ؽ��� ------------------------------------------*/	
	GUI_SetBkColor(GUI_WHITE);
	GUI_Clear();                         /*�޸������汳����ɫ������������������޸Ĳ��ɹ�	*/
	GUI_DrawBitmap(&bmLOGO,100,120);     /*��ʾ��ҵ���LOGO*/
	
	/* ��1�����������洰�ڵı���ͼƬ ------------------------------------------*//* �����ڴ��豸 */
	hMempic = GUI_MEMDEV_CreateFixed(0, 
	                                 0, 
	                                 LCD_GetXSize(), 
	                                 LCD_GetYSize(), 
									 GUI_MEMDEV_HASTRANS, 
									 GUI_MEMDEV_APILIST_16, 
									 GUICC_M565);
									 
	GUI_MEMDEV_Select(hMempic);/* ѡ���ڴ��豸 */
    GUI_JPEG_Draw(_acbkpic, sizeof(_acbkpic), 0, 0);/* ���� JPEG ���ڴ��豸�� */
	GUI_MEMDEV_Select(0);      /* ѡ���ڴ��豸��0 ��ʾѡ�� LCD */
	
	/* ��2����ʹ�����洰�ڵ� ------------------------------------------*/
	WM_SetCallback(WM_HBKWIN, _cbBkWindow);

	/* ��3�����������洰�ڵı���ͼƬ ------------------------------------------*/
	hMotion = WM_CreateWindowAsChild(0, 
	                                 0, 
									 SCREEN_Width*3, 
									 SCREEN_Height, 
					                 WM_HBKWIN,  
									 WM_CF_MOTION_X | WM_CF_SHOW | WM_CF_HASTRANS, 
									 _cbMotion, 
									 sizeof(pPara));
									 
    WM_SetUserData(hMotion, &pPara, sizeof(pPara));
	

	/* ��4�����������洰�ڵı���ͼƬ ------------------------------------------*/
	/* ��1������ͼ�� */
	hIcon1 = _CreateICONVIEW(hMotion, 
	                _aBitmapItem1, 
					GUI_COUNTOF(_aBitmapItem1), 
					GUI_ID_ICONVIEW0, 
					ICONVIEW_LBorder, //38��߾�
					ICONVIEW_TBorder, //10�ϱ߾�
					ICONVIEW_Width,   //�ؼ��Ŀ�724
					ICONVIEW_Height*2);//�ؼ��ĸ�100*2
	
	/* ��ֹ���棬�����Ժ��� */
	(void)hIcon1;

	/* ��5�����������洰�ڵı���ͼƬ ------------------------------------------*/
	CreateSysInfoDlg();	
	/* ��6�����������洰�ڵı���ͼƬ ------------------------------------------*/
	CreateStatusDlg();
	/* ��6������ʾ��� -------------------------------------------------------*/
	GUI_CURSOR_Show();
	
	while(1) 
	{
		GUI_Delay(10);
	}
}


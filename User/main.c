/*
*********************************************************************************************************
*
*	ģ������ : ������ģ��
*	�ļ����� : main.c
*	��    �� : V1.0
*	˵    �� : V6�������ۺ�Demo֮RTXʵʱ����ϵͳ���������м��
*              ʹ��ǰ��ע�������Ƶ��ʾ�͸���Ӧ�õĽ�ͼ˵������������Ҫ��������
*                http://bbs.armfly.com/read.php?tid=17330
*              �汾��Ϣ��
*                RTOS:         RL-RTX      V4.74
*                File System:  RL-FlashFS  V4.74
*                TCP/IP:       RL-TCPnet   V4.74
*                USB:          RL-USB      V4.74
*                CAN:          RL-CAN      V4.74
*                GUI:          emWin       V5.36
*                Modbus:       Designed by armfly
*              �������ã�
*                �ܹ�������15������
*                1. os_idle_demon��   ���ȼ�0: ϵͳ��������
*                2. AppTaskStatistic  ���ȼ�1��CPU�����ʼ�⡣
*                3. AppTaskGUI��      ���ȼ�3��GUI������
*               // 4. AppTaskTCPMain��  ���ȼ�4��TCPnet��������
*                5. AppTaskUserIF��   ���ȼ�5��������Ϣ��������
*                6. AppTaskMsgPro��   ���ȼ�6����Ҫ���ڽ�ͼ����
*                7. AppTaskStart��    ���ȼ�7����������
*               // 8. AppTaskCAN��      ���ȼ�8��CAN��������
*                9. AppTaskModbus��   ���ȼ�9��Modbus����
*              // 10. AppTaskUSB��      ���ȼ�10��USB����
*               11. AppTaskMusic��    ���ȼ�11�����������������ֲ���������Ƶ��������¼����������������
*               12. AppTaskPinDetect�����ȼ�12��SD����μ������
*              // 13. AppTaskTCPTimer�� ���ȼ�13��TCPnet���綨ʱ��������
*                   USB�����еĹ����лᴴ��2��������
*               14. USBD_RTX_EndPoint0          ���ȼ�2��USB�豸�˵�0����
*               15. USBD_RTX_MSC_EP_BULK_Event  ���ȼ�2��USB BULK���ݴ�������
*             �ж����ȼ���
*               NVIC���ȼ��������4������ռ���ȼ�����Χ0-15��
*               LTDC�жϣ�       ��ռ���ȼ� 0
*               ETH��̫���жϣ�  ��ռ���ȼ� 0
*               USB FS�жϣ�     ��ռ���ȼ� 0
*               CAN2 TX�жϣ�    ��ռ���ȼ� 1
*               CAN2 RX0�жϣ�   ��ռ���ȼ� 1
*               DMA2_Steam5�жϣ���ռ���ȼ� 1
*               DMA2_Steam1�жϣ���ռ���ȼ� 1
*               EXTI6�жϣ�      ��ռ���ȼ� 1
*               USART3�жϣ�     ��ռ���ȼ� 2
*               EXTI2�жϣ�      ��ռ���ȼ� 2
*               TIM2�жϣ�       ��ռ���ȼ� 4
*               USART1�жϣ�     ��ռ���ȼ� 13
*               RTC Alarm�жϣ�  ��ռ���ȼ� 13
*               SVC�жϣ�        ��ռ���ȼ� 14
*               Systick�жϣ�    ��ռ���ȼ� 15
*               PendSV�жϣ�     ��ռ���ȼ� 15
*              �ڴ���䣺
*               �ڲ�SRAM ,         CCM SRAM��SDRAM���š�
*               �ڲ�192KB SRAM   ��80KB���ڶ�̬�ڴ���䣬�������������ջ��ϵͳ��ջ���ļ�ϵͳ������Э��ջ�ȡ�
*               �ڲ�64KB CCM SRAM��������Ƶ������ʱ��JPEG���롣
*               �ⲿ16MB SDRAM   ��ǰ4MB����TFT�Դ档
*               �ⲿ16MB SDRAM   ����12MB����emWin��̬�ڴ档
*              ע�����
*                1. ÿ��MDK�İ�װĿ¼���涼����һ��RTXԴ�룬����MDK4.XX��˵�����ʹ�õ�
*                   �Ǹ�MDK�汾�����ʹ���Ǹ�MDK�汾�����RTX������ʹ��MDK�Դ���RTX�������
*                   ʱ��������ʾ��ȷ�ĵ�����Ϣ��
*                2. ��ǰʹ�õ�RTXԴ����MDK4.74����ģ�KEIL�ٷ��Ѿ�����MDK4ϵ�еĸ����ˣ�
*                   ����汾����MDK4ϵ���������°汾�ˡ������Ҫʹ��MDK�Դ���RTX���������ʾ
*                   ��Ϣ�������ʹ��MDK4.74.
*                3. ����MDK5.XX��RTXҲ���䰲װĿ¼���棬����RTX�Ѿ�����Ϊ�����汾�����ˣ�
*                   ����һ��ȫ�µ����ֽ�CMSIS-RTOS RTX��ARM�ٷ���RTX�Ļ����ϸ�RTX������һ���װ��
*                4. ��ʵ���Ƽ�ʹ�ô������SecureCRT��Ҫ�����ڴ�ӡЧ�������롣�������
*                   V6��������������С�
*                5. ��ؽ��༭��������������TAB����Ϊ4���Ķ����ļ���Ҫ��������ʾ�����롣
*
*	�޸ļ�¼ :
*		�汾��    ����         ����        ˵��
*		V1.0    2017-03-20   Eric2013      �׷�
*
*
*	Copyright (C), 2015-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/	
#include "includes.h"	
#include "MainTask.h"




/*
*********************************************************************************************************
*	                                  ���ڱ��ļ��ĵ���
*********************************************************************************************************
*/
#if 0
	#define printf_taskdbg printf
#else
	#define printf_taskdbg(...)
#endif


/*
**********************************************************************************************************
											��������
**********************************************************************************************************
*/
//��RTX���½�һ���������__task�������������Ĳ���ϵͳ
__task void AppTaskStatistic (void);
__task void AppTaskGUI(void);
__task void AppTaskUserIF(void);
__task void AppTaskMsgPro(void);
__task void AppTaskStart(void);
__task void AppTaskModbus(void);
__task void AppTaskMusic(void);
__task void AppTaskPinDetect(void);
static void AppTaskCreate (void);
static void AppObjCreate (void);


/*
**********************************************************************************************************
											 ����
**********************************************************************************************************
*/
static uint64_t AppTaskStatStk[1024/8];    	  /* ����ջ */
static uint64_t AppTaskGUIStk[4096/8];     	  /* ����ջ */
static uint64_t AppTaskUserIFStk[1024/8];     /* ����ջ */
static uint64_t AppTaskMsgProStk[1024/8];  	  /* ����ջ */
static uint64_t AppTaskStartStk[1024/8];   	  /* ����ջ */
static uint64_t AppTaskModbusStk[1024/8];     /* ����ջ */
static uint64_t AppTaskMusicStk[4096/8];      /* ����ջ */
static uint64_t AppTaskPinDetectStk[1024/8];  /* ����ջ */



/* ������ */
OS_TID HandleTaskStat = NULL;
OS_TID HandleTaskGUI = NULL;
OS_TID HandleTaskTCPMain = NULL;
OS_TID HandleTaskUserIF = NULL;
OS_TID HandleTaskMsgPro = NULL;
OS_TID HandleTaskStart = NULL;
OS_TID HandleTaskModbus = NULL;
OS_TID HandleMusicStart = NULL;
OS_TID HandlePinDetectStart = NULL;


/*
**********************************************************************************************************
											 ����
**********************************************************************************************************
*/
static OS_SEM semaphore;
volatile int16_t  iTemp;        

/* ͳ������ʹ�� */
__IO uint8_t   OSStatRdy;        /* ͳ�����������־ */
__IO uint32_t  OSIdleCtr;        /* ����������� */
__IO float     OSCPUUsage;       /* CPU�ٷֱ� */
uint32_t       OSIdleCtrMax;     /* 1�������Ŀ��м��� */
uint32_t       OSIdleCtrRun;     /* 1���ڿ�������ǰ���� */

os_mbx_declare (mailbox, 5);     /* ����һ��֧��5����Ϣ����Ϣ���� */

/* 80KB���ڲ�SRAM��64KB��CCM RAM��ͨ������os_init_mem��ʼ��Ϊ���鶯̬�ڴ�� */
uint64_t AppMalloc[80*1024/8];         
uint64_t AppMallocCCM[64*1024/8] __attribute__((at(0x10000000))); /* JPEG��� */

/*
*********************************************************************************************************
*	�� �� ��: main
*	����˵��: ��׼c������ڡ�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int main (void) 
{	
	/* ��ʼ������ */
	bsp_Init();
	
	/* ������������ */
 	os_sys_init_user (AppTaskStart,              /* ������ */
	                  7,                         /* �������ȼ� */
	                  &AppTaskStartStk,          /* ����ջ */
	                  sizeof(AppTaskStartStk));  /* ����ջ��С����λ�ֽ��� */
	while(1);
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskStatistic
*	����˵��: ͳ����������ʵ��CPU�����ʵ�ͳ�ơ�Ϊ�˲��Ը���׼ȷ�����Կ���ע�͵��õ�ȫ���жϿ���
*	��    ��: ��
*	�� �� ֵ: ��
*   �� �� ��: 1 (��ֵԽС���ȼ�Խ�ͣ������uCOS�෴)
*********************************************************************************************************
*/
void  OSStatInit (void)
{
	OSStatRdy = FALSE;
	
    os_dly_wait(2u);            /* ʱ��ͬ�� */
    OSIdleCtr    = 0uL;         /* ����м��� */	
    os_dly_wait(100);           /* ͳ��100ms�ڣ������м��� */
    OSIdleCtrMax = OSIdleCtr;   /* ���������м��� */
    OSStatRdy    = TRUE;
}

__task void  AppTaskStatistic (void)
{
    while (OSStatRdy == FALSE) 
	{
        os_dly_wait(200);     /* �ȴ�ͳ��������� */
    }

    OSIdleCtrMax /= 100uL;
    if (OSIdleCtrMax == 0uL) 
	{
        OSCPUUsage = 0u;
    }
    OSIdleCtr = OSIdleCtrMax * 100uL;  /* ���ó�ʼCPU������ 0% */	
    for (;;) 
	{
        OSIdleCtrRun = OSIdleCtr;    /* ���100ms�ڿ��м��� */
        OSIdleCtr    = 0uL;          /* ��λ���м��� */
        OSCPUUsage   = (100uL - (float)OSIdleCtrRun / OSIdleCtrMax);
        os_dly_wait(100);            /* ÿ100msͳ��һ�� */
    }
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskGUI
*	����˵��: GUI����
*	��    ��: ��
*	�� �� ֵ: ��
*   �� �� ��: 3 (��ֵԽС���ȼ�Խ�ͣ������uCOS�෴)
*********************************************************************************************************
*/
__task void AppTaskGUI(void)
{
    while(1)
    {
		MainTask();
    }
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskUserIF
*	����˵��: ������Ϣ����		
*	��    ��: ��
*	�� �� ֵ: ��
*   �� �� ��: 5
*********************************************************************************************************
*/
__task void AppTaskUserIF(void)
{
	uint8_t ucKeyCode;

    while(1)
    {
		ucKeyCode = bsp_GetKey();
		
		if (ucKeyCode != KEY_NONE)
		{
			switch (ucKeyCode)
			{
				/* K1�����£���ӡ����˵�� */
				case KEY_DOWN_K1:
					printf("K1�����£�ʹ��MDK���Դ���RTX��������������ʹ��MDK4.74�汾���е���\r\n");
					break;	
				
				/* K2�����£�ֱ�ӷ����ź���ͬ���źŸ�����AppTaskMsgPro */
				case KEY_DOWN_K2:
					printf("K2�����£�ֱ�ӷ����ź���ͬ���źŸ�����AppTaskMsgPro\r\n");
					os_sem_send (&semaphore);
					break;

				/* �����ļ�ֵ������ */
				default:                     
					break;
			}
		}		
		os_dly_wait(20);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskMsgPro
*	����˵��: ��Ϣ�����ȴ�����AppTaskUserIF�������¼���־��ʵ�ֽ�ͼ���ܣ���BMPͼƬ�洢��SD��
*	��    ��: ��
*	�� �� ֵ: ��
*   �� �� ��: 6 
*********************************************************************************************************
*/
__task void AppTaskMsgPro(void)
{
	OS_RESULT xResult;
	const uint16_t usMaxBlockTime = 0xFFFF;
	uint8_t	Pic_Name = 0;
	char buf[20];
	
    while(1)
    {
		xResult = os_sem_wait (&semaphore, usMaxBlockTime);
		
		switch (xResult)
		{
			/* ����ȴ����ܵ��ź���ͬ���ź� */
			case OS_R_OK:

			/* �ź��������ã�usMaxBlockTime�ȴ�ʱ�����յ��ź���ͬ���ź� */
			case OS_R_SEM:
				sprintf((char *)buf,"M0:\\PicSave\\%d.bmp",Pic_Name);
			
				foutbmp = fopen (buf, "w"); 
				if (foutbmp != NULL) 
				{
					/* ��SD������BMPͼƬ */
					GUI_BMP_Serialize(_WriteByte2File, foutbmp);
					
					/* �ر��ļ� */
					fclose(foutbmp);
				}
				printf_taskdbg("��ͼ���\r\n");			
				Pic_Name++; 	
				break;

			/* ��ʱ */
			case OS_R_TMO:
				break;
			
			/* ����ֵ������ */
			default:                     
				break;
		}	
    }
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskStart
*	����˵��: �����������ڰ������������ʹ����ļ�⡣
*	��    ��: ��
*	�� �� ֵ: ��
*   �� �� ��: 7  
*********************************************************************************************************
*/
__task void AppTaskStart(void)
{	
	/* ���ȴ���ͳ������-----------------*/
	HandleTaskStat = os_tsk_create_user(AppTaskStatistic,        /* ������ */ 
	                                    1,                       /* �������ȼ� */ 
					                    &AppTaskStatStk,         /* ����ջ */
					                    sizeof(AppTaskStatStk)); /* ����ջ��С����λ�ֽ��� */
	
	OSStatInit();	
	/* �ȴ��������ͨ�Ż��ƣ���ֹ�����ȴ�����ֱ�ӾͿ���ʹ�� */
	AppObjCreate();	
	/* �������� */
	AppTaskCreate();	
    while(1)
    {		
		/* 10msһ�δ���ɨ�裬���ݴ�����GT811 */
		if(g_GT811.Enable == 1)
		{
			bsp_KeyScan();  //ɨ�谴���Ƿ񱻰���
			GT811_OnePiontScan();
			BEEP_Pro();
			os_dly_wait(10);			
		}
    }
}


/*
*********************************************************************************************************
*	�� �� ��: AppTaskModbus
*	����˵��: RS485 Modbus���ߴ�������ʵ�ֵ���Modbus��վ
*	��    ��: ��
*	�� �� ֵ: ��
*   �� �� ��: 9  
*********************************************************************************************************
*/
__task void AppTaskModbus(void)
{
	OS_RESULT xResult;
	uint8_t ret = 0;
 
	
    while(1) 
    {
		if(os_evt_wait_or(0x01FF, 0xFFFF) == OS_R_EVT)
        {
            xResult = os_evt_get ();
            switch (xResult)
            {	
				/* O1H�Ĵ��� */
				case 0x0001:
					ret = MODH_ReadParam_01H(REG_D01, 4);
					break;
				
				/* 02H�Ĵ��� */
				case 0x0002:
					ret = MODH_ReadParam_02H(REG_T01, 3);
					break;
				
				/* 03H�Ĵ��� */
				case 0x0004:
					ret = MODH_ReadParam_03H(REG_P01, 2);
					break;
				
				/* 04H�Ĵ��� */
				case 0x0008:
					ret = MODH_ReadParam_04H(REG_A01, 1);
				break;
				
				/* 05H�Ĵ��� */
				case 0x0010:
					ret = MODH_WriteParam_05H(REG_D01, 1);
					break;
				
				/* 05H�Ĵ��� */
				case 0x0020:
					ret = MODH_WriteParam_05H(REG_D01, 0);
					break;
				
				/* 06H�Ĵ��� */
				case 0x0040:
					ret = MODH_WriteParam_06H(REG_P01, 1);
					break;
				
				/* 06H�Ĵ��� */
				case 0x0080:
					ret = MODH_WriteParam_06H(REG_P01, 0);
					break;
				
				/* 10H�Ĵ��� */
				case 0x0100:
					{
						uint8_t buf[4];						
						buf[0] = 0x01;
						buf[1] = 0x02;
						buf[2] = 0x03;
						buf[3] = 0x04;
						ret = MODH_WriteParam_10H(REG_P01, 2, buf);
					}
					break;
				
				default:
					break;
			}
			
			if(ret == 1)
			{
				/* ��hWinModbus���ڷ���Ϣ���Ĵ�������ɹ� */
				if(hWinModbus != WM_HWIN_NULL)
				{
					WM_SendMessageNoPara(hWinModbus, MSG_ModbusSuccess);	
				}
			}
			else 
			{
				/* ��hWinModbus���ڷ���Ϣ���Ĵ�������ʧ�� */
				if(hWinModbus != WM_HWIN_NULL)
				{
					WM_SendMessageNoPara(hWinModbus, MSG_ModbusErr);
				}
			}
		}			
    }
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskMusic
*	����˵��: ���ֲ���������Ƶ��������¼����������������ͨ���������ʵ�֡�
*	��    ��: ��
*	�� �� ֵ: ��
*   �� �� ��: 11
*********************************************************************************************************
*/
__task void AppTaskMusic(void)
{
	MusicMsg_T *pMsg;
	OS_RESULT xResult;
	const uint16_t usMaxBlockTime = 0xFFFF; /* �ӳ����� */
	
    while(1)
    {
		xResult = os_mbx_wait(&mailbox, (void *)&pMsg, usMaxBlockTime);
		switch (xResult)
		{
			/* ����ȴ����ܵ���Ϣ�������� */
			case OS_R_MBX:
			case OS_R_OK:
				/* DMA��ֹ��Ҫ�ȵ���ǰת����ϲſ���ֹͣ������������Ҫ�����Ӧ��־ */
				os_evt_clr(MusicTaskWaitFlag, HandleMusicStart);
			
				if(pMsg->ucType == MusicType_WAV)
				{
					/* WAV��Ƶ���� */
					WavMusicPlay((const char *)pMsg->ucName);					
				}
				else if(pMsg->ucType == MusicType_MP3)
				{
					/* MP3��Ƶ���� */
					MP3MusicPlay((const char *)pMsg->ucName);		
				}
				else if(pMsg->ucType == MusicType_REC)
				{
					/* ¼���� */
					WavRecPlay((const char *)pMsg->ucName);
				}	
				else if(pMsg->ucType == MusicType_Radio)
				{
					/* ������ */
					RadioPlay();				
				}
				break;	
			
			/* ����ֵ������ */
			default:                     
				break;
		}	
    }
}
/*
*********************************************************************************************************
*	�� �� ��: AppTaskPinDetect
*	����˵��: ��Ҫʵ��SD����μ��
*	��    ��: ��
*	�� �� ֵ: ��
*   �� �� ��: 12 
*********************************************************************************************************
*/
__task void AppTaskPinDetect(void)
{
	OS_RESULT xResult;
	const uint16_t usMaxBlockTime = 0xFFFF; /* �������޵ȴ� */
	
	/* ���޵ȴ���ֱ��GUI��״̬���Ի����Ѿ����� */
	if(os_evt_wait_or(GUIDetectBIT_15, usMaxBlockTime) == OS_R_EVT){}
	
	/* 
		����ϵ�ǰSD���Ƿ��Ѿ����룬����������޷�ͨ���ж����ż�������
	    ��Ϊû�������ػ����½��ص����䡣SD�����ǵ͵�ƽ���γ��Ǹߵ�ƽ��
		PE2
	*/
	if(!(GPIOE->IDR & (1 << 2)))
	{
		os_evt_set(SDDetectBIT_0, os_tsk_self());
	}
		
    while(1)
    {
		/*
			1. ���޵ȴ�SD�����״̬�仯��
		    2. ���޵ȴ����߲��״̬�仯����δ�����������ʵ�֡�
		*/
		if(os_evt_wait_or(PinDetect_ALL, usMaxBlockTime) == OS_R_EVT)
		{
			xResult = os_evt_get ();
		
			switch (xResult)
			{
				case SDDetectBIT_0:
				    /* ����һ���ӳ�ʱ�䣬�˳����SD��ʱ��ɵĶ���, �γ��������ϴ�����ȴ�100ms�ټ�� */
					os_dly_wait(100);
					if((GPIOE->IDR & (1 << 2)))
					{
						UnmountSD(); /* ж��SD�� */
						WM_SendMessageNoPara(hWinStatus, MSG_SDPlugOut);
					}
					else
					{
						MountSD();  /* ����SD�� */
						WM_SendMessageNoPara(hWinStatus, MSG_SDPlugIn);
					}
					EXTI->IMR |= (1<<2); /* ʹ���ж�  */
					break;
					
				/* ����ֵ������ */
				default:                     
					break;
			}	
		}
    }
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskCreate
*	����˵��: ����Ӧ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void AppTaskCreate (void)
{

	HandleTaskGUI = os_tsk_create_user(AppTaskGUI,              /* ������ */ 
	                                     3,                       /* �������ȼ� */ 
	                                     &AppTaskGUIStk,          /* ����ջ */
	                                     sizeof(AppTaskGUIStk));  /* ����ջ��С����λ�ֽ��� */
	
	HandleTaskUserIF = os_tsk_create_user(AppTaskUserIF,             /* ������ */ 
	                                      5,                         /* �������ȼ� */ 
	                                      &AppTaskUserIFStk,         /* ����ջ */
	                                      sizeof(AppTaskUserIFStk)); /* ����ջ��С����λ�ֽ��� */
	
	HandleTaskMsgPro = os_tsk_create_user(AppTaskMsgPro,             /* ������ */ 
	                                      6,                         /* �������ȼ� */ 
	                                      &AppTaskMsgProStk,         /* ����ջ */
	                                      sizeof(AppTaskMsgProStk)); /* ����ջ��С����λ�ֽ��� */
		
	HandleTaskModbus = os_tsk_create_user(AppTaskModbus,             /* ������ Modbus����*/ 
	                                      9,                         /* �������ȼ� */ 
	                                      &AppTaskModbusStk,         /* ����ջ */
	                                      sizeof(AppTaskModbusStk)); /* ����ջ��С����λ�ֽ��� */							   
		
	HandleMusicStart = os_tsk_create_user(AppTaskMusic,             /* ������ */ 
	                                      11,                       /* �������ȼ� */ 
	                                      &AppTaskMusicStk,         /* ����ջ */
	                                      sizeof(AppTaskMusicStk)); /* ����ջ��С����λ�ֽ��� */
	
	HandlePinDetectStart = os_tsk_create_user(AppTaskPinDetect,        /* ������ SD����μ������*/ 
									     12,                           /* �������ȼ� */ 
									     &AppTaskPinDetectStk,         /* ����ջ */
									     sizeof(AppTaskPinDetectStk)); /* ����ջ��С����λ�ֽ��� */
}

/*
*********************************************************************************************************
*	�� �� ��: AppObjCreate
*	����˵��: ��������ͨ�Ż���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void AppObjCreate (void)
{
	/* �����ź�������ֵ��0, ��������ͬ�� */
	os_sem_init (&semaphore, 0);
	
	 /* ������Ϣ���� */
	os_mbx_init (&mailbox, sizeof(mailbox));
	
	/* ���ڲ�SRAM����100KB����̬�ڴ�ʹ�� */	
	os_init_mem(AppMalloc, 1024*80);

	/* ���ڲ�CCM SRAM��64KBȫ������̬�ڴ�ʹ�ã�����JPEG��� */
	os_init_mem(AppMallocCCM, 1024*64);
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

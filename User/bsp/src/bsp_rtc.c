/*
*********************************************************************************************************
*
*	ģ������ : RTC
*	�ļ����� : bsp_rtc.c
*	��    �� : V1.0
*	˵    �� : RTC�ײ�����
*	�޸ļ�¼ :
*		�汾��   ����        ����       ˵��
*		V1.0    2016-01-20  Eric2013   ��ʽ����
*
*	Copyright (C), 2016-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"


#if 0
	#define printf_rtcdbg  printf
#else
	#define printf_rtcdbg(...)
#endif

/* ѡ��RTC��ʱ��Դ */
//#define RTC_CLOCK_SOURCE_LSE       /* LSE */
#define RTC_CLOCK_SOURCE_LSI     /* LSI */ 

RTC_TimeTypeDef  RTC_TimeStructure;
RTC_AlarmTypeDef RTC_AlarmStructure;
RTC_DateTypeDef  RTC_DateStructure;

/* 
*********************************************************************************************************
*  �� �� ��: bsp_InitRTC 
*  ����˵��: ��ʼ��RTC 
*  �� �Σ���  *  �� �� ֵ: ��   
*********************************************************************************************************
*/ 
void bsp_InitRTC(void) 
{ 
	NVIC_InitTypeDef NVIC_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;
	
	/* ���ڼ���Ƿ��Ѿ����ù�RTC��������ù��Ļ����������ý���ʱ 
	����RTC���ݼĴ���Ϊ0x32F2��������RTC���ݼĴ�������0x32F2   ��ô��ʾû�����ù�����Ҫ����RTC.   */ 
	if (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x32F2) 
	{ 
		RTC_Config(); 
		/* ��ӡ������Ϣ */ 
		printf_rtcdbg("��һ��ʹ��RTC \n\r"); 
		
		/* ����ϵ縴λ��־�Ƿ����� */ 
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET) 
		{ 
			/* �����ϵ縴λ */ 
			printf_rtcdbg("�����ϵ縴λ \n\r"); 
		} 
	} 
	else 
	{ 
		/* ��ӡ������Ϣ */ 
		printf_rtcdbg("��n��ʹ��RTC \n\r"); 
		
		/* ����ϵ縴λ��־�Ƿ����� */ 
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET) 
		{ 
			/* �����ϵ縴λ */ 
			printf_rtcdbg("�����ϵ縴λ \n\r"); 
		} 
		/* ������Ÿ�λ��־�Ƿ����� */ 
		else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET) 
		{ 
			/* �������Ÿ�λ */ 
			printf_rtcdbg("�������Ÿ�λ \n\r");     
		} 
		
		/* ʹ��PWRʱ�� */ 
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); 
		/* �������RTC */ 
		PWR_BackupAccessCmd(ENABLE); 
		/* �ȴ� RTC APB �Ĵ���ͬ�� */ 
		RTC_WaitForSynchro(); 
		/* ���RTC���ӱ�־ */ 
		RTC_ClearFlag(RTC_FLAG_ALRAF); 
		/* ���RTC�����жϹ����־ */ 
		EXTI_ClearITPendingBit(EXTI_Line17); 
	} 
	
	/* RTC�����ж����� */ 
	/* EXTI ���� */ 
	EXTI_ClearITPendingBit(EXTI_Line17); 
	EXTI_InitStructure.EXTI_Line = EXTI_Line17; 
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; 
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE; 
	EXTI_Init(&EXTI_InitStructure); 
	
	/* ʹ��RTC�����ж� */ 
	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 13; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure); 
}

/*
*********************************************************************************************************
*	�� �� ��: RTC_Config
*	����˵��: 1. ѡ��ͬ��RTCʱ��ԴLSI����LSE��
*             2. ����RTCʱ�ӡ�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RTC_Config(void)
{
	RTC_InitTypeDef  RTC_InitStructure;
	uint32_t uwAsynchPrediv = 0;
    uint32_t uwSynchPrediv = 0;
	
	/* ʹ��PWRʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	/* �������RTC */
	PWR_BackupAccessCmd(ENABLE);

	/* ѡ��LSI��Ϊʱ��Դ */
#if defined (RTC_CLOCK_SOURCE_LSI)  
	
	RCC_LSICmd(ENABLE); 
	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET){}

	/* ѡ��RTCʱ��Դ */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
	
	/* ck_spre(1Hz) = RTCCLK(LSE) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)*/
	uwSynchPrediv = 0xFF;
	uwAsynchPrediv = 0x7F;
	
	/* ѡ��LSE��ΪRTCʱ�� */
#elif defined (RTC_CLOCK_SOURCE_LSE)
	/* ʹ��LSE����  */
	RCC_LSEConfig(RCC_LSE_ON);

	/* �ȴ����� */  
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET){}

	/* ѡ��RTCʱ��Դ */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

	uwSynchPrediv = 0xFF;
	uwAsynchPrediv = 0x7F;

#else
	#error Please select the RTC Clock source inside the main.c file
#endif 

	/* ʹ��RTCʱ�� */
	RCC_RTCCLKCmd(ENABLE);

	/* �ȴ�RTC APB�Ĵ���ͬ�� */
	RTC_WaitForSynchro();

	/* ����RTC���ݼĴ����ͷ�Ƶ��  */
	RTC_InitStructure.RTC_AsynchPrediv = uwAsynchPrediv;
	RTC_InitStructure.RTC_SynchPrediv = uwSynchPrediv;
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
	RTC_Init(&RTC_InitStructure);

	/* ���������պ����� */
	RTC_DateStructure.RTC_Year = 0x14;
	RTC_DateStructure.RTC_Month = RTC_Month_November;
	RTC_DateStructure.RTC_Date = 0x11;
	RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Thursday;
	RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);

	/* ����ʱ���룬�Լ���ʾ��ʽ */
	RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
	RTC_TimeStructure.RTC_Hours   = 0x11;
	RTC_TimeStructure.RTC_Minutes = 0x59;
	RTC_TimeStructure.RTC_Seconds = 0x55; 
	RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);   

	/* ���ñ��ݼĴ�������ʾ�Ѿ����ù�RTC */
	RTC_WriteBackupRegister(RTC_BKP_DR0, 0x32F2);

	/* �������� BCD��ʽ��05Сʱ��20���ӣ�30�� */
	RTC_AlarmStructure.RTC_AlarmTime.RTC_H12     = RTC_H12_AM;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours   = 0x05;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = 0x20;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = 0x30;
	RTC_AlarmStructure.RTC_AlarmDateWeekDay = 0x31;
	RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
	RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;

	/* ���� RTC Alarm A �Ĵ��� */
	RTC_SetAlarm(RTC_Format_BCD, RTC_Alarm_A, &RTC_AlarmStructure);

	/* ʹ�� RTC Alarm A �ж� */
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);

	/* ʹ������ */
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);

	/* ���RTC���ӱ�־ */
	RTC_ClearFlag(RTC_FLAG_ALRAF);
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_CalcWeek
*	����˵��: �������ڼ������ڼ�
*	��    ��: _year _mon _day  ������  (����2�ֽ��������º������ֽ�������
*	�� �� ֵ: �ܼ� ��1-7�� 7��ʾ����
*********************************************************************************************************
*/
uint8_t RTC_CalcWeek(uint16_t _year, uint8_t _mon, uint8_t _day)
{
	/*
	���գ�Zeller����ʽ
		��ʷ�ϵ�ĳһ�������ڼ���δ����ĳһ�������ڼ�������������⣬�кܶ���㹫ʽ������ͨ�ü��㹫ʽ��
	һЩ�ֶμ��㹫ʽ�����������������ǲ��գ�Zeller����ʽ��
	    ��w=y+[y/4]+[c/4]-2c+[26(m+1)/10]+d-1

		��ʽ�еķ��ź������£�
	     w�����ڣ�
	     c����ĸ�2λ��������-1
	     y���꣨��λ������
	     m���£�m���ڵ���3��С�ڵ���14�����ڲ��չ�ʽ�У�ĳ���1��2��Ҫ������һ���13��14�������㣬
	  	    ����2003��1��1��Ҫ����2002���13��1�������㣩��
	     d���գ�
	     [ ]����ȡ������ֻҪ�������֡�

	    �������W����7�������Ǽ��������ڼ������������0����Ϊ�����ա�
        �������Ǹ�������������������Ҫ���⴦��
            �������ܰ�ϰ�ߵ������ĸ�����������ֻ�ܰ������е������Ķ������ࡣΪ�˷���
        ���㣬���ǿ��Ը�������һ��7����������ʹ����Ϊһ��������Ȼ����������

		��2049��10��1�գ�100������죩Ϊ�����ò��գ�Zeller����ʽ���м��㣬�������£�
		���գ�Zeller����ʽ��w=y+[y/4]+[c/4]-2c+[26(m+1)/10]+d-1
		=49+[49/4]+[20/4]-2��20+[26�� (10+1)/10]+1-1
		=49+[12.25]+5-40+[28.6]
		=49+12+5-40+28
		=54 (����7��5)
		��2049��10��1�գ�100������죩������5��
	*/
	uint8_t y, c, m, d;
	int16_t w;

	if (_mon >= 3)
	{
		m = _mon;
		y = _year % 100;
		c = _year / 100;
		d = _day;
	}
	else	/* ĳ���1��2��Ҫ������һ���13��14�������� */
	{
		m = _mon + 12;
		y = (_year - 1) % 100;
		c = (_year - 1) / 100;
		d = _day;
	}

	w = y + y / 4 +  c / 4 - 2 * c + ((uint16_t)26*(m+1))/10 + d - 1;
	if (w == 0)
	{
		w = 7;	/* ��ʾ���� */
	}
	else if (w < 0)	/* ���w�Ǹ����������������ʽ��ͬ */
	{
		w = 7 - (-w) % 7;
	}
	else
	{
		w = w % 7;
	}
	
	/* modified by eric2013 -- 2016-12-25 */
	if (w == 0)
	{
		w = 7;	/* ��ʾ���� */
	}
	
	return w;
}

/*
*********************************************************************************************************
*	�� �� ��: RTC_Alarm_IRQHandler
*	����˵��: �����жϡ�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RTC_Alarm_IRQHandler(void)
{
	if(RTC_GetITStatus(RTC_IT_ALRA) != RESET)
	{
		/* ����ʱ�䵽�󣬷���������5�� */
		BEEP_Start(5, 1, 5);	/* ����50ms��ͣ10ms�� 5�� */
		RTC_ClearITPendingBit(RTC_IT_ALRA);
		EXTI_ClearITPendingBit(EXTI_Line17);
	} 
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

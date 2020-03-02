/*
*********************************************************************************************************
*
*	ģ������ : FM������
*	�ļ����� : app_si4704_lib.c
*	��    �� : V1.0
*	˵    �� : Si4704�������������
*              1. Ϊ�˸���������̨����ظ������ӿڲ��϶������������������á�
*
*	�޸ļ�¼ :
*		�汾��   ����        ����       ˵��
*		V1.0    2017-01-13  Eric2013    �׷�
*
*	Copyright (C), 2015-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"




/*
*********************************************************************************************************
*	                                    ����       
*********************************************************************************************************
*/
RADIO_T g_tRadio;


/*
*********************************************************************************************************
*	�� �� ��: BSP_RadioInit
*	����˵��: Si4704��������ʼ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void BSP_RadioInit(void)
{
	uint8_t i;

	wm8978_OutMute(1);
	
	/* ������ִ��ʱ��28ms */
	i2c_CheckDevice(I2C_ADDR_SI4730_W); 
	SI4730_PowerUp_FM_Revice();

	/* �״�����������������Ҫ390ms��������Ҫ42ms���� */
	SI4704_SetFMIntput(0);    
	SI4730_SetFMFreq(8750); 
	
	g_tRadio.ucVolume = g_tWav.ucVolume;
	
	/* ����������������ͬ���� */
	/* ȱʡ���������ֵ63, �޸�Ϊ48 */
	SI4730_SetOutVolume(g_tRadio.ucVolume);	
	
	for(i = 0; i < 32; i++)
	{
		g_tRadio.usFMList[i] = 8750;
	}

	g_tRadio.usFreq = 8750;	
	g_tRadio.ucFMCount = 0;
	g_tRadio.ucIndexFM = 0;	
	
	/* ����WM8978оƬ������ΪAUX�ӿ�(������)�����Ϊ���� */
	wm8978_CfgAudioPath(AUX_ON, EAR_LEFT_ON | EAR_RIGHT_ON | SPK_ON);
	
	wm8978_OutMute(0);
}

/*
*********************************************************************************************************
*	�� �� ��: RadioPlay
*	����˵��: ����������
*	��    ��: �� 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RadioPlay(void)
{
	OS_RESULT xResult;
	uint8_t temp[7];
	
	BSP_RadioInit();
	
	while(1)
	{ 
		if(os_evt_wait_or(0xFFFF, 0xFFFF) == OS_R_EVT)
		{
			xResult = os_evt_get ();
			switch (xResult)
			{
				/* �������Զ�������̨ */
				case RadioTaskSearch_0:
					SI4730_SetFMFreq(8750);
					Si47XX_FMSeek(SEEKUP_HALT, g_tRadio.usFMList, (uint16_t *)&(g_tRadio.ucFMCount));
					g_tRadio.ucIndexFM = 0;
					g_tRadio.usFreq = g_tRadio.usFMList[g_tRadio.ucIndexFM];
					SI4730_SetFMFreq(g_tRadio.usFreq);
					break;
				
				/* �������źŻ�ȡ */
				case RadioTaskGetRSSI_1:
					SI4730_GetFMTuneStatus(temp);
					g_tRadio.ucRSSI = temp[3];
					g_tRadio.ucSNR = temp[4];
					break;

				/* ��������̨���� */
				case RadioTaskSetFM_2:
					SI4730_SetFMFreq(g_tRadio.usFreq); /* ��ʼ�����������Ҫ10��ms���� */
					break;

				/* �������ر� */
				case RadioTaskClose_3:
					wm8978_OutMute(1);
				    SI4730_PowerDown();
					wm8978_CfgAudioPath(DAC_ON,  EAR_LEFT_ON | EAR_RIGHT_ON | SPK_ON);
					wm8978_OutMute(0);
					return;	

				/* ����λ��δʹ�� */
				default: 
					break;
			}	
		}
	}
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

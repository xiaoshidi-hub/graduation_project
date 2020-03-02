/*
*********************************************************************************************************
*
*	模块名称 : FM收音机
*	文件名称 : app_si4704_lib.c
*	版    本 : V1.0
*	说    明 : Si4704收音机处理程序
*              1. 为了更好搜索电台，务必给耳机接口插上耳机当做收音机天线用。
*
*	修改记录 :
*		版本号   日期        作者       说明
*		V1.0    2017-01-13  Eric2013    首发
*
*	Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"




/*
*********************************************************************************************************
*	                                    变量       
*********************************************************************************************************
*/
RADIO_T g_tRadio;


/*
*********************************************************************************************************
*	函 数 名: BSP_RadioInit
*	功能说明: Si4704收音机初始化
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void BSP_RadioInit(void)
{
	uint8_t i;

	wm8978_OutMute(1);
	
	/* 这两个执行时间28ms */
	i2c_CheckDevice(I2C_ADDR_SI4730_W); 
	SI4730_PowerUp_FM_Revice();

	/* 首次运行这两个函数需要390ms，后面需要42ms左右 */
	SI4704_SetFMIntput(0);    
	SI4730_SetFMFreq(8750); 
	
	g_tRadio.ucVolume = g_tWav.ucVolume;
	
	/* 调节音量，左右相同音量 */
	/* 缺省音量是最大值63, 修改为48 */
	SI4730_SetOutVolume(g_tRadio.ucVolume);	
	
	for(i = 0; i < 32; i++)
	{
		g_tRadio.usFMList[i] = 8750;
	}

	g_tRadio.usFreq = 8750;	
	g_tRadio.ucFMCount = 0;
	g_tRadio.ucIndexFM = 0;	
	
	/* 配置WM8978芯片，输入为AUX接口(收音机)，输出为耳机 */
	wm8978_CfgAudioPath(AUX_ON, EAR_LEFT_ON | EAR_RIGHT_ON | SPK_ON);
	
	wm8978_OutMute(0);
}

/*
*********************************************************************************************************
*	函 数 名: RadioPlay
*	功能说明: 收音机播放
*	形    参: 无 
*	返 回 值: 无
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
				/* 收音机自动搜索电台 */
				case RadioTaskSearch_0:
					SI4730_SetFMFreq(8750);
					Si47XX_FMSeek(SEEKUP_HALT, g_tRadio.usFMList, (uint16_t *)&(g_tRadio.ucFMCount));
					g_tRadio.ucIndexFM = 0;
					g_tRadio.usFreq = g_tRadio.usFMList[g_tRadio.ucIndexFM];
					SI4730_SetFMFreq(g_tRadio.usFreq);
					break;
				
				/* 收音机信号获取 */
				case RadioTaskGetRSSI_1:
					SI4730_GetFMTuneStatus(temp);
					g_tRadio.ucRSSI = temp[3];
					g_tRadio.ucSNR = temp[4];
					break;

				/* 收音机电台设置 */
				case RadioTaskSetFM_2:
					SI4730_SetFMFreq(g_tRadio.usFreq); /* 初始化后，这里仅需要10几ms左右 */
					break;

				/* 收音机关闭 */
				case RadioTaskClose_3:
					wm8978_OutMute(1);
				    SI4730_PowerDown();
					wm8978_CfgAudioPath(DAC_ON,  EAR_LEFT_ON | EAR_RIGHT_ON | SPK_ON);
					wm8978_OutMute(0);
					return;	

				/* 其它位暂未使用 */
				default: 
					break;
			}	
		}
	}
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

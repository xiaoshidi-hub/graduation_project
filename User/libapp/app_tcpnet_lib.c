/*
*********************************************************************************************************
*
*	模块名称 : TCPnet网络协议栈测试
*	文件名称 : app_tcpnet_lib.c
*	版    本 : V1.0
*	说    明 : 主要实现动态IP获取，网线插拔处理，从SNTP服务器获取时间和网络协议栈主函数main_TcpNet的调用。
*              1. 移植了RL-TCPnet网络协议栈，网口对应DM9161，务必将其连接到能联网的路由或交换机上，否则很
	              多功能无法正常运行，网线可以随意插拔，支持自动重连。网络信息和SNTP客户端都是每秒更新一次。
			   2. DHCP已经使能，如果插上网线8秒内无法从路由器/交换机获得IP地址，将使用固定IP:192.168.1.200。
			   3. 国内免费稳定的SNTP服务器很少了，当前使用的这个时好时坏，如果大家测试不成功的话也是正常的。
*
*	修改记录 :
*		版本号   日期         作者        说明
*		V1.0    2016-06-03   Eric2013     首发
*
*	Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/	
#include "includes.h"
#include "MainTask.h"




/*
*********************************************************************************************************
*	                                  用于本文件的调试
*********************************************************************************************************
*/
#if 0
	#define printf_tcpdbg printf
#else
	#define printf_tcpdbg(...)
#endif


/*
**********************************************************************************************************
											外部调用
**********************************************************************************************************
*/
#define DHCP_TOUT   80                 /* DHCP动态IP获取的溢出时间设置为8秒 */
#define MY_IP localm[NETIF_ETH].IpAdr
extern LOCALM localm[];              
extern OS_TID HandleTaskGUI;


/*
**********************************************************************************************************
											变量
**********************************************************************************************************
*/
struct tm g_time;
uint32_t dhcp_tout;
uint8_t g_DHCPStatus = 0;
uint8_t g_SNTPStatus = 0;
uint8_t DHCP_Status[2][40] = {"IP: 192.168.X.X",               
                              "Waiting for DHCP"};


/*
*********************************************************************************************************
*	函 数 名: time_cback
*	功能说明: SNTP获取时间回到函数
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void time_cback (uint32_t time) 
{
	struct tm *t_tm;

	if (time == 0) 
	{
		g_SNTPStatus = 2;
		printf_tcpdbg ("错误, 服务器未响应或者网络状态比较差\r\n");
	}
	else 
	{
		time += 8*60*60;
		t_tm = localtime((unsigned int *)&time);
		t_tm->tm_year += 1900;
		t_tm->tm_mon += 1;
		
		g_time.tm_year = t_tm->tm_year;
		g_time.tm_mon = t_tm->tm_mon;
		g_time.tm_mday = t_tm->tm_mday;
		g_time.tm_hour = t_tm->tm_hour;
		g_time.tm_min = t_tm->tm_min;
		g_time.tm_sec = t_tm->tm_sec;
		
		printf_tcpdbg ("UNIX时间戳：%d 日期：%02d/%02d/%02d  时间：%02d:%02d:%02d\r\n", time, t_tm->tm_year, 
						t_tm->tm_mon, 
						t_tm->tm_mday,
						t_tm->tm_hour, 
						t_tm->tm_min, 
						t_tm->tm_sec);
	}
}								
/*
*********************************************************************************************************
*	函 数 名: get_time
*	功能说明: 从SNTP服务器获取当前时间
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/					  
static void get_time (void) 
{
	U8 ntp_server[4] = {182,16,3,162};  /* SNTP服务器地址，或者110,75,190,198 */
	
	
	if (sntp_get_time((U8 *)&ntp_server[0], &time_cback) == __TRUE) 
	{
		printf_tcpdbg ("SNTP请求已经发送成功\r\n");
		g_SNTPStatus = 0;
	}
	else 
	{
		g_SNTPStatus = 1;
		printf_tcpdbg ("失败, SNTP未就绪或者参数错误\r\n");
	}
}

/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: 标准c程序入口。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void dhcp_check(void) 
{
 
	/* 检测是否通过DHCP自动获得IP */
	if (mem_test (&MY_IP, 0, IP_ADRLEN) == __FALSE && !(dhcp_tout & 0x80000000)) 
	{
		/* 已经获得IP */
		dhcp_tout = 0;
		sprintf((char *)DHCP_Status[0],"%d.%d.%d.%d", MY_IP[0], MY_IP[1],
							   MY_IP[2], MY_IP[3]);
		sprintf((char *)DHCP_Status[1],"DHCP Success");
		printf_tcpdbg("%s\r\n", DHCP_Status[0]);
		printf_tcpdbg("%s\r\n", DHCP_Status[1]);
		g_DHCPStatus = 1;
		return;
	}
	
	/* 每100ms进行一次减减操作 */
	if (--dhcp_tout == 0) 
	{
		/* 设置的8秒自动获取IP时间已经到了，禁止DHCP，准备使用固定IP */
		dhcp_disable ();
		sprintf((char *)DHCP_Status[1],"DHCP Failed" );
		g_DHCPStatus = 2;
		/* 更新溢出时间，将固定IP的分配时间也设置为8秒 */
		dhcp_tout = 80 | 0x80000000;
		return;
	}
	
	/* 设置固定IP的8秒时间到 */
	if (dhcp_tout == 0x80000000) 
	{
		dhcp_tout = 0;
		sprintf((char *)DHCP_Status[0],"%d.%d.%d.%d", MY_IP[0], MY_IP[1],
							   MY_IP[2], MY_IP[3]);
		sprintf((char *)DHCP_Status[1],"DHCP Failed Use static IP");
		printf_tcpdbg("%s\r\n", DHCP_Status[0]);
		printf_tcpdbg("%s\r\n", DHCP_Status[1]);
		g_DHCPStatus = 3;
	}
}

/*
*********************************************************************************************************
*	函 数 名: TCPnetTest
*	功能说明: 主要实现动态IP获取，网线插拔处理，从SNTP服务器获取时间和网络协议栈主函数main_TcpNet的调用。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void TCPnetTest(void)
{
	uint32_t tstart, tend;
	uint32_t ulTimeStart = 0, ulTimeEnd = 0;
	uint8_t but_ex;
	
	dhcp_tout = DHCP_TOUT;
	tstart = os_time_get();
	ulTimeStart = os_time_get();
	but_ex = g_ucEthLinkStatus;
	
	/* 进入到这个任务，说明网线已经插上，更新GUI主界面网线标志 */
	WM_SendMessageNoPara(hWinStatus, MSG_NETPlugIn);
	while (1) 
	{
		/* 每100ms做一次DHCP检测 */
		if(dhcp_tout != 0)
		{
			tend = os_time_get() - tstart;
			printf_tcpdbg("%x %d\r\n", dhcp_tout, tend);
			if(tend >= 100)
			{
				tstart = os_time_get();
				dhcp_check();
			}
		}
		
		/* 
		   切换到新的网络环境时，重新插拔网线要用这种方式 
		   一般项目中可以将其省略，因为网络环境是固定的，上电时获取过一次网络的状态
		   并设置了PHY，无需重新获取状态再初始化了。
		*/
		if(g_ucEthLinkStatus ^ but_ex)
		{
			but_ex = g_ucEthLinkStatus;
			if(g_ucEthLinkStatus == 1)
			{
				os_tsk_prio_self(2);
				init_ethernet();
				dhcp_disable ();
				
				/* 老版本，DHCP初始化 */
				dhcp_init ();         
				dhcp_tout = DHCP_TOUT;
				tstart = os_time_get();
				os_tsk_prio_self(4);
				
				/* 更新网线插入标志 */
				WM_SendMessageNoPara(hWinStatus, MSG_NETPlugIn);
			}
			else
			{
				/* 更新网线拔出标志 */
				WM_SendMessageNoPara(hWinStatus, MSG_NETPlugOut);
			}
		}
			
		/* 每800ms从SNTP服务器获取一次时间 */
		ulTimeEnd = os_time_get() - ulTimeStart;
		if(ulTimeEnd > 800)
		{
			ulTimeStart = os_time_get();
			get_time();
		}

		os_evt_wait_and(0x0001, 0xFFFF);	
		while (main_TcpNet() == __TRUE);
	}
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

/*
*********************************************************************************************************
*
*	ģ������ : TCPnet����Э��ջ����
*	�ļ����� : app_tcpnet_lib.c
*	��    �� : V1.0
*	˵    �� : ��Ҫʵ�ֶ�̬IP��ȡ�����߲�δ�����SNTP��������ȡʱ�������Э��ջ������main_TcpNet�ĵ��á�
*              1. ��ֲ��RL-TCPnet����Э��ջ�����ڶ�ӦDM9161����ؽ������ӵ���������·�ɻ򽻻����ϣ������
	              �๦���޷��������У����߿��������Σ�֧���Զ�������������Ϣ��SNTP�ͻ��˶���ÿ�����һ�Ρ�
			   2. DHCP�Ѿ�ʹ�ܣ������������8�����޷���·����/���������IP��ַ����ʹ�ù̶�IP:192.168.1.200��
			   3. ��������ȶ���SNTP�����������ˣ���ǰʹ�õ����ʱ��ʱ���������Ҳ��Բ��ɹ��Ļ�Ҳ�������ġ�
*
*	�޸ļ�¼ :
*		�汾��   ����         ����        ˵��
*		V1.0    2016-06-03   Eric2013     �׷�
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
	#define printf_tcpdbg printf
#else
	#define printf_tcpdbg(...)
#endif


/*
**********************************************************************************************************
											�ⲿ����
**********************************************************************************************************
*/
#define DHCP_TOUT   80                 /* DHCP��̬IP��ȡ�����ʱ������Ϊ8�� */
#define MY_IP localm[NETIF_ETH].IpAdr
extern LOCALM localm[];              
extern OS_TID HandleTaskGUI;


/*
**********************************************************************************************************
											����
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
*	�� �� ��: time_cback
*	����˵��: SNTP��ȡʱ��ص�����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void time_cback (uint32_t time) 
{
	struct tm *t_tm;

	if (time == 0) 
	{
		g_SNTPStatus = 2;
		printf_tcpdbg ("����, ������δ��Ӧ��������״̬�Ƚϲ�\r\n");
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
		
		printf_tcpdbg ("UNIXʱ�����%d ���ڣ�%02d/%02d/%02d  ʱ�䣺%02d:%02d:%02d\r\n", time, t_tm->tm_year, 
						t_tm->tm_mon, 
						t_tm->tm_mday,
						t_tm->tm_hour, 
						t_tm->tm_min, 
						t_tm->tm_sec);
	}
}								
/*
*********************************************************************************************************
*	�� �� ��: get_time
*	����˵��: ��SNTP��������ȡ��ǰʱ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/					  
static void get_time (void) 
{
	U8 ntp_server[4] = {182,16,3,162};  /* SNTP��������ַ������110,75,190,198 */
	
	
	if (sntp_get_time((U8 *)&ntp_server[0], &time_cback) == __TRUE) 
	{
		printf_tcpdbg ("SNTP�����Ѿ����ͳɹ�\r\n");
		g_SNTPStatus = 0;
	}
	else 
	{
		g_SNTPStatus = 1;
		printf_tcpdbg ("ʧ��, SNTPδ�������߲�������\r\n");
	}
}

/*
*********************************************************************************************************
*	�� �� ��: main
*	����˵��: ��׼c������ڡ�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void dhcp_check(void) 
{
 
	/* ����Ƿ�ͨ��DHCP�Զ����IP */
	if (mem_test (&MY_IP, 0, IP_ADRLEN) == __FALSE && !(dhcp_tout & 0x80000000)) 
	{
		/* �Ѿ����IP */
		dhcp_tout = 0;
		sprintf((char *)DHCP_Status[0],"%d.%d.%d.%d", MY_IP[0], MY_IP[1],
							   MY_IP[2], MY_IP[3]);
		sprintf((char *)DHCP_Status[1],"DHCP Success");
		printf_tcpdbg("%s\r\n", DHCP_Status[0]);
		printf_tcpdbg("%s\r\n", DHCP_Status[1]);
		g_DHCPStatus = 1;
		return;
	}
	
	/* ÿ100ms����һ�μ������� */
	if (--dhcp_tout == 0) 
	{
		/* ���õ�8���Զ���ȡIPʱ���Ѿ����ˣ���ֹDHCP��׼��ʹ�ù̶�IP */
		dhcp_disable ();
		sprintf((char *)DHCP_Status[1],"DHCP Failed" );
		g_DHCPStatus = 2;
		/* �������ʱ�䣬���̶�IP�ķ���ʱ��Ҳ����Ϊ8�� */
		dhcp_tout = 80 | 0x80000000;
		return;
	}
	
	/* ���ù̶�IP��8��ʱ�䵽 */
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
*	�� �� ��: TCPnetTest
*	����˵��: ��Ҫʵ�ֶ�̬IP��ȡ�����߲�δ�����SNTP��������ȡʱ�������Э��ջ������main_TcpNet�ĵ��á�
*	��    ��: ��
*	�� �� ֵ: ��
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
	
	/* ���뵽�������˵�������Ѿ����ϣ�����GUI���������߱�־ */
	WM_SendMessageNoPara(hWinStatus, MSG_NETPlugIn);
	while (1) 
	{
		/* ÿ100ms��һ��DHCP��� */
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
		   �л����µ����绷��ʱ�����²������Ҫ�����ַ�ʽ 
		   һ����Ŀ�п��Խ���ʡ�ԣ���Ϊ���绷���ǹ̶��ģ��ϵ�ʱ��ȡ��һ�������״̬
		   ��������PHY���������»�ȡ״̬�ٳ�ʼ���ˡ�
		*/
		if(g_ucEthLinkStatus ^ but_ex)
		{
			but_ex = g_ucEthLinkStatus;
			if(g_ucEthLinkStatus == 1)
			{
				os_tsk_prio_self(2);
				init_ethernet();
				dhcp_disable ();
				
				/* �ϰ汾��DHCP��ʼ�� */
				dhcp_init ();         
				dhcp_tout = DHCP_TOUT;
				tstart = os_time_get();
				os_tsk_prio_self(4);
				
				/* �������߲����־ */
				WM_SendMessageNoPara(hWinStatus, MSG_NETPlugIn);
			}
			else
			{
				/* �������߰γ���־ */
				WM_SendMessageNoPara(hWinStatus, MSG_NETPlugOut);
			}
		}
			
		/* ÿ800ms��SNTP��������ȡһ��ʱ�� */
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

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

/*
*********************************************************************************************************
*
*	ģ������ : STM32F4XX��MAC����
*	�ļ����� : ETH_STM32F4XX.C
*	��    �� : V1.0
*	˵    �� : �ɹٷ��������޸Ķ�����
*              1. ԭʼ����֧��DP83848C ��ST802RT1������DM9161C��
*              2. ����PHY���жϴ������ܣ���Ҫ���ڼ�����ߵ�����״̬��
*              3. ����g_ucEthLinkStatus���ڱ�ʾ����״̬��
*              4. ������2017����TCPnet�̳�ʱ��Դ��ļ���ר�ŵĹ淶����
*                         
*	�޸ļ�¼ :
*		�汾��    ����         ����            ˵��
*       V1.0    2015-12-22    Eric2013         �׷�
*
*	Copyright (C), 2015-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/
/*-----------------------------------------------------------------------------
 *      RL-ARM - TCPnet
 *-----------------------------------------------------------------------------
 *      Name:    ETH_STM32F4XX.C
 *      Purpose: Driver for ST STM32F4xx Ethernet Controller
 *      Rev.:    V4.73
 *-----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2013 KEIL - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/
 
#include <Net_Config.h>
#include <stm32f4xx.h>             
#include "ETH_STM32F4xx.h"
#include "stdio.h"


/*
	STM32-V6������ʹ�õ���RMII�ӿڣ�PHYоƬ��DM9161:

	PA1/ETH_RMII_RX_CLK
	PA2/ETH_MDIO
	PA7/RMII_CRS_DV
	PC1/ETH_MDC
	PC4/ETH_RMII_RX_D0
	PC5/ETH_RMII_RX_D1
	PG11/ETH_RMII_TX_EN
	PG13/FSMC_A24/ETH_RMII_TXD0
	PB13/ETH_RMII_TXD1
	PH6/MII_INT ----- �ж�����,���ｫ���������߶Ͽ��������ӵ�״̬����
*/
#if 0
	#define printf_eth printf
#else
	#define printf_eth(...)
#endif


/* ��̫������״̬��0 ��ʾδ���ӣ�1 ��ʾ���� */
__IO uint8_t  g_ucEthLinkStatus = 0;  


/* The following macro definitions may be used to select the speed
   of the physical link:

  _10MBIT_   - connect at 10 MBit only
  _100MBIT_  - connect at 100 MBit only

  By default an autonegotiation of the link speed is used. This may take 
  longer to connect, but it works for 10MBit and 100MBit physical links.
  
    The following macro definitions may be used to select PYH interface:
  -MII_      - use MII interface instead of RMII
                                                                              */
/* #define _MII_ */

/* Net_Config.c */
extern U8 own_hw_adr[];

/* Local variables */
static U8 TxBufIndex;
static U8 RxBufIndex;

/* ENET local DMA Descriptors. */
static RX_Desc Rx_Desc[NUM_RX_BUF];
static TX_Desc Tx_Desc[NUM_TX_BUF];

/* ENET local DMA buffers. */
static U32 rx_buf[NUM_RX_BUF][ETH_BUF_SIZE>>2];
static U32 tx_buf[NUM_TX_BUF][ETH_BUF_SIZE>>2];

/*-----------------------------------------------------------------------------
 *      ENET Ethernet Driver Functions
 *-----------------------------------------------------------------------------
 *  Required functions for Ethernet driver module:
 *  a. Polling mode: - void init_ethernet ()
 *                   - void send_frame (OS_FRAME *frame)
 *                   - void poll_ethernet (void)
 *  b. Interrupt mode: - void init_ethernet ()
 *                     - void send_frame (OS_FRAME *frame)
 *                     - void int_enable_eth ()
 *                     - void int_disable_eth ()
 *                     - interrupt function 
 *----------------------------------------------------------------------------*/

/* Local Function Prototypes */
static void rx_descr_init (void);
static void tx_descr_init (void);
static void write_PHY (U32 PhyReg, U16 Value);
static U16  read_PHY (U32 PhyReg);
static void Eth_Link_EXTIConfig(void);

/*--------------------------- init_ethernet ----------------------------------*/

void init_ethernet (void) {
  /* Initialize the ETH ethernet controller. */
  U32 regv,tout,conn; 
	
  /* �ر�PHY�жϴ������� added by eric2013 */
  NVIC_DisableIRQ(EXTI9_5_IRQn);
	
  /* Enable System configuration controller clock */
  RCC->APB2ENR |= (1 << 14);
  
  /* Reset Ethernet MAC */
  RCC->AHB1RSTR |=  0x02000000;
#ifdef _MII_
  SYSCFG->PMC &= ~(1 << 23);
#else
  SYSCFG->PMC |=  (1 << 23);
#endif
  RCC->AHB1RSTR &= ~0x02000000;

#ifdef _MII_
  /* Enable Ethernet and GPIOA, GPIOB, GPIOC, GPIOG, GPIOH clocks */
  RCC->AHB1ENR |= 0x1E0000C7;
#else
  /* Enable Ethernet and GPIOA, GPIOB, GPIOC, GPIOG clocks */
  RCC->AHB1ENR |= 0x1E000047;
#endif
  
#ifdef __XYNERGY
  /* Configure Port A ethernet pins (PA.1, PA.2, PA.7) */
  GPIOA->MODER   &= ~0x0000C03C;
  GPIOA->MODER   |=  0x00008028;              /* Pins to alternate function */
  GPIOA->OTYPER  &= ~0x00000086;              /* Pins in push-pull mode     */
  GPIOA->OSPEEDR |=  0x0000C03C;              /* Slew rate as 100MHz pin    */
  GPIOA->PUPDR   &= ~0x0000C03C;              /* No pull up, no pull down   */

  GPIOA->AFR[0]  &= ~0xF0000FF0;
  GPIOA->AFR[0]  |=  0xB0000BB0;              /* Pins to AF11 (Ethernet)    */

  /* Configure Port B ethernet pins (PB.10, PB.11, PB.12, PB13) */
  GPIOB->MODER   &= ~0x0FF00000;
  GPIOB->MODER   |=  0x0AA00000;              /* Pins to alternate function */
  GPIOB->OTYPER  &= ~0x00003C00;              /* Pins in push-pull mode     */
  GPIOB->OSPEEDR |=  0x0FF00000;              /* Slew rate as 100MHz pin    */
  GPIOB->PUPDR   &= ~0x0FF00000;              /* No pull up, no pull down   */

  GPIOB->AFR[1]  &= ~0x00FFFF00;
  GPIOB->AFR[1]  |=  0x00BBBB00;              /* Pins to AF11 (Ethernet)    */

  /* Configure Port C ethernet pins (PC.1, PC.4, PC.5) */
  GPIOC->MODER   &= ~0x00000F0C;
  GPIOC->MODER   |=  0x00000A08;              /* Pins to alternate function */
  GPIOC->OTYPER  &= ~0x00000032;              /* Pins in push-pull mode     */
  GPIOC->OSPEEDR |=  0x00000F0C;              /* Slew rate as 100MHz pin    */
  GPIOC->PUPDR   &= ~0x00000F0C;              /* No pull up, no pull down   */

  GPIOC->AFR[0]  &= ~0x00FF00F0;
  GPIOC->AFR[0]  |=  0x00BB00B0;              /* Pins to AF11 (Ethernet)    */
#else
  /* 
     ԭʼ������������PA8�����ڸ�PHYоƬ�ṩʱ�ӣ�V5������������Դ������������
	 PA8�������������PA1��PA2��PA7����
  */
  GPIOA->MODER   &= ~0x0000C03C;
  GPIOA->MODER   |=  0x00008028;              /* Pins to alternate function */
  GPIOA->OTYPER  &= ~0x00000086;              /* Pins in push-pull mode     */
  GPIOA->OSPEEDR |=  0x0003C03C;              /* Slew rate as 100MHz pin    */
  GPIOA->PUPDR   &= ~0x0003C03C;              /* No pull up, no pull down   */

  GPIOA->AFR[0]  &= ~0xF0000FF0;
  GPIOA->AFR[0]  |=  0xB0000BB0;              /* Pins to AF11 (Ethernet)    */

#ifdef _MII_
  /* Configure Port B ethernet pin (PB8) */
  GPIOB->MODER   &= ~0x00030000;
  GPIOB->MODER   |=  0x00020000;              /* Pins to alternate function */
  GPIOB->OTYPER  &= ~0x00000100;              /* Pin in push-pull mode      */
  GPIOB->OSPEEDR |=  0x00030000;              /* Slew rate as 100MHz pin    */
  GPIOB->PUPDR   &= ~0x00030000;              /* No pull up, no pull down   */

  GPIOB->AFR[1]  &= ~0x0000000F;
  GPIOB->AFR[1]  |=  0x0000000B;              /* Pin to AF11 (Ethernet)     */

  /* Configure Port C ethernet pins (PC.1, PC.2, PC.3, PC.4, PC.5) */
  GPIOC->MODER   &= ~0x00000FFC;
  GPIOC->MODER   |=  0x00000AA8;              /* Pins to alternate function */
  GPIOC->OTYPER  &= ~0x0000003E;              /* Pins in push-pull mode     */
  GPIOC->OSPEEDR |=  0x00000FFC;              /* Slew rate as 100MHz pin    */
  GPIOC->PUPDR   &= ~0x00000FFC;              /* No pull up, no pull down   */

  GPIOC->AFR[0]  &= ~0x00FFFFF0;
  GPIOC->AFR[0]  |=  0x00BBBBB0;              /* Pins to AF11 (Ethernet)    */
#else
  /* Configure Port C ethernet pins (PC.1, PC.4, PC.5) */
  GPIOC->MODER   &= ~0x00000F0C;
  GPIOC->MODER   |=  0x00000A08;              /* Pins to alternate function */
  GPIOC->OTYPER  &= ~0x00000032;              /* Pins in push-pull mode     */
  GPIOC->OSPEEDR |=  0x00000F0C;              /* Slew rate as 100MHz pin    */
  GPIOC->PUPDR   &= ~0x00000F0C;              /* No pull up, no pull down   */

  GPIOC->AFR[0]  &= ~0x00FF00F0;
  GPIOC->AFR[0]  |=  0x00BB00B0;              /* Pins to AF11 (Ethernet)    */
#endif
  
  /* Configure Port G ethernet pins (PG.11, PG.13, PB.13) */
  GPIOG->MODER   &= ~0x0CC00000;
  GPIOG->MODER   |=  0x08800000;              /* Pin to alternate function  */
  GPIOG->OTYPER  &= ~0x00002800;              /* Pin in push-pull mode      */
  GPIOG->OSPEEDR |=  0x0CC00000;              /* Slew rate as 100MHz pin    */
  GPIOG->PUPDR   &= ~0x0CC00000;              /* No pull up, no pull down   */

  GPIOG->AFR[1]  &= ~0x00F0F000;
  GPIOG->AFR[1]  |=  0x00B0B000;              /* Pin to AF11 (Ethernet)     */
  
  //////////////////////////////
  GPIOB->MODER   &= ~0x0C000000;
  GPIOB->MODER   |=  0x08000000;              /* Pin to alternate function  */
  GPIOB->OTYPER  &= ~0x00002000;              /* Pin in push-pull mode      */
  GPIOB->OSPEEDR |=  0x0C000000;              /* Slew rate as 100MHz pin    */
  GPIOB->PUPDR   &= ~0x0C000000;              /* No pull up, no pull down   */

  GPIOB->AFR[1]  &= ~0x00F00000;
  GPIOB->AFR[1]  |=  0x00B00000;              /* Pin to AF11 (Ethernet)     */

#ifdef _MII_
  /* Configure Port H ethernet pins (PH.0, PH.2, PH.3, PH.6, PH.7) */
  GPIOH->MODER   &= ~0x0000F0F3;
  GPIOH->MODER   |=  0x0000A0A2;              /* Pins to alternate function */
  GPIOH->OTYPER  &= ~0x000000CD;              /* Pins in push-pull mode     */
  GPIOH->OSPEEDR |=  0x0000F0F3;              /* Slew rate as 100MHz pin    */
  GPIOH->PUPDR   &= ~0x0000F0F3;              /* No pull up, no pull down   */

  GPIOH->AFR[0]  &= ~0xFF00FF0F;
  GPIOH->AFR[0]  |=  0xBB00BB0B;              /* Pins to AF11 (Ethernet)    */  
#endif

#endif

  /* 
     ����λ�� 1 ʱ��MAC DMA�������Ḵλ����MAC��ϵͳ���ڲ��Ĵ������߼���
	 �������ں�ʱ������ɸ�λ�����󣬸�λ�Զ����㡣
  */
  ETH->DMABMR  |= DBMR_SR;
  while (ETH->DMABMR & DBMR_SR);
  conn = 0;

  /* HCLK Clock range 100-120MHz. */
  ETH->MACMIIAR = 0x00000004;

  /*
     ���Է���DM9161�����ϵ��Ͷ�ȡ��ID�Ĵ���������DM9162���У���Ҫ�ӳ�һ��ʱ��
     ����Ϊ�˷��������ֱ�ӽ��临λ�����͸�λָ���������ִ�С�
  */
   /* Check if this is a DM9161C PHY. --------------------------*/
	printf_eth("PHY_ID_DM9161C Init\r\n");
    /* Put the DM9161C in reset mode */
    write_PHY (PHY_REG_BMCR, 0x8000);

    /* Wait for hardware reset to end. */
    for (tout = 0; tout < 0x10000; tout++) {
      regv = read_PHY (PHY_REG_BMCR);
      if (!(regv & 0x8800)) {
        /* Reset complete, device not Power Down. */
		printf_eth("reset\r\n");
        break;
      }
    }
	
    /* Configure the PHY device */
#if defined (_10MBIT_)
    /* Connect at 10MBit */
    write_PHY (PHY_REG_BMCR, PHY_FULLD_10M);
#elif defined (_100MBIT_)
    /* Connect at 100MBit */
    write_PHY (PHY_REG_BMCR, PHY_FULLD_100M);
#else
    /* Use autonegotiation about the link speed. */
    write_PHY (PHY_REG_BMCR, PHY_AUTO_NEG);
    /* Wait to complete Auto_Negotiation. */
    for (tout = 0; tout < 0x100000; tout++) {
      regv = read_PHY (PHY_REG_BMSR);
      if (regv & 0x0020) {
		  printf_eth("Autonegotiation Complete\r\n");
        /* Autonegotiation Complete. */
        break;
      }
    }
#endif
    /* Check the link status. */
    for (tout = 0; tout < 0x10000; tout++) {
      regv = read_PHY (PHY_REG_BMSR);
      if (regv & (1 << 2)) {
		/* Link */ 
		g_ucEthLinkStatus = 1;
        /* Link is on, get connection info */
        regv = read_PHY (PHY_REG_DSCSR);
        if ((regv & (1 << 15))|(regv & (1 << 13))) {
		  printf_eth("Full-duplex connection\r\n");
          /* Full-duplex connection */
          conn |= PHY_CON_SET_FULLD;
        }
        if ((regv & (1 << 15))|(regv & (1 << 14))) {
		  printf_eth("100Mb/s mode\r\n");
          /* 100Mb/s mode */
          conn |= PHY_CON_SET_100M;
        }
        break;
      }
	  else{
		/* UnLink */ 
	  	g_ucEthLinkStatus = 0;
	  }
    }
	
	/* ʹ��DM9161C�������ж� Link Change */
	write_PHY (PHY_REG_INTERRUPT, 1<<12);
	
	/* ��������PH6�������ж��ź� */
	Eth_Link_EXTIConfig();
    
  /* 
	 Initialize MAC configuration register 
	 ����λ�� 1 ʱ��MAC ��ֹ�ڰ�˫��ģʽ�½���֡��
	 ����λ��λʱ��MAC ���շ���ʱ PHY �ṩ�����а���
	 ��� MAC ��ȫ˫��ģʽ�¹�������λ�����á�
  */
  ETH->MACCR  = MCR_ROD;

  /* 
	 Configure Full/Half Duplex mode. 
     ����λ�� 1 ʱ��MAC ��ȫ˫��ģʽ�¹�������ʱ������ͬʱ���ͺͽ��ա�
  */
  if (conn & PHY_CON_SET_FULLD) {
    /* Full duplex is enabled. */
    ETH->MACCR |= MCR_DM;
  }

  /* 
     Configure 100MBit/10MBit mode. 
     ָʾ������̫�� (MII) ģʽ�µ��ٶȣ�
	 0��10 Mbit/s
     1��100 Mbit/s
  */
  if (conn & PHY_CON_SET_100M) {
    /* 100MBit mode. */
    ETH->MACCR |= MCR_FES;
  }

  /* 
     MAC address filtering, accept multicast packets. 
     MACFFR ��̫��֡���˼Ĵ�����
     MACFCR ��̫�������ƼĴ�����ZQPD��ʱ��Ƭ��ͣ��ֹ
  */
  ETH->MACFFR = MFFR_HPF | MFFR_PAM;
  ETH->MACFCR = MFCR_ZQPD;

  /* Set the Ethernet MAC Address registers */
  ETH->MACA0HR = ((U32)own_hw_adr[5] <<  8) | (U32)own_hw_adr[4];
  ETH->MACA0LR = ((U32)own_hw_adr[3] << 24) | (U32)own_hw_adr[2] << 16 |
                 ((U32)own_hw_adr[1] <<  8) | (U32)own_hw_adr[0];

  /* Initialize Tx and Rx DMA Descriptors */
  rx_descr_init ();
  tx_descr_init ();

  /* Flush FIFO, start DMA Tx and Rx 
     DMAOMR ����ģʽ�Ĵ���
     DOMR_FTF��
       ��λ��1ʱ������FIFO�������߼��ḴλΪĬ��ֵ����ˣ�Tx FIFO�е��������ݾ��ᶪ
	   ʧ / ˢ�¡�ˢ�²�������ʱ��λ���ڲ����㡣��λ����֮ǰ���öԹ���ģʽ�Ĵ���ִ��д������
	 DOMR_ST��
       ��λ��1ʱ�����͹��̻��������״̬��DMA���鵱ǰλ�õķ����б���Ҵ����͵�֡��
	 DOMR_SR��
	   ��λ��1ʱ�����չ��̻��������״̬��DMA���Դӽ����б��л�ȡ��������������֡��
  */
  ETH->DMAOMR = DOMR_FTF | DOMR_ST | DOMR_SR;

  /* Enable receiver and transmiter */
  ETH->MACCR |= MCR_TE | MCR_RE;

  /* Reset all interrupts */
  ETH->DMASR  = 0xFFFFFFFF;

  /* 
     Enable Rx and Tx interrupts. 
     NISE�������жϻ���ʹ�� 
	 AISE���쳣�жϻ���ʹ��
	 RBUIE�����ջ������������ж�ʹ��
	 RIE�������ж�ʹ��
  */
  ETH->DMAIER = ETH_DMAIER_NISE | ETH_DMAIER_AISE | ETH_DMAIER_RBUIE | ETH_DMAIER_RIE;
  
  /* ������NVIC->ISER���õ�Ĭ�����ȼ�Ҳ��������ȼ�0 */
  NVIC_SetPriority(ETH_IRQn, 0);
}

/*--------------------------- int_enable_eth ---------------------------------*/

void int_enable_eth (void) {
  /* Ethernet Interrupt Enable function. */
  NVIC->ISER[1] = 1 << 29;
}


/*--------------------------- int_disable_eth --------------------------------*/

void int_disable_eth (void) {
  /* Ethernet Interrupt Disable function. */
  NVIC->ICER[1] = 1 << 29;
}


/*--------------------------- send_frame -------------------------------------*/
extern OS_TID HandleTaskTCPMain;
void send_frame (OS_FRAME *frame) {
  /* Send frame to ETH ethernet controller */
  U32 *sp,*dp;
  U32 i,j;

  j = TxBufIndex;
  /* Wait until previous packet transmitted. */
  while (Tx_Desc[j].CtrlStat & DMA_TX_OWN);

  sp = (U32 *)&frame->data[0];
  dp = (U32 *)(Tx_Desc[j].Addr & ~3);

  /* Copy frame data to ETH IO buffer. */
  for (i = (frame->length + 3) >> 2; i; i--) {
    *dp++ = *sp++;
  }
  Tx_Desc[j].Size      = frame->length;
  Tx_Desc[j].CtrlStat |= DMA_TX_OWN;
  if (++j == NUM_TX_BUF) j = 0;
  TxBufIndex = j;
  /* Start frame transmission. */
  ETH->DMASR   = DSR_TPSS;
  ETH->DMATPDR = 0;
  
  os_evt_set(0x0001, HandleTaskTCPMain);
}


/*--------------------------- interrupt_ethernet -----------------------------*/

void ETH_IRQHandler (void) {
  OS_FRAME *frame;
  U32 i, RxLen;
  U32 *sp,*dp;

  i = RxBufIndex;
  do {
    if (Rx_Desc[i].Stat & DMA_RX_ERROR_MASK) {
      goto rel;
    }
    if ((Rx_Desc[i].Stat & DMA_RX_SEG_MASK) != DMA_RX_SEG_MASK) {
      goto rel;
    }
    RxLen = ((Rx_Desc[i].Stat >> 16) & 0x3FFF) - 4;
    if (RxLen > ETH_MTU) {
      /* Packet too big, ignore it and free buffer. */
      goto rel;
    }
    /* Flag 0x80000000 to skip sys_error() call when out of memory. */
    frame = alloc_mem (RxLen | 0x80000000);
    /* if 'alloc_mem()' has failed, ignore this packet. */
    if (frame != NULL) {
      sp = (U32 *)(Rx_Desc[i].Addr & ~3);
      dp = (U32 *)&frame->data[0];
      for (RxLen = (RxLen + 3) >> 2; RxLen; RxLen--) {
        *dp++ = *sp++;
      }
      put_in_queue (frame);
    }
    /* Release this frame from ETH IO buffer. */
rel:Rx_Desc[i].Stat = DMA_RX_OWN;

    if (++i == NUM_RX_BUF) i = 0;
  }
  while (!(Rx_Desc[i].Stat & DMA_RX_OWN));
  RxBufIndex = i;

  if (ETH->DMASR & INT_RBUIE) {
    /* Receive buffer unavailable, resume DMA */
    ETH->DMASR = ETH_DMASR_RBUS;
    ETH->DMARPDR = 0;
  }
  /* Clear pending interrupt bits */
  ETH->DMASR = ETH_DMASR_NIS | ETH_DMASR_AIS | ETH_DMASR_RS;
  
  isr_evt_set(0x0001, HandleTaskTCPMain);
}


/*--------------------------- rx_descr_init ----------------------------------*/

static void rx_descr_init (void) {
  /* Initialize Receive DMA Descriptor array. */
  U32 i,next;

  RxBufIndex = 0;
  for (i = 0, next = 0; i < NUM_RX_BUF; i++) {
    if (++next == NUM_RX_BUF) next = 0;
    Rx_Desc[i].Stat = DMA_RX_OWN;
    Rx_Desc[i].Ctrl = DMA_RX_RCH | ETH_BUF_SIZE;
    Rx_Desc[i].Addr = (U32)&rx_buf[i];
    Rx_Desc[i].Next = (U32)&Rx_Desc[next];
  }
  /* �����������б��ַ�Ĵ�����ָ������������б����ʼ�� */
  ETH->DMARDLAR = (U32)&Rx_Desc[0];
}



/*--------------------------- tx_descr_init ----------------------------------*/

static void tx_descr_init (void) {
  /* Initialize Transmit DMA Descriptor array. */
  U32 i,next;

  TxBufIndex = 0;
  for (i = 0, next = 0; i < NUM_TX_BUF; i++) {
    if (++next == NUM_TX_BUF) next = 0;
    Tx_Desc[i].CtrlStat = DMA_TX_TCH | DMA_TX_LS | DMA_TX_FS;
    Tx_Desc[i].Addr     = (U32)&tx_buf[i];
    Tx_Desc[i].Next     = (U32)&Tx_Desc[next];
  }
  /* �����������б��ַ�Ĵ�����ָ�����������б����ʼ�� */
  ETH->DMATDLAR = (U32)&Tx_Desc[0];
}


/*--------------------------- write_PHY --------------------------------------*/

static void write_PHY (U32 PhyReg, U16 Value) {
  /* Write a data 'Value' to PHY register 'PhyReg'. */
  U32 tout;

  /* ��ĳ�ι���д����֮ǰҪд��PHY��16λ���� */
  ETH->MACMIIDR = Value;
  ETH->MACMIIAR = PHY_DEF_ADDR << 11 | PhyReg << 6 | MMAR_MW | MMAR_MB;

  /* Wait utill operation completed */
  tout = 0;
  for (tout = 0; tout < MII_WR_TOUT; tout++) {
    if ((ETH->MACMIIAR & MMAR_MB) == 0) {
      break;
    }
  }
}


/*--------------------------- read_PHY ---------------------------------------*/

static U16 read_PHY (U32 PhyReg) {
  /* Read a PHY register 'PhyReg'. */
  U32 tout;

  /*
	PHY_DEF_ADDR��PHY��ַ
	PhyReg ����Щλ����ѡPHY������ѡ����Ҫ��MII�Ĵ�����
	MMAR_MB��MIIæµ ��
	��ETH_MACMIIAR��ETH_MACMIIDRд��ǰ����λӦ��ȡ�߼�0����ETH_MACMIIARд������У���
	λҲ���븴λΪ0����PHY�Ĵ������ʹ����У���λ��Ӧ�ó�����Ϊ0b1��ָʾ����д��������
	�����С��ڶ�PHY����д���������У�ETH_MACMIIDR��MII���ݣ�Ӧʼ�ձ�����Ч��ֱ��MAC��
	��λ���㡣�ڶ�PHY���ж����������У�ETH_MACMIIDR ʼ����Ч��ֱ��MAC����λ���㡣�ڴ�
	λ����󣬲ſ�����ETH_MACMIIAR��MII ��ַ��д�롣
  */
  ETH->MACMIIAR = PHY_DEF_ADDR << 11 | PhyReg << 6 | MMAR_MB;

  /* Wait until operation completed */
  tout = 0;
  for (tout = 0; tout < MII_RD_TOUT; tout++) {
    if ((ETH->MACMIIAR & MMAR_MB) == 0) {
      break;
    }
  }
  /* ��ĳ�ι��������֮��� PHY �ж�ȡ�� 16 λ����ֵ */
  return (ETH->MACMIIDR & MMDR_MD);
}

/*--------------------------- added by eric2013 ---------------------------------------*/
/*--------------------------- Eth_Link_EXTIConfig ---------------------------------------*/
static void Eth_Link_EXTIConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* ������STM32-V6������ʹ��PH6��Ϊ�ж������, �½��ر�ʾ�ж��ź� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* �����ж����������� */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOH, &GPIO_InitStructure);

	/* �����ⲿ�ж������ӵ���Ӧ���� */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOH, EXTI_PinSource6);

	/* �����ⲿ�ж��� */
	EXTI_InitStructure.EXTI_Line = EXTI_Line6;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* ʹ���ж�ͨ�� */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*--------------------------- EXTI9_5_IRQHandler ---------------------------------------*/
//#define ETH_CONSTATUS
#define ETH_CONNECT    "ETH_LINK Connect\r\n"
#define ETH_DISCONNECT "ETH_LINK Disconnect\r\n"
void EXTI9_5_IRQHandler(void)
{
	U32 regv, tout;
	
	if (EXTI_GetITStatus(EXTI_Line6) != RESET)
	{
		/* ���Կ����ڴ˴������ӳ٣���ʱ����״̬���ˣ����ǼĴ���û�м�ʱ����*/
		regv = read_PHY(PHY_REG_INTERRUPT);
		if(regv & (1 << 2))
		{
			/* ���²����Ҫ������Σ���֤�Ĵ���BMSR������ */
			for(tout = 0; tout < 10; tout++) 
			{
				regv = read_PHY (PHY_REG_BMSR);
				if (regv & (1 << 2)) 
				{
					break;
				}
			}

			if(regv & (1 << 2)) 
			{
				#ifdef ETH_CONSTATUS
					const char *pError = ETH_CONNECT;
					uint8_t i;
				#endif
				
				g_ucEthLinkStatus = 1;
				
				#ifdef ETH_CONSTATUS
					for (i = 0; i < sizeof(ETH_CONNECT); i++)
					{
						USART1->DR = pError[i];
						/* �ȴ����ͽ��� */
						while ((USART1->SR & USART_FLAG_TC) == (uint16_t)RESET);
					}
				#endif 
			}
			else
			{
				#ifdef ETH_CONSTATUS
					const char *pError = ETH_DISCONNECT;
					uint8_t i;
				#endif
				
				g_ucEthLinkStatus = 0;
				
				#ifdef ETH_CONSTATUS
					for (i = 0; i < sizeof(ETH_DISCONNECT); i++)
					{
						USART1->DR = pError[i];
						/* �ȴ����ͽ��� */
						while ((USART1->SR & USART_FLAG_TC) == (uint16_t)RESET);
					}
				#endif
				
			}
			
		}
		/* ���жϹ���λ */
		EXTI_ClearITPendingBit(EXTI_Line6);
	}
}

/*-----------------------------------------------------------------------------
 * end of file
 *----------------------------------------------------------------------------*/


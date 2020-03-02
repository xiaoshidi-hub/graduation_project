/*
*********************************************************************************************************
*
*	模块名称 : 独立按键驱动模块
*	文件名称 : bsp_key.c
*	版    本 : V1.0
*	说    明 : 扫描独立按键，具有软件滤波机制，具有按键FIFO。可以检测如下事件：
*				(1) 按键按下
*				(2) 按键弹起
*				(3) 长按键
*				(4) 长按时自动连发
*
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2013-02-01 armfly  正式发布
*		V1.1    2013-06-29 armfly  增加1个读指针，用于bsp_Idle() 函数读取系统控制组合键（截屏）
*								   增加 K1 K2 组合键 和 K2 K3 组合键，用于系统控制
*
*	Copyright (C), 2013-2014, 安富莱电子 www.armfly.com

  1.bsp_key.c文件中核心函数是bsp_DetectKey()，这个函数先通过函数指针调用检测按键GPIO口状态的函数，判断是否有按键按下。
  2.检测到按键按下时先进行相应的消抖操作，消抖持续时间通过bsp_key.h中定义的宏 KEY_FILTER_TIME设置。确定按键按下后，
如果在此之前按键是松开的就把相应标志位置位，把按键按下的状态值写入到FIFO中。如果使能了检测按键长按功能（LongCount>0），
程序会继续检测按键是否达到长按时间，如果到达长按时间，将长按状态值写入到FIFO中，在继续检测是否支持长按时状态连续发送功
能，如果使能了长按时连续发送功能（RepeatSpeed>0），达到周期发送时间时，会继续写入FIFO按键按下状态值。
  3.检测到按键释放时，首先也会进行相应滤波处理。确认按键松开后，若之前按键处于按下状态，会将检查到的按键松开状况写入
到FIFO中，然后清除长按、重复发生的计数值，为下次查询做准备。
*
*********************************************************************************************************
*/

#include "bsp.h"

/*
	该程序适用于安富莱STM32-V6开发板

	如果用于其它硬件，请修改GPIO定义和 IsKeyDown1 - IsKeyDown8 函数

	如果用户的按键个数小于8个，你可以将多余的按键全部定义为和第1个按键一样，并不影响程序功能
	#define KEY_COUNT    8	  这个在 bsp_key.h 文件中定义
*/

/*
	安富莱STM32-V6 按键口线分配：
		K1 键      : PI8   (低电平表示按下)
		K2 键      : PC13  (低电平表示按下)
		K3 键      : PH4   (低电平表示按下)
		
		摇杆UP键   : PG2   (低电平表示按下)
		摇杆DOWN键 : PF10   (低电平表示按下)
		摇杆LEFT键 : PG3   (低电平表示按下)
		摇杆RIGHT键: PG7   (低电平表示按下)
		摇杆OK键   : PI11  (低电平表示按下)
*/

/* 按键口对应的RCC时钟 */
#define RCC_ALL_KEY 	(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOH | RCC_AHB1Periph_GPIOI | RCC_AHB1Periph_GPIOG)

#define GPIO_PORT_K1    GPIOI  //K1 键 
#define GPIO_PIN_K1	    GPIO_Pin_8

#define GPIO_PORT_K2    GPIOC //K2 键
#define GPIO_PIN_K2	    GPIO_Pin_13

#define GPIO_PORT_K3    GPIOH  //K2 键
#define GPIO_PIN_K3	    GPIO_Pin_4

#define GPIO_PORT_K4    GPIOG //摇杆UP键
#define GPIO_PIN_K4	    GPIO_Pin_2

#define GPIO_PORT_K5    GPIOF  //摇杆DOWN键
#define GPIO_PIN_K5	    GPIO_Pin_10

#define GPIO_PORT_K6    GPIOG  //摇杆LEFT键
#define GPIO_PIN_K6	    GPIO_Pin_3

#define GPIO_PORT_K7    GPIOG  //摇杆RIGHT键
#define GPIO_PIN_K7	    GPIO_Pin_7

#define GPIO_PORT_K8    GPIOI  //摇杆OK键
#define GPIO_PIN_K8	    GPIO_Pin_11

static KEY_T s_tBtn[KEY_COUNT];  //使用静态变量static 在这里KEY_T是全局变量 这能在这个文件中使用
static KEY_FIFO_T s_tKey;		/* 按键FIFO变量,结构体 */  //使用静态变量static 在这里KEY_T是全局变量 这能在这个文件中使用

//加上static 表示bsp_InitKeyVar()函数只能在本文件内部使用，其他地方不得引用此文件
static void bsp_InitKeyVar(void);  //初始化按键变量
static void bsp_InitKeyHard(void); //配置按键对应的GPIO
static void bsp_DetectKey(uint8_t i);//检测一个按键。非阻塞状态，必须被周期性的调用。

/*
*********************************************************************************************************
*	函 数 名: IsKeyDownX
*	功能说明: 判断按键是否按下
*	形    参: 无
*	返 回 值: 返回值1 表示按下，0表示未按下
*********************************************************************************************************
*/
//#ifdef 标识符
//   程序段1   (这里的程序段可以是数组，也可以是命令行)
//#else
//   程序段段2 (这里的程序段可以是数组，也可以是命令行)
//#endif
//作用是人如果所指定的标识符已经被 #define 定义过，则程序执行 程序段1 ，否则执行 程序段1


#ifdef STM32_X3		/* 安富莱 STM32-X3 开发板 */
	static uint8_t IsKeyDown1(void) {if ((GPIO_PORT_K1->IDR & GPIO_PIN_K1) == 0) return 1;else return 0;}
	static uint8_t IsKeyDown2(void) {if ((GPIO_PORT_K2->IDR & GPIO_PIN_K2) == 0) return 1;else return 0;}
	static uint8_t IsKeyDown3(void) {if ((GPIO_PORT_K3->IDR & GPIO_PIN_K3) == 0) return 1;else return 0;}
	static uint8_t IsKeyDown4(void) {if ((GPIO_PORT_K4->IDR & GPIO_PIN_K4) == 0) return 1;else return 0;}
	static uint8_t IsKeyDown5(void) {if ((GPIO_PORT_K5->IDR & GPIO_PIN_K5) == 0) return 1;else return 0;}
	static uint8_t IsKeyDown6(void) {if ((GPIO_PORT_K6->IDR & GPIO_PIN_K6) == 0) return 1;else return 0;}
	static uint8_t IsKeyDown7(void) {if ((GPIO_PORT_K7->IDR & GPIO_PIN_K7) == 0) return 1;else return 0;}
	static uint8_t IsKeyDown8(void) {if ((GPIO_PORT_K8->IDR & GPIO_PIN_K8) == 0) return 1;else return 0;}
#else				/* 安富莱 STM32-V5 开发板 */
	static uint8_t IsKeyDown1(void) {if ((GPIO_PORT_K1->IDR & GPIO_PIN_K1) == 0) return 1;else return 0;}
	static uint8_t IsKeyDown2(void) {if ((GPIO_PORT_K2->IDR & GPIO_PIN_K2) == 0) return 1;else return 0;}
	static uint8_t IsKeyDown3(void) {if ((GPIO_PORT_K3->IDR & GPIO_PIN_K3) == 0) return 1;else return 0;}
	static uint8_t IsKeyDown4(void) {if ((GPIO_PORT_K4->IDR & GPIO_PIN_K4) == 0) return 1;else return 0;}
	static uint8_t IsKeyDown5(void) {if ((GPIO_PORT_K5->IDR & GPIO_PIN_K5) == 0) return 1;else return 0;}
	static uint8_t IsKeyDown6(void) {if ((GPIO_PORT_K6->IDR & GPIO_PIN_K6) == 0) return 1;else return 0;}
	static uint8_t IsKeyDown7(void) {if ((GPIO_PORT_K7->IDR & GPIO_PIN_K7) == 0) return 1;else return 0;}
	static uint8_t IsKeyDown8(void) {if ((GPIO_PORT_K8->IDR & GPIO_PIN_K8) == 0) return 1;else return 0;}
#endif
	static uint8_t IsKeyDown9(void) {if (IsKeyDown1() && IsKeyDown2()) return 1;else return 0;}
	static uint8_t IsKeyDown10(void) {if (IsKeyDown1() && IsKeyDown2()) return 1;else return 0;}

/*
*********************************************************************************************************
*	函 数 名: bsp_InitKey
*	功能说明: 初始化按键. 该函数被 bsp_Init() 调用。
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitKey(void)
{
	bsp_InitKeyVar();		/* 初始化按键变量 */
	bsp_InitKeyHard();		/* 初始化按键硬件 配置按键对应的GPIO */
}

/*
*********************************************************************************************************
*	函 数 名: bsp_InitKeyVar
*	功能说明: 初始化按键变量
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
static void bsp_InitKeyVar(void)
{
	uint8_t i;

	/* 对按键FIFO读写指针清零 */
	s_tKey.Read = 0;
	s_tKey.Write = 0;
	s_tKey.Read2 = 0;

	/* 给每个按键结构体成员变量赋一组缺省值 */
	for (i = 0; i < KEY_COUNT; i++)
	{
		s_tBtn[i].LongTime = KEY_LONG_TIME;			/* 长按时间 0 表示不检测长按键事件 */
		s_tBtn[i].Count = KEY_FILTER_TIME / 2;		/* 计数器设置为滤波时间的一半 */
		s_tBtn[i].State = 0;							/* 按键缺省状态，0为未按下 */
		s_tBtn[i].RepeatSpeed = 0;						/* 按键连发的速度，0表示不支持连发 */
		s_tBtn[i].RepeatCount = 0;						/* 连发计数器 */
	}

	/* 如果需要单独更改某个按键的参数，可以在此单独重新赋值 */
	/* 比如，我们希望按键1按下超过1秒后，自动重发相同键值 */
	s_tBtn[KID_JOY_U].LongTime = 100;    //s_tBtn[3].LongTime = 100;
	s_tBtn[KID_JOY_U].RepeatSpeed = 5;	/* 每隔50ms自动发送键值 */

	s_tBtn[KID_JOY_D].LongTime = 100;    //s_tBtn[4].LongTime = 100;
	s_tBtn[KID_JOY_D].RepeatSpeed = 5;	/* 每隔50ms自动发送键值 */

	s_tBtn[KID_JOY_L].LongTime = 100;   //s_tBtn[5].LongTime = 100;
	s_tBtn[KID_JOY_L].RepeatSpeed = 5;	/* 每隔50ms自动发送键值 */

	s_tBtn[KID_JOY_R].LongTime = 100;   //s_tBtn[6].LongTime = 100;
	s_tBtn[KID_JOY_R].RepeatSpeed = 5;	/* 每隔50ms自动发送键值 */

	/* 判断按键按下的函数 */
	s_tBtn[0].IsKeyDownFunc = IsKeyDown1;//K1按下
	s_tBtn[1].IsKeyDownFunc = IsKeyDown2;//K2按下
	s_tBtn[2].IsKeyDownFunc = IsKeyDown3;//K3按下
	s_tBtn[3].IsKeyDownFunc = IsKeyDown4;//上按下
	s_tBtn[4].IsKeyDownFunc = IsKeyDown5;//下按下
	s_tBtn[5].IsKeyDownFunc = IsKeyDown6;//左按下
	s_tBtn[6].IsKeyDownFunc = IsKeyDown7;//右按下
	s_tBtn[7].IsKeyDownFunc = IsKeyDown8;//OK按下

	/* 组合键 */
	s_tBtn[8].IsKeyDownFunc = IsKeyDown9;
	s_tBtn[9].IsKeyDownFunc = IsKeyDown10;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_InitKeyHard
*	功能说明: 配置按键对应的GPIO
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
static void bsp_InitKeyHard(void)//配置8个按键的GPIO口，为推挽模式
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* 第1步：打开GPIO时钟 */
	RCC_AHB1PeriphClockCmd(RCC_ALL_KEY, ENABLE);

	/* 第2步：配置所有的按键GPIO为浮动输入模式(实际上CPU复位后就是输入状态) */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		/* 设为输入口 */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* 设为推挽模式 */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* 无需上下拉电阻 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	/* IO口最大速度 */

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_K1;
	GPIO_Init(GPIO_PORT_K1, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_K2;
	GPIO_Init(GPIO_PORT_K2, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_K3;
	GPIO_Init(GPIO_PORT_K3, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_K4;
	GPIO_Init(GPIO_PORT_K4, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_K5;
	GPIO_Init(GPIO_PORT_K5, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_K6;
	GPIO_Init(GPIO_PORT_K6, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_K7;
	GPIO_Init(GPIO_PORT_K7, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_K8;
	GPIO_Init(GPIO_PORT_K8, &GPIO_InitStructure);
}

/*
*********************************************************************************************************
*	函 数 名: bsp_PutKey
*	功能说明: 将1个键值压入按键FIFO缓冲区。可用于模拟一个按键。
*	形    参:  _KeyCode : 按键代码
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_PutKey(uint8_t _KeyCode)//按下按键存入到FIFO中
{
	s_tKey.Buf[s_tKey.Write] = _KeyCode;

	if (++s_tKey.Write  >= KEY_FIFO_SIZE)//KEY_FIFO_SIZE的值是10
	{
		s_tKey.Write = 0;
	}
}

/*
*********************************************************************************************************
*	函 数 名: bsp_GetKey
*	功能说明: 从按键FIFO缓冲区读取一个键值。调用此函数前，务必优先调用函数 bsp_InitKey 进行初始化。
*	形    参:  无
*	返 回 值: 按键代码
*********************************************************************************************************
*/
uint8_t bsp_GetKey(void)
{
	uint8_t ret;

	if (s_tKey.Read == s_tKey.Write)
	{
		return KEY_NONE;  //0 表示有按键事件发生
	}
	else
	{
		ret = s_tKey.Buf[s_tKey.Read];//可以判断是哪一个按键按下

		if (++s_tKey.Read >= KEY_FIFO_SIZE)//KEY_FIFO_SIZE表示有10种按键结果
		{
			s_tKey.Read = 0;
		}
		return ret;
	}
}

/*
*********************************************************************************************************
*	函 数 名: bsp_GetKey2
*	功能说明: 从按键FIFO缓冲区读取一个键值。独立的读指针。
*	形    参:  无
*	返 回 值: 按键代码
*********************************************************************************************************
*/
uint8_t bsp_GetKey2(void)
{
	uint8_t ret;

	if (s_tKey.Read2 == s_tKey.Write)//如果写入到FIFO中的按键和从FIFO中读出的按键值相同
	{
		return KEY_NONE;     //0 表示有按键事件发生
	}
	else
	{
		ret = s_tKey.Buf[s_tKey.Read2];//可以判断是哪一个按键按下

		if (++s_tKey.Read2 >= KEY_FIFO_SIZE)//KEY_FIFO_SIZE表示有10种按键结果
		{
			s_tKey.Read2 = 0;
		}
		return ret;
	}
}

/*
*********************************************************************************************************
*	函 数 名: bsp_GetKeyState
*	功能说明: 读取按键的状态
*	形    参:  _ucKeyID : 按键ID，从0开始
*	返 回 值: 1 表示按下， 0 表示未按下
*********************************************************************************************************
*/
uint8_t bsp_GetKeyState(KEY_ID_E _ucKeyID)
{
	return s_tBtn[_ucKeyID].State;//返回按键是被按下了还是松开了
}

/*
*********************************************************************************************************
*	函 数 名: bsp_SetKeyParam
*	功能说明: 设置按键参数
*	形    参：_ucKeyID : 按键ID，从0开始
*			  _LongTime : 长按事件时间
*			  _RepeatSpeed : 连发速度
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_SetKeyParam(uint8_t _ucKeyID, uint16_t _LongTime, uint8_t  _RepeatSpeed)
{
	s_tBtn[_ucKeyID].LongTime = _LongTime;			/* 长按时间 0 表示不检测长按键事件 */
	s_tBtn[_ucKeyID].RepeatSpeed = _RepeatSpeed;	/* 按键连发的速度，0表示不支持连发 */
	s_tBtn[_ucKeyID].RepeatCount = 0;				/* 连发计数器 */
}


/*
*********************************************************************************************************
*	函 数 名: bsp_ClearKey
*	功能说明: 清空按键FIFO缓冲区
*	形    参：无
*	返 回 值: 按键代码
*********************************************************************************************************
*/
void bsp_ClearKey(void)
{
	s_tKey.Read = s_tKey.Write;
}
/*
*********************************************************************************************************
*	函 数 名: bsp_DetectKey
*	功能说明: 检测/扫描一个按键。非阻塞状态，必须被周期性的调用。
*	形    参: 按键结构变量指针
*	返 回 值: 无
*********************************************************************************************************
*/
static void bsp_DetectKey(uint8_t i)
{
	KEY_T *pBtn;

	/*
		如果没有初始化按键函数，则报错
		if (s_tBtn[i].IsKeyDownFunc == 0)
		{
			printf("Fault : DetectButton(), s_tBtn[i].IsKeyDownFunc undefine");
		}
	*/
	pBtn = &s_tBtn[i];
	if (pBtn->IsKeyDownFunc())//如果某一个按键被按下
	{
		if (pBtn->Count < KEY_FILTER_TIME)//如果按键计数器滤波计数时间小于50ms
		{
			pBtn->Count = KEY_FILTER_TIME;//让按键计数器滤波计数时间等于50ms
		}
		else if(pBtn->Count < 2 * KEY_FILTER_TIME)//如果按键滤波时间小于100ms
		{
			pBtn->Count++;//让按键计数器滤波计数自增直到大于100
		}
		else  //这里当然是按键时间超过100ms
		{
			if (pBtn->State == 0)//判断按键是否松开
			{
				pBtn->State = 1;//如果按键松开，记录按键已经按下了。设置按键状态为1

				/* 发送按钮按下的消息 */
				bsp_PutKey((uint8_t)(3 * i + 1));//将这个按键压入FIFO缓冲区
			}

			if (pBtn->LongTime > 0) //如果按键是长按 因为0表示不检测长按
			{
				if (pBtn->LongCount < pBtn->LongTime)//在长按的情况下，如果长按计数器的值小于长按时间
				{
					/* 发送按钮持续按下的消息 */
					if (++pBtn->LongCount == pBtn->LongTime)//自增长安计数器直到等于长按时间
					{
						/* 键值放入按键FIFO */
						bsp_PutKey((uint8_t)(3 * i + 3));
					}
				}
				else    //如果按键不是长按是短按 因为0表示不检测长按
				{
					if (pBtn->RepeatSpeed > 0)
					{
						if (++pBtn->RepeatCount >= pBtn->RepeatSpeed)
						{
							pBtn->RepeatCount = 0;
							/* 常按键后，每隔10ms发送1个按键 */
							bsp_PutKey((uint8_t)(3 * i + 1));
						}
					}
				}
			}
		}
	}
	else  //如果按键被松开
	{
		if(pBtn->Count > KEY_FILTER_TIME)
		{
			pBtn->Count = KEY_FILTER_TIME;
		}
		else if(pBtn->Count != 0)
		{
			pBtn->Count--;
		}
		else
		{
			if (pBtn->State == 1)
			{
				pBtn->State = 0;

				/* 发送按钮弹起的消息 */
				bsp_PutKey((uint8_t)(3 * i + 2));
			}
		}

		pBtn->LongCount = 0;
		pBtn->RepeatCount = 0;
	}
}

/*
*********************************************************************************************************
*	函 数 名: bsp_KeyScan
*	功能说明: 扫描所有按键。非阻塞，被systick中断周期性的调用
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_KeyScan(void)
{
	uint8_t i;

	for (i = 0; i < KEY_COUNT; i++)//扫描10个按键
	{
		bsp_DetectKey(i);
	}
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

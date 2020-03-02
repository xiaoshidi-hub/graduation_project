/*
*********************************************************************************************************
*	                                  
*	模块名称 : GUI界面任务
*	文件名称 : MainTask.c
*	版    本 : V1.0
*	说    明 : 桌面窗口上面建立JPG背景图片，创建一个支持三个屏大小的motion窗口，用于实现三个屏界面的切换，
*              每个屏大小的位置创建一个ICONVIEW控件，创建了三个。
*              1. 共创建了10个应用图标。
*              2. 滑动的时候最好在图标以外的区域滑动，虽然可以点击图标的区域进行滑动，但是做的还不够好。
*              3. 状态栏看文件App_StatusDlg.c开头的说明，系统信息栏看文件App_SysInfoDlg.c开头的说明。
*
*	修改记录 :
*		版本号    日期         作者         说明
*       V1.0    2019-12      果果小师弟     首发
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
*	                                  用于本文件的调试
*********************************************************************************************************
*/
#if 0
	#define printf_main printf
#else
	#define printf_main(...)
#endif


/*
*********************************************************************************************************
*                                         宏定义 
*  ICONVIEW的桌面布局 800*480，每个图标太小是72*72
*  1. 图标的宽度位100像素。
*  2. ICONVIEW控件距左右两个边界的距离都是38个像素。
*  3. ICONVIEW控件距顶边界是10个像素。
*  4. 图标与图标之间的X距离是4个像素。
*  5. 图标与图标之间的Y距离是5个像素。
* 
*  |-----------------------------------800------------------------------|---
*  |    | 100|   | 100|   | 100|   | 100|   | 100|   | 100|   | 100|    | 10                                                          |
*  |-38-|-图-|-4-|-图-|-4-|-图-|-4-|-图-|-4-|-图-|-4-|-图-|-4-|-图-|-38-|-----》95 图标高度
*  |                                                                    | 5 
*  |-38-|-图-|-4-|-图-|-4-|-图-|-4-|-图-|-4-|-图-|-4-|-图-|-4-|-图-|-38-|-----》95 图标高度
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


#define ICONVIEW_TBorder   10   /* 控件ICONVIEW的上边距 */
#define ICONVIEW_LBorder   40   /* 控件ICONVIEW的左边距 */  
#define ICONVIEW_Width     724  /* 控件ICONVIEW的宽 */  
#define ICONVIEW_Height    100  /* 控件ICONVIEW的高 */  
#define ICONVIEW_YSpace    5    /* 控件ICONVIEW的Y方向间距 */
#define ICONVIEW_XSpace    4    /* 控件ICONVIEW的Y方向间距 */

#define ICON_Width         100      /* 控件ICONVIEW的图标的宽度 */  
#define ICON_Height        95       /* 控件ICONVIEW的图标的高度, 含图标下面的文本，一起的高度 */ 

#define SCREEN_Width    800     /* 显示屏的宽度 */  
#define SCREEN_Height   480     /* 显示屏高度   */ 
#define LineCap         16      /* 字符点阵的高是16 */

/*
*********************************************************************************************************
*                                         变量
*********************************************************************************************************
*/ 
static WM_HWIN hIcon1;
static GUI_MEMDEV_Handle   hMempic;
WM_HWIN hMotion;


/*
*********************************************************************************************************
*                               创建ICONVIEW控件所需变量
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


/* 用于第一屏桌面上ICONVIEW图标的创建 */
static const BITMAP_ITEM _aBitmapItem1[] = 
{
	{&bmComputer, "Computer",   "我的电脑"},
	{&bmSettings, "Settings",   "系统信息"},
	{&bmMusic,    "Music",      "音乐播放"},
	{&bmVideo,    "Vedio",      "视频播放"},
	{&bmRecorder, "Recorder",   "录音机"},
	{&bmRadio,    "FM/AM",      "收音机"},
	{&bmReader,   "Reader",     "文件阅读"},
	
	{&bmPicture,   "Picture",     "图片浏览"},
	{&bmADC,       "ADC",         "模数转换"},
	{&bmClock,     "Clock",          "时钟"},
	{&bmCalculator,"Calculator",  "计算器"}, 
};



/*
*********************************************************************************************************
*                                  应用程序入口函数
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
*	函 数 名: _cbBkWindow
*	功能说明: 桌面窗口的回调函数,这里主要是绘制背景窗口和界面切换时，切换标志的绘制 
*	形    参: pMsg  WM_MESSAGE类型指针变量   
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbBkWindow(WM_MESSAGE * pMsg) 
{

	switch (pMsg->MsgId) 
	{
		/* 重绘消息*/
		case WM_PAINT:
			GUI_MEMDEV_WriteAt(hMempic, 0, 0);//将内存设备的内容写入当前选定设备的指定位置，带alpha 通道
			break;
	
		default:
			WM_DefaultProc(pMsg);
			break;
	}
}

/*
*********************************************************************************************************
*	函 数 名: _cbMotion
*	功能说明: Motion窗口的回调函数，主要是桌面图标的滑动处理
*	形    参: pMsg  WM_MESSAGE类型指针变量   
*	返 回 值: 无
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
				/* 第一个界面上的图标 */
				case GUI_ID_ICONVIEW0:
					switch (NCode) 
					{
						/* ICON控件点击消息 */
						case WM_NOTIFICATION_CLICKED:
							tStart = GUI_GetTime();
							break;

						/* ICON控件释放消息 */
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
*	函 数 名: _CreateICONVIEW
*	功能说明: 创建ICONVIEW
*	形    参：hParent   父窗口
*             pBm       ICONVIEW上的位图
*             BitmapNum ICONVIEW上图标个数       
*             x         x轴坐标
*             y         y轴坐标
*             w         ICONVIEW宽
*             h         ICONVIEW高   
*	返 回 值: 无
*********************************************************************************************************
*/
static WM_HWIN _CreateICONVIEW(WM_HWIN hParent, const BITMAP_ITEM *pBm, int BitmapNum, int Id, int x, int y, int w, int h) 
{
	WM_HWIN hIcon;
	int i;
	
	/*在指定位置创建指定尺寸的ICONVIEW 小工具*/
	hIcon = ICONVIEW_CreateEx(x, 					/* 小工具的最左像素（在父坐标中）*/
						     y, 					/* 小工具的最上像素（在父坐标中）*/
							 w,                     /* 小工具的水平尺寸（单位：像素）*/
							 h, 	                /* 小工具的垂直尺寸（单位：像素）*/
	                         hParent, 				            /* 父窗口的句柄。如果为0 ，则新小工具将成为桌面（顶级窗口）的子窗口 */
							 WM_CF_SHOW | WM_CF_HASTRANS,       /* 窗口创建标记。为使小工具立即可见，通常使用 WM_CF_SHOW */ 
	                         0,                                 /* 默认是0，如果不够现实可设置增减垂直滚动条 */
							 Id, 			                    /* 小工具的窗口ID */
							 ICON_Width, 				        /* 图标的水平尺寸100 */
							 ICON_Height);						/* 95图标的垂直尺寸,图标和文件都包含在里面，不要大于ICONVIEW的高度，导致Text显示不完整*/
	
	
	/* 向ICONVIEW 小工具添加新图标 */
	for (i = 0; i < BitmapNum; i++) 
	{
		ICONVIEW_AddBitmapItem(hIcon, pBm[i].pBitmap, pBm[i].pTextCn);
	}	
	ICONVIEW_SetFont(hIcon, &GUI_SysFontHZ16);	
	/* 设置小工具的背景色 32 位颜色值的前8 位可用于alpha混合处理效果*/
	ICONVIEW_SetBkColor(hIcon, ICONVIEW_CI_SEL, GUI_WHITE | 0x80000000);	
	/* 设置X方向的边界值为0，默认不是0, Y方向默认是0，这里我们也进行一下设置，方便以后修改 */
	ICONVIEW_SetFrame(hIcon, GUI_COORD_X, 0);
	ICONVIEW_SetFrame(hIcon, GUI_COORD_Y, 0);	
	/* 设置图标在x 或y 方向上的间距。*/
	ICONVIEW_SetSpace(hIcon, GUI_COORD_X, ICONVIEW_XSpace);
	ICONVIEW_SetSpace(hIcon, GUI_COORD_Y, ICONVIEW_YSpace);	
	/* 设置对齐方式 在5.22版本中最新加入的 */
	ICONVIEW_SetIconAlign(hIcon, ICONVIEW_IA_HCENTER|ICONVIEW_IA_TOP);
	
	return hIcon;
}

/*
*********************************************************************************************************
*	函 数 名: MainTask
*	功能说明: GUI主函数
*	形    参: 无   	
*	返 回 值: 无
*********************************************************************************************************
*/
void MainTask(void) 
{ 
	
	PARA Para;
	PARA *pPara;
	
	/* 数据初始化 */
	pPara = &Para;
    pPara->Pos   = 0;
	
    GUI_Init();  /* 初始化 */
	
	/*
	 关于多缓冲和窗口内存设备的设置说明
	   1. 使能多缓冲是调用的如下函数，用户要在LCDConf_Lin_Template.c文件中配置了多缓冲，调用此函数才有效：
		  WM_MULTIBUF_Enable(1);
	   2. 窗口使能使用内存设备是调用函数：WM_SetCreateFlags(WM_CF_MEMDEV);
	   3. 如果emWin的配置多缓冲和窗口内存设备都支持，二选一即可，且务必优先选择使用多缓冲，实际使用
		  STM32F429BIT6 + 32位SDRAM + RGB565/RGB888平台测试，多缓冲可以有效的降低窗口移动或者滑动时的撕裂
		  感，并有效的提高流畅性，通过使能窗口使用内存设备是做不到的。
	   4. 所有emWin例子默认是开启三缓冲。
	*/
	WM_MULTIBUF_Enable(1);
	/*
       触摸校准函数默认是注释掉的，电阻屏需要校准，电容屏无需校准。如果用户需要校准电阻屏的话，执行
	   此函数即可，会将触摸校准参数保存到EEPROM里面，以后系统上电会自动从EEPROM里面加载。
	*/	
	WM_MOTION_Enable(0);    /* 不能滑动 如果要滑动写1*/ 
	/* 默认是500ms，这里将其修改为50ms，这个参数一定程度上决定的灵敏度，能决定灵敏度，主要还是
	   因为F429的性能有限。 
	*/
	WM_MOTION_SetDefaultPeriod(50);
	
	/* 第0步：系统加载界面 ------------------------------------------*/	
	GUI_SetBkColor(GUI_WHITE);
	GUI_Clear();                         /*修改完桌面背景颜色后必须清清屏，否则修改不成功	*/
	GUI_DrawBitmap(&bmLOGO,100,120);     /*显示毕业设计LOGO*/
	
	/* 第1步：绘制桌面窗口的背景图片 ------------------------------------------*//* 创建内存设备 */
	hMempic = GUI_MEMDEV_CreateFixed(0, 
	                                 0, 
	                                 LCD_GetXSize(), 
	                                 LCD_GetYSize(), 
									 GUI_MEMDEV_HASTRANS, 
									 GUI_MEMDEV_APILIST_16, 
									 GUICC_M565);
									 
	GUI_MEMDEV_Select(hMempic);/* 选择内存设备 */
    GUI_JPEG_Draw(_acbkpic, sizeof(_acbkpic), 0, 0);/* 绘制 JPEG 到内存设备中 */
	GUI_MEMDEV_Select(0);      /* 选择内存设备，0 表示选中 LCD */
	
	/* 第2步：使能桌面窗口的 ------------------------------------------*/
	WM_SetCallback(WM_HBKWIN, _cbBkWindow);

	/* 第3步：绘制桌面窗口的背景图片 ------------------------------------------*/
	hMotion = WM_CreateWindowAsChild(0, 
	                                 0, 
									 SCREEN_Width*3, 
									 SCREEN_Height, 
					                 WM_HBKWIN,  
									 WM_CF_MOTION_X | WM_CF_SHOW | WM_CF_HASTRANS, 
									 _cbMotion, 
									 sizeof(pPara));
									 
    WM_SetUserData(hMotion, &pPara, sizeof(pPara));
	

	/* 第4步：绘制桌面窗口的背景图片 ------------------------------------------*/
	/* 第1个界面图标 */
	hIcon1 = _CreateICONVIEW(hMotion, 
	                _aBitmapItem1, 
					GUI_COUNTOF(_aBitmapItem1), 
					GUI_ID_ICONVIEW0, 
					ICONVIEW_LBorder, //38左边距
					ICONVIEW_TBorder, //10上边距
					ICONVIEW_Width,   //控件的宽724
					ICONVIEW_Height*2);//控件的搞100*2
	
	/* 防止警告，留待以后备用 */
	(void)hIcon1;

	/* 第5步：绘制桌面窗口的背景图片 ------------------------------------------*/
	CreateSysInfoDlg();	
	/* 第6步：绘制桌面窗口的背景图片 ------------------------------------------*/
	CreateStatusDlg();
	/* 第6步：显示鼠标 -------------------------------------------------------*/
	GUI_CURSOR_Show();
	
	while(1) 
	{
		GUI_Delay(10);
	}
}


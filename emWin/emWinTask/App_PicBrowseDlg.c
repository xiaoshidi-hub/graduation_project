/*
*********************************************************************************************************
*	                                  
*	模块名称 : 图片浏览
*	文件名称 : App_PicBrowse.c
*	版    本 : V1.0
*	说    明 : BMP GIF图片浏览。
*              1. 默认已经限制可显示的BMP，GIF图片都是10张。
*              2. 图片的显示区默认显示为480*320，GIF图片不受限制，但不要太大，因为未做处理。
*              3. 删除了JPEG图片的显示，因为视频播放要做优化，优化后不支持函数GUI_JPEG_DrawScaled。
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



/*
*********************************************************************************************************
*	                                  用于本文件的调试
*********************************************************************************************************
*/
#if 0
	#define printf_picdbg printf
#else
	#define printf_picdbg(...)
#endif


/*
*********************************************************************************************************
*                                         宏定义 
*********************************************************************************************************
*/ 
#define g_ucPicNum       10      /* 支持的图片显示数量 */
#define PIC_Size         80      /* 将图片放缩到80*80像素以内放到最下面显示 */
#define DispArea_Width   480
#define DispArea_Height  320

#define BUTTONT_LeftSartX   0
#define BUTTONT_LeftEndX    48
#define BUTTONT_Size        48


/*
*********************************************************************************************************
*                                         变量 
*********************************************************************************************************
*/
/* BMP图片 */
static char BMP_Name[g_ucPicNum][30];  
static uint32_t BMP_Size[g_ucPicNum];
static int8_t BMP_Num=0;
static int8_t BMP_Index=0;

/* GIF图片 */
static char GIF_Name[g_ucPicNum][30];  
static uint32_t GIF_Size[g_ucPicNum];
static int8_t GIF_Num=0;
static int8_t GIF_Index=0;
static uint32_t GIF_xSize, GIF_ySize;
static GUI_GIF_INFO InfoGif1;
static GUI_GIF_IMAGE_INFO InfoGif2;

static uint8_t s_AppRet;
static char  *pic_buffer;
static uint8_t g_ucPicType = 0;
static uint8_t s_ucGIFFlag = 0;
static uint16_t s_ucStart = 0;
static GUI_MEMDEV_Handle  hMemDispPic;

static const char s_MusicPathDir[] = {"M0:\\pic\\"};  /* 存储器中图片存放的路径 */

extern 	WM_HWIN hMotion;


/*
*********************************************************************************************************
*	函 数 名: _LoadPic()
*	功能说明: 加载SD中的图片数据到外部SDRAM中
*	形    参: sFilename  文件名        	
*	返 回 值: 1：表示成功 0：表示失败
*********************************************************************************************************
*/
static uint8_t _LoadPic(const char * sFilename, int size, char *buf) 
{
	FILE *fout;
	
	printf_picdbg("%s\r\n", sFilename);
	
	/* 读取BMP图片数据到外部SDRAM中 */
	fout = fopen ((char *)sFilename, "r"); 	

	if(fout != NULL)
	{
		fread (buf, 1, size, fout);
		fclose(fout);		
	}

	return 1;
}

/*
*********************************************************************************************************
*	函 数 名: BMP_LoadDisplay
*	功能说明: 从外部存储器中加载BMP图片数据，并在TFT上显示出来
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void BMP_LoadDisplay(void) 
{
	uint16_t xSize, ySize;
	uint32_t Min, Max;
	
	/* 设置BMP图片名的索引从第一幅图开始 */
	BMP_Index = 0;
	
	/* 检索出所有的图片并显示出来 */
	while(BMP_Index < BMP_Num)
	{
		/* 加载图片数据 */
		_LoadPic(BMP_Name[BMP_Index], BMP_Size[BMP_Index], pic_buffer);
		
		/* 获取图片数据的长和宽 */
		xSize = GUI_BMP_GetXSize(pic_buffer);
		ySize = GUI_BMP_GetYSize(pic_buffer);
		
		/* 获得那个图片的长和宽的大小顺序*/
		if(xSize <= ySize)
		{
			Max = ySize;
			Min = xSize;
		}
		else
		{
			Max = xSize;
			Min = ySize;
		}
		
		/* 将图片的显示放缩到80*80以内 */
		GUI_BMP_DrawScaled(pic_buffer,
						   PIC_Size*BMP_Index,
						   LCD_GetYSize()-Min*PIC_Size/Max, 
		                   PIC_Size, 
		                   Max);
		
		BMP_Index++;
	}
	
	/* 重置BMP图片名的索引从第一幅图开始 */
	BMP_Index=0;
}

/*
*********************************************************************************************************
*	函 数 名: BMP_LoadShiftDisp
*	功能说明: 加载图片的切换显示
*	形    参: _ucShift  0:左移  1：右移
*	返 回 值: 无
*********************************************************************************************************
*/
static void BMP_LoadShiftDisp(uint8_t _ucShift) 
{
	uint8_t ScaleFlag;
	uint32_t xSize, ySize, MinScale;
	char buf[20];
	
	/* 左移图片和右移图片 */
	if(_ucShift == 0)
	{
		BMP_Index--;
		if(BMP_Index < 0) 
			BMP_Index = BMP_Num-1;
	}
	else
	{
		BMP_Index++;
		if(BMP_Index > BMP_Num-1) 
			BMP_Index = 0;
	}
	

	/* 加载BMP图片，并获得图片的长和宽 */
	_LoadPic(BMP_Name[BMP_Index], BMP_Size[BMP_Index], pic_buffer);
	xSize = GUI_BMP_GetXSize(pic_buffer);
	ySize = GUI_BMP_GetYSize(pic_buffer);

	/* 把变成最大的按照比例进行放缩 */
	if(xSize < ySize)//图片为长方形
	{
		ScaleFlag = 0;
	}
	else
	{
		ScaleFlag = 1;//图片为长柱形
	}

	/* 这里一定要进行一次清除操作，要不会有上次图片数据的残余*/
	GUI_MULTIBUF_Begin();

	/* 对下面四种情况分别做处理 */
	/* 第一种：xSize >= ySize的情况，且xSize <= DispArea_Width */
	if((ScaleFlag == 1) && (xSize <= DispArea_Width))
	{
		/* 根据ySize 和 DispArea_Height的大小，再分两种情况 */
		if(ySize <= DispArea_Height)
		{
			GUI_BMP_Draw(pic_buffer,
						 (LCD_GetXSize()-xSize)/2,
						 (LCD_GetYSize()-ySize-PIC_Size)/2);
		}
		else
		{
			GUI_BMP_DrawScaled(pic_buffer,
							  (LCD_GetXSize()-xSize*DispArea_Height/ySize)/2,
							  (LCD_GetYSize()-DispArea_Height-PIC_Size)/2, 
							  DispArea_Height, 
							  ySize);
		}
	}
	
	/* 第二种：xSize >= ySize的情况，且xSize > DispArea_Width */
	else if((ScaleFlag == 1) && (xSize > DispArea_Width))
	{
		MinScale = ySize * DispArea_Width / xSize;

		/* 这里根据MinScale 和 DispArea_Height的大小，分两种情况 */
		if(MinScale <= DispArea_Height)
		{
		GUI_BMP_DrawScaled(pic_buffer,
						   (LCD_GetXSize()-DispArea_Width)/2,
						   (LCD_GetYSize()-MinScale-PIC_Size)/2, 
							DispArea_Width, 
							xSize);
		}
		else
		{
		GUI_BMP_DrawScaled(pic_buffer,
						   (LCD_GetXSize()-xSize*DispArea_Height/ySize)/2,
						   (LCD_GetYSize()-DispArea_Height-PIC_Size)/2, 
							DispArea_Height, 
							ySize);
		}
	}
	
	/* 第三种：xSize < ySize的情况，且ySize <= DispArea_Height */
	else if((ScaleFlag == 0) && (ySize <= DispArea_Height))
	{
		GUI_BMP_Draw(pic_buffer,
					(LCD_GetXSize()-xSize)/2,
					(LCD_GetYSize()-ySize-PIC_Size)/2);
	}
	
	/* 第四种：xSize < ySize的情况，且ySize > DispArea_Height */
	else if((ScaleFlag == 0) && (ySize > DispArea_Height))
	{
		MinScale = xSize * DispArea_Height / ySize;

		/* 这里根据MinScale 和 DispArea_Width的大小，分两种情况 */
		if(MinScale <= DispArea_Width)
		{
			GUI_BMP_DrawScaled(pic_buffer,
						   (LCD_GetXSize()-MinScale)/2,
						   (LCD_GetYSize()-DispArea_Height-PIC_Size)/2, 
							DispArea_Height, 
							ySize);
		}
		else
		{
			GUI_BMP_DrawScaled(pic_buffer,
						  (LCD_GetXSize()-DispArea_Width)/2,
						  (LCD_GetYSize()-ySize*DispArea_Width/xSize-PIC_Size)/2, 
						  DispArea_Width, 
						  xSize);
		}
	}
	
	sprintf(buf,"<%dx%d>", xSize, ySize);
	GUI_SetFont(&GUI_FontLubalGraph24B);
	GUI_DispStringHCenterAt(buf, 400, 5);//在指定位置显示水平居中的字符串
	
	GUI_MULTIBUF_End();
}

/*
*********************************************************************************************************
*	函 数 名: GIF_LoadDisplay
*	功能说明: 从外部存储器中加载GIF图片数据，并在TFT上显示出来
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void GIF_LoadDisplay(void) 
{
	uint16_t xSize, ySize;
	uint32_t Min, Max;
	
	
	/* 设置JPG图片名的索引从第一幅图开始 */
	GIF_Index = 0;
	
	/* 检索出所有的图片并显示出来 */
	while(GIF_Index < GIF_Num)
	{
		/* 加载图片数据 */
		_LoadPic(GIF_Name[GIF_Index], GIF_Size[GIF_Index], pic_buffer);
		
		/* 获取图片数据的长和宽 */
		GUI_GIF_GetInfo(pic_buffer, GIF_Size[GIF_Index], &InfoGif1);

		xSize = InfoGif1.xSize;
		ySize = InfoGif1.ySize;
		
		/* 获得那个图片的长和宽的大小顺序*/
		if(xSize <= ySize)
		{
			Max = ySize;
			Min = xSize;
		}
		else
		{
			Max = xSize;
			Min = ySize;
		}
		

		/* 将图片的显示放缩到80*80以内 */
		GUI_GIF_DrawSubScaled(pic_buffer, 
							  GIF_Size[GIF_Index], 
							  PIC_Size*GIF_Index,
							  LCD_GetYSize()-Min*PIC_Size/Max, 
							  0,
							  PIC_Size, 
							  Max);


		GIF_Index++;
	}
	
	/* 重置GIF图片名的索引从第一幅图开始 */
	GIF_Index = 0;
}

/*
*********************************************************************************************************
*	函 数 名: GIF_LoadShiftDisp
*	功能说明: 加载图片的切换显示
*	形    参: _ucShift  0:左移  1：右移
*	返 回 值: 无
*********************************************************************************************************
*/
static void GIF_LoadShiftDisp(uint8_t _ucShift) 
{
	char buf[20];

	/* 左移图片和右移图片 */
	s_ucGIFFlag = 1;
	s_ucStart = 0;

	if(_ucShift == 0)
	{
		GIF_Index--;
		if(GIF_Index < 0) GIF_Index = GIF_Num-1;
	}
	else
	{
		GIF_Index++;
		if(GIF_Index > GIF_Num-1) GIF_Index = 0;
	}

	/* 加载图片，并获得图片的长和宽 */
	_LoadPic(GIF_Name[GIF_Index], GIF_Size[GIF_Index], pic_buffer);

	/* 获取图片数据的长和宽 */
	GUI_GIF_GetInfo(pic_buffer, GIF_Size[GIF_Index], &InfoGif1);
	GIF_xSize =  (LCD_GetXSize() - InfoGif1.xSize)/2;
	GIF_ySize =  (LCD_GetYSize() - InfoGif1.ySize - PIC_Size)/2;

	/* 刷新左移图标的背景 */
	sprintf(buf,"<%dx%d>", InfoGif1.xSize, InfoGif1.ySize);
	GUI_SetFont(&GUI_FontLubalGraph24B);
	GUI_DispStringHCenterAt(buf, 350, 5);
}

/*
*********************************************************************************************************
*	函 数 名: _cbBkWindow
*	功能说明: 回调函数 
*	形    参: pMsg  WM_MESSAGE类型指针变量   
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbBkWindow(WM_MESSAGE * pMsg) 
{
	GUI_PID_STATE TouchState;  /* 用于获取触摸的位置和触摸状态 */
	
	switch (pMsg->MsgId) 
	{
		case WM_TOUCH:
		   GUI_PID_GetState(&TouchState);
		
			/* 点击推出图标 */
		   if((TouchState.x > 750) && (TouchState.x < 800) && (TouchState.y > 40)&& (TouchState.y < 88))//点击取消在屏幕上的坐标
		   {
				
			    /* 等待松手，松手后再执行以下操作 */
				while(TouchState.Pressed == 1)
				{
					GUI_PID_GetState(&TouchState);
				}
				/* 刷新左移图标的背景 */
				GUI_SetBkColor(GUI_LIGHTCYAN);
				GUI_ClearRect(LCD_GetXSize()-BUTTONT_Size, LCD_GetYSize()/2-200, BUTTONT_Size, LCD_GetYSize()/2+156);
				/* 设置透明效果重新显示，表示按下的效果 */
				GUI_SetAlpha(80);
				GUI_DrawBitmap(&bmreturn, LCD_GetXSize()-BUTTONT_Size, LCD_GetYSize()/2-200);//返回去小	
				GUI_SetAlpha(0);
				
				/* 设置此变量，退出此界面 */
				s_AppRet = 0;
			}
		
		   	/* 点击类型切换图标 */
		   if((TouchState.x > 750) && (TouchState.x < 800) && (TouchState.y > 140) && (TouchState.y < 188))
		   {
				
			    /* 等待松手，松手后再执行以下操作 */
				while(TouchState.Pressed == 1)
				{
					GUI_PID_GetState(&TouchState);
				}
				
				/* 两种图片类型的切换 */
				if(g_ucPicType == 0)
				{
					g_ucPicType = 1;
					s_ucGIFFlag = 0;
					GUI_DrawBitmap(&bmgif,    LCD_GetXSize()-BUTTONT_Size, LCD_GetYSize()/2-100);//转换
					/* 清除地边上显示的小图标 */
					GUI_SetBkColor(GUI_LIGHTCYAN);
					GUI_ClearRect(0, LCD_GetYSize()-PIC_Size, LCD_GetXSize(), LCD_GetYSize());
					
					/* 清除上一次图片显示 */
					GUI_ClearRect(48, 0, LCD_GetXSize() - 48,LCD_GetYSize()-PIC_Size);
					/* 加载图片 */
					GIF_LoadDisplay();
				}
				else
				{
					g_ucPicType = 0;
					GUI_DrawBitmap(&bmbmp,    LCD_GetXSize()-BUTTONT_Size, LCD_GetYSize()/2-100);//转换
					/* 清除地边上显示的小图标 */
					GUI_SetBkColor(GUI_LIGHTCYAN);
					GUI_ClearRect(0, LCD_GetYSize()-PIC_Size, LCD_GetXSize(), LCD_GetYSize());
					
					/* 清除上一次图片显示 */
					GUI_ClearRect(48,  0, LCD_GetXSize()-48,LCD_GetYSize()-PIC_Size);

					/* 加载图片 */
					BMP_LoadDisplay();	
				}			
				
			}
			
			/* 点击图片左移 */
		   if((TouchState.x > 750) && (TouchState.x < 800) && (TouchState.y > 240) && (TouchState.y < 288))
		   {
				
			    /* 等待松手，松手后再执行以下操作 */
				while(TouchState.Pressed == 1)
				{
					GUI_PID_GetState(&TouchState);
				}
				
				/* 刷新左移图标的背景 */
				GUI_SetBkColor(GUI_LIGHTCYAN);
				GUI_ClearRect(LCD_GetXSize()-BUTTONT_Size, LCD_GetYSize()/2, BUTTONT_Size, LCD_GetYSize()/2+BUTTONT_Size+10);
				
				/* 设置透明效果重新显示，表示按下的效果 */
				GUI_SetAlpha(80);
				GUI_DrawBitmap(&bmleft,   LCD_GetXSize()-BUTTONT_Size, LCD_GetYSize()/2);//上一张
				GUI_SetAlpha(0);
				
				/* 清除上一次图片显示 */
				GUI_ClearRect(48,  0, LCD_GetXSize()-48,LCD_GetYSize()-PIC_Size);
			  
				/* 下面显示的是BMP格式的图片 */
				if(g_ucPicType == 0)
				{
					BMP_LoadShiftDisp(0);
				}
				else
				{
					GIF_LoadShiftDisp(0);
				}
				GUI_DrawBitmap(&bmleft,   LCD_GetXSize()-BUTTONT_Size, LCD_GetYSize()/2);//上一张
			}
			
			/* 点击图片右移 */
		   if((TouchState.x > 750) && (TouchState.x < 800) && (TouchState.y > 340) &&(TouchState.y <388))
		   {
				
			    /* 等待松手，松手后再执行以下操作 */
				while(TouchState.Pressed == 1)
				{
					GUI_PID_GetState(&TouchState);
				}
				
				/* 刷新左移图标的背景 */
				GUI_SetBkColor(GUI_LIGHTCYAN);
				GUI_ClearRect(LCD_GetXSize()-BUTTONT_Size, LCD_GetYSize()/2+100, LCD_GetXSize(), LCD_GetYSize()/2+BUTTONT_Size+10);
				
				/* 设置透明效果重新显示，表示按下的效果 */
				GUI_SetAlpha(80);
				GUI_DrawBitmap(&bmright,  LCD_GetXSize()-BUTTONT_Size, LCD_GetYSize()/2+100);//下一张
				GUI_SetAlpha(0);
				
				/* 清除上一次图片显示 */	
				GUI_ClearRect(48, 0, LCD_GetXSize()-48, LCD_GetYSize()-PIC_Size);
			  
				
				/* 下面显示的是BMP格式的图片 */
				if(g_ucPicType == 0)
				{
					BMP_LoadShiftDisp(1);
				}
				else
				{
					GIF_LoadShiftDisp(1);
				}
				GUI_DrawBitmap(&bmright,  LCD_GetXSize()-BUTTONT_Size, LCD_GetYSize()/2+100);//下一张
			}
		    break; 
			
		default:
			WM_DefaultProc(pMsg);
	}
}

/*
*********************************************************************************************************
*	函 数 名: App_PicBrowse
*	功能说明: 图片浏览主函数, 此函数处于嵌套调用，这种方式并不好，后期升级版将其完全独立成一个对话框。
*	形    参: hWin 所创建对话框父窗口 	
*	返 回 值: 无
*********************************************************************************************************
*/
void App_PicBrowse(WM_HWIN hWin) 
{ 	
	uint32_t t0, t1;
	FINFO info;
	WM_HWIN hWinPic, hWinOld;
	char searchbuf[50];	
	
	BMP_Num=0;
	BMP_Index=0;
	
	GIF_Num=0;
	GIF_Index=0;
	g_ucPicType = 0;
	
	
	/* 申请一块内存空间 并且将其清零  这里申请6MB的内存 */
	hMemDispPic = GUI_ALLOC_AllocZero(1024*1024*6);
	/* 将申请到内存的句柄转换成指针类型 */
	pic_buffer = GUI_ALLOC_h2p(hMemDispPic);
	
	hWinPic = WM_CreateWindowAsChild(0, 0, 799, 479, hWin, WM_CF_SHOW, _cbBkWindow, 0);
	/* 这个聚焦函数很有必要，要不会导致退出此界面后，主界面上的ICONVIEW控件还被聚焦着 */
	WM_SetFocus(hWinPic);
	
	/* 选择操作此窗口 */
	hWinOld = WM_SelectWindow(hWinPic);
	GUI_SetColor(GUI_RED);
	GUI_SetTextMode(GUI_TM_TRANS);
	GUI_SetFont(&GUI_FontLubalGraph24B);
	
	GUI_SetBkColor(GUI_LIGHTCYAN);
	GUI_Clear();   //修改完桌面背景颜色后必须清清屏，否则修改不成功
	
	/* 
	   将根目录下的所有文件列出来。
	   1. "*" 或者 "*.*" 搜索指定路径下的所有文件
	   2. "abc*"         搜索指定路径下以abc开头的所有文件
	   3. "*.jpg"        搜索指定路径下以.jpg结尾的所有文件
	   4. "abc*.jpg"     搜索指定路径下以abc开头和.jpg结尾的所有文件
	
	   以下是实现搜索根目录下所有文件
	*/
	info.fileID = 0;   /* 每次使用ffind函数前，info.fileID必须初始化为0 */
	
	/* 搜索MP3歌曲添加到listview控件上 */
	strcpy((char *)searchbuf, s_MusicPathDir);
	strcat((char *)searchbuf, "*");
	while(ffind ((const char *)searchbuf, &info) == 0)  
	{ 
		/* BMP */
		if(strstr((char *)info.name,".BMP")||strstr((char *)info.name,".bmp"))
		{
			if(BMP_Num < 10)
			{	
				BMP_Size[BMP_Num] = info.size;				
				strcpy(BMP_Name[BMP_Num], s_MusicPathDir);	
				strcat(BMP_Name[BMP_Num++], (char *)info.name);

			}
		}
		
		/* GIF */
		if(strstr((char *)info.name,".GIF")||strstr((char *)info.name, ".gif"))
		{
			if(GIF_Num < 10)
			{
				GIF_Size[GIF_Num] = info.size;	
				strcpy(GIF_Name[GIF_Num], s_MusicPathDir);	
				strcat(GIF_Name[GIF_Num++], (char *)info.name);
			}
		}
		
		printf_picdbg("%s\r\n", info.name);
    }	
	
	/* 初次显示，默认加载BMP图片 */
 	BMP_LoadDisplay();
	GUI_DrawBitmap(&bmreturn, LCD_GetXSize()-BUTTONT_Size, LCD_GetYSize()/2-200);//返回	
	GUI_DrawBitmap(&bmbmp,    LCD_GetXSize()-BUTTONT_Size, LCD_GetYSize()/2-100);//bmp与gif转换
    GUI_DrawBitmap(&bmleft,   LCD_GetXSize()-BUTTONT_Size, LCD_GetYSize()/2);    //上一张	
	GUI_DrawBitmap(&bmright,  LCD_GetXSize()-BUTTONT_Size, LCD_GetYSize()/2+100);//下一张	
	/* 设置为1，在窗口回调函数里面会设置为0 */
	s_AppRet = 1;
	while(s_AppRet)
	{					 				
		if(g_ucPicType == 1)
		{
    		if(s_ucGIFFlag == 1)
			{
				if(s_ucStart <= InfoGif1.NumImages)
				{                                    
					GUI_GIF_GetImageInfo(pic_buffer, GIF_Size[GIF_Index], &InfoGif2, s_ucStart);
					
					if(InfoGif2.Delay == 0)
					{
						GUI_Delay(100);
					}
					else
					{
						t0 = GUI_GetTime();
						
						/* 这里一定要进行一次清除操作，要不会有上次图片数据的残余*/
						GUI_MULTIBUF_Begin();
						GUI_GIF_DrawSub(pic_buffer, 
										GIF_Size[GIF_Index], 
										GIF_xSize, 
										GIF_ySize, 
										s_ucStart++); 
						GUI_MULTIBUF_End();
						t1 = GUI_GetTime() - t0;
						
						/* 如果GIF的解码和加载时间操作延迟时间就不做延迟 */
						if (t1 < InfoGif2.Delay * 10) 
						{
							GUI_Delay(InfoGif2.Delay * 10 - t1);
						}
						else
						{
							GUI_Delay(1); /* 加上个简短的延迟，防止触摸无法使用 */
						}
					}
				}
				else
				{
					s_ucStart = 0;
				}	
			}
			else
			{
				GUI_Delay(10);
			}
		}
		else
		{	
			GUI_Delay(10); 
		}
	}
	
	/* 重新选择操作窗口，删除窗口，是否申请的动态内存 */
	WM_SelectWindow(hWinOld);
	WM_DeleteWindow(hWinPic);
	GUI_ALLOC_Free(hMemDispPic); 
	
	/* 
	   这个函数非常有必要，保证了退出此界面后，首次滑动界面是可用，否则首次滑动不能使用，这点很奇怪
	   估计与嵌套调用有关，这种嵌套调用，后期有时间务必升级下。
	*/
	WM_MOTION_SetMovement(hMotion, GUI_COORD_X, 10, 0);
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

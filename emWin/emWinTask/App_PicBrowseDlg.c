/*
*********************************************************************************************************
*	                                  
*	ģ������ : ͼƬ���
*	�ļ����� : App_PicBrowse.c
*	��    �� : V1.0
*	˵    �� : BMP GIFͼƬ�����
*              1. Ĭ���Ѿ����ƿ���ʾ��BMP��GIFͼƬ����10�š�
*              2. ͼƬ����ʾ��Ĭ����ʾΪ480*320��GIFͼƬ�������ƣ�����Ҫ̫����Ϊδ������
*              3. ɾ����JPEGͼƬ����ʾ����Ϊ��Ƶ����Ҫ���Ż����Ż���֧�ֺ���GUI_JPEG_DrawScaled��
*
*	�޸ļ�¼ :
*		�汾��    ����         ����         ˵��
*       V1.0    2019-12      ����Сʦ��     �׷�
*
*	Copyright (C), 2020-2025, https://space.bilibili.com/357524621 
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
	#define printf_picdbg printf
#else
	#define printf_picdbg(...)
#endif


/*
*********************************************************************************************************
*                                         �궨�� 
*********************************************************************************************************
*/ 
#define g_ucPicNum       10      /* ֧�ֵ�ͼƬ��ʾ���� */
#define PIC_Size         80      /* ��ͼƬ������80*80�������ڷŵ���������ʾ */
#define DispArea_Width   480
#define DispArea_Height  320

#define BUTTONT_LeftSartX   0
#define BUTTONT_LeftEndX    48
#define BUTTONT_Size        48


/*
*********************************************************************************************************
*                                         ���� 
*********************************************************************************************************
*/
/* BMPͼƬ */
static char BMP_Name[g_ucPicNum][30];  
static uint32_t BMP_Size[g_ucPicNum];
static int8_t BMP_Num=0;
static int8_t BMP_Index=0;

/* GIFͼƬ */
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

static const char s_MusicPathDir[] = {"M0:\\pic\\"};  /* �洢����ͼƬ��ŵ�·�� */

extern 	WM_HWIN hMotion;


/*
*********************************************************************************************************
*	�� �� ��: _LoadPic()
*	����˵��: ����SD�е�ͼƬ���ݵ��ⲿSDRAM��
*	��    ��: sFilename  �ļ���        	
*	�� �� ֵ: 1����ʾ�ɹ� 0����ʾʧ��
*********************************************************************************************************
*/
static uint8_t _LoadPic(const char * sFilename, int size, char *buf) 
{
	FILE *fout;
	
	printf_picdbg("%s\r\n", sFilename);
	
	/* ��ȡBMPͼƬ���ݵ��ⲿSDRAM�� */
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
*	�� �� ��: BMP_LoadDisplay
*	����˵��: ���ⲿ�洢���м���BMPͼƬ���ݣ�����TFT����ʾ����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void BMP_LoadDisplay(void) 
{
	uint16_t xSize, ySize;
	uint32_t Min, Max;
	
	/* ����BMPͼƬ���������ӵ�һ��ͼ��ʼ */
	BMP_Index = 0;
	
	/* ���������е�ͼƬ����ʾ���� */
	while(BMP_Index < BMP_Num)
	{
		/* ����ͼƬ���� */
		_LoadPic(BMP_Name[BMP_Index], BMP_Size[BMP_Index], pic_buffer);
		
		/* ��ȡͼƬ���ݵĳ��Ϳ� */
		xSize = GUI_BMP_GetXSize(pic_buffer);
		ySize = GUI_BMP_GetYSize(pic_buffer);
		
		/* ����Ǹ�ͼƬ�ĳ��Ϳ�Ĵ�С˳��*/
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
		
		/* ��ͼƬ����ʾ������80*80���� */
		GUI_BMP_DrawScaled(pic_buffer,
						   PIC_Size*BMP_Index,
						   LCD_GetYSize()-Min*PIC_Size/Max, 
		                   PIC_Size, 
		                   Max);
		
		BMP_Index++;
	}
	
	/* ����BMPͼƬ���������ӵ�һ��ͼ��ʼ */
	BMP_Index=0;
}

/*
*********************************************************************************************************
*	�� �� ��: BMP_LoadShiftDisp
*	����˵��: ����ͼƬ���л���ʾ
*	��    ��: _ucShift  0:����  1������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void BMP_LoadShiftDisp(uint8_t _ucShift) 
{
	uint8_t ScaleFlag;
	uint32_t xSize, ySize, MinScale;
	char buf[20];
	
	/* ����ͼƬ������ͼƬ */
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
	

	/* ����BMPͼƬ�������ͼƬ�ĳ��Ϳ� */
	_LoadPic(BMP_Name[BMP_Index], BMP_Size[BMP_Index], pic_buffer);
	xSize = GUI_BMP_GetXSize(pic_buffer);
	ySize = GUI_BMP_GetYSize(pic_buffer);

	/* �ѱ�����İ��ձ������з��� */
	if(xSize < ySize)//ͼƬΪ������
	{
		ScaleFlag = 0;
	}
	else
	{
		ScaleFlag = 1;//ͼƬΪ������
	}

	/* ����һ��Ҫ����һ�����������Ҫ�������ϴ�ͼƬ���ݵĲ���*/
	GUI_MULTIBUF_Begin();

	/* ��������������ֱ������� */
	/* ��һ�֣�xSize >= ySize���������xSize <= DispArea_Width */
	if((ScaleFlag == 1) && (xSize <= DispArea_Width))
	{
		/* ����ySize �� DispArea_Height�Ĵ�С���ٷ�������� */
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
	
	/* �ڶ��֣�xSize >= ySize���������xSize > DispArea_Width */
	else if((ScaleFlag == 1) && (xSize > DispArea_Width))
	{
		MinScale = ySize * DispArea_Width / xSize;

		/* �������MinScale �� DispArea_Height�Ĵ�С����������� */
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
	
	/* �����֣�xSize < ySize���������ySize <= DispArea_Height */
	else if((ScaleFlag == 0) && (ySize <= DispArea_Height))
	{
		GUI_BMP_Draw(pic_buffer,
					(LCD_GetXSize()-xSize)/2,
					(LCD_GetYSize()-ySize-PIC_Size)/2);
	}
	
	/* �����֣�xSize < ySize���������ySize > DispArea_Height */
	else if((ScaleFlag == 0) && (ySize > DispArea_Height))
	{
		MinScale = xSize * DispArea_Height / ySize;

		/* �������MinScale �� DispArea_Width�Ĵ�С����������� */
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
	GUI_DispStringHCenterAt(buf, 400, 5);//��ָ��λ����ʾˮƽ���е��ַ���
	
	GUI_MULTIBUF_End();
}

/*
*********************************************************************************************************
*	�� �� ��: GIF_LoadDisplay
*	����˵��: ���ⲿ�洢���м���GIFͼƬ���ݣ�����TFT����ʾ����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void GIF_LoadDisplay(void) 
{
	uint16_t xSize, ySize;
	uint32_t Min, Max;
	
	
	/* ����JPGͼƬ���������ӵ�һ��ͼ��ʼ */
	GIF_Index = 0;
	
	/* ���������е�ͼƬ����ʾ���� */
	while(GIF_Index < GIF_Num)
	{
		/* ����ͼƬ���� */
		_LoadPic(GIF_Name[GIF_Index], GIF_Size[GIF_Index], pic_buffer);
		
		/* ��ȡͼƬ���ݵĳ��Ϳ� */
		GUI_GIF_GetInfo(pic_buffer, GIF_Size[GIF_Index], &InfoGif1);

		xSize = InfoGif1.xSize;
		ySize = InfoGif1.ySize;
		
		/* ����Ǹ�ͼƬ�ĳ��Ϳ�Ĵ�С˳��*/
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
		

		/* ��ͼƬ����ʾ������80*80���� */
		GUI_GIF_DrawSubScaled(pic_buffer, 
							  GIF_Size[GIF_Index], 
							  PIC_Size*GIF_Index,
							  LCD_GetYSize()-Min*PIC_Size/Max, 
							  0,
							  PIC_Size, 
							  Max);


		GIF_Index++;
	}
	
	/* ����GIFͼƬ���������ӵ�һ��ͼ��ʼ */
	GIF_Index = 0;
}

/*
*********************************************************************************************************
*	�� �� ��: GIF_LoadShiftDisp
*	����˵��: ����ͼƬ���л���ʾ
*	��    ��: _ucShift  0:����  1������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void GIF_LoadShiftDisp(uint8_t _ucShift) 
{
	char buf[20];

	/* ����ͼƬ������ͼƬ */
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

	/* ����ͼƬ�������ͼƬ�ĳ��Ϳ� */
	_LoadPic(GIF_Name[GIF_Index], GIF_Size[GIF_Index], pic_buffer);

	/* ��ȡͼƬ���ݵĳ��Ϳ� */
	GUI_GIF_GetInfo(pic_buffer, GIF_Size[GIF_Index], &InfoGif1);
	GIF_xSize =  (LCD_GetXSize() - InfoGif1.xSize)/2;
	GIF_ySize =  (LCD_GetYSize() - InfoGif1.ySize - PIC_Size)/2;

	/* ˢ������ͼ��ı��� */
	sprintf(buf,"<%dx%d>", InfoGif1.xSize, InfoGif1.ySize);
	GUI_SetFont(&GUI_FontLubalGraph24B);
	GUI_DispStringHCenterAt(buf, 350, 5);
}

/*
*********************************************************************************************************
*	�� �� ��: _cbBkWindow
*	����˵��: �ص����� 
*	��    ��: pMsg  WM_MESSAGE����ָ�����   
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _cbBkWindow(WM_MESSAGE * pMsg) 
{
	GUI_PID_STATE TouchState;  /* ���ڻ�ȡ������λ�úʹ���״̬ */
	
	switch (pMsg->MsgId) 
	{
		case WM_TOUCH:
		   GUI_PID_GetState(&TouchState);
		
			/* ����Ƴ�ͼ�� */
		   if((TouchState.x > 750) && (TouchState.x < 800) && (TouchState.y > 40)&& (TouchState.y < 88))//���ȡ������Ļ�ϵ�����
		   {
				
			    /* �ȴ����֣����ֺ���ִ�����²��� */
				while(TouchState.Pressed == 1)
				{
					GUI_PID_GetState(&TouchState);
				}
				/* ˢ������ͼ��ı��� */
				GUI_SetBkColor(GUI_LIGHTCYAN);
				GUI_ClearRect(LCD_GetXSize()-BUTTONT_Size, LCD_GetYSize()/2-200, BUTTONT_Size, LCD_GetYSize()/2+156);
				/* ����͸��Ч��������ʾ����ʾ���µ�Ч�� */
				GUI_SetAlpha(80);
				GUI_DrawBitmap(&bmreturn, LCD_GetXSize()-BUTTONT_Size, LCD_GetYSize()/2-200);//����ȥС	
				GUI_SetAlpha(0);
				
				/* ���ô˱������˳��˽��� */
				s_AppRet = 0;
			}
		
		   	/* ��������л�ͼ�� */
		   if((TouchState.x > 750) && (TouchState.x < 800) && (TouchState.y > 140) && (TouchState.y < 188))
		   {
				
			    /* �ȴ����֣����ֺ���ִ�����²��� */
				while(TouchState.Pressed == 1)
				{
					GUI_PID_GetState(&TouchState);
				}
				
				/* ����ͼƬ���͵��л� */
				if(g_ucPicType == 0)
				{
					g_ucPicType = 1;
					s_ucGIFFlag = 0;
					GUI_DrawBitmap(&bmgif,    LCD_GetXSize()-BUTTONT_Size, LCD_GetYSize()/2-100);//ת��
					/* ����ر�����ʾ��Сͼ�� */
					GUI_SetBkColor(GUI_LIGHTCYAN);
					GUI_ClearRect(0, LCD_GetYSize()-PIC_Size, LCD_GetXSize(), LCD_GetYSize());
					
					/* �����һ��ͼƬ��ʾ */
					GUI_ClearRect(48, 0, LCD_GetXSize() - 48,LCD_GetYSize()-PIC_Size);
					/* ����ͼƬ */
					GIF_LoadDisplay();
				}
				else
				{
					g_ucPicType = 0;
					GUI_DrawBitmap(&bmbmp,    LCD_GetXSize()-BUTTONT_Size, LCD_GetYSize()/2-100);//ת��
					/* ����ر�����ʾ��Сͼ�� */
					GUI_SetBkColor(GUI_LIGHTCYAN);
					GUI_ClearRect(0, LCD_GetYSize()-PIC_Size, LCD_GetXSize(), LCD_GetYSize());
					
					/* �����һ��ͼƬ��ʾ */
					GUI_ClearRect(48,  0, LCD_GetXSize()-48,LCD_GetYSize()-PIC_Size);

					/* ����ͼƬ */
					BMP_LoadDisplay();	
				}			
				
			}
			
			/* ���ͼƬ���� */
		   if((TouchState.x > 750) && (TouchState.x < 800) && (TouchState.y > 240) && (TouchState.y < 288))
		   {
				
			    /* �ȴ����֣����ֺ���ִ�����²��� */
				while(TouchState.Pressed == 1)
				{
					GUI_PID_GetState(&TouchState);
				}
				
				/* ˢ������ͼ��ı��� */
				GUI_SetBkColor(GUI_LIGHTCYAN);
				GUI_ClearRect(LCD_GetXSize()-BUTTONT_Size, LCD_GetYSize()/2, BUTTONT_Size, LCD_GetYSize()/2+BUTTONT_Size+10);
				
				/* ����͸��Ч��������ʾ����ʾ���µ�Ч�� */
				GUI_SetAlpha(80);
				GUI_DrawBitmap(&bmleft,   LCD_GetXSize()-BUTTONT_Size, LCD_GetYSize()/2);//��һ��
				GUI_SetAlpha(0);
				
				/* �����һ��ͼƬ��ʾ */
				GUI_ClearRect(48,  0, LCD_GetXSize()-48,LCD_GetYSize()-PIC_Size);
			  
				/* ������ʾ����BMP��ʽ��ͼƬ */
				if(g_ucPicType == 0)
				{
					BMP_LoadShiftDisp(0);
				}
				else
				{
					GIF_LoadShiftDisp(0);
				}
				GUI_DrawBitmap(&bmleft,   LCD_GetXSize()-BUTTONT_Size, LCD_GetYSize()/2);//��һ��
			}
			
			/* ���ͼƬ���� */
		   if((TouchState.x > 750) && (TouchState.x < 800) && (TouchState.y > 340) &&(TouchState.y <388))
		   {
				
			    /* �ȴ����֣����ֺ���ִ�����²��� */
				while(TouchState.Pressed == 1)
				{
					GUI_PID_GetState(&TouchState);
				}
				
				/* ˢ������ͼ��ı��� */
				GUI_SetBkColor(GUI_LIGHTCYAN);
				GUI_ClearRect(LCD_GetXSize()-BUTTONT_Size, LCD_GetYSize()/2+100, LCD_GetXSize(), LCD_GetYSize()/2+BUTTONT_Size+10);
				
				/* ����͸��Ч��������ʾ����ʾ���µ�Ч�� */
				GUI_SetAlpha(80);
				GUI_DrawBitmap(&bmright,  LCD_GetXSize()-BUTTONT_Size, LCD_GetYSize()/2+100);//��һ��
				GUI_SetAlpha(0);
				
				/* �����һ��ͼƬ��ʾ */	
				GUI_ClearRect(48, 0, LCD_GetXSize()-48, LCD_GetYSize()-PIC_Size);
			  
				
				/* ������ʾ����BMP��ʽ��ͼƬ */
				if(g_ucPicType == 0)
				{
					BMP_LoadShiftDisp(1);
				}
				else
				{
					GIF_LoadShiftDisp(1);
				}
				GUI_DrawBitmap(&bmright,  LCD_GetXSize()-BUTTONT_Size, LCD_GetYSize()/2+100);//��һ��
			}
		    break; 
			
		default:
			WM_DefaultProc(pMsg);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: App_PicBrowse
*	����˵��: ͼƬ���������, �˺�������Ƕ�׵��ã����ַ�ʽ�����ã����������潫����ȫ������һ���Ի���
*	��    ��: hWin �������Ի��򸸴��� 	
*	�� �� ֵ: ��
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
	
	
	/* ����һ���ڴ�ռ� ���ҽ�������  ��������6MB���ڴ� */
	hMemDispPic = GUI_ALLOC_AllocZero(1024*1024*6);
	/* �����뵽�ڴ�ľ��ת����ָ������ */
	pic_buffer = GUI_ALLOC_h2p(hMemDispPic);
	
	hWinPic = WM_CreateWindowAsChild(0, 0, 799, 479, hWin, WM_CF_SHOW, _cbBkWindow, 0);
	/* ����۽��������б�Ҫ��Ҫ���ᵼ���˳��˽�����������ϵ�ICONVIEW�ؼ������۽��� */
	WM_SetFocus(hWinPic);
	
	/* ѡ������˴��� */
	hWinOld = WM_SelectWindow(hWinPic);
	GUI_SetColor(GUI_RED);
	GUI_SetTextMode(GUI_TM_TRANS);
	GUI_SetFont(&GUI_FontLubalGraph24B);
	
	GUI_SetBkColor(GUI_LIGHTCYAN);
	GUI_Clear();   //�޸������汳����ɫ������������������޸Ĳ��ɹ�
	
	/* 
	   ����Ŀ¼�µ������ļ��г�����
	   1. "*" ���� "*.*" ����ָ��·���µ������ļ�
	   2. "abc*"         ����ָ��·������abc��ͷ�������ļ�
	   3. "*.jpg"        ����ָ��·������.jpg��β�������ļ�
	   4. "abc*.jpg"     ����ָ��·������abc��ͷ��.jpg��β�������ļ�
	
	   ������ʵ��������Ŀ¼�������ļ�
	*/
	info.fileID = 0;   /* ÿ��ʹ��ffind����ǰ��info.fileID�����ʼ��Ϊ0 */
	
	/* ����MP3������ӵ�listview�ؼ��� */
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
	
	/* ������ʾ��Ĭ�ϼ���BMPͼƬ */
 	BMP_LoadDisplay();
	GUI_DrawBitmap(&bmreturn, LCD_GetXSize()-BUTTONT_Size, LCD_GetYSize()/2-200);//����	
	GUI_DrawBitmap(&bmbmp,    LCD_GetXSize()-BUTTONT_Size, LCD_GetYSize()/2-100);//bmp��gifת��
    GUI_DrawBitmap(&bmleft,   LCD_GetXSize()-BUTTONT_Size, LCD_GetYSize()/2);    //��һ��	
	GUI_DrawBitmap(&bmright,  LCD_GetXSize()-BUTTONT_Size, LCD_GetYSize()/2+100);//��һ��	
	/* ����Ϊ1���ڴ��ڻص��������������Ϊ0 */
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
						
						/* ����һ��Ҫ����һ�����������Ҫ�������ϴ�ͼƬ���ݵĲ���*/
						GUI_MULTIBUF_Begin();
						GUI_GIF_DrawSub(pic_buffer, 
										GIF_Size[GIF_Index], 
										GIF_xSize, 
										GIF_ySize, 
										s_ucStart++); 
						GUI_MULTIBUF_End();
						t1 = GUI_GetTime() - t0;
						
						/* ���GIF�Ľ���ͼ���ʱ������ӳ�ʱ��Ͳ����ӳ� */
						if (t1 < InfoGif2.Delay * 10) 
						{
							GUI_Delay(InfoGif2.Delay * 10 - t1);
						}
						else
						{
							GUI_Delay(1); /* ���ϸ���̵��ӳ٣���ֹ�����޷�ʹ�� */
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
	
	/* ����ѡ��������ڣ�ɾ�����ڣ��Ƿ�����Ķ�̬�ڴ� */
	WM_SelectWindow(hWinOld);
	WM_DeleteWindow(hWinPic);
	GUI_ALLOC_Free(hMemDispPic); 
	
	/* 
	   ��������ǳ��б�Ҫ����֤���˳��˽�����״λ��������ǿ��ã������״λ�������ʹ�ã��������
	   ������Ƕ�׵����йأ�����Ƕ�׵��ã�������ʱ����������¡�
	*/
	WM_MOTION_SetMovement(hMotion, GUI_COORD_X, 10, 0);
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

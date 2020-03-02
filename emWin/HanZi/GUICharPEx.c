/*
*********************************************************************************************************
*	                                  
*	模块名称 : 汉字显示接口
*	文件名称 : GUICharPEx.c
*	版    本 : V1.0
*	说    明 : 这种汉字显示方式适合使用等宽字体，非等宽的微软雅黑等字体显示效果较差。
*              特别说明：本例子仅用到了16点阵的ASCII和GB2312编码汉字，其余点阵都未用到。
*              1. ASCII点阵数据存在于文件ascii16.c文件
*              2. 汉字点阵数据存在于文件song16.c文件
*              3. 为了方便客户使用这个实例，点阵数据都是直接以C文件形式存储到内部flash，
*                 STM32F429BIT6的内部FLASH有2MB，足够使用。
*
*	修改记录 :
*		版本号    日期           作者         说明
*		V1.0    2015-04-24     Eric2013       首版
*
*      Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"
#include "MainTask.h"
#include "fonts.h"


/* 点阵数据缓存, 必须大于等于单个字模需要的存储空间*/ 
#define BYTES_PER_FONT      512 
static U8 *pFont;

/*
*********************************************************************************************************
*	函 数 名: GUI_GetDataFromMemory
*	功能说明: 读取点阵数据
*	形    参: pProp  GUI_FONT_PROP类型结构
*             c      字符
*	返 回 值: 无
*********************************************************************************************************
*/
static void GUI_GetDataFromMemory(const GUI_FONT_PROP GUI_UNI_PTR *pProp, U16P c) 
{ 
	U16 BytesPerFont; 
    U32 oft, BaseAdd = 0; 
	U8 *ptr = NULL;
	
	char *font = (char *)pProp->paCharInfo->pData; 

	/* 每个字模的数据字节数 */
    BytesPerFont = GUI_pContext->pAFont->YSize * pProp->paCharInfo->BytesPerLine; 
    if (BytesPerFont > BYTES_PER_FONT)
	{
		BytesPerFont = BYTES_PER_FONT;
	}
	
	/* 英文字符地址偏移计算 */ 
    if (c < 0x80)                                                                
    { 
		if(strncmp("A12", font, 3) == 0)     /* 6*12 ASCII字符 */
		{
			BaseAdd = 0x1DBE00;
		}
		else if(strncmp("A16", font, 3) == 0) /* 8*16 ASCII字符 */
		{
			BaseAdd = 0;
			ptr = (U8 *)g_Ascii16;
		}
		else if(strncmp("A24", font, 3) == 0) /* 12*24 ASCII字符 */
		{
			BaseAdd = 0x1DFF00;
		}
		else if(strncmp("A32", font, 3) == 0) /* 24*48 ASCII字符 */
		{
			BaseAdd = 0x1E5A50;
		}
		
		oft = c * BytesPerFont + BaseAdd; 
    } 
    else                                                                           
    { 
		if(strncmp("H12", font, 3) == 0)      /* 12*12 字符 */
		{
			BaseAdd = 0x0;
		}
		else if(strncmp("H16", font, 3) == 0)  /* 16*16 字符 */
		{
			BaseAdd = 0;
			ptr = (U8 *)_acsong16;
		}
		else if(strncmp("H24", font, 3) == 0)  /* 24*24 字符 */
		{
			BaseAdd = 0x68190;
		}
		else if(strncmp("H32", font, 3) == 0)  /* 32*32 字符 */
		{
			BaseAdd = 0XEDF00;
		}
		
		oft = ((((c >> 8)-0xA1)) + ((c & 0xFF)-0xA1) * 94)* BytesPerFont;

    }
	
	pFont =  (U8 *)(ptr + oft);
} 

/*
*********************************************************************************************************
*	函 数 名: GUIUSER_DispChar
*	功能说明: 显示字符
*	形    参: c 显示的字符
*	返 回 值: 无
*********************************************************************************************************
*/
void GUIUSER_DispChar(U16P c)  
{ 
    int BytesPerLine; 
    GUI_DRAWMODE DrawMode = GUI_pContext->TextMode; 
    const GUI_FONT_PROP GUI_UNI_PTR *pProp = GUI_pContext->pAFont->p.pProp; 
	
    /* 确定显示的字符是否在有效范围内 */  
    for (; pProp; pProp = pProp->pNext)                                          
    { 
        if ((c >= pProp->First) && (c <= pProp->Last))break; 
    } 
	
	/* 字符有效，进行显示 */
    if (pProp) 
    { 
        GUI_DRAWMODE OldDrawMode;
        const GUI_CHARINFO GUI_UNI_PTR * pCharInfo = pProp->paCharInfo;
        GUI_GetDataFromMemory(pProp, c);
        BytesPerLine = pCharInfo->BytesPerLine;                
        OldDrawMode  = LCD_SetDrawMode(DrawMode);
        LCD_DrawBitmap(GUI_pContext->DispPosX, GUI_pContext->DispPosY,
                       pCharInfo->XSize, GUI_pContext->pAFont->YSize,
                       GUI_pContext->pAFont->XMag, GUI_pContext->pAFont->YMag,
                       1,    
                       BytesPerLine,
                       pFont,
                       &LCD_BKCOLORINDEX
                       );
		
        /* 填充背景 */
        if (GUI_pContext->pAFont->YDist > GUI_pContext->pAFont->YSize) 
        {
            int YMag = GUI_pContext->pAFont->YMag;
            int YDist = GUI_pContext->pAFont->YDist * YMag;
            int YSize = GUI_pContext->pAFont->YSize * YMag;
            if (DrawMode != LCD_DRAWMODE_TRANS) 
            {
                LCD_COLOR OldColor = GUI_GetColor();
                GUI_SetColor(GUI_GetBkColor());
                LCD_FillRect(GUI_pContext->DispPosX, GUI_pContext->DispPosY + YSize, 
                             GUI_pContext->DispPosX + pCharInfo->XSize, 
                             GUI_pContext->DispPosY + YDist);
                GUI_SetColor(OldColor);
            }
        }
		
        LCD_SetDrawMode(OldDrawMode);
//      if (!GUI_MoveRTL)
        GUI_pContext->DispPosX += pCharInfo->XDist * GUI_pContext->pAFont->XMag;
    }
} 

/*
*********************************************************************************************************
*	函 数 名: GUIUSER_X_GetCharDistX
*	功能说明: 获取字符的X轴间距
*	形    参: c  字符
*	返 回 值: 无
*********************************************************************************************************
*/
int GUIUSER_GetCharDistX(U16P c, int * pSizeX) 
{
    const GUI_FONT_PROP GUI_UNI_PTR * pProp = GUI_pContext->pAFont->p.pProp;  
    for (; pProp; pProp = pProp->pNext)                                         
    {
        if ((c >= pProp->First) && (c <= pProp->Last))break;
    }
    return (pProp) ? (pProp->paCharInfo)->XSize * GUI_pContext->pAFont->XMag : 0;
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

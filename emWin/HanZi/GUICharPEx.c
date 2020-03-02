/*
*********************************************************************************************************
*	                                  
*	ģ������ : ������ʾ�ӿ�
*	�ļ����� : GUICharPEx.c
*	��    �� : V1.0
*	˵    �� : ���ֺ�����ʾ��ʽ�ʺ�ʹ�õȿ����壬�ǵȿ��΢���źڵ�������ʾЧ���ϲ
*              �ر�˵���������ӽ��õ���16�����ASCII��GB2312���뺺�֣��������δ�õ���
*              1. ASCII�������ݴ������ļ�ascii16.c�ļ�
*              2. ���ֵ������ݴ������ļ�song16.c�ļ�
*              3. Ϊ�˷���ͻ�ʹ�����ʵ�����������ݶ���ֱ����C�ļ���ʽ�洢���ڲ�flash��
*                 STM32F429BIT6���ڲ�FLASH��2MB���㹻ʹ�á�
*
*	�޸ļ�¼ :
*		�汾��    ����           ����         ˵��
*		V1.0    2015-04-24     Eric2013       �װ�
*
*      Copyright (C), 2015-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"
#include "MainTask.h"
#include "fonts.h"


/* �������ݻ���, ������ڵ��ڵ�����ģ��Ҫ�Ĵ洢�ռ�*/ 
#define BYTES_PER_FONT      512 
static U8 *pFont;

/*
*********************************************************************************************************
*	�� �� ��: GUI_GetDataFromMemory
*	����˵��: ��ȡ��������
*	��    ��: pProp  GUI_FONT_PROP���ͽṹ
*             c      �ַ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void GUI_GetDataFromMemory(const GUI_FONT_PROP GUI_UNI_PTR *pProp, U16P c) 
{ 
	U16 BytesPerFont; 
    U32 oft, BaseAdd = 0; 
	U8 *ptr = NULL;
	
	char *font = (char *)pProp->paCharInfo->pData; 

	/* ÿ����ģ�������ֽ��� */
    BytesPerFont = GUI_pContext->pAFont->YSize * pProp->paCharInfo->BytesPerLine; 
    if (BytesPerFont > BYTES_PER_FONT)
	{
		BytesPerFont = BYTES_PER_FONT;
	}
	
	/* Ӣ���ַ���ַƫ�Ƽ��� */ 
    if (c < 0x80)                                                                
    { 
		if(strncmp("A12", font, 3) == 0)     /* 6*12 ASCII�ַ� */
		{
			BaseAdd = 0x1DBE00;
		}
		else if(strncmp("A16", font, 3) == 0) /* 8*16 ASCII�ַ� */
		{
			BaseAdd = 0;
			ptr = (U8 *)g_Ascii16;
		}
		else if(strncmp("A24", font, 3) == 0) /* 12*24 ASCII�ַ� */
		{
			BaseAdd = 0x1DFF00;
		}
		else if(strncmp("A32", font, 3) == 0) /* 24*48 ASCII�ַ� */
		{
			BaseAdd = 0x1E5A50;
		}
		
		oft = c * BytesPerFont + BaseAdd; 
    } 
    else                                                                           
    { 
		if(strncmp("H12", font, 3) == 0)      /* 12*12 �ַ� */
		{
			BaseAdd = 0x0;
		}
		else if(strncmp("H16", font, 3) == 0)  /* 16*16 �ַ� */
		{
			BaseAdd = 0;
			ptr = (U8 *)_acsong16;
		}
		else if(strncmp("H24", font, 3) == 0)  /* 24*24 �ַ� */
		{
			BaseAdd = 0x68190;
		}
		else if(strncmp("H32", font, 3) == 0)  /* 32*32 �ַ� */
		{
			BaseAdd = 0XEDF00;
		}
		
		oft = ((((c >> 8)-0xA1)) + ((c & 0xFF)-0xA1) * 94)* BytesPerFont;

    }
	
	pFont =  (U8 *)(ptr + oft);
} 

/*
*********************************************************************************************************
*	�� �� ��: GUIUSER_DispChar
*	����˵��: ��ʾ�ַ�
*	��    ��: c ��ʾ���ַ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void GUIUSER_DispChar(U16P c)  
{ 
    int BytesPerLine; 
    GUI_DRAWMODE DrawMode = GUI_pContext->TextMode; 
    const GUI_FONT_PROP GUI_UNI_PTR *pProp = GUI_pContext->pAFont->p.pProp; 
	
    /* ȷ����ʾ���ַ��Ƿ�����Ч��Χ�� */  
    for (; pProp; pProp = pProp->pNext)                                          
    { 
        if ((c >= pProp->First) && (c <= pProp->Last))break; 
    } 
	
	/* �ַ���Ч��������ʾ */
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
		
        /* ��䱳�� */
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
*	�� �� ��: GUIUSER_X_GetCharDistX
*	����˵��: ��ȡ�ַ���X����
*	��    ��: c  �ַ�
*	�� �� ֵ: ��
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

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

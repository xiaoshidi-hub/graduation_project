/*
*********************************************************************************************************
*	                                  
*	ģ������ : 12��������
*	�ļ����� : GUI_Font12.c
*	��    �� : V1.0
*	˵    �� : ���ֺ�����ʾ��ʽ�ʺ�ʹ�õȿ����壬�ǵȿ��΢���źڵ�������ʾЧ���ϲ
*              
*	�޸ļ�¼ :
*		�汾��   ����         ����          ˵��
*		V1.0    2016-07-16   Eric2013  	    �װ�    
*                                     
*	Copyright (C), 2015-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "GUI.h"
#include "GUI_Type.h"



GUI_CONST_STORAGE GUI_CHARINFO GUI_FontHZ12_CharInfo[2] =  
{     
    {  6,    6,  1, (void *)"A12"},   /* �ַ� */     
    {  12,  12,  2, (void *)"H12"},   /* ���� */        
};

GUI_CONST_STORAGE GUI_FONT_PROP GUI_FontHZ12_PropHZ= { 
      0xA1A1,  
      0xFEFE,  
      &GUI_FontHZ12_CharInfo[1], 
      (void *)0,  
}; 

GUI_CONST_STORAGE  GUI_FONT_PROP GUI_FontHZ12_PropASC= { 
      0x0020,  
      0x007F,  
      &GUI_FontHZ12_CharInfo[0], 
      (void GUI_CONST_STORAGE *)&GUI_FontHZ12_PropHZ,  
};

GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ12 =  
{ 
	  GUI_FONTTYPE_USER,
      12,  
      12,  
      1,   
      1,   
      (void GUI_CONST_STORAGE *)&GUI_FontHZ12_PropASC 
};

GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ12x2 =  
{ 
	  GUI_FONTTYPE_USER,
      12,  
      12,  
      2,   
      2,   
      (void GUI_CONST_STORAGE *)&GUI_FontHZ12_PropASC 
}; 

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

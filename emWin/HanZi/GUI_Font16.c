/*
*********************************************************************************************************
*	                                  
*	ģ������ : 16��������
*	�ļ����� : GUI_Font16.c
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


GUI_CONST_STORAGE GUI_CHARINFO GUI_FontHZ16_CharInfo[2] =  
{     
    {  8,    8,  1, (void *)"A16"},      
    {  16,  16,  2, (void *)"H16"},           
}; 

GUI_CONST_STORAGE GUI_FONT_PROP GUI_FontHZ16_PropHZ= { 
      0xA1A1,  
      0xFEFE,  
      &GUI_FontHZ16_CharInfo[1], 
      (void *)0,  
}; 

GUI_CONST_STORAGE  GUI_FONT_PROP GUI_FontHZ16_PropASC= { 
      0x0020,  
      0x007F,  
      &GUI_FontHZ16_CharInfo[0], 
      (void GUI_CONST_STORAGE *)&GUI_FontHZ16_PropHZ,  
}; 

GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ16 =  
{ 
	  GUI_FONTTYPE_USER,
      16,  
      16,  
      1,   
      1,   
      (void GUI_CONST_STORAGE *)&GUI_FontHZ16_PropASC,
}; 
GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ16x2 =  
{ 
	  GUI_FONTTYPE_USER,
      16,  
      16,  
      2,   
      2,   
      (void GUI_CONST_STORAGE *)&GUI_FontHZ16_PropASC 
};

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/


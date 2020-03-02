/*
*********************************************************************************************************
*
*	模块名称 : 我的电脑之文件浏览界面
*	文件名称 : App_ComputerChildDlg.c
*	版    本 : V1.0
*	说    明 : 我的电脑之文件浏览设置界面设计，采用的CHOOSEFILE对话框，此对话框存在如下问题
*              1. 这个对话框最大的败笔就是创建后无法配置控件的位置和大小，手册上面说可以根据字体大小自动
*                 调整，但是按钮的高度没办法自适应，搞的非常不好用。
*              2. 这个对话框的第二个败笔，创建后，如果用的不是他默认支持的字体，他会有个动态调整的过程，特
*                 别是上面的listview控件，明显可以看到网格的调整过程，也就是说创建了CHOOSEFILE对话框后，内
*                 部会自动执行一次更新。
*              3. 这个对话框的第三个败笔，确切的说应该是个bug，CHOOSEFILE对话框创建函数有个参数是设置对话框
*                 标志的，发现设置为隐藏后没有任何效果，依然是显示状态。
*              4. 介于上面的问题，采用逐个获取对话框上子控件的方法来进行调整。但是第2条中的问题是无法避免的。
*                 另外就是按钮控件不能单独设置字体，单独设置字体没有效果，必须设置全局默认字体。
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
	#define printf_filebrowsedbg printf
#else
	#define printf_filebrowsedbg(...)
#endif

/*
*********************************************************************************************************
*				                      变量
*********************************************************************************************************
*/
static struct 
{
	U32 Mask;
	char c;
} _aAttrib[] = 
{
	{ATTR_READ_ONLY,  'R'},
	{ATTR_HIDDEN,	  'H'},
	{ATTR_SYSTEM,	  'S'},
	{ATTR_VOLUME_ID,  'V'},
	{ATTR_DIRECTORY,  'L'},
	{ATTR_ARCHIVE,	  'A'},
};

FINFO fsinfo;
const char        acMask[]         = "*.*";
CHOOSEFILE_INFO   ChFileInfo       = { 0 };
WM_HWIN           hWinFileBrowse;
char              acDir[255]       = "M0:";
char const        *apDrives[1]     = { 0 };

/*
*********************************************************************************************************
*	函 数 名: _GetExt
*	功能说明: 获取文件名的后缀
*	形    参: pFile  文件名地址
*             pExt   存储文件名后缀的地址
*	返 回 值: 无
*********************************************************************************************************
*/
static void _GetExt(char * pFile, char * pExt) 
{
	int Len;
	int i;
	int j;

	//
	// 获得文件名后缀的起始位置
	//
	Len = strlen(pFile);
	for (i = Len; i > 0; i--) 
	{
		if (*(pFile + i) == '.') 
		{
			*(pFile + i) = '\0';  
			break;
		}
	}
	
	//
	// 复制文件名后缀字符到缓冲pExt里面
	//
	j = 0;
	while (*(pFile + ++i) != '\0') 
	{
		*(pExt + j) = *(pFile + i);
		j++;
	}
	*(pExt + j) = '\0';        
}

/*
*********************************************************************************************************
*	函 数 名: _GetData
*	功能说明: CHOOSEFILE对话框的回调函数
*	形    参: pInfo  CHOOSEFILE_INFO类指针变量
*	返 回 值: 无
*********************************************************************************************************
*/
static int _GetData(CHOOSEFILE_INFO *pInfo) 
{
	static char  acAttrib[20]  = { 0 };
	static char  acExt   [100];
	char         c;
	int          i;
	int          r = -1;	

	
	switch (pInfo->Cmd) 
	{
		case CHOOSEFILE_FINDFIRST:
			/* 
			   将根目录下的所有文件列出来。
			   1. "*" 或者 "*.*" 搜索指定路径下的所有文件
			   2. "abc*"         搜索指定路径下以abc开头的所有文件
			   3. "*.jpg"        搜索指定路径下以.jpg结尾的所有文件
			   4. "abc*.jpg"     搜索指定路径下以abc开头和.jpg结尾的所有文件
			
			   以下是实现搜索根目录下所有文件
			*/
			fsinfo.fileID = 0;   /* 每次使用ffind函数前，info.fileID必须初始化为0 */
			printf_filebrowsedbg("FINDFIRST%s\r\n", pInfo->pRoot);
			strcat((char *)pInfo->pRoot, "*");
			printf_filebrowsedbg("FINDFIRST%s\r\n", pInfo->pRoot);
			if(ffind ((const char *)pInfo->pRoot, &fsinfo) == 0)
			{
				r = 0;
				printf_filebrowsedbg("FINDFIRST%s\r\n", fsinfo.name);
			}
			else
			{
				r = 1;
			}
			pInfo->pRoot[strlen(pInfo->pRoot) - 1] = '\0';
			break;
			
		case CHOOSEFILE_FINDNEXT:
			strcat((char *)pInfo->pRoot, "*");
			printf_filebrowsedbg("FINDNEXT%s\r\n", pInfo->pRoot);
			if(ffind ((const char *)pInfo->pRoot, &fsinfo) == 0)
			{
				r = 0;
				printf_filebrowsedbg("FINDFIRST%s\r\n", fsinfo.name);
			}
			else
			{
				r = 1;
			}
			pInfo->pRoot[strlen(pInfo->pRoot) - 1] = '\0';
			break;
	}
	
	if (r == 0)
	{
		pInfo->Flags = (fsinfo.attrib & 0x10) ? CHOOSEFILE_FLAG_DIRECTORY : 0;
		for (i = 0; i < GUI_COUNTOF(_aAttrib); i++) 
		{
			if (fsinfo.attrib & _aAttrib[i].Mask) 
			{
				c = _aAttrib[i].c;
			} else 
			{
				c = '-';
			}
			acAttrib[i] = c;
		}
		
		if(fsinfo.attrib & 0x10)
		{
			pInfo->pExt    = "文件夹";
		}
		else
		{
			_GetExt((char *)fsinfo.name, acExt);
			pInfo->pExt    = acExt;	
		}

		pInfo->pAttrib = acAttrib;
		pInfo->pName   = (char *)fsinfo.name;
		pInfo->SizeL   = fsinfo.size << 10;
		pInfo->SizeH   = fsinfo.size >> 22;
	}
	
	return r;
}

/*
*********************************************************************************************************
*	函 数 名: App_ComputerChild
*	功能说明: CHOOSEFILE对话框创建
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void App_ComputerChild(void) 
{
	WM_HWIN hWinTemp;
	GUI_RECT Rect;
	HEADER_Handle hHeader;
	SCROLLBAR_Handle hScrollbar;
	
	/* 配置默认字体 */
	DROPDOWN_SetDefaultFont(&GUI_SysFontHZ16);
	BUTTON_SetDefaultFont(&GUI_SysFontHZ16);
	SCROLLBAR_SetDefaultWidth(18);
	
	/* 初始化参数 */
	apDrives[0] = acDir; 
	ChFileInfo.pfGetData = _GetData;
	ChFileInfo.pMask     = acMask;
	
	hWinFileBrowse = CHOOSEFILE_Create(0, 	                //Handle of parent window.
									   100, 			    //X position in pixels of the  dialog in clie nt coordinates.
							           30, 			        //Y position in pixels of the  dialog in client coordinates.
							           600, 	            //X-size of the dialog in pixels.
							           400,                 //Y-size of the dialog in pixels
							           apDrives, 		    //Pointer to an array of strings containing the root directories to be used.
							           GUI_COUNTOF(apDrives),//Number of root directories. 
							           0, 				    //Initial index of the root directory to be used.
							           "文件浏览", 	        //Title to be shown in the title bar.
							           0, 		            //Additional flags for the FRAMEWIN widget.
							           &ChFileInfo);		//Pointer to a CHOOSEFILE_INFO structure.
	
	/* 设置模态 */
	WM_MakeModal(hWinFileBrowse);

   	CHOOSEFILE_SetButtonText(hWinFileBrowse, CHOOSEFILE_BI_CANCEL, "关闭");
	CHOOSEFILE_SetButtonText(hWinFileBrowse, CHOOSEFILE_BI_UP, "返回上级");
	CHOOSEFILE_SetButtonText(hWinFileBrowse, CHOOSEFILE_BI_OK, "进入目录");

	FRAMEWIN_SetTextColor(hWinFileBrowse, 0x0000ff);
    FRAMEWIN_SetFont(hWinFileBrowse, &GUI_SysFontHZ16);
    FRAMEWIN_SetTextAlign(hWinFileBrowse, GUI_TA_VCENTER|GUI_TA_CENTER);
    FRAMEWIN_SetTitleHeight(hWinFileBrowse, 30);

	/* DropWown控件调整 ************************************************/
	hWinTemp = WM_GetFirstChild(WM_GetClientWindow(hWinFileBrowse));
	printf_filebrowsedbg("WM_GetNextSibling =%d\r\n", hWinTemp);
	WM_GetWindowRectEx(hWinTemp, &Rect);
	WM_SetWindowPos(hWinTemp, Rect.x0,
						   Rect.y0 - 15,
						   Rect.x1 - Rect.x0 + 1,
						   Rect.y1 - Rect.y0 + 1 + 5);

	DROPDOWN_SetTextAlign(hWinTemp, GUI_TA_LEFT|GUI_TA_VCENTER);

	/* 返回上一级按钮调整 ************************************************/
	hWinTemp = WM_GetNextSibling(hWinTemp);;
	printf_filebrowsedbg("WM_GetNextSibling =%d\r\n", hWinTemp);
	WM_GetWindowRectEx(hWinTemp, &Rect);
	WM_SetWindowPos(hWinTemp, Rect.x0,
						   Rect.y0 - 25,
						   Rect.x1 - Rect.x0 + 1,
						   Rect.y1 - Rect.y0 + 1 + 20);
	
	
	/* 进入目录按钮调整 **************************************************/
	hWinTemp = WM_GetNextSibling(hWinTemp);
	printf_filebrowsedbg("WM_GetNextSibling =%d\r\n", hWinTemp);
	WM_GetWindowRectEx(hWinTemp, &Rect);
	WM_SetWindowPos(hWinTemp, Rect.x0,
						   Rect.y0 - 25,
						   Rect.x1 - Rect.x0 + 1,
						   Rect.y1 - Rect.y0 + 1 + 20);
	
	
	/* 关闭按钮调整 ******************************************************/
	hWinTemp = WM_GetNextSibling(hWinTemp);
	printf_filebrowsedbg("WM_GetNextSibling =%d\r\n", hWinTemp);
	WM_GetWindowRectEx(hWinTemp, &Rect);
	WM_SetWindowPos(hWinTemp, Rect.x0,
						   Rect.y0 - 25,
						   Rect.x1 - Rect.x0 + 1,
						   Rect.y1 - Rect.y0 + 1 + 20);
	
	/* ListView控件调整 **************************************************/
	hWinTemp = WM_GetNextSibling(hWinTemp);
	printf_filebrowsedbg("WM_GetNextSibling =%d\r\n", hWinTemp);
	WM_GetWindowRectEx(hWinTemp, &Rect); 
	WM_SetWindowPos(hWinTemp, Rect.x0,
						   Rect.y0,
						   Rect.x1 - Rect.x0 + 1,
						   Rect.y1 - Rect.y0 + 1 -28);
			
	LISTVIEW_SetAutoScrollV(hWinTemp, 0);	
	LISTVIEW_SetAutoScrollH(hWinTemp, 0);
	hScrollbar = SCROLLBAR_CreateAttached(hWinTemp, SCROLLBAR_CF_VERTICAL);
    SCROLLBAR_SetWidth(hScrollbar, 18);
						   
	hHeader = LISTVIEW_GetHeader(hWinTemp);
    HEADER_SetFont(hHeader, &GUI_SysFontHZ16);
    HEADER_SetHeight(hHeader, 25);
	HEADER_SetItemText(hHeader, 0, "文件名（仅限ascii字符）");
	HEADER_SetTextAlign(hHeader, 0, GUI_TA_LEFT|GUI_TA_VCENTER);
	
	HEADER_SetItemText(hHeader, 1, "文件大小");
	HEADER_SetTextAlign(hHeader, 1, GUI_TA_LEFT|GUI_TA_VCENTER);	
	
	HEADER_SetItemText(hHeader, 2, "文件类型");
	HEADER_SetTextAlign(hHeader, 2, GUI_TA_LEFT|GUI_TA_VCENTER);	
	
	HEADER_SetItemText(hHeader, 3, "文件属性");
	HEADER_SetTextAlign(hHeader, 3, GUI_TA_LEFT|GUI_TA_VCENTER);	


    /* 设置LISTVIEW控件 */
    LISTVIEW_SetFont(hWinTemp, &GUI_SysFontHZ16);
    LISTVIEW_SetRowHeight(hWinTemp, 24);
    LISTVIEW_SetColumnWidth(hWinTemp, 0, 250);
    LISTVIEW_SetColumnWidth(hWinTemp, 1, 115);
    LISTVIEW_SetColumnWidth(hWinTemp, 2, 115);
	LISTVIEW_SetColumnWidth(hWinTemp, 3, 115);
	
	LISTVIEW_SetTextAlign(hWinTemp, 0, GUI_TA_LEFT|GUI_TA_VCENTER);
	LISTVIEW_SetTextAlign(hWinTemp, 1, GUI_TA_LEFT|GUI_TA_VCENTER);
	LISTVIEW_SetTextAlign(hWinTemp, 2, GUI_TA_LEFT|GUI_TA_VCENTER);
	LISTVIEW_SetTextAlign(hWinTemp, 3, GUI_TA_LEFT|GUI_TA_VCENTER);
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

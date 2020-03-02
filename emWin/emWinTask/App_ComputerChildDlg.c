/*
*********************************************************************************************************
*
*	ģ������ : �ҵĵ���֮�ļ��������
*	�ļ����� : App_ComputerChildDlg.c
*	��    �� : V1.0
*	˵    �� : �ҵĵ���֮�ļ�������ý�����ƣ����õ�CHOOSEFILE�Ի��򣬴˶Ի��������������
*              1. ����Ի������İܱʾ��Ǵ������޷����ÿؼ���λ�úʹ�С���ֲ�����˵���Ը��������С�Զ�
*                 ���������ǰ�ť�ĸ߶�û�취����Ӧ����ķǳ������á�
*              2. ����Ի���ĵڶ����ܱʣ�����������õĲ�����Ĭ��֧�ֵ����壬�����и���̬�����Ĺ��̣���
*                 ���������listview�ؼ������Կ��Կ�������ĵ������̣�Ҳ����˵������CHOOSEFILE�Ի������
*                 �����Զ�ִ��һ�θ��¡�
*              3. ����Ի���ĵ������ܱʣ�ȷ�е�˵Ӧ���Ǹ�bug��CHOOSEFILE�Ի��򴴽������и����������öԻ���
*                 ��־�ģ���������Ϊ���غ�û���κ�Ч������Ȼ����ʾ״̬��
*              4. ������������⣬���������ȡ�Ի������ӿؼ��ķ��������е��������ǵ�2���е��������޷�����ġ�
*                 ������ǰ�ť�ؼ����ܵ����������壬������������û��Ч������������ȫ��Ĭ�����塣
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
	#define printf_filebrowsedbg printf
#else
	#define printf_filebrowsedbg(...)
#endif

/*
*********************************************************************************************************
*				                      ����
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
*	�� �� ��: _GetExt
*	����˵��: ��ȡ�ļ����ĺ�׺
*	��    ��: pFile  �ļ�����ַ
*             pExt   �洢�ļ�����׺�ĵ�ַ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _GetExt(char * pFile, char * pExt) 
{
	int Len;
	int i;
	int j;

	//
	// ����ļ�����׺����ʼλ��
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
	// �����ļ�����׺�ַ�������pExt����
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
*	�� �� ��: _GetData
*	����˵��: CHOOSEFILE�Ի���Ļص�����
*	��    ��: pInfo  CHOOSEFILE_INFO��ָ�����
*	�� �� ֵ: ��
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
			   ����Ŀ¼�µ������ļ��г�����
			   1. "*" ���� "*.*" ����ָ��·���µ������ļ�
			   2. "abc*"         ����ָ��·������abc��ͷ�������ļ�
			   3. "*.jpg"        ����ָ��·������.jpg��β�������ļ�
			   4. "abc*.jpg"     ����ָ��·������abc��ͷ��.jpg��β�������ļ�
			
			   ������ʵ��������Ŀ¼�������ļ�
			*/
			fsinfo.fileID = 0;   /* ÿ��ʹ��ffind����ǰ��info.fileID�����ʼ��Ϊ0 */
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
			pInfo->pExt    = "�ļ���";
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
*	�� �� ��: App_ComputerChild
*	����˵��: CHOOSEFILE�Ի��򴴽�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void App_ComputerChild(void) 
{
	WM_HWIN hWinTemp;
	GUI_RECT Rect;
	HEADER_Handle hHeader;
	SCROLLBAR_Handle hScrollbar;
	
	/* ����Ĭ������ */
	DROPDOWN_SetDefaultFont(&GUI_SysFontHZ16);
	BUTTON_SetDefaultFont(&GUI_SysFontHZ16);
	SCROLLBAR_SetDefaultWidth(18);
	
	/* ��ʼ������ */
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
							           "�ļ����", 	        //Title to be shown in the title bar.
							           0, 		            //Additional flags for the FRAMEWIN widget.
							           &ChFileInfo);		//Pointer to a CHOOSEFILE_INFO structure.
	
	/* ����ģ̬ */
	WM_MakeModal(hWinFileBrowse);

   	CHOOSEFILE_SetButtonText(hWinFileBrowse, CHOOSEFILE_BI_CANCEL, "�ر�");
	CHOOSEFILE_SetButtonText(hWinFileBrowse, CHOOSEFILE_BI_UP, "�����ϼ�");
	CHOOSEFILE_SetButtonText(hWinFileBrowse, CHOOSEFILE_BI_OK, "����Ŀ¼");

	FRAMEWIN_SetTextColor(hWinFileBrowse, 0x0000ff);
    FRAMEWIN_SetFont(hWinFileBrowse, &GUI_SysFontHZ16);
    FRAMEWIN_SetTextAlign(hWinFileBrowse, GUI_TA_VCENTER|GUI_TA_CENTER);
    FRAMEWIN_SetTitleHeight(hWinFileBrowse, 30);

	/* DropWown�ؼ����� ************************************************/
	hWinTemp = WM_GetFirstChild(WM_GetClientWindow(hWinFileBrowse));
	printf_filebrowsedbg("WM_GetNextSibling =%d\r\n", hWinTemp);
	WM_GetWindowRectEx(hWinTemp, &Rect);
	WM_SetWindowPos(hWinTemp, Rect.x0,
						   Rect.y0 - 15,
						   Rect.x1 - Rect.x0 + 1,
						   Rect.y1 - Rect.y0 + 1 + 5);

	DROPDOWN_SetTextAlign(hWinTemp, GUI_TA_LEFT|GUI_TA_VCENTER);

	/* ������һ����ť���� ************************************************/
	hWinTemp = WM_GetNextSibling(hWinTemp);;
	printf_filebrowsedbg("WM_GetNextSibling =%d\r\n", hWinTemp);
	WM_GetWindowRectEx(hWinTemp, &Rect);
	WM_SetWindowPos(hWinTemp, Rect.x0,
						   Rect.y0 - 25,
						   Rect.x1 - Rect.x0 + 1,
						   Rect.y1 - Rect.y0 + 1 + 20);
	
	
	/* ����Ŀ¼��ť���� **************************************************/
	hWinTemp = WM_GetNextSibling(hWinTemp);
	printf_filebrowsedbg("WM_GetNextSibling =%d\r\n", hWinTemp);
	WM_GetWindowRectEx(hWinTemp, &Rect);
	WM_SetWindowPos(hWinTemp, Rect.x0,
						   Rect.y0 - 25,
						   Rect.x1 - Rect.x0 + 1,
						   Rect.y1 - Rect.y0 + 1 + 20);
	
	
	/* �رհ�ť���� ******************************************************/
	hWinTemp = WM_GetNextSibling(hWinTemp);
	printf_filebrowsedbg("WM_GetNextSibling =%d\r\n", hWinTemp);
	WM_GetWindowRectEx(hWinTemp, &Rect);
	WM_SetWindowPos(hWinTemp, Rect.x0,
						   Rect.y0 - 25,
						   Rect.x1 - Rect.x0 + 1,
						   Rect.y1 - Rect.y0 + 1 + 20);
	
	/* ListView�ؼ����� **************************************************/
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
	HEADER_SetItemText(hHeader, 0, "�ļ���������ascii�ַ���");
	HEADER_SetTextAlign(hHeader, 0, GUI_TA_LEFT|GUI_TA_VCENTER);
	
	HEADER_SetItemText(hHeader, 1, "�ļ���С");
	HEADER_SetTextAlign(hHeader, 1, GUI_TA_LEFT|GUI_TA_VCENTER);	
	
	HEADER_SetItemText(hHeader, 2, "�ļ�����");
	HEADER_SetTextAlign(hHeader, 2, GUI_TA_LEFT|GUI_TA_VCENTER);	
	
	HEADER_SetItemText(hHeader, 3, "�ļ�����");
	HEADER_SetTextAlign(hHeader, 3, GUI_TA_LEFT|GUI_TA_VCENTER);	


    /* ����LISTVIEW�ؼ� */
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

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

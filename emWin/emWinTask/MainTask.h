/*
*********************************************************************************************************
*	                                  
*	ģ������ : GUIͷ�ļ�
*	�ļ����� : MainTask.c
*	��    �� : V1.0
*	˵    �� : ����GUIӦ�ù��õ�ͷ�ļ�
*
*	�޸ļ�¼ :
*		�汾��   ����         ����       ˵��
*		V1.0    2017-03-13  Eric2013  	emWin V5.36
*                                     
*	Copyright (C), 2017-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#ifndef __MainTask_H
#define __MainTask_H

#include "GUI.h"
#include "DIALOG.h"
#include "WM.h"
#include "BUTTON.h"
#include "CHECKBOX.h"
#include "DROPDOWN.h"
#include "EDIT.h"
#include "FRAMEWIN.h"
#include "LISTBOX.h"
#include "MULTIEDIT.h"
#include "RADIO.h"
#include "SLIDER.h"
#include "TEXT.h"
#include "PROGBAR.h"
#include "SCROLLBAR.h"
#include "LISTVIEW.h"
#include "GRAPH.h"
#include "MENU.h"
#include "MULTIPAGE.h"
#include "ICONVIEW.h"
#include "TREEVIEW.h"
#include "IMAGE.h"
#include "ST_GUI_Addons.h"

/*
*********************************************************************************************************
*                                       �����ͱ���
*********************************************************************************************************
*/
/* �������ϵ�ϵͳ��Ϣ���ں�״̬�� */
extern WM_HWIN hWinStatus;
extern void CreateSysInfoDlg(void);
extern void CreateStatusDlg(void);

/* �ҵĵ���APP */
extern void App_Computer(WM_HWIN hWin);
extern void App_ComputerChild(void);

/* ϵͳ����APP */
extern void App_SysConfig(WM_HWIN hWin);

/* ���ֲ�����APP */
extern WM_HWIN  hWinMusic;
extern WM_HWIN  hWinMusicList;
extern void App_Music(WM_HWIN hWin);
extern void App_MusicSet(WM_HWIN hWin);
extern void App_MusicList(WM_HWIN hWin);

/* ��Ƶ������APP */
extern WM_HWIN  hWinVideo;
extern WM_HWIN  hWinVideoList;
extern void App_Video(WM_HWIN hWin);
extern void App_VideoList(WM_HWIN hWin);

/* ¼����APP */
extern WM_HWIN  hWinRecorder;   
extern void App_Recorder(WM_HWIN hWin);

/* ������APP */
extern void App_Radio(WM_HWIN hWin);

/* �ı��Ķ�APP */
extern WM_HWIN  hWinText;
extern WM_HWIN  hWinTextList;
extern void App_FileBrowse(WM_HWIN hWin);
extern void App_FileBrowseChild(WM_HWIN hWin); 

/* ͼƬ���APP */
extern void App_PicBrowse(WM_HWIN hWin);

/* ������ APP */
extern void App_Calendar(WM_HWIN hWin);
extern void App_CalendarChild(WM_HWIN hWin) ;
/* ������ APP */
extern void App_Calculator(WM_HWIN hWin);

/* Modbus���� APP */
extern WM_HWIN  hWinModbus;  
extern void App_Modbus(WM_HWIN hWin);

/* ����δ����APP ��ʹ��������� */
extern void App_Reserved(WM_HWIN hWin);


/*
*********************************************************************************************************
*                                       emWin����
*********************************************************************************************************
*/
#define DTCOLOR         0X200000
extern GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ_SimSun_16;
extern GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ16;
extern GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ24;
extern GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ32;
/* Ĭ������£�ϵͳ�������GUI_FontHZ16 */
#define  GUI_SysFontHZ16  GUI_FontHZ16


/*
*********************************************************************************************************
*                                       ͼƬ��ͼ��
*********************************************************************************************************
*/
extern const unsigned char _acbkpic[180875UL + 1];




extern GUI_CONST_STORAGE GUI_BITMAP _bmBatteryEmpty_27x14;
extern GUI_CONST_STORAGE GUI_BITMAP * _apbmCharge[];
extern GUI_CONST_STORAGE GUI_BITMAP _bmAlarm_16x16;
extern GUI_CONST_STORAGE GUI_BITMAP bmNetStatus;
extern GUI_CONST_STORAGE GUI_BITMAP bmSdcard;
extern GUI_CONST_STORAGE GUI_BITMAP bmusbtrans;
extern GUI_CONST_STORAGE GUI_BITMAP bmusbdisk;
extern GUI_CONST_STORAGE GUI_BITMAP bmLinkError;

extern GUI_CONST_STORAGE GUI_BITMAP _bmWhiteCircle_10x10;
extern GUI_CONST_STORAGE GUI_BITMAP _bmWhiteCircle_6x6;

/* ��1��ICONVIEW���� */
extern GUI_CONST_STORAGE GUI_BITMAP bmComputer;
extern GUI_CONST_STORAGE GUI_BITMAP bmSettings;
extern GUI_CONST_STORAGE GUI_BITMAP bmMusic;
extern GUI_CONST_STORAGE GUI_BITMAP bmVideo;
extern GUI_CONST_STORAGE GUI_BITMAP bmRecorder;
extern GUI_CONST_STORAGE GUI_BITMAP bmRadio;
extern GUI_CONST_STORAGE GUI_BITMAP bmReader;


extern GUI_CONST_STORAGE GUI_BITMAP bmPicture;
extern GUI_CONST_STORAGE GUI_BITMAP bmADC;
extern GUI_CONST_STORAGE GUI_BITMAP bmClock;
extern GUI_CONST_STORAGE GUI_BITMAP bmCalculator;


/* ��2��ICONVIEW���� */

//extern GUI_CONST_STORAGE GUI_BITMAP bmDAC;
//extern GUI_CONST_STORAGE GUI_BITMAP bmUSB;
//extern GUI_CONST_STORAGE GUI_BITMAP bmNet;
//extern GUI_CONST_STORAGE GUI_BITMAP bmCamera;


/* ��3��ICONVIEW���� */
//extern GUI_CONST_STORAGE GUI_BITMAP bmGPS;
//extern GUI_CONST_STORAGE GUI_BITMAP bmGPRS;
//extern GUI_CONST_STORAGE GUI_BITMAP bm3D;
//extern GUI_CONST_STORAGE GUI_BITMAP bmSensor;
//extern GUI_CONST_STORAGE GUI_BITMAP bmWIFI;
//extern GUI_CONST_STORAGE GUI_BITMAP bmWireless;
//extern GUI_CONST_STORAGE GUI_BITMAP bmSignal;

/* �ҵĵ��� */
extern GUI_CONST_STORAGE GUI_BITMAP bmfiledisk;
extern GUI_CONST_STORAGE GUI_BITMAP bmfileusb;
extern GUI_CONST_STORAGE GUI_BITMAP bmfilesd;

/* ͼƬ��� */
extern GUI_CONST_STORAGE GUI_BITMAP bmleft;
extern GUI_CONST_STORAGE GUI_BITMAP bmright;
extern GUI_CONST_STORAGE GUI_BITMAP bmreturn;
extern GUI_CONST_STORAGE GUI_BITMAP bmbmp;
extern GUI_CONST_STORAGE GUI_BITMAP bmjpg;
extern GUI_CONST_STORAGE GUI_BITMAP bmgif;

/* ����ͼ�� */
extern GUI_CONST_STORAGE GUI_BITMAP bmManualSearch;
extern GUI_CONST_STORAGE GUI_BITMAP bmReturn;
extern GUI_CONST_STORAGE GUI_BITMAP bmSetting;
extern GUI_CONST_STORAGE GUI_BITMAP bmclock;
extern GUI_CONST_STORAGE GUI_BITMAP bmClockIcon;

extern GUI_CONST_STORAGE GUI_BITMAP bmVideoIcon;
extern GUI_CONST_STORAGE GUI_BITMAP bmMusicStop;
extern GUI_CONST_STORAGE GUI_BITMAP bmMusicPlay;
extern GUI_CONST_STORAGE GUI_BITMAP bmMusicPre;
extern GUI_CONST_STORAGE GUI_BITMAP bmMusicNext;
extern GUI_CONST_STORAGE GUI_BITMAP bmMusicPause;
extern GUI_CONST_STORAGE GUI_BITMAP bmSpeaker;
extern GUI_CONST_STORAGE GUI_BITMAP bmSpeakerMute;

extern GUI_CONST_STORAGE GUI_BITMAP bmMusicList;
extern GUI_CONST_STORAGE GUI_BITMAP bmMicrophone;
extern GUI_CONST_STORAGE GUI_BITMAP bmRecorderIcon;
extern GUI_CONST_STORAGE GUI_BITMAP bmRadioSearch;


/*
*********************************************************************************************************
*                                       emWin�Զ�����Ϣ�궨��
*********************************************************************************************************
*/
#define MSG_TextStart     (GUI_ID_USER + 0x01)
#define MSG_TextCancel    (GUI_ID_USER + 0x02)

#define MSG_SDPlugIn      (GUI_ID_USER + 0x10)
#define MSG_SDPlugOut     (GUI_ID_USER + 0x11)
#define MSG_NETPlugIn     (GUI_ID_USER + 0x12)
#define MSG_NETPlugOut    (GUI_ID_USER + 0x13)

#define MSG_DispSpec      (GUI_ID_USER + 0x30)
#define MSG_MusicStart    (GUI_ID_USER + 0x31)
#define MSG_NextMusic     (GUI_ID_USER + 0x32)
#define MSG_MusicCancel   (GUI_ID_USER + 0x33)

#define MSG_CANReceive  (GUI_ID_USER + 0x40)

#define MSG_ModbusErr     (GUI_ID_USER + 0x50)
#define MSG_ModbusSuccess (GUI_ID_USER + 0x51)

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

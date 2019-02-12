#ifndef _CDLG_MUSICPLAYER_H_
#define _CDLG_MUSICPLAYER_H_

#include <emXGUI.h>

#define GUI_RGB_BACKGROUNG_PIC      "avi_desktop.jpg"


typedef struct 
{
  HWND mMusicPlayer_hwnd;//²¥·ÅÆ÷¾ä±ú
  HWND mMusic_wnd_time;//½ø¶ÈÌõ¾ä±ú
  HDC  mhdc_bk;        //²¥·ÅÆ÷±³¾°²ã
  
  int power;
  
  BOOL LoadPic_State;
}Cdlg_Master_Struct;

typedef enum
{
  eID_BUTTON_Power = 0x1000,
  eID_BUTTON_List,
  eID_BUTTON_Equa,
  eID_BUTTON_BACK,
  eID_BUTTON_NEXT,
  eID_BUTTON_START,
  eID_SCROLLBAR_TIMER,
  eID_SCROLLBAR_POWER,
  eID_TB5,
  eID_TB1,
  eID_TB2,
}Cdlg_Master_ID;


#endif /* _CDLG_MUSICPLAYER_H_ */




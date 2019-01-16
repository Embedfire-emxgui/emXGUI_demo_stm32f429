#ifndef _CDlg_MusicPlayer_H_
#define _CDlg_MusicPlayer_H_

#include <emXGUI.h>
#include "ExitButton.h"
enum CDlg_MusicPlayerID
{
   ID_BUTTON_Power = 0x1000,
   ID_BUTTON_List,
   ID_BUTTON_LRC,
   ID_BUTTON_BACK,
   ID_BUTTON_NEXT,
   ID_BUTTON_START,
   ID_BUTTON_EXIT,
   //滑动条
   ID_SCROLLBAR_TIMER,
   ID_SCROLLBAR_POWER,
   //TEXTBOX
   ID_TEXTBOX_LRC1,
   ID_TEXTBOX_LRC2,
   ID_TEXTBOX_LRC3,
   ID_TEXTBOX_LRC4,
   ID_TEXTBOX_LRC5,
   
   ID_TEXTBOX_MUSICNAME,
   ID_TEXTBOX_CURTIME,
   ID_TEXTBOX_ALLTIME,
};

typedef struct{
   char *icon_name;//图标名
   RECT rc;        //位置信息
   BOOL state;     //状态  
}icon_S;

typedef struct
{ 
   HWND h_MusicPlayer; //播放器句柄 
   HWND h_MusicLrc;    //歌词句柄
   
   COLORREF back_col;  //背景颜色
   COLORREF bt_default_col;  //控件被按下的颜色
   COLORREF bt_pushed_col;  //控件被按下的颜色
   
   SCROLLINFO sif_time;//歌曲进度条
   SCROLLINFO sif_power;//音量进度条
   RECT rc_cli;        //播放器窗口位置大小            
   int power;          //音量
   BOOL power_state;   //音量变化标志
   
   ScrollBar_S ScrollBar_parm1;//滑动条参数结构体
   ScrollBar_S ScrollBar_parm2;//滑动条参数结构体
   icon_S *pCDlg_MusicPlayerICON; //图标的位置大小
   void (* exit_bt_draw)(DRAWITEM_HDR*);
}CDlg_MusicPlayer_s;



extern void GUI_CDlg_MusicPlayer(void);

#endif /*  _CDlg_MusicPlayer_H_  */



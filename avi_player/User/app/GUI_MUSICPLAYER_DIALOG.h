#ifndef _GUI_MUSICPLAYER_DIALOG_H
#define _GUI_MUSICPLAYER_DIALOG_H
/*****************滑动条控件ID值*********************/
#define ID_SCROLLBAR_POWER   0x1104   //音量条
#define ID_SCROLLBAR_TIMER   0x1105   //进度条

/******************按钮控件ID值***********************/
#define ID_BUTTON_Power      0x1000   //音量 
#define ID_BUTTON_List       0x1001   //音乐List


typedef struct{
   char *icon_name;//图标名
   RECT rc;        //位置信息
   BOOL state;     //状态  
}icon_S;



#endif


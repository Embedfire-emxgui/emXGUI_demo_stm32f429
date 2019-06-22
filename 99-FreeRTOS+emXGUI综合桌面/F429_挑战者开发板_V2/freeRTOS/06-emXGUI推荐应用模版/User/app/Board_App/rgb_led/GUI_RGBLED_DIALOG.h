#ifndef _GUI_RGBLED_DIALOG_H_
#define _GUI_RGBLED_DIALOG_H_

#include <emXGUI.h>

#define GUI_RGB_BACKGROUNG_PIC      "desktop.jpg"

#define APP_BackColor                ARGB8888(0,169,169,169)

//标题栏颜色参数
#define ID_TEXTBOX_Title_BackColor   APP_BackColor          //背景颜色
#define ID_TEXTBOX_Title_TextColor   RGB888(255,255,255)    //文字颜色

#define ID_TEXTBOX_APP_BackColor     APP_BackColor 
#define ID_TEXTBOX_R_LED_BackColor   APP_BackColor
#define ID_TEXTBOX_G_LED_BackColor   APP_BackColor
#define ID_TEXTBOX_B_LED_BackColor   APP_BackColor
#define ID_TEXTBOX_R_NUM_BackColor   APP_BackColor
#define ID_TEXTBOX_R_BackColor       APP_BackColor
#define ID_TEXTBOX_G_BackColor       APP_BackColor
#define ID_TEXTBOX_B_BackColor       APP_BackColor
#define ID_SCROLLBAR_BackColor       APP_BackColor

enum GUI_RGBLEDID
{
   /*滑动条ID宏定义*/
   ID_SCROLLBAR_R = 0x1100,
   ID_SCROLLBAR_G  ,
   ID_SCROLLBAR_B ,

   /*文本框ID宏定义*/
   ID_TEXTBOX_Title,
   ID_TEXTBOX_APP,
   ID_TEXTBOX_R,
  
   ID_TEXTBOX_R_LED,   
   ID_TEXTBOX_R_NUM,
   ID_TEXTBOX_G,	 
  
   ID_TEXTBOX_G_LED,   
   ID_TEXTBOX_G_NUM,  
   ID_TEXTBOX_B,
  
   ID_TEXTBOX_B_LED,   
   ID_TEXTBOX_B_NUM,  
   /*按钮ID宏定义*/
   ID_EXIT,  
   ID_CHECKBOX_SW, 
   
   
};
typedef struct
{
   HWND RGBLED_Hwnd;//窗口句柄
   BOOL State;      //硬件RGB灯的开关
   HDC hdc_mem;
  
	 SCROLLINFO sif_R;//红色分量值
   SCROLLINFO sif_G;//红色分量值
   SCROLLINFO sif_B;//红色分量值
      
   u8 *jpg_buf;     //背景图片数组
   u32 jpg_size;    //背景图片大小
   int col_R;       //红色分量初始值
   int col_G;       //绿色分量初始值
   int col_B;       //蓝色分量初始值
   void (* exit_bt_draw)(DRAWITEM_HDR*);//窗口退出按钮回调函数
}RGBLED_DIALOG_s;
#endif /* _GUI_RGBLED_DIALOG_H_ */



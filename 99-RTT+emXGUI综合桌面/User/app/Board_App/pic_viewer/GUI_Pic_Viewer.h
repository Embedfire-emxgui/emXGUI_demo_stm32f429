#ifndef _GUI_PICVIEWR_H_
#define _GUI_PICVIEWR_H_

#include <emXGUI.h>

#define PICFILE_NUM_MAX           20
#define PICFILE_NAME_MAXLEN       100	

typedef enum 
{
  eID_Pic_Name,
  eID_Pic_EXIT,
  eID_Pic_PREV,
  eID_Pic_NEXT,
}GUI_PicViewer_ID;

typedef struct 
{
  HWND mPicViewer;

  HDC  mhdc_bk;        //背景层
  HDC  mhdc_pic;       //图片层
  
  char** pic_list;   //文件路径（包含文件名）
  char** pic_lcdlist;//文件名
  
  uint16_t pic_width;
  uint16_t pic_height;
  uint8_t show_index;
  uint8_t pic_nums;
}PicViewer_Master_Struct;

#endif /* _CDLG_PICVIEWR_H_ */




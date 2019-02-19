#include "emXGUI.h"
#include "Widget.h"
#include "GUI_Pic_Viewer.h"
#include "GUI_AppDef.h"
#include "ff.h"
#include "x_libc.h"
#include <string.h>
#include "emXGUI_JPEG.h"

#define PIC_OWN_MSG    WM_USER+1

PicViewer_Master_Struct PicViewer = 
{
  .pic_nums = 0,
  .show_index = 0,
  .Menu_State = 0,
  .SecMenu_State = 0,
  .cur_type = eID_Pic_JPG,
  .cur_path = eID_Pic_SDCARD,
};

static char path[100] = "0:";//文件根目录

icon_S GUI_PicViewer_Icon[10] = 
{
  {"Pic_Name",           {100,0,600,35},        FALSE},
  {"Pic_MSGBOX",         {200,240,400,70},      FALSE},
  {"Pic_Res",            {90,35,200,35},       FALSE},
  {"Pic_Res_Value",      {290,35,100,35},       FALSE},
  {"Pic_Scale",          {400,35,120,35},       FALSE}, 
  {"Pic_Scale_Value",    {520,35,150,35},       FALSE},
  {"In_Flash",           {0,420,250,60},       FALSE},
  {"Ex_Flash",           {250,420,250,60},       FALSE}, 
  {"SD_Card",            {500,420,300,60},       FALSE}, 
  
};

u8 *jpeg_buf;
u8 *bmp_buf;
u32 jpeg_size;
u32 bmp_size;
JPG_DEC *dec;

//透明文本
static void PicViewer_TBOX_OwnerDraw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
	HWND hwnd;
	HDC hdc;
	RECT rc, rc_tmp;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button的窗口句柄.
	hdc = ds->hDC;   //button的绘图上下文句柄.
  GetClientRect(hwnd, &rc_tmp);//得到控件的位置
  GetClientRect(hwnd, &rc);//得到控件的位置
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换
  
  SetBrushColor(hdc, MapRGB(hdc, 0,0,0));
  FillRect(hdc, &rc);
  //BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, PicViewer.mhdc_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);
  SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));


  GetWindowText(hwnd, wbuf, 128); //获得按钮控件的文字

  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//绘制文字(居中对齐方式)

}
////透明文本
//static void PicViewer_AButton_OwnerDraw(DRAWITEM_HDR *ds) //绘制一个按钮外观
//{
//	HWND hwnd;
//	HDC hdc/*, hdc_mem*/;
//	RECT rc;
//	WCHAR wbuf[128];

//	hwnd = ds->hwnd; //button的窗口句柄.
//	hdc = ds->hDC;   //button的绘图上下文句柄.
//  GetClientRect(hwnd, &rc);//得到控件的位置
//  
//  //hdc_mem = CreateMemoryDC(SURF_ARGB4444, rc.w, rc.h);
//  
//  SetBrushColor(hdc, MapRGB(hdc, 105,105,105));
//  DrawRect(hdc, &rc);
//  InflateRect(&rc, -1, -1);
//   if (ds->State & BST_PUSHED)
//	{ //按钮是按下状态
//    SetBrushColor(hdc, MapRGB(hdc,215,61,50));
//	}
//	else
//	{ //按钮是弹起状态
//    SetBrushColor(hdc, MapRGB(hdc, 50,205,50));
//	}  
//  
//  FillRect(hdc, &rc);
//  GetClientRect(hwnd, &rc);
//  //BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_mem, rc.x, rc.y, SRCCOPY);
//  //SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));


//  GetWindowText(hwnd, wbuf, 128); //获得按钮控件的文字

//  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//绘制文字(居中对齐方式)
//  //DeleteDC(hdc_mem);

//}
static void PicViewer_Button_OwnerDraw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
	HWND hwnd;
	HDC hdc;
	RECT rc, rc_tmp;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button的窗口句柄.
	hdc = ds->hDC;   //button的绘图上下文句柄.
	rc = ds->rc;     //button的绘制矩形区.

  GetClientRect(hwnd, &rc_tmp);//得到控件的位置
  GetClientRect(hwnd, &rc);//得到控件的位置
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换

//  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, PicViewer.mhdc_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);
  SetBrushColor(hdc, MapRGB(hdc, 0,0,0));
  FillRect(hdc, &rc);
	if (IsWindowEnabled(hwnd) == FALSE)
	{
		SetTextColor(hdc, MapRGB(hdc, COLOR_INVALID));
	}
	else if (ds->State & BST_PUSHED)
	{ //按钮是按下状态
//    GUI_DEBUG("ds->ID=%d,BST_PUSHED",ds->ID);
//		SetBrushColor(hdc,MapRGB(hdc,150,200,250)); //设置填充色(BrushColor用于所有Fill类型的绘图函数)
//		SetPenColor(hdc,MapRGB(hdc,250,0,0));        //设置绘制色(PenColor用于所有Draw类型的绘图函数)
		SetTextColor(hdc, MapRGB(hdc, 105, 105, 105));      //设置文字色
	}
	else
	{ //按钮是弹起状态
//		SetBrushColor(hdc,MapRGB(hdc,255,255,255));
//		SetPenColor(hdc,MapRGB(hdc,0,250,0));
		SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
	}


	//	SetBrushColor(hdc,COLOR_BACK_GROUND);

	//	FillRect(hdc,&rc); //用矩形填充背景
	//	DrawRect(hdc,&rc); //画矩形外框
	//  
	//  FillCircle(hdc,rc.x+rc.w/2,rc.x+rc.w/2,rc.w/2); //用矩形填充背景FillCircle
	//	DrawCircle(hdc,rc.x+rc.w/2,rc.x+rc.w/2,rc.w/2); //画矩形外框

	  /* 使用控制图标字体 */
	SetFont(hdc, controlFont_64);
	//  SetTextColor(hdc,MapRGB(hdc,255,255,255));

	GetWindowText(ds->hwnd, wbuf, 128); //获得按钮控件的文字

	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER | DT_CENTER);//绘制文字(居中对齐方式)


  /* 恢复默认字体 */
	SetFont(hdc, defaultFont);

}

static void PicViewer_ExitButton_OwnerDraw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
	HWND hwnd;
	HDC hdc;
	RECT rc,rc_tmp;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button的窗口句柄.
	hdc = ds->hDC;   //button的绘图上下文句柄.
	rc = ds->rc;     //button的绘制矩形区.
  GetClientRect(hwnd, &rc_tmp);//得到控件的位置
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换
  SetBrushColor(hdc, MapRGB(hdc, 0,0,0));
  FillRect(hdc, &rc);
  //BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, PicViewer.mhdc_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);
	SetBrushColor(hdc, MapRGB(hdc, COLOR_DESKTOP_BACK_GROUND));
   
  FillCircle(hdc, rc.x+rc.w, rc.y, rc.w);
	//FillRect(hdc, &rc); //用矩形填充背景

  if (ds->State & BST_PUSHED)
	{ //按钮是按下状态
//    GUI_DEBUG("ds->ID=%d,BST_PUSHED",ds->ID);
//		SetBrushColor(hdc,MapRGB(hdc,150,200,250)); //设置填充色(BrushColor用于所有Fill类型的绘图函数)
//		SetPenColor(hdc,MapRGB(hdc,250,0,0));        //设置绘制色(PenColor用于所有Draw类型的绘图函数)
		SetTextColor(hdc, MapRGB(hdc, 105, 105, 105));      //设置文字色
	}
	else
	{ //按钮是弹起状态
//		SetBrushColor(hdc,MapRGB(hdc,255,255,255));
//		SetPenColor(hdc,MapRGB(hdc,0,250,0));
		SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
	}

	  /* 使用控制图标字体 */
	SetFont(hdc, controlFont_64);
	//  SetTextColor(hdc,MapRGB(hdc,255,255,255));

	GetWindowText(hwnd, wbuf, 128); //获得按钮控件的文字
  rc.y = -10;
  rc.x = 16;
	DrawText(hdc, wbuf, -1, &rc, NULL);//绘制文字(居中对齐方式)


  /* 恢复默认字体 */
	SetFont(hdc, defaultFont);

}
static void Pic_ReturnBTN_Ownerdraw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button的窗口句柄.
	hdc = ds->hDC;   //button的绘图上下文句柄.
	rc = ds->rc;     //button的绘制矩形区.
  if(ds->State & BST_PUSHED) //按钮是按下状态
	{

		SetTextColor(hdc, MapRGB(hdc, 105, 105, 105));      //设置文字色
	}
	else//按钮是弹起状态
	{ 

		SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
	}

	/* 使用控制图标字体 */
	SetFont(hdc, controlFont_32);

	GetWindowText(hwnd, wbuf, 128); //获得按钮控件的文字

	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER);//绘制文字(居中对齐方式)
   rc.x = 25; 
  /* 恢复默认字体 */
	SetFont(hdc, defaultFont);
  DrawText(hdc, L"返回", -1, &rc, DT_VCENTER);
}

static FRESULT scan_Picfiles(char* path, char* type) 
{ 
  FRESULT res; 		//部分在递归过程被修改的变量，不用全局变量	
  FILINFO fno; 
  DIR dir; 
  int i; 
  char *fn; 
  char file_name[100];	
	
#if _USE_LFN 
  static char lfn[_MAX_LFN * (0x81 ? 2 : 1) + 1]; 	//长文件名支持
  fno.lfname = lfn; 
  fno.lfsize = sizeof(lfn); 
#endif  
  res = f_opendir(&dir, path); //打开目录
  if (res == FR_OK) 
  { 
    i = strlen(path); 
    for (;;) 
    { 
      res = f_readdir(&dir, &fno); 										//读取目录下的内容
     if (res != FR_OK || fno.fname[0] == 0) break; 	//为空时表示所有项目读取完毕，跳出
#if _USE_LFN 
      fn = *fno.lfname ? fno.lfname : fno.fname; 
#else 
      fn = fno.fname; 
#endif 
      if (*fn == '.') continue; 											//点表示当前目录，跳过			
      if (fno.fattrib & AM_DIR) 
			{ 																							//目录，递归读取
        sprintf(&path[i], "/%s", fn); 							//合成完整目录名
        res = scan_Picfiles(path,type);											//递归遍历 
        if (res != FR_OK) 
					break; 																		//打开失败，跳出循环
        path[i] = 0; 
      } 
      else 
		{ 
				//printf("%s%s\r\n", path, fn);								//输出文件名
				if(strstr(fn, type))
				{
					if ((strlen(path)+strlen(fn)<100)&&(PicViewer.pic_nums<100))
					{
						sprintf(file_name, "%s/%s", path, fn);						
						memcpy(PicViewer.pic_list[PicViewer.pic_nums],file_name,strlen(file_name));//打开路径
            printf("%s\r\n", PicViewer.pic_list[PicViewer.pic_nums]);
						memcpy(PicViewer.pic_lcdlist[PicViewer.pic_nums],fn,strlen(fn));						//文件名
						PicViewer.pic_nums++;//记录文件个数
					}
				}//if
      }//else
     } //for
  } 
  return res; 
}
//float iii = 0.0;
void Draw_Pic_JPG(char *file_name)
{
  BOOL res;
  RECT rc = {0,0,800,480};
  res= FS_Load_Content(file_name, (char**)&jpeg_buf, &jpeg_size);
  if(res)
  {
    /* 根据图片数据创建JPG_DEC句柄 */
    dec = JPG_Open(jpeg_buf, jpeg_size);
    /* 读取图片文件信息 */
    JPG_GetImageSize(&PicViewer.pic_width, &PicViewer.pic_height,dec);
    
    PicViewer.mhdc_pic = CreateMemoryDC(SURF_SCREEN, 800, 480);
    SetBrushColor(PicViewer.mhdc_pic, MapRGB(PicViewer.mhdc_pic, 0, 0, 0));
    FillRect(PicViewer.mhdc_pic, &rc);     
    
    
    
//    if(PicViewer.pic_width!=800 && PicViewer.pic_height != 480)
//    {   
//      /* 绘制至内存对象 */
//      JPG_Draw(PicViewer.mhdc_pic, 400-PicViewer.pic_width/2, 280 - PicViewer.pic_height/2, dec);
//    }
//    else
//    {
//      HDC hdc_tmp;
//      hdc_tmp = CreateMemoryDC(SURF_SCREEN, 800, 480);
//      JPG_Draw(hdc_tmp, 400-PicViewer.pic_width/2, 240 - PicViewer.pic_height/2, dec);
//      StretchBlt(PicViewer.mhdc_pic,70,70, 660,410,hdc_tmp,0,0,800,480,SRCCOPY);
//      DeleteDC(hdc_tmp);
//    }
      HDC hdc_tmp;
      int scale_w = (PicViewer.pic_width>660)?660:PicViewer.pic_width;
      int scale_h = (PicViewer.pic_height>410)?410:PicViewer.pic_height;
      
      PicViewer.scale_x = (scale_w == PicViewer.pic_width)?1:(float)660/PicViewer.pic_width;
      PicViewer.scale_y = (scale_h == PicViewer.pic_height)?1:(float)410/PicViewer.pic_height;
      
      hdc_tmp = CreateMemoryDC(SURF_SCREEN, 800, 480);
      JPG_Draw(hdc_tmp, 0, 0, dec);
      StretchBlt(PicViewer.mhdc_pic,400-scale_w/2,280 - scale_h/2, scale_w,scale_h,
                 hdc_tmp,0,0,PicViewer.pic_width,PicViewer.pic_height,SRCCOPY);
      DeleteDC(hdc_tmp);
      /* 关闭JPG_DEC句柄 */
      JPG_Close(dec);
    //DeleteDC(PicViewer.mhdc_pic);
  }
  /* 释放图片内容空间 */
  RES_Release_Content((char **)&jpeg_buf);  
}
#if 1
BITMAPINFO bm_info;
void Draw_Pic_BMP(HDC hdc, char *file_name)
{
  

  PIC_BMP_GetInfo_FS(&bm_info,file_name);

  PicViewer.pic_width = bm_info.Width;
  PicViewer.pic_height = bm_info.Height;

  int scale_w = (PicViewer.pic_width>660)?660:PicViewer.pic_width;
  int scale_h = (PicViewer.pic_height>410)?410:PicViewer.pic_height;

  PicViewer.scale_x = (scale_w == PicViewer.pic_width)?1:(float)660/PicViewer.pic_width;
  PicViewer.scale_y = (scale_h == PicViewer.pic_height)?1:(float)410/PicViewer.pic_height; 
   /* 绘制bmp到hdc */
  PIC_BMP_Draw_FS(hdc, 400-scale_w/2,280 - scale_h/2, file_name, NULL);  
  
}
#endif
void PicViewer_Init(void)
{
  int i = 0, j = 0;
  
  //Step1:分配内存空间
 
  
  PicViewer.pic_list = (char **)GUI_VMEM_Alloc(sizeof(char*) * PICFILE_NUM_MAX);//分配行空间
  for(i = 0; i < PICFILE_NUM_MAX; i++)
  {
    PicViewer.pic_list[i] = (char *)GUI_VMEM_Alloc(sizeof(char) * PICFILE_NAME_MAXLEN);//分配列空间
  }
  
  PicViewer.pic_lcdlist = (char **)GUI_VMEM_Alloc(sizeof(char*) * PICFILE_NUM_MAX);//分配行空间
  for(i = 0; i < PICFILE_NUM_MAX; i++)
  {
    PicViewer.pic_lcdlist[i] = (char *)GUI_VMEM_Alloc(sizeof(char) * PICFILE_NAME_MAXLEN);//分配列空间
  }  
  for(i = 0; i < PICFILE_NUM_MAX; i++)
  {
    for(j = 0; j < PICFILE_NAME_MAXLEN; j++)
    {
      PicViewer.pic_lcdlist[i][j] = '\0';
      PicViewer.pic_list[i][j] = '\0';
    }
  }
  //Step2：扫描图片文件
  scan_Picfiles(path,".jpg");
}
void PicViewer_Quit(void)
{
  int i = 0;
  for(;i < PICFILE_NUM_MAX; i++)
  {
    GUI_VMEM_Free(PicViewer.pic_list[i]);
    GUI_VMEM_Free(PicViewer.pic_lcdlist[i]);
  }
  GUI_VMEM_Free(PicViewer.pic_list);
  GUI_VMEM_Free(PicViewer.pic_lcdlist);
  
  PicViewer.pic_nums = 0;
  PicViewer.show_index = 0;
  
  //DeleteDC(PicViewer.mhdc_bk);
  
}

static	LRESULT	DlgType_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_CREATE:
    {
      RECT rc = {0,55,300,30};
      CreateWindow(BUTTON, L"F", BS_FLAT | BS_NOTIFY|WS_TRANSPARENT|WS_OWNERDRAW |WS_VISIBLE,
                    0, 0, 75, 50, hwnd, eID_Pic_Return, NULL, NULL);   

      CreateWindow(BUTTON, L"JPG", BS_RADIOBOX|WS_VISIBLE|WS_TRANSPARENT ,rc.x, rc.y, rc.w, rc.h,
                   hwnd, (2<<16)|eID_Pic_JPG, NULL, NULL);   
      OffsetRect(&rc, 0, rc.h+5); 
      CreateWindow(BUTTON, L"PNG", BS_RADIOBOX|WS_VISIBLE|WS_TRANSPARENT ,rc.x, rc.y, rc.w, rc.h,
                   hwnd, (2<<16)|eID_Pic_PNG, NULL, NULL); 
      OffsetRect(&rc, 0, rc.h+5); 
      CreateWindow(BUTTON, L"GIF", BS_RADIOBOX|WS_VISIBLE|WS_TRANSPARENT ,rc.x, rc.y, rc.w, rc.h,
                   hwnd, (2<<16)|eID_Pic_GIF, NULL, NULL); 
      OffsetRect(&rc, 0, rc.h+5); 
      CreateWindow(BUTTON, L"BMP", BS_RADIOBOX|WS_VISIBLE|WS_TRANSPARENT ,rc.x, rc.y, rc.w, rc.h,
                   hwnd, (2<<16)|eID_Pic_BMP, NULL, NULL); 
      switch(PicViewer.cur_type)
      {
        case eID_Pic_JPG:
        {
          SendMessage(GetDlgItem(hwnd, eID_Pic_JPG),BM_SETSTATE,BST_CHECKED,0);
          break;
        }
        case eID_Pic_PNG:
        {
          SendMessage(GetDlgItem(hwnd, eID_Pic_PNG),BM_SETSTATE,BST_CHECKED,0);
          break;
        }    
        case eID_Pic_GIF:
        {
          SendMessage(GetDlgItem(hwnd, eID_Pic_GIF),BM_SETSTATE,BST_CHECKED,0);
          break;
        } 
        case eID_Pic_BMP:
        {
          SendMessage(GetDlgItem(hwnd, eID_Pic_BMP),BM_SETSTATE,BST_CHECKED,0);
          break;
        }        
      }      
      break;
    }
    case	WM_CTLCOLOR:
    {
      u16 id;
      id =LOWORD(wParam);
      CTLCOLOR *cr;
      cr =(CTLCOLOR*)lParam;
      if(id >=eID_Pic_JPG && id <= eID_Pic_BMP)
      {
        cr->TextColor =RGB888(250,250,250);
        cr->BackColor =RGB888(105,105,105);
        cr->BorderColor =RGB888(50,50,50);
        cr->ForeColor =RGB888(105,105,105);
        return TRUE;            
      }

      return FALSE;

    }
    case WM_NOTIFY:
    {
      u16 code,id;
      code =HIWORD(wParam); //获得通知码类型.
      id   =LOWORD(wParam); //获得产生该消息的控件ID.      
      if(id >= eID_Pic_JPG && id<= eID_Pic_BMP)
      {
        if(code == BN_CLICKED)
        { 
          PicViewer.cur_type = id;
          switch(PicViewer.cur_type)
          {
            case eID_Pic_JPG:
            {
              int i, j;
              PicViewer.pic_nums = 0;
              PicViewer.show_index = 0;
              for(i = 0; i < PICFILE_NUM_MAX; i++)
              {
                for(j = 0; j < PICFILE_NAME_MAXLEN; j++)
                {
                  PicViewer.pic_lcdlist[i][j] = '\0';
                  PicViewer.pic_list[i][j] = '\0';
                }
              }              
              scan_Picfiles(path, ".jpg");
              InvalidateRect(PicViewer.mPicViewer, NULL, TRUE);
              break;  
            }          
            case eID_Pic_PNG:
            {

              break;
            }
            case eID_Pic_GIF:
            {

              break;
            }
            case eID_Pic_BMP:
            {
              int i, j;
              PicViewer.pic_nums = 0;
              PicViewer.show_index = 0;
              for(i = 0; i < PICFILE_NUM_MAX; i++)
              {
                for(j = 0; j < PICFILE_NAME_MAXLEN; j++)
                {
                  PicViewer.pic_lcdlist[i][j] = '\0';
                  PicViewer.pic_list[i][j] = '\0';
                }
              }              
              scan_Picfiles(path, ".bmp");
              InvalidateRect(PicViewer.mPicViewer, NULL, TRUE);
              break;
            }            
          }
        
        }
      }
      break;
    }    
    case WM_ERASEBKGND:
    {
      HDC hdc =(HDC)wParam;
      RECT rc =*(RECT*)lParam;
      RECT rc_title = {75,0,150,50};
      
      SetBrushColor(hdc, MapRGB(hdc, 0,0,0));
      FillRect(hdc, &rc);
      
      SetTextColor(hdc, MapRGB(hdc, 250,250,250));
      DrawText(hdc,L"SD卡",-1,&rc_title,DT_SINGLELINE|DT_VCENTER|DT_CENTER);
      SetPenColor(hdc, MapRGB(hdc, 105,105,105));
      HLine(hdc,0,45,rc.w);
      return TRUE;
    }
    case WM_DRAWITEM:
    {
      DRAWITEM_HDR *ds;
      ds = (DRAWITEM_HDR*)lParam;
      Pic_ReturnBTN_Ownerdraw(ds);
      return TRUE;
    }
    default:
      return	DefWindowProc(hwnd, msg, wParam, lParam);
  }
  return WM_NULL;
}

static	LRESULT	PicViewer_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
  {
    case WM_CREATE:
    {
      RECT rc;
      GetClientRect(hwnd, &rc); 
      CreateWindow(BUTTON, L"图片名称", WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE, 
                   GUI_PicViewer_Icon[0].rc.x, GUI_PicViewer_Icon[0].rc.y, 
                   GUI_PicViewer_Icon[0].rc.w, GUI_PicViewer_Icon[0].rc.h,          
                   hwnd, eID_Pic_Name, NULL, NULL);  
      CreateWindow(BUTTON, L"O", BS_FLAT | BS_NOTIFY |WS_OWNERDRAW|WS_VISIBLE,
                   730, 0, 70, 70, hwnd, eID_Pic_EXIT, NULL, NULL);      
      //SetWindowFont(GetDlgItem(hwnd, eID_Pic_Name), controlFont_32);
      CreateWindow(BUTTON, L"L", BS_FLAT | BS_NOTIFY | WS_OWNERDRAW |WS_VISIBLE,
                   0, rc.h * 1 / 2, 70, 70, hwnd, eID_Pic_PREV, NULL, NULL);
      SetWindowFont(GetDlgItem(hwnd,eID_Pic_PREV), controlFont_48); 
      CreateWindow(BUTTON, L"K", BS_FLAT | BS_NOTIFY | WS_OWNERDRAW | WS_VISIBLE,
                   rc.w - 65, rc.h * 1 / 2, 70, 70, hwnd, eID_Pic_NEXT, NULL, NULL);
      SetWindowFont(GetDlgItem(hwnd,eID_Pic_NEXT), controlFont_48);
      
      CreateWindow(TEXTBOX, L" ", NULL, 
                   GUI_PicViewer_Icon[1].rc.x, GUI_PicViewer_Icon[1].rc.y, 
                   GUI_PicViewer_Icon[1].rc.w, GUI_PicViewer_Icon[1].rc.h,          
                   hwnd, eID_Pic_MsgBOX, NULL, NULL);
      SendMessage(GetDlgItem(hwnd,eID_Pic_MsgBOX),TBM_SET_TEXTFLAG,0,DT_VCENTER|DT_CENTER|DT_BORDER|DT_BKGND);  
      
      CreateWindow(TEXTBOX, L"图片分辨率：", WS_VISIBLE, 
                   GUI_PicViewer_Icon[2].rc.x, GUI_PicViewer_Icon[2].rc.y, 
                   GUI_PicViewer_Icon[2].rc.w, GUI_PicViewer_Icon[2].rc.h,          
                   hwnd, eID_Pic_Res, NULL, NULL);
      SendMessage(GetDlgItem(hwnd,eID_Pic_Res),TBM_SET_TEXTFLAG,0,DT_VCENTER|DT_RIGHT|DT_BKGND);

      CreateWindow(TEXTBOX, L" ", WS_VISIBLE, 
                   GUI_PicViewer_Icon[3].rc.x, GUI_PicViewer_Icon[3].rc.y, 
                   GUI_PicViewer_Icon[3].rc.w, GUI_PicViewer_Icon[3].rc.h,          
                   hwnd, eID_Pic_Res_Value, NULL, NULL);
      SendMessage(GetDlgItem(hwnd,eID_Pic_Res_Value),TBM_SET_TEXTFLAG,0,DT_VCENTER|DT_CENTER|DT_BKGND);

      CreateWindow(TEXTBOX, L"缩放比例：", WS_VISIBLE, 
                   GUI_PicViewer_Icon[4].rc.x, GUI_PicViewer_Icon[4].rc.y, 
                   GUI_PicViewer_Icon[4].rc.w, GUI_PicViewer_Icon[4].rc.h,          
                   hwnd, eID_Pic_Scale, NULL, NULL);
      SendMessage(GetDlgItem(hwnd,eID_Pic_Scale),TBM_SET_TEXTFLAG,0,DT_VCENTER|DT_LEFT|DT_BKGND);

      CreateWindow(TEXTBOX, L" ", WS_VISIBLE, 
                   GUI_PicViewer_Icon[5].rc.x, GUI_PicViewer_Icon[5].rc.y, 
                   GUI_PicViewer_Icon[5].rc.w, GUI_PicViewer_Icon[5].rc.h,          
                   hwnd, eID_Pic_Scale_Value, NULL, NULL);
      SendMessage(GetDlgItem(hwnd,eID_Pic_Scale_Value),TBM_SET_TEXTFLAG,0,DT_VCENTER|DT_LEFT|DT_BKGND);


      CreateWindow(BUTTON, L"内部FLASH", BS_PUSHLIKE | BS_RADIOBOX ,
                   GUI_PicViewer_Icon[6].rc.x, GUI_PicViewer_Icon[6].rc.y, 
                   GUI_PicViewer_Icon[6].rc.w, GUI_PicViewer_Icon[6].rc.h,
                   hwnd, (1<<16)|eID_Pic_INTFLASH, NULL, NULL);
      CreateWindow(BUTTON, L"外部FLASH", BS_PUSHLIKE | BS_RADIOBOX ,
                   GUI_PicViewer_Icon[7].rc.x, GUI_PicViewer_Icon[7].rc.y, 
                   GUI_PicViewer_Icon[7].rc.w, GUI_PicViewer_Icon[7].rc.h,
                   hwnd, (1<<16)|eID_Pic_EXTFLASH, NULL, NULL);
      CreateWindow(BUTTON, L"SD卡", BS_PUSHLIKE | BS_RADIOBOX   ,
                   GUI_PicViewer_Icon[8].rc.x, GUI_PicViewer_Icon[8].rc.y, 
                   GUI_PicViewer_Icon[8].rc.w, GUI_PicViewer_Icon[8].rc.h,
                   hwnd, (1<<16)|eID_Pic_SDCARD, NULL, NULL);                   
      switch(PicViewer.cur_path)
      {
        case eID_Pic_INTFLASH:
        {
          SendMessage(GetDlgItem(hwnd, eID_Pic_INTFLASH),BM_SETSTATE,BST_CHECKED,0);
          break;
        }
        case eID_Pic_EXTFLASH:
        {
          SendMessage(GetDlgItem(hwnd, eID_Pic_EXTFLASH),BM_SETSTATE,BST_CHECKED,0);
          break;
        }    
        case eID_Pic_SDCARD:
        {
          SendMessage(GetDlgItem(hwnd, eID_Pic_SDCARD),BM_SETSTATE,BST_CHECKED,0);
          break;
        }        
      }      
      
      //PicViewer.mhdc_bk = CreateMemoryDC(SURF_SCREEN,800,480);
      PicViewer_Init();
      
      break;
    }
		case	WM_CTLCOLOR:
		{
			u16 id;
			id =LOWORD(wParam);
			if(id== eID_Pic_Res || id== eID_Pic_Res_Value || id== eID_Pic_Scale || id== eID_Pic_Scale_Value)
			{
				CTLCOLOR *cr;
				cr =(CTLCOLOR*)lParam;
				cr->TextColor =RGB888(255,255,255);//文字颜色（RGB888颜色格式)
				cr->BackColor =RGB888(0,0,0);//背景颜色（RGB888颜色格式)
//				cr->BorderColor =RGB888(255,0,0);//边框颜色（RGB888颜色格式)
				return TRUE;
			}
      else if(id == eID_Pic_INTFLASH || id == eID_Pic_EXTFLASH || id == eID_Pic_SDCARD)
      {
        CTLCOLOR *cr;
        cr =(CTLCOLOR*)lParam;
				if(SendMessage(GetDlgItem(hwnd,id),BM_GETSTATE,0,0)&BST_CHECKED)
        {
					cr->TextColor =RGB888(0,0,0);
         //设置背景颜色
					cr->BackColor =RGB888(215,61,50);
          //设置边框颜色
					cr->BorderColor =RGB888(0,0,0);
          return TRUE; 
				}
				else
				{
					cr->TextColor =RGB888(0,0,0);
					cr->BackColor =RGB888(50,205,50);
          cr->BorderColor =RGB888(0,0,0);
          return TRUE; 
        }
               
      }
			else
			{
				return FALSE;
			}
		}
    case PIC_OWN_MSG:
    {
      BOOL state = (BOOL)wParam;
      if(state == 1)
      {
        GUI_DEBUG("1");
        ShowWindow(GetDlgItem(hwnd, eID_Pic_INTFLASH), SW_SHOW); 
        ShowWindow(GetDlgItem(hwnd, eID_Pic_EXTFLASH), SW_SHOW);
        ShowWindow(GetDlgItem(hwnd, eID_Pic_SDCARD), SW_SHOW);      
      }
      else
      {
        GUI_DEBUG("2");
        ShowWindow(GetDlgItem(hwnd, eID_Pic_INTFLASH), SW_HIDE); 
        ShowWindow(GetDlgItem(hwnd, eID_Pic_EXTFLASH), SW_HIDE);
        ShowWindow(GetDlgItem(hwnd, eID_Pic_SDCARD), SW_HIDE);
        if(PicViewer.SecMenu_State == 1)
        {
          PicViewer.SecMenu_State = 0;
          SendMessage(PicViewer.mPicMenu, WM_CLOSE, NULL, NULL);  
        }          
      }
      break;
    }
    case WM_LBUTTONDOWN:
    {
      POINT pt;
      static int count = 0;//按键状态
      RECT rc_prev = {0, 240, 70, 70};
      RECT rc_next = {800-65, 240, 70, 70};
      pt.x =GET_LPARAM_X(lParam); //获得X坐标
      pt.y =GET_LPARAM_Y(lParam); //获得Y坐标
      if(!(PtInRect(&rc_prev, &pt) || PtInRect(&rc_next, &pt)))
      {
        if(count == 0)
        {
          count = 1;
          PicViewer.Menu_State = 1;
        }
        else
        {
          count = 0;
          PicViewer.Menu_State = 0;
        }
      }
      SendMessage(hwnd, PIC_OWN_MSG, PicViewer.Menu_State, NULL);
      break;
    }
      
    case WM_DRAWITEM:
    {
       DRAWITEM_HDR *ds;
       ds = (DRAWITEM_HDR*)lParam;
       switch(ds->ID)
       {
          case eID_Pic_Name:
          {              
             PicViewer_TBOX_OwnerDraw(ds);
             return TRUE;              
          } 
          case eID_Pic_EXIT:
          {
             PicViewer_ExitButton_OwnerDraw(ds);
             return TRUE;             
          }
          case eID_Pic_PREV:
          {
            PicViewer_Button_OwnerDraw(ds); //执行自绘制按钮
            return TRUE; 
          }            
          case eID_Pic_NEXT:
          {
            PicViewer_Button_OwnerDraw(ds); //执行自绘制按钮
            return TRUE; 
          }                      
       }
//       if(ds->ID == eID_Pic_INTFLASH || ds->ID == eID_Pic_EXTFLASH || ds->ID == eID_Pic_SDCARD)
//       {
//         
//         PicViewer_AButton_OwnerDraw(ds);//带透明度控件
//         return TRUE;
//       }
       break;
    }

    case WM_ERASEBKGND:
    {
      HDC hdc =(HDC)wParam;
      RECT rc =*(RECT*)lParam;
      WCHAR wbuf[128];
      switch(PicViewer.cur_type)
      {
        case eID_Pic_JPG:
        {
          GUI_DEBUG("JPG");
          Draw_Pic_JPG(PicViewer.pic_list[PicViewer.show_index]);
          BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, PicViewer.mhdc_pic, rc.x, rc.y, SRCCOPY);
          DeleteDC(PicViewer.mhdc_pic);
          break;
        }
        case eID_Pic_BMP:
        {
          SetBrushColor(hdc, MapRGB(hdc, 0, 0, 0));
          FillRect(hdc, &rc); 
          Draw_Pic_BMP(hdc, PicViewer.pic_list[PicViewer.show_index]);
          GUI_DEBUG("BMP");

          break;
        }
      }
      //
      
      x_mbstowcs_cp936(wbuf, PicViewer.pic_lcdlist[PicViewer.show_index], PICFILE_NAME_MAXLEN);
      SetWindowText(GetDlgItem(hwnd, eID_Pic_Name), wbuf); 
      x_wsprintf(wbuf, L"%d*%d", PicViewer.pic_width, PicViewer.pic_height);
      SetWindowText(GetDlgItem(hwnd, eID_Pic_Res_Value), wbuf); 
      x_wsprintf(wbuf, L"%d%% : %d%%", (int)(PicViewer.scale_x*100), (int)(PicViewer.scale_y*100));
      SetWindowText(GetDlgItem(hwnd, eID_Pic_Scale_Value), wbuf); 
      //x_mbstowcs_cp936(wbuf, PicViewer.pic_lcdlist[PicViewer.show_index], PICFILE_NAME_MAXLEN);
//      SetBrushColor(PicViewer.mhdc_bk, MapRGB(PicViewer.mhdc_bk, 0, 0, 0));
//      FillRect(PicViewer.mhdc_bk, &rc);
      //PIC_BMP_Draw_FS(hdc, 70, 70, PicViewer.pic_list[PicViewer.show_index], NULL); 
      //BitBlt(PicViewer.mhdc_bk, rc.x, rc.y, rc.w, rc.h, PicViewer.mhdc_pic, 0, 0, SRCCOPY);
      
      
      
      return TRUE;
    }
    case WM_TIMER:
    {
      ShowWindow(GetDlgItem(hwnd, eID_Pic_MsgBOX), SW_HIDE);
      break;
    }
    case WM_PAINT:
    {
      PAINTSTRUCT ps;
      HDC hdc;//屏幕hdc

      hdc = BeginPaint(hwnd, &ps);   
      SetPenColor(hdc, MapRGB(hdc, 105, 105, 105)); //设置颜色， 线条使用 PenColor。
      HLine(hdc, 0, 70, 800);
      EndPaint(hwnd, &ps);
      break;
    }
    case WM_NOTIFY:
    {
      u16 code, id;
      id  =LOWORD(wParam);//获取消息的ID码
      code=HIWORD(wParam);//获取消息的类型
      if(code == BN_CLICKED)
      { 
        switch(id)
        {
          case eID_Pic_NEXT:
          {
            PicViewer.show_index++;
            
            if(PicViewer.show_index < PicViewer.pic_nums)
              RedrawWindow(hwnd, NULL, RDW_ALLCHILDREN|RDW_ERASE|RDW_INVALIDATE);
            else
            {
              PicViewer.show_index--;
              SetWindowText(GetDlgItem(hwnd, eID_Pic_MsgBOX), L"此照片已经是最后一张了");
              SetTimer(hwnd,1,1000,TMR_START|TMR_SINGLE,NULL);
              ShowWindow(GetDlgItem(hwnd, eID_Pic_MsgBOX), SW_SHOW);    
            }
            break;
          }
          case eID_Pic_PREV:
          {
            PicViewer.show_index--;
            if(PicViewer.show_index >= 0)
              RedrawWindow(hwnd, NULL, RDW_ALLCHILDREN|RDW_ERASE|RDW_INVALIDATE);
            else
            {
              PicViewer.show_index++;
              SetWindowText(GetDlgItem(hwnd, eID_Pic_MsgBOX), L"此照片已经是第一张了");
              SetTimer(hwnd,1,1000,TMR_START|TMR_SINGLE,NULL);
              ShowWindow(GetDlgItem(hwnd, eID_Pic_MsgBOX), SW_SHOW);    
            }
            //InvalidateRect(hwnd, NULL, TRUE);
            break;
          }
          case eID_Pic_EXIT:
          {
            PostCloseMessage(hwnd);
            break;
          }           
        }
        
      }
      
			if(id==eID_Pic_SDCARD&& code==BN_CLICKED)
			{
        NMHDR *nr=(NMHDR*)lParam;
        PicViewer.cur_path = eID_Pic_SDCARD;
        if(SendMessage(nr->hwndFrom,BM_GETSTATE,0,0)&BST_CHECKED&&PicViewer.SecMenu_State == 0) //获取当前状态
        { //复选框选中.
          WNDCLASS wcex;

          wcex.Tag	 		= WNDCLASS_TAG;
          wcex.Style			= CS_HREDRAW | CS_VREDRAW;
          wcex.lpfnWndProc	= (WNDPROC)DlgType_proc;
          wcex.cbClsExtra		= 0;
          wcex.cbWndExtra		= 0;
          wcex.hInstance		= NULL;
          wcex.hIcon			= NULL;
          wcex.hCursor		= NULL;
          PicViewer.SecMenu_State = 1;
          PicViewer.mPicMenu = CreateWindowEx(WS_EX_FRAMEBUFFER,
                                &wcex,L"SDCARD",
                                WS_OVERLAPPED|WS_CLIPCHILDREN|WS_VISIBLE,
                                500,220,300,200,
                                hwnd,0,NULL,NULL);
          
        }                        
			}
			if(id==eID_Pic_INTFLASH&& code==BN_CLICKED)
			{
        PicViewer.cur_path = eID_Pic_INTFLASH;
        NMHDR *nr=(NMHDR*)lParam;
        if(SendMessage(nr->hwndFrom,BM_GETSTATE,0,0)&BST_CHECKED) //获取当前状态
        { //复选框选中.
          if(PicViewer.SecMenu_State == 1)
          {
            //ShowWindow(PicViewer.mPicMenu, SW_HIDE);
            SendMessage(PicViewer.mPicMenu, WM_CLOSE, NULL, NULL);
            PicViewer.SecMenu_State = 0;
          }
        }                        
			}
			if(id==eID_Pic_EXTFLASH&& code==BN_CLICKED)
			{
        NMHDR *nr=(NMHDR*)lParam;
        PicViewer.cur_path = eID_Pic_EXTFLASH;
        if(SendMessage(nr->hwndFrom,BM_GETSTATE,0,0)&BST_CHECKED) //获取当前状态
        { //复选框选中.
          if(PicViewer.SecMenu_State == 1)
          {
            GUI_DEBUG("%d",SendMessage(PicViewer.mPicMenu, WM_CLOSE, NULL, NULL));
            //ShowWindow(PicViewer.mPicMenu, SW_HIDE);
            //SendMessage(PicViewer.mPicMenu, WM_CLOSE, NULL, NULL);
            PicViewer.SecMenu_State = 0;
          }
        }                        
			}      
      break;
    }
    case WM_DESTROY:
    {
      PicViewer_Quit();
      
      return PostQuitMessage(hwnd);	
    }
    default:
      return	DefWindowProc(hwnd, msg, wParam, lParam);
  }
	return	WM_NULL;
}


void GUI_PicViewer_DIALOG(void)
{
	//HWND	hwnd;
	WNDCLASS	wcex;
	MSG msg;


	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = PicViewer_proc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);
   
	//创建主窗口
	PicViewer.mPicViewer = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,
                        &wcex,
                        L"GUI_LED_DIALOG",
                        WS_CLIPCHILDREN,
                        0, 0, GUI_XSIZE, GUI_YSIZE,
                        NULL, NULL, NULL, NULL);
   //显示主窗口
	ShowWindow(PicViewer.mPicViewer, SW_SHOW);
	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while (GetMessage(&msg, PicViewer.mPicViewer))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

}


#include <stdio.h>
#include "emXGUI.h"
#include "x_libc.h"
#include "GUI_AppDef.h"
#include "emxgui_png.h"



/**********************分界线*********************/

/* 各类控件ID */
enum eID
{
   ID_LOGO,
   ID_TEXT1,
   ID_TEXT2,
   ID_PROGBAR,
};
/* 外部图片数据 */
extern char bootlogo[];
/* 外部图片数据大小 */
extern unsigned int bootlogo_size(void);
HWND Boot_progbar;
/**
  * @brief  烧录应用线程
  */


/**
  * @brief  烧录应用回调函数
  */
static	LRESULT	win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   static  BITMAP png_bm;
   static  PNG_DEC *png_dec; 
   PROGRESSBAR_CFG cfg;
	switch (msg)
	{  
      case WM_CREATE:
      {
         RECT rc;
         
			GetClientRect(hwnd,&rc); //获得窗口的客户区矩形

         /* 根据图片数据创建PNG_DEC句柄 */
         png_dec = PNG_Open((u8 *)bootlogo, bootlogo_size());
         /* 把图片转换成bitmap */
         PNG_GetBitmap(png_dec, &png_bm);
         CreateWindow(TEXTBOX, L"emXGUI booting...", WS_VISIBLE, 
                      50,260,800,40,
                      hwnd, ID_TEXT1, NULL, NULL);
         SendMessage(GetDlgItem(hwnd, ID_TEXT1),TBM_SET_TEXTFLAG,0,
                        DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_BKGND); 
         
         CreateWindow(TEXTBOX, L"copying FontLIB form SPIFALSH to SDRAM...", WS_VISIBLE, 
                      50,300,800,40,
                      hwnd, ID_TEXT2, NULL, NULL);
         SendMessage(GetDlgItem(hwnd, ID_TEXT2),TBM_SET_TEXTFLAG,0,
                        DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_BKGND); 


         //PROGRESSBAR_CFG结构体的大小
         cfg.cbSize	 = sizeof(PROGRESSBAR_CFG);
         //开启所有的功能
         cfg.fMask    = PB_CFG_ALL;
         //文字格式水平，垂直居中
         cfg.TextFlag = DT_VCENTER|DT_CENTER;  

         Boot_progbar = CreateWindow(PROGRESSBAR,L"",
                       PBS_TEXT|PBS_ALIGN_LEFT|WS_VISIBLE,
                       50, 380, 700, 40 ,hwnd,ID_PROGBAR,NULL,NULL);

         SendMessage(Boot_progbar,PBM_GET_CFG,TRUE,(LPARAM)&cfg);
			SendMessage(Boot_progbar,PBM_SET_CFG,TRUE,(LPARAM)&cfg);
         SendMessage(Boot_progbar,PBM_SET_VALUE,TRUE,0);         
         break;
      }
      case WM_ERASEBKGND:
      {
         HDC hdc =(HDC)wParam;
         RECT rc =*(RECT*)lParam;
         
         SetBrushColor(hdc, MapRGB(hdc, 0, 0, 0));
         FillRect(hdc, &rc);           
         /* 显示图片 */
         DrawBitmap(hdc, 250, 80, &png_bm, NULL);     
         return TRUE;
         
      }

		      
		case	WM_CTLCOLOR:
		{
			u16 id;
         CTLCOLOR *cr;
			id =LOWORD(wParam);				
         cr =(CTLCOLOR*)lParam;
			if(id == ID_TEXT1 || id == ID_TEXT2)
			{

				cr->TextColor =RGB888(255,255,255);//文字颜色（RGB888颜色格式)
				cr->BackColor =RGB888(0,0,0);//背景颜色（RGB888颜色格式)
				//cr->BorderColor =RGB888(255,10,10);//边框颜色（RGB888颜色格式)
				return TRUE;
			}
         
         break;
		}  
		case WM_DESTROY: //窗口销毁时，会自动产生该消息，在这里做一些资源释放的操作.
		{
         /* 关闭PNG_DEC句柄 */
         PNG_Close(png_dec);
      
			return PostQuitMessage(hwnd); //调用PostQuitMessage，使用主窗口结束并退出消息循环.
		}      
      default:
         return	DefWindowProc(hwnd, msg, wParam, lParam);
      }
      return	WM_NULL;                                     
}

HWND GUI_Boot_hwnd;
void	GUI_Boot_Interface_DIALOG(void)
{

	WNDCLASS	wcex;
	HWND hwnd;
	MSG msg;

	wcex.Tag 		    = WNDCLASS_TAG;

	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= win_proc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;//hInst;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;//LoadCursor(NULL, IDC_ARROW);

	//创建桌面窗口.
	hwnd = GUI_CreateDesktop(	WS_EX_LOCKPOS,
                              &wcex,
                              L"DESKTOP",
                              WS_VISIBLE|WS_CLIPCHILDREN,
                              0,0,GUI_XSIZE,GUI_YSIZE,
                              NULL,0,NULL,NULL);

	GUI_Printf("HWND_Desktop=%08XH\r\n",	hwnd);

	//显示桌面窗口.
	ShowWindow(hwnd,SW_SHOW);

	//设置系统打开光标显示(可以按实际情况看是否需要).
//	ShowCursor(TRUE);

	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}


}

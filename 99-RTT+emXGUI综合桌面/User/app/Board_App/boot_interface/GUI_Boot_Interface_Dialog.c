
#include <stdio.h>
#include "emXGUI.h"
#include "x_libc.h"
#include "GUI_AppDef.h"
#include "emxgui_png.h"



/**********************分界线*********************/

/* 各类控件ID */
#define ID_LOGO            0x3000
#define ID_TEXT            0x3001
/* 外部图片数据 */
extern char bootlogo[];
/* 外部图片数据大小 */
extern unsigned int bootlogo_size(void);

/**
  * @brief  烧录应用线程
  */
static void App_FLASH_Writer(void )
{
  static int thread=0;
	static rt_thread_t h_flash;
  
   //HDC hdc;
   
	if(thread==0)
	{  
      h_flash=rt_thread_create("Flash writer",(void(*)(void*))App_FLASH_Writer,NULL,5*1024,1,5);
      thread =1;
      rt_thread_startup(h_flash);//启动线程
      return;
	}
	while(thread) //线程已创建了
	{     
    

    thread = 0;       

    rt_thread_delete(h_flash);

	}
  return;
}

/**
  * @brief  烧录应用回调函数
  */
static	LRESULT	win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   static  BITMAP png_bm;
   static  PNG_DEC *png_dec; 
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
         //SendMessage(GetDlgItem(hwnd, ID_LOGO),TBM_SET_TEXTFLAG,0,DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND);           
         break;
      }
      case WM_ERASEBKGND:
      {
         HDC hdc =(HDC)wParam;
         RECT rc =*(RECT*)lParam;
         
         SetBrushColor(hdc, MapRGB(hdc, 0, 0, 0));
         FillRect(hdc, &rc);      
         return TRUE;
         
      }
		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{
			PAINTSTRUCT ps;
			HDC hdc;
//			WCHAR wbuf[128];
      
			hdc =BeginPaint(hwnd,&ps);


         /* 显示图片 */
         DrawBitmap(hdc, 0, 0, &png_bm, NULL);          


			EndPaint(hwnd,&ps);
         break;
		}
		      
//		case	WM_CTLCOLOR:
//		{
//			u16 id;
//         CTLCOLOR *cr;
//			id =LOWORD(wParam);				
//         cr =(CTLCOLOR*)lParam;
//			if(id == ID_LOGO)
//			{

//				cr->TextColor =RGB888(255,255,255);//文字颜色（RGB888颜色格式)
//				cr->BackColor =RGB888(0,0,0);//背景颜色（RGB888颜色格式)
//				//cr->BorderColor =RGB888(255,10,10);//边框颜色（RGB888颜色格式)
//				return TRUE;
//			}
//         
//         break;
//		}  
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


void	GUI_Boot_Interface_DIALOG(void)
{
	HWND	hwnd;
	WNDCLASS	wcex;
	MSG msg;

  /* 重设默认字体为ASCII 内部flash字库，防止擦除时出错 */
  GUI_SetDefFont(defaultFontEn);  //设置默认的字体

	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = win_proc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);
	//创建主窗口
	hwnd = CreateWindowEx(WS_EX_FRAMEBUFFER,
                        &wcex,
                        L"GUI Boot Interface",
                        WS_CLIPCHILDREN,
                        0, 0, GUI_XSIZE, GUI_YSIZE,
                        NULL, NULL, NULL, NULL);
   //显示主窗口
	ShowWindow(hwnd, SW_SHOW);
	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while (GetMessage(&msg, hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}


}

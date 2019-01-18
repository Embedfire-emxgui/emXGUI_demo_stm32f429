
#include <stdio.h>
#include "emXGUI.h"
#include "x_libc.h"
#include "GUI_AppDef.h"




/**********************分界线*********************/

/* 各类控件ID */
#define ID_LOGO            0x3000
#define ID_TEXT            0x3001

static COLORREF logo_col[4]={RGB888(169,169,169), RGB888(96,0,7), RGB888(220,20,60), RGB888(255,0,0)};

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
   static int col = 0;//选择颜色
	switch (msg)
	{  
      
      case WM_CREATE:
      {
         CreateWindow(TEXTBOX, L"B", WS_VISIBLE, 0,100,800,280,        
                      hwnd, ID_LOGO, NULL, NULL);
         SendMessage(GetDlgItem(hwnd, ID_LOGO),TBM_SET_TEXTFLAG,0,
                        DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND);   
         SetWindowFont(GetDlgItem(hwnd, ID_LOGO), logoFont_200);
         
         CreateWindow(TEXTBOX, L"tuvwxyz", WS_VISIBLE, 0,380,800,100,        
                      hwnd, ID_TEXT, NULL, NULL);
         SendMessage(GetDlgItem(hwnd, ID_TEXT),TBM_SET_TEXTFLAG,0,
                        DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND);   
         SetWindowFont(GetDlgItem(hwnd, ID_TEXT), controlFont_32);   

         SetTimer(hwnd,1, 500, TMR_START,NULL);   
         break;
      }
      case WM_TIMER:
      {
         static int state = -1;
         state++;
         switch(state)
         {
            case 0:
            {
               col = 1;
               InvalidateRect(GetDlgItem(hwnd, ID_LOGO), NULL, FALSE);
               SetWindowText(GetDlgItem(hwnd, ID_TEXT), L"tuvwxyz.");
               break;
            }
            case 1:
            {
               col = 2;
               InvalidateRect(GetDlgItem(hwnd, ID_LOGO), NULL, FALSE);
               SetWindowText(GetDlgItem(hwnd, ID_TEXT), L"tuvwxyz..");
               break;
            }
            case 2:
            { 
               col = 3;
               state = -1;
               InvalidateRect(GetDlgItem(hwnd, ID_LOGO), NULL, FALSE);
               SetWindowText(GetDlgItem(hwnd, ID_TEXT), L"tuvwxyz...");
               break;
            }          
         }
         
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
      case WM_CTLCOLOR:
      {
         u16 id;
         id =LOWORD(wParam);         
         CTLCOLOR *cr;
         cr =(CTLCOLOR*)lParam;
         
         switch(id)
         {
            case ID_LOGO:
            {
               cr->TextColor = logo_col[col];
               cr->BackColor = RGB888(0,0,0);            
               return TRUE;               
            }
            break;
            case ID_TEXT:
            {
               cr->TextColor = RGB888(169,169,169);
               cr->BackColor = RGB888(0,0,0);            
               return TRUE;               
            }            
         }
         break;
      }
      case	WM_PAINT: //窗口需要重绘制时，会自动收到该消息.
      {	
         PAINTSTRUCT ps;
   //      HDC hdc;//屏幕hdc
   //      hdc = BeginPaint(hwnd, &ps); 
       BeginPaint(hwnd, &ps); 

         EndPaint(hwnd, &ps);
         return	TRUE;
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

  /* 恢复中文默认字体 */
  GUI_SetDefFont(defaultFont);  

}

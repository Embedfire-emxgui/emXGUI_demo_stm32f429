
#include <stdio.h>
#include "emXGUI.h"
#include "x_libc.h"
#include "./led/bsp_led.h"  
#include "GUI_AppDef.h"



/**********************分界线*********************/

/*滑动条ID宏定义*/
#define ID_SCROLLBAR_R  0x1100
#define ID_SCROLLBAR_G  0x1101
#define ID_SCROLLBAR_B  0x1102

/*复选框ID宏定义*/
#define ID_TEXTBOX_R	      0x1010
#define ID_TEXTBOX_R_NUM	0x1013
#define ID_TEXTBOX_G	      0x1011
#define ID_TEXTBOX_G_NUM   0x1014
#define ID_TEXTBOX_B	      0x1012
#define ID_TEXTBOX_B_NUM   0x1015

#define ID_EXIT            0x3000
#define ID_BURN           0x3001
#define ID_RESET           0x3002


static	LRESULT	win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	RECT rc;
	HWND wnd;

	//static RECT rc_R, rc_G, rc_B;//RGB分量指示框

   //HDC hdc_mem2pic;
	switch (msg)
	{
    case WM_CREATE: {

    rc.x = 40;
    rc.y = 40;
    rc.w = 250;
    rc.h = 70;
  
    CreateWindow(BUTTON, L"Burn File to FLASH",WS_VISIBLE,
                  rc.x, rc.y, rc.w, rc.h, hwnd, ID_BURN, NULL, NULL); 
  
    OffsetRect(&rc,rc.w+50,0);  
    CreateWindow(BUTTON, L"Reset System",WS_VISIBLE,
                  rc.x, rc.y, rc.w, rc.h, hwnd, ID_RESET, NULL, NULL); 



      break;
	}
 
	case WM_NOTIFY: {
      NMHDR *nr;
      WCHAR wbuf[128];
      u16 ctr_id; 
      u16 code,  id;
      id  =LOWORD(wParam);//获取消息的ID码
      code=HIWORD(wParam);//获取消息的类型
      ctr_id = LOWORD(wParam); //wParam低16位是发送该消息的控件ID.
      nr = (NMHDR*)lParam; //lParam参数，是以NMHDR结构体开头.
//		if(id == ID_EXIT && code == BN_CLICKED)
//      {
//         PostCloseMessage(hwnd);
//      }
      
      if(id == ID_BURN && code == BN_CLICKED)
      {
         BurnFile();
      }
      
      if(id == ID_RESET && code == BN_CLICKED)
      {
         NVIC_SystemReset();
      }      
      
      
		break;
	}
	case WM_DRAWITEM:
	{
		DRAWITEM_HDR *ds;
		ds = (DRAWITEM_HDR*)lParam;

//      if(ds->ID == ID_EXIT)
//      {
//			exit_owner_draw(ds);
//			return TRUE;      
//      }

	}
//   case	WM_CTLCOLOR:
//   {
//      /* 控件在绘制前，会发送 WM_CTLCOLOR到父窗口.
//       * wParam参数指明了发送该消息的控件ID;lParam参数指向一个CTLCOLOR的结构体指针.
//       * 用户可以通过这个结构体改变控件的颜色值.用户修改颜色参数后，需返回TRUE，否则，系统
//       * 将忽略本次操作，继续使用默认的颜色进行绘制.
//       *
//       */
//      u16 id;
//      id =LOWORD(wParam);         
//      CTLCOLOR *cr;
//      cr =(CTLCOLOR*)lParam;

//      
//      return FALSE;
//      
//   }   
   case WM_ERASEBKGND:
   {
      HDC hdc =(HDC)wParam;
      RECT rc;
      RECT rc_text = {0, 0, 100, 40};
      GetClientRect(hwnd, &rc);
      
      SetBrushColor(hdc, MapRGB(hdc, 0, 0, 0));
      FillRect(hdc, &rc);
      
//      SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
//      DrawText(hdc, L"FLASH Writer", -1, &rc_text, DT_VCENTER);
      
      return TRUE;
      
   }
/*由文字框，颜色指示框，复选框（开关），颜色分量值，滑动条控件组成；
 *位置关系：
 *	  文字框     颜色      颜色			滑
 *										      动
 *	  复选框    指示框	    分量值	   条
*/ 
	case	WM_PAINT: //窗口需要重绘制时，会自动收到该消息.
	{	
      PAINTSTRUCT ps;
      HDC hdc, hdc_mem, hdc_mem1;//屏幕hdc
      RECT rc = {0,0,72,72};
      RECT rc_cli = {0,0,72,72};
      GetClientRect(hwnd, &rc_cli);
      hdc = BeginPaint(hwnd, &ps); 
//      hdc_mem = CreateMemoryDC(SURF_SCREEN, 72, 72);
//      hdc_mem1 = CreateMemoryDC(SURF_SCREEN, 72, 72);
//      
//      
//      /****************返回主界面按钮******************/
//      SetBrushColor(hdc, MapRGB(hdc, 0,0,0));
//      FillCircle(hdc, rc_cli.w, 0, 80);  
//      
//      SetBrushColor(hdc, MapRGB(hdc, 250,0,0));
//      FillCircle(hdc, rc_cli.w, 0, 76); 
//      //字体层
//      SetBrushColor(hdc_mem1, MapRGB(hdc, 250,0,0));
//      FillRect(hdc_mem1, &rc);        
//      
//      SetFont(hdc, controlFont_72);
//      SetTextColor(hdc, MapRGB(hdc, 250, 250,250));
//      TextOut(hdc, 742, -10, L"O", -1);

////      StretchBlt(hdc, 755, 12, 40, 40, 
////                 hdc_mem1, 0, 0, 72, 72, SRCCOPY);

//      DeleteDC(hdc_mem);
//      DeleteDC(hdc_mem1);
		EndPaint(hwnd, &ps);
		return	TRUE;
	}
	default:
		return	DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return	WM_NULL;
}


void	GUI_RES_WRITER_DIALOG(void)
{
	HWND	hwnd;
	WNDCLASS	wcex;
	MSG msg;


	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = win_proc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);
	//创建主窗口
	hwnd = CreateWindowEx(NULL,
                        &wcex,
                        L"GUI FLASH Writer",
                        WS_OVERLAPPEDWINDOW,
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

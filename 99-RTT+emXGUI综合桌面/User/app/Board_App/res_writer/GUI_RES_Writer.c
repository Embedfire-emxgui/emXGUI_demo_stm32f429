
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

static void exit_owner_draw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button的窗口句柄.
	hdc = ds->hDC;   //button的绘图上下文句柄.
	rc = ds->rc;     //button的绘制矩形区.
   
	SetBrushColor(hdc, MapRGB(hdc, 0,0,0));   
   FillRect(hdc, &rc); //用矩形填充背景
	SetBrushColor(hdc, MapRGB(hdc, COLOR_DESKTOP_BACK_GROUND));
   
   FillCircle(hdc, rc.x+rc.w, rc.y, rc.w);
	//

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

	GetWindowText(ds->hwnd, wbuf, 128); //获得按钮控件的文字
   rc.y = -10;
   rc.x = 16;
	DrawText(hdc, wbuf, -1, &rc, NULL);//绘制文字(居中对齐方式)


  /* 恢复默认字体 */
	SetFont(hdc, defaultFont);

}

static	LRESULT	win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
//	RECT rc;
	HWND wnd;

	//static RECT rc_R, rc_G, rc_B;//RGB分量指示框

   //HDC hdc_mem2pic;
	switch (msg)
	{
	case WM_CREATE: {
		/*设置滑动条的参数*/
		SCROLLINFO sif;
      
		sif.cbSize = sizeof(sif);
		sif.fMask = SIF_ALL;
		sif.nMin = 0;
		sif.nMax = 255;
		sif.nValue = 0;
		sif.TrackSize = 30;
		sif.ArrowSize = 0;//20;


      CreateWindow(BUTTON, L"O",WS_OWNERDRAW|WS_VISIBLE,
                        730, 0, 70, 70, hwnd, ID_EXIT, NULL, NULL); 
    
      CreateWindow(BUTTON, L"Burn File to FLASH",WS_VISIBLE,
                    40, 40, 250, 70, hwnd, ID_BURN, NULL, NULL); 


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
		if(id == ID_EXIT && code == BN_CLICKED)
      {
         PostCloseMessage(hwnd);
      }
      
    if(id == ID_BURN )
      {
         BurnFile();
      }
      
      
		break;
	}
	case WM_DRAWITEM:
	{
		DRAWITEM_HDR *ds;
		ds = (DRAWITEM_HDR*)lParam;

      if(ds->ID == ID_EXIT)
      {
			exit_owner_draw(ds);
			return TRUE;      
      }

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
      
      SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
      DrawText(hdc, L"FLASH Writer", -1, &rc_text, DT_VCENTER);
      
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
                        L"GUI_FLASH_WRITER_DIALOG",
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

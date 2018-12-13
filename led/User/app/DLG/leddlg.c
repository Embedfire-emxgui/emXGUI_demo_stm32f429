
#include "emXGUI.h"

/*===================================================================================*/
static	LRESULT	win_proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{			
		default:
				return	DefWindowProc(hwnd,msg,wParam,lParam);
	}

}

void GUI_LEDDLG(void)
{
   WNDCLASS wcex;
   HWND hwnd;
   MSG msg;
     
   wcex.Tag   = WNDCLASS_TAG;
   wcex.Style = CS_HREDRAW | CS_VREDRAW;   
   wcex.cbWndExtra = 0;
   wcex.cbClsExtra = 0;
   wcex.lpfnWndProc = win_proc;
   wcex.hInstance = NULL;
   wcex.hIcon = NULL;
   wcex.hCursor = NULL;

   
   
   hwnd = CreateWindowEx(NULL, &wcex, L"LEDDLG",
                            WS_CLIPCHILDREN, 0, 0,
                            GUI_XSIZE, GUI_YSIZE,
                            NULL, NULL, NULL, NULL);
   
   ShowWindow(hwnd,SW_SHOW);
   
   while(GetMessage(&msg, hwnd))
   {
      TranslateMessage(&msg);
      DispatchMessage(&msg);   
   
   }

}


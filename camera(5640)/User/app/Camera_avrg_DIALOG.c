#include <emXGUI.h>

static LRESULT WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch(msg)
   {
      case WM_CREATE:
      {
         CreateWindow(BUTTON, L"L", BS_FLAT | BS_NOTIFY  |WS_VISIBLE,
                      0, 0, 70, 70, hwnd, 0x1000, NULL, NULL);         
         
         
         break;
      }
      case WM_ERASEBKGND:
      {
         HDC hdc = (HDC)wParam;
         RECT rc_cli;
         GetClientRect(hwnd, &rc_cli);
         SetBrushColor(hdc, MapRGB(hdc, 0, 0, 0));
         FillRect(hdc, &rc_cli);
       
         break;
      }
      default:
         return DefWindowProc(hwnd, msg, wParam, lParam);
   }
   return WM_NULL;
}
void	GUI_CameraAvrg_DIALOG(void)
{	
	WNDCLASS	wcex;
	MSG msg;
   HWND hwnd;

	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WinProc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);

	//创建主窗口
	hwnd = CreateWindowEx(WS_EX_NOFOCUS,
                                    &wcex,
                                    L"GUI_CameraAvrg_DIALOG",
                                    WS_VISIBLE,
                                    0, 0, 800, GUI_YSIZE,
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



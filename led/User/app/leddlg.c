#include "emXGUI.h"
/*控件参数*/
//退出按键的宽高
#define EXIT_H		 (80)
#define EXIT_W		 (80)
#define EXIT_POS_X	 (GUI_XSIZE - EXIT_W)	
#define ID_EXIT		 0x1000

static void button_owner_draw(DRAWITEM_HDR *ds)
{
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];
	hdc = ds->hDC;   //button的绘图上下文句柄.
	rc = ds->rc;     //button的绘制矩形区.

	if (ds->State & BST_PUSHED)
	{ //按钮是按下状态
		SetBrushColor(hdc, MapRGB(hdc, 150, 200, 250)); //设置填充色(BrushColor用于所有Fill类型的绘图函数)
		SetPenColor(hdc, MapRGB(hdc, 250, 0, 0));        //设置绘制色(PenColor用于所有Draw类型的绘图函数)
		SetTextColor(hdc, MapRGB(hdc, 250, 0, 0));      //设置文字色
	}
	else
	{ //按钮是弹起状态
		SetBrushColor(hdc, MapRGB(hdc, 30, 150, 30));
		SetPenColor(hdc, MapRGB(hdc, 0, 250, 0));
		SetTextColor(hdc, MapRGB(hdc, 0, 50, 100));
	}
	FillRect(hdc, &rc); //用矩形填充背景
	DrawRect(hdc, &rc); //画矩形外框
	GetWindowText(ds->hwnd, wbuf, 128); //获得按钮控件的文字
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER | DT_CENTER);//绘制文字(居中对齐方式)

}


static LRESULT win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	RECT rc_cli;
	switch (msg)
	{
	case WM_CREATE:
	{
		GetClientRect(hwnd, &rc_cli);
		CreateWindow(BUTTON, L"×", WS_VISIBLE|WS_OWNERDRAW, EXIT_POS_X, 0, 80, 80, hwnd, ID_EXIT, NULL, NULL);
	}
	case WM_DRAWITEM:
	{
		DRAWITEM_HDR *ds;
		ds = (DRAWITEM_HDR*)lParam;
		if (ds->ID == ID_EXIT)
		{
			button_owner_draw(ds); //执行自绘制按钮
		}
		return TRUE;

	}
	default: 
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

}


void	GUI_LED_DIALOG(void)
{
	HWND	hwnd;
	WNDCLASS	wcex;
	MSG msg;

	/////
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
		L"GUI_LED_DIALOG",
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

#include "emXGUI.h"

#include <stdio.h>
#define ID_LISTBOX1    0x2100
#define ID_LISTBOX2    0x2101


extern char timg[];
/*============================================================================*/
static BITMAP bm;
static HDC hdc_mem;
static int hdc_mem_tmp = 0;
static void _listbox_owner_draw_x(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
	RECT rc, rc_cli;
	int i, count;
	hwnd = ds->hwnd;
	hdc = ds->hDC;
	GetClientRect(hwnd, &rc_cli);
	RECT rc1 = { 5, 0, 40,58 };//文字
	WCHAR wbuf[128];
	GetClientRect(hwnd, &rc_cli);
	if(hdc_mem_tmp == 0)
	{
		hdc_mem_tmp = 1;
		hdc_mem = CreateMemoryDC(SURF_SCREEN, rc_cli.w, rc_cli.h);
	}
	SetBrushColor(hdc_mem, MapRGB(hdc_mem, 0, 0, 0));
	FillRect(hdc_mem, &rc_cli);

	//设置位图结构参数
	bm.Format = BM_RGB888;     //位图格式
	bm.Width = 72;              //宽度
	bm.Height = 58;             //高度
	bm.WidthBytes = bm.Width * 3; //每行字节数
	bm.LUT = NULL;                //查找表(RGB/ARGB格式不使用该参数)
	bm.Bits = (void*)timg;    //位图数据
	
	

	SetTextColor(hdc_mem, MapRGB(hdc_mem, 255, 255, 255));
	
	//DrawText(hdc_mem, wbuf, -1, &rc1, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	
	
	//获取当前的第一项
	i = SendMessage(hwnd, LB_GETTOPINDEX, 0, 0);
	//获取列表项目总数
	count = SendMessage(hwnd, LB_GETCOUNT, 0, 0);
	while (i < count)
	{
		SendMessage(hwnd, LB_GETITEMRECT, i, (LPARAM)&rc);
		

		DrawBitmap(hdc_mem, 50, (rc.y + 2), &bm, NULL);

		rc1.x = rc.x + 5;
		rc1.y = rc.y + 2;
		rc1.w = 40;
		rc1.h = 58;
		if (rc.y > rc_cli.h)
		{
			break;
		}
		SendMessage(hwnd, LB_GETTEXT, i, (LPARAM)wbuf);
		DrawText(hdc_mem, wbuf, -1, &rc1, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		i++;
	}
	
	BitBlt(hdc, 0, 0, rc_cli.w, rc_cli.h, hdc_mem, 0, 0, SRCCOPY);
}

static LRESULT WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND wnd1, wnd2;
	int i = 0;
	switch (msg)
	{
	case WM_CREATE:
	{
		wnd1 = CreateWindow(LISTBOX, _T("Listbox"), WS_OWNERDRAW |  LBS_LINE | LBS_NOTIFY | WS_VISIBLE,
			0, 80, 400, 400, hwnd, ID_LISTBOX1, NULL, NULL);
		//设置列表项的内容
		SendMessage(wnd1, LB_ADDSTRING, 0, (LPARAM)L"01");
		SendMessage(wnd1, LB_ADDSTRING, 1, (LPARAM)L"03");
		SendMessage(wnd1, LB_ADDSTRING, 2, (LPARAM)L"05");
		SendMessage(wnd1, LB_ADDSTRING, 3, (LPARAM)L"07");
		SendMessage(wnd1, LB_ADDSTRING, 4, (LPARAM)L"09");
		SendMessage(wnd1, LB_ADDSTRING, 5, (LPARAM)L"11");
		for(i = 0; i < 6; i++)
			SendMessage(wnd1, LB_SETITEMHEIGHT, i, 58);
		wnd2 = CreateWindow(LISTBOX, _T("Listbox"), WS_OWNERDRAW | LBS_LINE | LBS_NOTIFY | WS_VISIBLE |WS_DISABLED,
			400, 80, 400, 400, hwnd, ID_LISTBOX2, NULL, NULL);
		//设置列表项的内容
		SendMessage(wnd2, LB_ADDSTRING, 6, (LPARAM)L"02");
		SendMessage(wnd2, LB_ADDSTRING, 7, (LPARAM)L"04");
		SendMessage(wnd2, LB_ADDSTRING, 8, (LPARAM)L"06");
		SendMessage(wnd2, LB_ADDSTRING, 9, (LPARAM)L"08");
		SendMessage(wnd2, LB_ADDSTRING, 10, (LPARAM)L"10");
		SendMessage(wnd2, LB_ADDSTRING, 11, (LPARAM)L"12");
		for (i = 0; i < 6; i++)
			SendMessage(wnd2, LB_SETITEMHEIGHT, i, 58);

		
		break;
	}
	case	WM_DRAWITEM:
	{
		DRAWITEM_HDR *ds;
		ds = (DRAWITEM_HDR*)lParam;

		if (wParam == ID_LISTBOX1 || wParam == ID_LISTBOX2)
		{
			_listbox_owner_draw_x(ds);
			return TRUE;
		}
		return FALSE;
	}
	case WM_NOTIFY:
	{
		NMHDR *nr;
		
		nr = (NMHDR*)lParam;
		if (nr->idFrom == ID_LISTBOX2 && nr->code == LBN_SELCHANGE)
		{
			int i;
			i = SendMessage(nr->hwndFrom, LB_GETCURSEL, 0, 0);
			printf("%d\n", i);
		}
		if (nr->idFrom == ID_LISTBOX1 && nr->code == LBN_SELCHANGE)
		{
			int i;
			i = SendMessage(nr->hwndFrom, LB_GETCURSEL, 0, 0);
			printf("%d\n", i);
		}		
		if (nr->idFrom == ID_LISTBOX1 && nr->code == LBN_POSCHANGE) {
			printf("WM——\n");
			int t;
			t = SendMessage(wnd1, LB_GETPOS, 0, 0);
			SendMessage(wnd2, LB_SETPOS, 0, t);
			InvalidateRect(wnd2, NULL, TRUE);
		}
		if (nr->idFrom == ID_LISTBOX2 && nr->code == LBN_POSCHANGE){
			int it;
			it = SendMessage(nr->hwndFrom, LB_GETPOS, 0, 0);
			SendMessage(wnd1, LB_SETPOS, 0, i);
			InvalidateRect(wnd1, NULL,TRUE);
		}
		break;
	}

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		RECT rc;
		GetClientRect(hwnd, &rc);
		
		SetBrushColor(hdc, MapRGB(hdc, 0, 0, 0));
		FillRect(hdc, &rc);
		EndPaint(hwnd, &ps);
		break;
	}	
	case WM_CLOSE: {
		DeleteDC(hdc_mem);
		return DestroyWindow(hwnd);
	}
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
   return WM_NULL;
}

void GUI_DEMO_OwndrawListbox(void)
{
	HWND	hwnd;
	WNDCLASS	wcex;
	MSG msg;

	/////
	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WinProc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);

	//创建主窗口
	hwnd = CreateWindowEx(NULL,
		&wcex,
		_T("DrawBitmap(ARGB8888 Format)"),
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

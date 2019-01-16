#include <emXGUI.h>

//定义控件的通知码
#define MYBTN_UP   0
#define MYBTN_DOWN 1

//定义控件的私有消息(所有控件的私有消息要从 WM_WIDGET 开始定义)
#define MSG_MYBTN_SET_STATE WM_WIDGET+0 //设置按扭状态
#define MSG_MYBTN_GET_STATE WM_WIDGET+1 //获得按扭状态
//定义控件的状态
#define BTN_STATE_PRESSED (1<<0)
//定义控件的扩展数据结构.
typedef struct
{
	u32 state; //用于记录按钮状态.
}my_btn_ext;
//控件的窗口过程函数
static LRESULT	my_btn_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	my_btn_ext *btn_ex;
	btn_ex = (my_btn_ext*)GetWindowLong(hwnd, GWL_EXDATA_ADDR); //获得窗口扩展数据区。
	switch (msg)
	{
	case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
	{
		btn_ex->state = 0;
	}
	return TRUE;
	case WM_LBUTTONDOWN: //鼠标左键/触摸在窗口内按下了。
	{
		btn_ex->state = ~btn_ex->state; //设置 “按下”状态.
		NotifyParent(hwnd, MYBTN_DOWN); //发通知消息给父窗口
		//使用窗口重绘.
		InvalidateRect(hwnd, NULL, FALSE);
	}
	break;
	//		case WM_LBUTTONUP:  //鼠标左键/触摸在窗口内“抬起”了。
	//		{
	//			btn_ex->state  &= ~BTN_STATE_PRESSED; //清除“按下”状态.
	//			NotifyParent(hwnd,MYBTN_UP); //发通知消息给父窗口
	//			//使用窗口重绘.
	//			InvalidateRect(hwnd,NULL,FALSE);
	//		}
	case MSG_MYBTN_SET_STATE: //设置按扭状态
	{
		btn_ex->state = wParam;

		InvalidateRect(hwnd, NULL, FALSE); //改变状态后应该要重绘制一次...
	}
	break;
	case MSG_MYBTN_GET_STATE: //获得按扭状态
	{
		return btn_ex->state;
	}

	case WM_ERASEBKGND:
	{
		RECT rc;
		HDC hdc = (HDC)wParam;
      HDC hdc_mem;
      
		GetClientRect(hwnd, &rc); //获得窗口的客户区矩形
      hdc_mem = CreateMemoryDC(SURF_ARGB4444, rc.w, rc.h);
		SetBrushColor(hdc_mem, MapARGB(hdc_mem, 0, 255, 255, 255));
		FillRect(hdc_mem, &rc);
      BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_mem, 0, 0, SRCCOPY);
      DeleteDC(hdc_mem);
	}
	break;

	case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
	{
		PAINTSTRUCT ps;
		HDC hdc;
		RECT rc;
		WCHAR wbuf[64];
		hdc = BeginPaint(hwnd, &ps); //开始绘图

		GetClientRect(hwnd, &rc); //获得窗口的客户区矩形
//		SetBrushColor(hdc, MapRGB(hdc, 255, 255, 255));
//		FillRect(hdc, &rc);


		GetWindowText(hwnd, wbuf, 64); //获得窗口文字.
		GetClientRect(hwnd, &rc);
		if (btn_ex->state)
		{ // “按下”状态
			SetTextColor(hdc, MapRGB(hdc, 250, 10, 10));
			SetPenColor(hdc, MapRGB(hdc, 0, 255, 0));
			SetBrushColor(hdc, MapRGB(hdc, 0, 255, 0));
		}
		else
		{	// 非“按下”状态
			SetTextColor(hdc, MapRGB(hdc, 0, 30, 30));
			SetPenColor(hdc, MapRGB(hdc, 255, 0, 0));
			SetBrushColor(hdc, MapRGB(hdc, 255, 0, 0));
		}
		//DrawCircle(hdc, rc.x + rc.w / 4, rc.y + rc.w / 4, rc.w / 4 - 2);
		FillCircle(hdc, rc.x + rc.w / 4, rc.x + rc.w / 4, rc.w / 4-2);

		EndPaint(hwnd, &ps); //结束绘图
	}
	break;
	//		case WM_CLOSE: //窗口关闭时，会自动产生该消息.
	//		{
	//			return DestroyWindow(hwnd); //调用DestroyWindow函数销毁窗口.
	//		}
	default: //用户不关心的消息,由系统处理.
	{
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	}
	return WM_NULL;
}




//定义一个控件类结构实例.
const WNDCLASS my_roundbutton =
{
	.Tag = WNDCLASS_TAG,
	.Style = 0,
	.lpfnWndProc = my_btn_proc,
	.cbClsExtra = 0,
	.cbWndExtra = sizeof(my_btn_ext),
	.hInstance = NULL,
	.hIcon = NULL,
	.hCursor = NULL,
};


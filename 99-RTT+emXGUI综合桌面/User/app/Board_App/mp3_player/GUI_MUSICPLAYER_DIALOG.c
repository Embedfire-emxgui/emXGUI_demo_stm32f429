#include "emXGUI.h"
#include "emXGUI_JPEG.h"

/* 外部图片数据 */
extern char tiger_jpg[];
/* 外部图片数据大小 */
extern unsigned int tiger_jpg_size(void);
static LRESULT win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  /* 外部图片尺寸大小 */
  static U16 pic_width,pic_height;
  static HDC hdc_mem=NULL;
  
	RECT rc;
	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口
		{
      JPG_DEC *dec;
      
			GetClientRect(hwnd,&rc); //获得窗口的客户区矩形

      /* 根据图片数据创建JPG_DEC句柄 */
      dec = JPG_Open(tiger_jpg, tiger_jpg_size());
      /* 读取图片文件信息 */
      JPG_GetImageSize(&pic_width, &pic_height,dec);
      
      /* 判断边界 */
      pic_width = pic_width >= GUI_XSIZE ? GUI_XSIZE:pic_width;
      pic_height = pic_height >= GUI_YSIZE ? GUI_YSIZE:pic_height;

      /* 创建内存对象 */
      hdc_mem =CreateMemoryDC(SURF_SCREEN,pic_width,pic_height); 
      
      /* 绘制至内存对象 */
      JPG_Draw(hdc_mem, 0, 0, dec);
            
      /* 关闭JPG_DEC句柄 */
      JPG_Close(dec);
      
			//SetTimer(hwnd,0,50,TMR_START,NULL);

		}
		return TRUE;
		////


		case WM_TIMER:

			InvalidateRect(hwnd,&rc,FALSE);
			break;

		case WM_ERASEBKGND:
		{
			HDC hdc=(HDC)wParam;

			GetClientRect(hwnd,&rc);
			SetBrushColor(hdc,MapRGB(hdc,0,30,130));
			FillRect(hdc,&rc);
		}
		return TRUE;
		/////

		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{
			PAINTSTRUCT ps;
			HDC hdc;
//			WCHAR wbuf[128];
			RECT rc0;
			int x=0,y=0;
      
			hdc =BeginPaint(hwnd,&ps);

			////用户的绘制内容...
			GetClientRect(hwnd,&rc0);

			for(y=0; y<rc0.h; y+=pic_height)
			{
				for(x=0; x<rc0.w; x+=pic_width)
				{

          /* 把内存对象绘制至屏幕 */
          BitBlt(hdc,x,y,pic_width,pic_height,hdc_mem,0,0,SRCCOPY); //将MEMDC输出到窗口中。

          rc.x=x;
					rc.y=y;
					rc.w=pic_width;
					rc.h=pic_height;
					DrawRect(hdc,&rc);
				}
			}

			EndPaint(hwnd,&ps);
			//////////
/*
			rc.x=10;
			rc.y=10;
			rc.w=100;
			rc.h=200;
			InvalidateRect(hwnd,&rc,FALSE);
*/
		}
		break;
		////

		case WM_DESTROY: //窗口销毁时，会自动产生该消息，在这里做一些资源释放的操作.
		{
      DeleteDC(hdc_mem);

			return PostQuitMessage(hwnd); //调用PostQuitMessage，使用主窗口结束并退出消息循环.
		}
//		break;
		////

		default:
		{
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}

	}

	return WM_NULL;
}

//音乐播放器句柄
HWND	MusicPlayer_hwnd;
void	GUI_MUSICPLAYER_DIALOG(void)
{
	
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
	MusicPlayer_hwnd = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,
		&wcex,
		L"GUI_MUSICPLAYER_DIALOG",
		WS_VISIBLE,
		0, 0, GUI_XSIZE, GUI_YSIZE,
		NULL, NULL, NULL, NULL);

	//显示主窗口
	ShowWindow(MusicPlayer_hwnd, SW_SHOW);

	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while (GetMessage(&msg, MusicPlayer_hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}



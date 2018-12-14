
#include <emXGUI.h>
#include "gui_drv.h"

#ifndef	_T
#define	_T(x) L##x
#endif

/*===================================================================================*/
#define	ID_EXIT		0x1000
#define	ID_LISTBOX1	0x1101
#define	ID_LISTBOX2	0x1102
#define	ID_LISTBOX3	0x1103

/*===================================================================================*/
RECT rc_m;
static void _draw_listbox(HDC hdc,HWND hwnd,COLOR_RGB32 text_c,COLOR_RGB32 back_c,HFONT hFont)
{
	RECT rc,rc_cli;
	int i,count;
	WCHAR wbuf[128];
	GetClientRect(hwnd,&rc_cli);

	SetBrushColor(hdc,MapRGB888(hdc,back_c));
	FillRect(hdc,&rc_cli);
   
	//定义一个中间的矩形．
	rc_m.w =rc_cli.w;
	rc_m.h =40;
	rc_m.x =0;
	rc_m.y =(rc_cli.h-rc_m.h)>>1;   
   //中间框绘制到hdc1中．
	SetBrushColor(hdc,MapRGB(hdc,255,255,255));
	FillRect(hdc,&rc_m);   
         
   SetFont(hdc,hFont);
	SetTextColor(hdc,MapRGB888(hdc,text_c));

	i=SendMessage(hwnd,LB_GETTOPINDEX,0,0);
	count=SendMessage(hwnd,LB_GETCOUNT,0,0);
	while(i<count)
	{
		SendMessage(hwnd,LB_GETITEMRECT,i,(LPARAM)&rc);
		if(rc.y > rc_cli.h)
		{
			break;
		}

      
		SendMessage(hwnd,LB_GETTEXT,i,(LPARAM)wbuf);
		DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_CENTER|DT_VCENTER);

		i++;
	}
}

int i;
static void _listbox_owner_draw_x(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc,hdc0,hdc1;
   HDC hdc_mem;
   RECT rc;
	int x,y,w,h;

	hwnd =ds->hwnd;
	hdc =ds->hDC;

	//创建三个一样大小的DC,把listbox分别绘制进去，但颜色参数不同的.
   hdc_mem = CreateMemoryDC(SURF_SCREEN,ds->rc.w,ds->rc.h);//透明图层 
   hdc0 =CreateMemoryDC(SURF_SCREEN,ds->rc.w,ds->rc.h);//缩小图层
	hdc1 =CreateMemoryDC(SURF_SCREEN,ds->rc.w,ds->rc.h);//放大图层
  
   
   //绘制与窗口背景颜色一样的矩形（实现透明）
	SetBrushColor(hdc_mem,MapRGB(hdc_mem,207,212,215));
	FillRect(hdc_mem,&ds->rc);     
    
	//一个listbox绘到hdc0中
	_draw_listbox(hdc0,hwnd,RGB888(100,149,237),RGB888(250,0,0),hDefaultFont);
   /*************************/
	//一个listbox绘到hdc1中（图片的第三部分）
	_draw_listbox(hdc1,hwnd,RGB888(0,0,0),RGB888(250,0,0),hZoomFont);
   
   //获取列表框中的第二行的高度 
   SendMessage(hwnd,LB_GETITEMRECT,2,(LPARAM)&rc);
   /*********第一步***********/
	//将透明部分从hdc_mem里复制出来.
	x =0;
	y =0;
	w =rc_m.w;
	h =rc.y;
	BitBlt(hdc,x,y,w,h,hdc_mem,x,y,SRCCOPY);
   /*********第二步***********/
   //将缩小部分从hdc0里复制出来.
   x = 0;
   y = rc.y;
   w = rc_m.w;
   h = rc_m.y - rc.y;
   BitBlt(hdc,x,y,w,h,hdc0,x,y,SRCCOPY);
   /*********第三步***********/
	//中间矩形部分从hdc1里复制出来.
	BitBlt(hdc,rc_m.x,rc_m.y,rc_m.w,rc_m.h,hdc1,rc_m.x,rc_m.y,SRCCOPY);
   /*********第四步***********/
	//下面的矩形部分从hdc0里复制出来.
	x =0;
	y =rc_m.y+rc_m.h;
	w =rc_m.w;
	h =ds->rc.h-(rc_m.y+rc_m.h);
	BitBlt(hdc,x,y,w,h,hdc0,x,y,SRCCOPY);
   //释放内存
	DeleteDC(hdc0);
	DeleteDC(hdc1);
   DeleteDC(hdc_mem);
}

static	LRESULT	win_proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{

	HWND wnd;
	
	switch(msg)
	{
		case	WM_CREATE:
            {  
               //创建自绘制列表框LISTBOX1 
               wnd=CreateWindow(LISTBOX,_T("Listbox"),WS_OWNERDRAW|LBS_LINE|LBS_NOTIFY|WS_VISIBLE,
                                8,120,160,200,hwnd,ID_LISTBOX1,NULL,NULL);
               //设置列表项的内容
               SendMessage(wnd,LB_ADDSTRING,0,(LPARAM)L" ");
               SendMessage(wnd,LB_ADDSTRING,1,(LPARAM)L" ");
               SendMessage(wnd,LB_ADDSTRING,2,(LPARAM)L"Item-1-2");
               SendMessage(wnd,LB_ADDSTRING,3,(LPARAM)L"Item-1-3");
               SendMessage(wnd,LB_ADDSTRING,4,(LPARAM)L"Item-1-4");
               SendMessage(wnd,LB_ADDSTRING,5,(LPARAM)L"Item-1-5");
               SendMessage(wnd,LB_ADDSTRING,6,(LPARAM)L"Item-1-6");
               SendMessage(wnd,LB_ADDSTRING,7,(LPARAM)L"Item-1-7");
               SendMessage(wnd,LB_ADDSTRING,8,(LPARAM)L"Item-1-8");
               SendMessage(wnd,LB_ADDSTRING,9,(LPARAM)L"Item-1-9");
               SendMessage(wnd,LB_ADDSTRING,10,(LPARAM)L"Item-1-10");
               //设置LISTBOX1中的列表项行高   
               for(int i = 0; i < 11; i++)
                  SendMessage(wnd,LB_SETITEMHEIGHT,i,40);
               //创建列表框LISTBOX2 
               wnd=CreateWindow(LISTBOX,_T("Listbox2"),LBS_LINE|WS_BORDER|WS_VISIBLE,
                                200,8,100,160,hwnd,ID_LISTBOX2,NULL,NULL);
               //设置列表项的内容
               SendMessage(wnd,LB_ADDSTRING,0,(LPARAM)L"Item-2-0");
               SendMessage(wnd,LB_ADDSTRING,1,(LPARAM)L"Item-2-1");
               SendMessage(wnd,LB_ADDSTRING,2,(LPARAM)L"Item-2-2");
               SendMessage(wnd,LB_ADDSTRING,3,(LPARAM)L"Item-2-3");
               SendMessage(wnd,LB_ADDSTRING,4,(LPARAM)L"Item-2-4");
               SendMessage(wnd,LB_ADDSTRING,5,(LPARAM)L"Item-2-5");
               SendMessage(wnd,LB_ADDSTRING,6,(LPARAM)L"Item-2-6");
               SendMessage(wnd,LB_ADDSTRING,7,(LPARAM)L"Item-2-7");
               SendMessage(wnd,LB_ADDSTRING,8,(LPARAM)L"Item-2-8");

               //创建列表框LISTBOX3
               wnd=CreateWindow(LISTBOX,_T("Listbox3"),LBS_LINE|WS_BORDER|WS_VISIBLE,
                                8,8,160,100,hwnd,ID_LISTBOX3,NULL,NULL);
               //设置列表项的内容
               SendMessage(wnd,LB_ADDSTRING,0,(LPARAM)L"Item-3-0");
               SendMessage(wnd,LB_ADDSTRING,1,(LPARAM)L"Item-3-1");
               SendMessage(wnd,LB_ADDSTRING,2,(LPARAM)L"Item-3-2");
               SendMessage(wnd,LB_ADDSTRING,3,(LPARAM)L"Item-3-3");
               SendMessage(wnd,LB_ADDSTRING,4,(LPARAM)L"Item-3-4");
               SendMessage(wnd,LB_ADDSTRING,5,(LPARAM)L"Item-3-5");
               SendMessage(wnd,LB_ADDSTRING,6,(LPARAM)L"Item-3-6");
               SendMessage(wnd,LB_ADDSTRING,7,(LPARAM)L"Item-3-7");
               SendMessage(wnd,LB_ADDSTRING,8,(LPARAM)L"Item-3-8");

               return TRUE;
            }			
		case	WM_CTLCOLOR:
            {
               u16 id;
               id =LOWORD(wParam);
               //修改ID_LISTBOX3的颜色 
               if(id== ID_LISTBOX3)
               {
                  CTLCOLOR *cr;
                  cr =(CTLCOLOR*)lParam;
                  cr->TextColor =RGB888(255,255,255);//文字颜色
                  cr->BackColor =RGB888(0,0,0);//背景颜色
                  cr->BorderColor =RGB888(50,150,50);//边框颜色
                  cr->ForeColor =RGB888(0,50,0);//选中框颜色
                  return TRUE;
               }
               else
               {
                  return FALSE;
               }
            }
		case	WM_DRAWITEM:
            {
               DRAWITEM_HDR *ds;
               ds =(DRAWITEM_HDR*)lParam;

               if(wParam==ID_LISTBOX1)
               {
                  _listbox_owner_draw_x(ds);
                  return TRUE;
               }
               return FALSE;
            }						
      case WM_PAINT:
            {
               PAINTSTRUCT ps;
               HDC hdc;
               RECT rc;
               hdc = BeginPaint(hwnd, &ps);
               GetClientRect(hwnd, &rc);
               SetBrushColor(hdc,MapRGB(hdc,207,212,215));
               FillRect(hdc, &rc);
               EndPaint(hwnd, &ps);
            
            }
      default:
				return	DefWindowProc(hwnd,msg,wParam,lParam);
	}

}
void	GUI_DEMO_Listbox(void)
{
		HWND	hwnd;
		WNDCLASS	wcex;
		MSG msg;
   
		InvalidateRect(GetDesktopWindow(),NULL,TRUE);

		wcex.Tag 		    = WNDCLASS_TAG;
		wcex.Style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= win_proc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= 0;//hInst;
		wcex.hIcon			= 0;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
		wcex.hCursor		= 0;//LoadCursor(NULL, IDC_ARROW);	
		hwnd	=CreateWindowEx(	NULL,
									&wcex,
									_T("GUI Demo - Listbox"),
									/*WS_MEMSURFACE|*/WS_CAPTION|WS_DLGFRAME|WS_BORDER|WS_CLIPCHILDREN,
									0,0,GUI_XSIZE,GUI_YSIZE,
									NULL,NULL,NULL,NULL);			
		ShowWindow(hwnd,SW_SHOW);	
		while(GetMessage(&msg,hwnd))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	
}
/*===================================================================================*/
/*===================================================================================*/
/*===================================================================================*/
/*===================================================================================*/

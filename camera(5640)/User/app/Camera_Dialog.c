#include <emXGUI.h>
#include "./camera/bsp_ov5640.h"
#include "x_libc.h"
#include "./camera/ov5640_AF.h"

#define ID_BUTTON_Exit  0x1000
#define FONT_H          72
#define FONT_W          72

uint8_t focus_status = 0;
rt_thread_t h1;
BOOL update_flag = 0;//帧率更新标志
static RECT win_rc;
static int b_close=FALSE;
uint8_t fps=0;//帧率
OV5640_IDTypeDef OV5640_Camera_ID;
RECT rc_fps = {0,400,800,72};//帧率显示子窗口
HWND Cam_hwnd;//主窗口句柄
int state = 0;
U16 *bits;
GUI_SEM *cam_sem = NULL;//同步信号量（二值型）
int fgg = 0;//默认没有被按下
//定义控件ID
enum eID
{
	eID_OK =0x1000,
	eID_SET,
	eID_SET1,
	eID_SET2,
	eID_SET3,
	eID_SET4,
   eID_SET5,
   eID_Focus,
   eID_WhiteBalance,
   eID_SCROLLBAR
   
};

/*
 * @brief  绘制滚动条
 * @param  hwnd:   滚动条的句柄值
 * @param  hdc:    绘图上下文
 * @param  back_c：背景颜色
 * @param  Page_c: 滚动条Page处的颜色
 * @param  fore_c：滚动条滑块的颜色
 * @retval NONE
*/
static void draw_scrollbar(HWND hwnd, HDC hdc, COLOR_RGB32 back_c, COLOR_RGB32 Page_c, COLOR_RGB32 fore_c)
{
	RECT rc;
   RECT rc_scrollbar;
	GetClientRect(hwnd, &rc);
	/* 背景 */
	SetBrushColor(hdc, back_c);
	FillRect(hdc, &rc);

   rc_scrollbar.x = rc.x;
   rc_scrollbar.y = rc.h/2;
   rc_scrollbar.w = rc.w;
   rc_scrollbar.h = 2;
   
	SetBrushColor(hdc, MapRGB888(hdc, Page_c));
	FillRect(hdc, &rc_scrollbar);

	/* 滑块 */
	SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc);

	SetBrushColor(hdc, MapRGB(hdc, 169, 169, 169));
	//rc.y += (rc.h >> 2) >> 1;
	//rc.h -= (rc.h >> 2);
	/* 边框 */
	//FillRoundRect(hdc, &rc, MIN(rc.w, rc.h) >> 2);
	FillCircle(hdc, rc.x + rc.w / 2, rc.y + rc.h / 2, rc.h / 2);
   InflateRect(&rc, -2, -2);

	SetBrushColor(hdc, MapRGB888(hdc, fore_c));
	FillCircle(hdc, rc.x + rc.w / 2, rc.y + rc.h / 2, rc.h / 2);
   //FillRoundRect(hdc, &rc, MIN(rc.w, rc.h) >> 2);
}
/*
 * @brief  自定义滑动条绘制函数
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void scrollbar_owner_draw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
	HDC hdc_mem;
	HDC hdc_mem1;
	RECT rc;
	RECT rc_cli;
	//	int i;

	hwnd = ds->hwnd;
	hdc = ds->hDC;
	GetClientRect(hwnd, &rc_cli);

	hdc_mem = CreateMemoryDC(SURF_SCREEN, rc_cli.w, rc_cli.h);
	hdc_mem1 = CreateMemoryDC(SURF_SCREEN, rc_cli.w, rc_cli.h);   
         
   	
	//绘制白色类型的滚动条
	draw_scrollbar(hwnd, hdc_mem1, RGB888( 0, 0, 0), RGB888( 250, 250, 250), RGB888( 255, 255, 255));
	//绘制绿色类型的滚动条
	draw_scrollbar(hwnd, hdc_mem, RGB888( 0, 0, 0), RGB888( 250, 0, 0), RGB888( 250, 0, 0));
   SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc);   

	//左
	BitBlt(hdc, rc_cli.x, rc_cli.y, rc.x, rc_cli.h, hdc_mem, 0, 0, SRCCOPY);
	//右
	BitBlt(hdc, rc.x + rc.w, 0, rc_cli.w - (rc.x + rc.w) , rc_cli.h, hdc_mem1, rc.x + rc.w, 0, SRCCOPY);

	//绘制滑块
	if (ds->State & SST_THUMBTRACK)//按下
	{
      BitBlt(hdc, rc.x, 0, rc.w, rc_cli.h, hdc_mem1, rc.x, 0, SRCCOPY);
		
	}
	else//未选中
	{
		BitBlt(hdc, rc.x, 0, rc.w+1, rc_cli.h, hdc_mem, rc.x, 0, SRCCOPY);
	}
	//释放内存MemoryDC
	DeleteDC(hdc_mem1);
	DeleteDC(hdc_mem);
}

static void Checkbox_owner_draw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
	//	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	//	hwnd =ds->hwnd; //button的窗口句柄.
	hdc = ds->hDC;   //button的绘图上下文句柄.
	rc = ds->rc;     //button的绘制矩形区.
	SetBrushColor(hdc, MapRGB(hdc, 0, 0, 0)); //设置填充色(BrushColor用于所有Fill类型的绘图函数)
	FillRect(hdc, &rc);

	if (fgg)
	{ //按钮是按下状态
		SetBrushColor(hdc, MapRGB(hdc, 119, 136, 153)); //设置填充色(BrushColor用于所有Fill类型的绘图函数)
		FillRoundRect(hdc, &rc, rc.h / 2); //用矩形填充背景
		InflateRect(&rc, -3, -3);

		SetBrushColor(hdc, MapRGB(hdc, 60, 179, 113)); //设置填充色(BrushColor用于所有Fill类型的绘图函数)
		FillRoundRect(hdc, &rc, rc.h / 2); //用矩形填充背景

		GetClientRect(ds->hwnd, &rc);
		SetBrushColor(hdc, MapRGB(hdc, 119, 136, 153)); //设置填充色(BrushColor用于所有Fill类型的绘图函数)
		FillCircle(hdc, rc.w - 15, 15, 15);


		SetBrushColor(hdc, MapRGB(hdc, 250, 250, 250)); //设置填充色(BrushColor用于所有Fill类型的绘图函数)
		FillCircle(hdc, rc.w - 15, 15, 12);
	}
	else
	{ //按钮是弹起状态
		SetBrushColor(hdc, MapRGB(hdc, 119, 136, 153)); //设置填充色(BrushColor用于所有Fill类型的绘图函数)
		FillRoundRect(hdc, &rc, rc.h/2); //用矩形填充背景
		InflateRect(&rc, -3,  -3);

		SetBrushColor(hdc, MapRGB(hdc, 250, 250, 250)); //设置填充色(BrushColor用于所有Fill类型的绘图函数)
		FillRoundRect(hdc, &rc, rc.h / 2); //用矩形填充背景

		GetClientRect(ds->hwnd, &rc);
		SetBrushColor(hdc, MapRGB(hdc, 119, 136, 153)); //设置填充色(BrushColor用于所有Fill类型的绘图函数)
		FillCircle(hdc, rc.x + 15, 15, 15);

		
		SetBrushColor(hdc, MapRGB(hdc, 250, 250, 250)); //设置填充色(BrushColor用于所有Fill类型的绘图函数)
		FillCircle(hdc, rc.x + 15, 15, 12);

	}


}
static void Update_Dialog()
{
	int app=0;

	while(1) //线程已创建了
	{
		if(app==0)
		{
         app=1;
			GUI_SemWait(cam_sem, 0xFFFFFFFF);
         InvalidateRect(Cam_hwnd,NULL,FALSE);
			app=0;
		}
	}
}
/*============================================================================*/
static LRESULT	dlg_set_xx_WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{
         RECT rc;
         GetClientRect(hwnd, &rc);
         
         
         
		}
		return TRUE;
		////
		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{
			PAINTSTRUCT ps;
			HDC hdc;
			RECT rc;
			hdc =BeginPaint(hwnd,&ps); //开始绘图

			////用户的绘制内容...

			SetTextColor(hdc,MapRGB(hdc,250,250,250));
			SetBrushColor(hdc,MapRGB(hdc,240,10,10));

			rc.x =10;
			rc.y =20;
			rc.w =200;
			rc.h =24;
			DrawText(hdc,L"Set xxx..",-1,&rc,DT_CENTER|DT_VCENTER|DT_BKGND);
			//TextOut(hdc,10,10,L"Hello",-1);

			EndPaint(hwnd,&ps); //结束绘图
		}
		break;
      case WM_DRAWITEM:
      {
         
         
         DRAWITEM_HDR *ds;
         ds = (DRAWITEM_HDR*)lParam;
         if (ds->ID == eID_SCROLLBAR)
         {
            scrollbar_owner_draw(ds);
            return TRUE;
         }
//         if (ds->ID >= 0x1000 && ds->ID<= 0x1099)
//         {
//            button_owner_draw(ds);
//            return TRUE;
//         }
      } 
		default: //用户不关心的消息,由系统处理.
		{
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}

	}
	return WM_NULL;

}

static LRESULT	dlg_set_WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc,rc0;
   static SCROLLINFO sif;
	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{
			b_close =FALSE;

//			GetClientRect(hwnd,&rc0); //获得窗口的客户区矩形.
//			CreateWindow(BUTTON,L"OK",WS_VISIBLE,rc0.w-80,8,68,32,hwnd,eID_OK,NULL,NULL);
			rc.x =10;
			rc.y =20;
			rc.w =200;
			rc.h =40;
         CreateWindow(TEXTBOX,L"自动对焦",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_Focus,NULL,NULL);
         SendMessage(GetDlgItem(hwnd, eID_Focus),TBM_SET_TEXTFLAG,0,
                     DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_BKGND); 
         
			rc.y =60;
         CreateWindow(TEXTBOX,L"白平衡",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_WhiteBalance,NULL,NULL);
         SendMessage(GetDlgItem(hwnd, eID_WhiteBalance),TBM_SET_TEXTFLAG,0,
                     DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_BKGND);         
         
         
			rc.x =10;
			rc.y =100;
			rc.w =80;
			rc.h =40;
			CreateWindow(BUTTON,L"饱和度",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_SET4,NULL,NULL);

			OffsetRect(&rc,0,rc.h+4);
			CreateWindow(BUTTON,L"对比度",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_SET3,NULL,NULL);

			OffsetRect(&rc,0,rc.h+4);
			CreateWindow(BUTTON,L"分辨率",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_SET1,NULL,NULL);

			OffsetRect(&rc,0,rc.h+4);
			CreateWindow(BUTTON,L"光线模式",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_SET2,NULL,NULL);

			OffsetRect(&rc,0,rc.h+4);
			CreateWindow(BUTTON,L"特殊效果",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_SET5,NULL,NULL);
			//SetTimer(hwnd,1,500,TMR_START,NULL);
			SetTimer(hwnd,2,20,TMR_START,NULL);
         
         sif.cbSize = sizeof(sif);
         sif.fMask = SIF_ALL;
         sif.nMin = 0;
         sif.nMax = 63;//音量最大值为63
         sif.nValue = 20;//初始音量值
         sif.TrackSize = 31;//滑块值
         sif.ArrowSize = 0;//两端宽度为0（水平滑动条）
         CreateWindow(SCROLLBAR, L"SCROLLBAR_R", WS_VISIBLE, 
                       220, 64, 150, 31, hwnd, eID_SCROLLBAR, NULL, NULL);
         SendMessage(GetDlgItem(hwnd, eID_SCROLLBAR), SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif);
         
         
         CreateWindow(BUTTON,L" ",WS_OWNERDRAW|WS_VISIBLE,
                      280,30,60,30,hwnd,0x1234,NULL,NULL);
		}
		return TRUE;

		case WM_TIMER:
		{
			switch(wParam)
			{

				case 1:
				{
					//tmr_500ms++;
					//InvalidateRect(hwnd,NULL,FALSE);

				}
				break;
				////

				case 2:
				{
					if(GetKeyState(VK_LBUTTON)!=0)
					{
						break;
					}

					////
					GetWindowRect(hwnd,&rc);

					if(b_close==FALSE)
					{
						if(rc.y < win_rc.y )
						{
							if((win_rc.y-rc.y)>50)
							{
								rc.y +=30;
							}
							if((win_rc.y-rc.y)>30)
							{
								rc.y +=20;
							}
							else
							{
								rc.y +=4;
							}
							ScreenToClient(GetParent(hwnd),(POINT*)&rc.x,1);
							MoveWindow(hwnd,rc.x,rc.y,rc.w,rc.h,TRUE);
						}
					}
					else
					{
						if(rc.y > -(rc.h))
						{
							rc.y -= 40;

							ScreenToClient(GetParent(hwnd),(POINT*)&rc.x,1);
							MoveWindow(hwnd,rc.x,rc.y,rc.w,rc.h,TRUE);
						}
						else
						{
							PostCloseMessage(hwnd);
						}
					}


				}
				break;
			}
		}
		break;
		////

		case WM_NOTIFY: //WM_NOTIFY消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
		{
			u16 code,id;
         int i;
         NMHDR *nr;  
         u16 ctr_id = LOWORD(wParam); //wParam低16位是发送该消息的控件ID. 
         nr = (NMHDR*)lParam; //lParam参数，是以NMHDR结构体开头.
			code =HIWORD(wParam); //获得通知码类型.
			id   =LOWORD(wParam); //获得产生该消息的控件ID.
         if (ctr_id == eID_SCROLLBAR)
         {
            NM_SCROLLBAR *sb_nr;
            sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar的通知消息实际为 NM_SCROLLBAR扩展结构,里面附带了更多的信息.
            switch (nr->code)
            {
               case SBN_THUMBTRACK: //R滑块移动
               {
                  i= sb_nr->nTrackValue; //得到当前的音量值
                  SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, i); //发送SBM_SETVALUE，设置音量值
               }
               break;
            }
         } 
			if(id==eID_SET1 && code==BN_CLICKED)
			{
				WNDCLASS wcex;

				GetWindowRect(GetDlgItem(hwnd,eID_SET1),&rc);
				ScreenToClient(hwnd,(POINT*)&rc.x,1);
				rc.x += rc.w+1;
				rc.w =180;
				rc.h =100;
            //rc.y = 64;
				wcex.Tag	 		= WNDCLASS_TAG;
				wcex.Style			= CS_HREDRAW | CS_VREDRAW;
				wcex.lpfnWndProc	= (WNDPROC)dlg_set_xx_WinProc;
				wcex.cbClsExtra		= 0;
				wcex.cbWndExtra		= 0;
				wcex.hInstance		= NULL;
				wcex.hIcon			= NULL;
				wcex.hCursor		= NULL;


				CreateWindowEx(
								WS_EX_FRAMEBUFFER,
								&wcex,L"Set_1_xxx",
								WS_OVERLAPPED|WS_CLIPCHILDREN|WS_VISIBLE,
								rc.x,rc.y,rc.w,rc.h,
								hwnd,0,NULL,NULL);

			}

			if(id==eID_SET2 && code==BN_CLICKED)
			{
				WNDCLASS wcex;

				GetWindowRect(GetDlgItem(hwnd,eID_SET2),&rc);
				ScreenToClient(hwnd,(POINT*)&rc.x,1);
				rc.x += rc.w+1;
				rc.w =220;
				rc.h =100;

				wcex.Tag	 		= WNDCLASS_TAG;
				wcex.Style			= CS_HREDRAW | CS_VREDRAW;
				wcex.lpfnWndProc	= (WNDPROC)dlg_set_xx_WinProc;
				wcex.cbClsExtra		= 0;
				wcex.cbWndExtra		= 0;
				wcex.hInstance		= NULL;
				wcex.hIcon			= NULL;
				wcex.hCursor		= NULL;


				CreateWindowEx(
								WS_EX_FRAMEBUFFER,
								&wcex,L"Set_2_xxx",
								WS_OVERLAPPED|WS_CLIPCHILDREN|WS_CAPTION|WS_DLGFRAME|WS_BORDER|WS_VISIBLE|WS_CLOSEBOX,
								rc.x,rc.y,rc.w,rc.h,
								hwnd,0,NULL,NULL);

			}

			if(id==eID_OK && code==BN_CLICKED) // 按钮“单击”了.
			{
				PostCloseMessage(hwnd); //使产生WM_CLOSE消息关闭窗口.
			}
         
			if (id == 0x1234 && code == BN_CLICKED)
			{
				fgg = ~fgg;

			}         
         
		}
		break;
		////
      
//      case WM_ERASEBKGND:
//      {
//         HDC hdc =(HDC)wParam;
//         HDC hdc_mem;
//         RECT rc;
//         GetClientRect(hwnd, &rc);
//         hdc_mem = CreateMemoryDC(SURF_ARGB4444, rc.w, rc.h);
//         
//         SetPenColor(hdc_mem, MapARGB(hdc, 255, 0, 0, 0));
//         SetBrushColor(hdc_mem, MapARGB(hdc, 125, 112,128,144));
//         
//         DrawRect(hdc_mem, &rc);
//         FillRect(hdc_mem, &rc);
//         
//         
//         BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_mem, rc.x, rc.y, SRCCOPY);
//         DeleteDC(hdc_mem);
//      }

		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{
			PAINTSTRUCT ps;
			HDC hdc;
			RECT rc;
			hdc =BeginPaint(hwnd,&ps); //开始绘图

			////用户的绘制内容...
         GetClientRect(hwnd, &rc);
//			SetTextColor(hdc,MapRGB(hdc,240,10,10));
			SetBrushColor(hdc,MapRGB(hdc,0,0,0));
         FillRect(hdc, &rc);
//			rc.x =10;
//			rc.y =20;
//			rc.w =200;
//			rc.h =24;
//			DrawText(hdc,L"Set...",-1,&rc,DT_CENTER|DT_VCENTER|DT_BKGND);
//			//TextOut(hdc,10,10,L"Hello",-1);

			EndPaint(hwnd,&ps); //结束绘图
         break;
		}
		case	WM_CTLCOLOR:
		{
			/* 控件在绘制前，会发送 WM_CTLCOLOR到父窗口.
			 * wParam参数指明了发送该消息的控件ID;lParam参数指向一个CTLCOLOR的结构体指针.
			 * 用户可以通过这个结构体改变控件的颜色值.用户修改颜色参数后，需返回TRUE，否则，系统
			 * 将忽略本次操作，继续使用默认的颜色进行绘制.
			 *
			 */
			u16 id;
			id =LOWORD(wParam);
         CTLCOLOR *cr;
         cr =(CTLCOLOR*)lParam;
			if(id== eID_Focus || id == eID_WhiteBalance)
			{
				
				
				cr->TextColor =RGB888(255,255,255);//文字颜色（RGB888颜色格式)
				cr->BackColor =RGB888(0,0,0);//背景颜色（RGB888颜色格式)
				cr->BorderColor =RGB888(255,0,0);//边框颜色（RGB888颜色格式)
				return TRUE;
			}
         if(id >=eID_SET1 && id <= eID_SET5)
         {
				cr->TextColor =RGB888(255,255,255);//文字颜色（RGB888颜色格式)
				cr->BackColor =RGB888(0,0,0);//背景颜色（RGB888颜色格式)
				cr->BorderColor =RGB888(0,0,0);//边框颜色（RGB888颜色格式)
				return TRUE;            
         }
         
			return FALSE;
			
		}	

		case	WM_DRAWITEM:
		{
			/*　当控件指定了WS_OWNERDRAW风格，则每次在绘制前都会给父窗口发送WM_DRAWITEM消息。
			 *  wParam参数指明了发送该消息的控件ID;lParam参数指向一个DRAWITEM_HDR的结构体指针，
			 *  该指针成员包含了一些控件绘制相关的参数.
			 */
			DRAWITEM_HDR *ds;
			ds = (DRAWITEM_HDR*)lParam;
			if (ds->ID == 0x1234)
			{
				Checkbox_owner_draw(ds); //执行自绘制按钮
			}
         if (ds->ID == eID_SCROLLBAR)
         {
            scrollbar_owner_draw(ds);
            return TRUE;
         }         
         
         
			return TRUE;
		}
		////
		case WM_CLOSE:
		{
			b_close=TRUE;

			GetWindowRect(hwnd,&rc);

			if(rc.y <= -(rc.h))
			{
				DestroyWindow(hwnd);
				return TRUE;
			}
			return FALSE;
		}
		break;
		////
 
		default: //用户不关心的消息,由系统处理.
		{
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}

	}

	return WM_NULL;

}

static LRESULT WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch(msg)
   {
      case  WM_CREATE:
      {

        /* 初始化摄像头GPIO及IIC */
        OV5640_HW_Init();  
        /* 读取摄像头芯片ID，确定摄像头正常连接 */
        OV5640_ReadID(&OV5640_Camera_ID);

        if(OV5640_Camera_ID.PIDH  == 0x56)
        {
          GUI_DEBUG("OV5640 ID:%x %x",OV5640_Camera_ID.PIDH ,OV5640_Camera_ID.PIDL);
        }
        else
        {
          MSGBOX_OPTIONS ops;
          //const WCHAR *btn[]={L"确定"};
          int x,y,w,h;

          ops.Flag =MB_ICONERROR;
          //ops.pButtonText =btn;
          ops.ButtonCount =0;
          w =500;
          h =200;
          x =(GUI_XSIZE-w)>>1;
          y =(GUI_YSIZE-h)>>1;
          MessageBox(hwnd,x,y,w,h,L"没有检测到OV5640摄像头，\n请重新检查连接。",L"消息",&ops); 
          break;  
        }     
        cam_sem = GUI_SemCreate(1,1);
        h1=rt_thread_create("Update_Dialog",(void(*)(void*))Update_Dialog,NULL,4096,5,5);
        rt_thread_startup(h1);	
        bits = (U16 *)GUI_VMEM_Alloc(800*480); 
		  SetTimer(hwnd,1,1000,TMR_START,NULL);  
        RECT rc;
        GetClientRect(hwnd, &rc);
        CreateWindow(BUTTON,L"Set",NULL,rc.w-80,rc.h-32-2,68,32,hwnd,eID_SET,NULL,NULL);        
        
        
        break;
      }
//      case WM_LBUTTONDOWN:
//      {
//         POINT pt;
//         pt.x =GET_LPARAM_X(lParam); //获得X坐标
//         pt.y =GET_LPARAM_Y(lParam); //获得Y坐标
//         RECT rc = {718, 0, 72, 72};
//         if(PtInRect(&rc, &pt))
//         {
//            
//            //产生WM_CLOSE消息关闭主窗口
//         }
//         else
//         {
//            PostCloseMessage(hwnd);
//            
//            show_menu = ~show_menu;
//         }
//         
//         break;
//      }
 		case WM_TIMER:
      {
         switch(state)
         {
            case 0:
            {
              OV5640_Init();  
              OV5640_RGB565Config();
              OV5640_USER_Config();
              OV5640_FOCUS_AD5820_Init();
               
               if(cam_mode.auto_focus ==1)
               {
                  OV5640_FOCUS_AD5820_Constant_Focus();
                  focus_status = 1;
               }
               //使能DCMI采集数据
              DCMI_Cmd(ENABLE); 
              DCMI_CaptureCmd(ENABLE); 
                             
               state = 1;
               break;
            }
            case 1:
            {
               ShowWindow(GetDlgItem(hwnd, eID_SET), SW_SHOW);
               state=2;
               break;
            }
            case 2:
            {
               update_flag = 1;
               break;
            }
         }
         break;
      }     
      case WM_PAINT:
      {
         PAINTSTRUCT ps;
         SURFACE *pSurf;
         HDC hdc_mem;
         HDC hdc;
         RECT rc;
         static int old_fps = 0;
         WCHAR wbuf[128];
         hdc = BeginPaint(hwnd,&ps);
         GetClientRect(hwnd,&rc);
			if(state==0)
			{
				SetTextColor(hdc,MapRGB(hdc,250,0,0));
				SetBrushColor(hdc,MapRGB(hdc,50,0,0));
				SetPenColor(hdc,MapRGB(hdc,250,0,0));

				DrawText(hdc,L"正在初始化摄像头\r\n请等待...",-1,&rc,DT_VCENTER|DT_CENTER|DT_BKGND);

			}              
         if(state == 2)
         {         
            pSurf =CreateSurface(SURF_RGB565,GUI_XSIZE, GUI_YSIZE, 0, bits);
            hdc_mem =CreateDC(pSurf,NULL);
            if(update_flag)
            {
               update_flag = 0;
               old_fps = fps;
               fps = 0;
            } 
            SetTextColor(hdc_mem, MapRGB(hdc_mem, 255,255,255));                 
            x_wsprintf(wbuf,L"帧率FPS:%d/s",old_fps);
            DrawText(hdc_mem, wbuf, -1, &rc_fps, DT_SINGLELINE| DT_VCENTER|DT_CENTER);
            
//            /****************绘制退出按钮******************/
//            SetBrushColor(hdc_mem, MapRGB(hdc_mem, 0,0,0));
//            FillCircle(hdc_mem, rc.w, 0, 54);
//            //边框
//            SetBrushColor(hdc_mem, MapRGB(hdc_mem, 250,0,0));
//            FillCircle(hdc_mem, rc.w, 0, 50);
//            
//            SetFont(hdc_mem, hFont_SDCARD);
//            TextOut(hdc_mem, rc.w - 20, 0, L"O", -1);
//            
//            if(show_menu)
//            {
//               RECT rc = {0, 0, 72, 480};
//               
//               SetBrushColor(hdc_mem, MapARGB(hdc_mem,50, 0,0,0));
//               FillRect(hdc_mem, &rc);
//            }            
            BitBlt(hdc, 0, 0, 800, 480, 
                   hdc_mem, 0, 0, SRCCOPY);          
            DeleteSurface(pSurf);
            DeleteDC(hdc_mem);
         }

         EndPaint(hwnd,&ps);
         break;
      }
      case WM_DESTROY:
      {
         state = 0;
         OV5640_Capture_Control(DISABLE);
         DMA_ITConfig(DMA2_Stream1,DMA_IT_TC,DISABLE); 
         DCMI_Cmd(DISABLE); 
         DCMI_CaptureCmd(DISABLE); 
         rt_thread_delete(h1);
         GUI_VMEM_Free(bits);
         GUI_CameraAvrg_DIALOG();
         return PostQuitMessage(hwnd);	
      }    
 		case WM_NOTIFY: //WM_NOTIFY消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
		{
			u16 code,id;

			code =HIWORD(wParam); //获得通知码类型.
			id   =LOWORD(wParam); //获得产生该消息的控件ID.

			if(id==eID_SET && code==BN_CLICKED)
			{
				WNDCLASS wcex;


				wcex.Tag	 		= WNDCLASS_TAG;
				wcex.Style			= CS_HREDRAW | CS_VREDRAW;
				wcex.lpfnWndProc	= (WNDPROC)dlg_set_WinProc;
				wcex.cbClsExtra		= 0;
				wcex.cbWndExtra		= 0;
				wcex.hInstance		= NULL;
				wcex.hIcon			= NULL;
				wcex.hCursor		= NULL;

				if(1)
				{
					RECT rc;

					GetClientRect(hwnd,&rc);
					//ClientToScreen(hwnd,(POINT*)&rc.x,1);

					win_rc.w =400;
					win_rc.h =360;

					win_rc.x = rc.x+(rc.w-win_rc.w)/2;
					win_rc.y = rc.y;//rc.y+(rc.h>>2);

					CreateWindowEx(
									NULL,
									&wcex,L"Set",
									WS_OVERLAPPED|WS_CLIPCHILDREN|WS_VISIBLE,

									win_rc.x,-win_rc.h-2,win_rc.w,win_rc.h,
									hwnd,0,NULL,NULL);
				}

			}
         break;  
		}
		   
      default:
         return DefWindowProc(hwnd, msg, wParam, lParam);
   }
   return WM_NULL;
}
void	GUI_Camera_DIALOG(void)
{	
	WNDCLASS	wcex;
	MSG msg;


	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WinProc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);

	//创建主窗口
	Cam_hwnd = CreateWindowEx(WS_EX_NOFOCUS,
                                    &wcex,
                                    L"GUI_Camera_Dialog",
                                    WS_VISIBLE|WS_CLIPCHILDREN,
                                    0, 0, GUI_XSIZE, GUI_YSIZE,
                                    NULL, NULL, NULL, NULL);

	//显示主窗口
	ShowWindow(Cam_hwnd, SW_SHOW);

	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while (GetMessage(&msg, Cam_hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

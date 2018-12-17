

/*============================================================================*/
#include <math.h>


#include "emXGUI.h"
#include "GUI_Font_XFT.h"
#include "x_libc.h"
#include "x_obj.h"

/*============================================================================*/

extern const char ASCII_20_4BPP[];
extern const char ASCII_24_4BPP[];

/*============================================================================*/

#define	LEFT_OFFSET	  80
#define	TOP_OFFSET	  40
#define	RIGHT_OFFSET  40
#define BOTTOM_OFFSET 32

#define	CUR_BOX_SIZE 32


static POINT pt_wav[800];
//static int x_cur,y_cur;


static HFONT hFont20,hFont24;

static RECT rc_main,rc_wav,rc_x1_cur,rc_x2_cur,rc_y1_cur,rc_y2_cur;


#define	Y_STEP_NUM	9
#define	X_STEP_NUM	9

static int y_step_cur=0;
static int x_step_cur=0;

static const float y_step[Y_STEP_NUM]={0.1,0.2,0.5,1.0,2.0,5.0,10.0,20.0,50.0};

static const WCHAR *y_step_str[Y_STEP_NUM]={
	L"0.1mV",
	L"0.2mV",
	L"0.5mV",
	L"1mV",
	L"2mV",
	L"5mV",
	L"10mV",
	L"20mV",
	L"50mV",

};

static const int x_step[X_STEP_NUM]={50,100,200,300,400,500,600,700,1000};

static const WCHAR *x_step_str[X_STEP_NUM]={
	L"50mS",
	L"100mS",
	L"200mS",
	L"300mS",
  L"400mS",
	L"500mS",
  L"600mS",	
	L"700mS",
  L"1000mS",
};


#define	CUR_X1	1
#define	CUR_X2	2
#define	CUR_Y1	3
#define	CUR_Y2	4
static int focus_cur=0;

static int x1_cur,x2_cur,y1_cur,y2_cur;


/*============================================================================*/

/*============================================================================*/
enum eMSG{
	MY_MSG_FISRT	=WM_USER,

	MSG_CUR_X1_CHANGE, //X1 光标位置有变势.
	MSG_CUR_X2_CHANGE, //X2 光标位置有变势.
	MSG_CUR_Y1_CHANGE, //Y1 光标位置有变势.
	MSG_CUR_Y2_CHANGE, //Y2 光标位置有变势.

};

enum eID{

	ID_BTN_BEGIN =0x1000,

	ID_EXIT,
	ID_START,
	ID_SENSOR_ON,
	ID_SENSOR_OFF,
	ID_Y_STEP,
  ID_X_STEP,
	ID_PUMP,

	ID_BTN_END,//////////////


	ID_X1_CUR,
	ID_X2_CUR,
	ID_Y1_CUR,
	ID_Y2_CUR,

	ID_Y_STEP_SUB,
	ID_Y_STEP_ADD,
  
  ID_X_STEP_SUB,
	ID_X_STEP_ADD,


	ID_TEST,
	ID_X_STR,
	ID_Y_STR,

	ID_WAVE,

  ID_SCROLL1,
  ID_SCROLL2,

  ID_RB1,
  ID_RB2,  
  ID_RB3,  
  ID_RB4,

  ID_TEXT1,
  ID_TEXT2,
  ID_TEXT3,
  ID_TEXT4,

};



static struct __x_obj_item *focus_obj =NULL;
static struct __x_obj_item *button_item =NULL;

static struct __x_obj_item *x1_cur_obj =NULL;
static struct __x_obj_item *x2_cur_obj =NULL;
static struct __x_obj_item *y1_cur_obj =NULL;
static struct __x_obj_item *y2_cur_obj =NULL;

/*============================================================================*/

static	void btn_draw(HDC hdc,struct __x_obj_item * obj)
{
	HFONT hFont;

	hFont =GetFont(hdc);

	if(obj->rc.w > 60 && obj->rc.h > 20)
	{
		SetFont(hdc,hFont20);
	}


	if(obj == focus_obj)
	{
		SetTextColor(hdc,MapRGB(hdc,255,0,255));
		SetPenColor(hdc,MapRGB(hdc,200,0,200));
		SetBrushColor(hdc,MapRGB(hdc,100,0,100));
		DrawText(hdc,obj->pszText,-1,&obj->rc,DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_BKGND|DT_BORDER);

	}
	else
	{
		RECT rc;
/*
		CopyRect(&rc,&obj->rc);
		OffsetRect(&rc,2,2);
		SetBrushColor(hdc,MapRGB(hdc,20,20,20));
		FillRect(hdc,&rc);
*/
		SetTextColor(hdc,MapRGB(hdc,0,200,255));
		SetPenColor(hdc,MapRGB(hdc,0,150,200));
		SetBrushColor(hdc,MapRGB(hdc,0,60,80));
		DrawText(hdc,obj->pszText,-1,&obj->rc,DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_BKGND|DT_BORDER);
	}

	SetFont(hdc,hFont);

}


static void button_owner_draw(DRAWITEM_HDR *ds) //绘制一个按钮外觿
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	hwnd =ds->hwnd; //button的窗口句柿.
	hdc =ds->hDC;   //button的绘图上下文句柄.
	rc =ds->rc;     //button的绘制矩形区.

	SetFont(hdc,hFont24);

	if(ds->State & BST_PUSHED)
	{ //按钮是按下状怿
		SetTextColor(hdc,MapARGB(hdc,255,255,0,255));
		SetPenColor(hdc,MapARGB(hdc,255,200,0,200));
		SetBrushColor(hdc,MapARGB(hdc,150,100,0,100));
	}
	else
	{ //按钮是弹起状怿
		SetBrushColor(hdc,MapARGB(hdc,80,0,150,150));
		SetPenColor(hdc,MapARGB(hdc,200,0,200,250));
		SetTextColor(hdc,MapARGB(hdc,250,200,255,255));
	}

	FillRect(hdc,&rc); //用矩形填充背晿
	DrawRect(hdc,&rc); //画矩形外桿

	GetWindowText(hwnd,wbuf,128); //获得按钮控件的文孿
	DrawText(hdc,wbuf,-1,&rc,DT_VCENTER|DT_CENTER);//绘制文字(居中对齐方式)

}

static void DrawFrame(HDC hdc,const POINT *pt,int count,COLORREF color);

static void DrawFrame(HDC hdc,const POINT *pt,int count,COLORREF color)
{
	int i,x,y;
	WCHAR wbuf[128];
	RECT rc;

	ClrDisplay(hdc,NULL,MapRGB(hdc,0,40,40));


	SetPenColor(hdc,MapRGB(hdc,255,255,0));
	DrawRect(hdc,&rc_wav);

	////Draw Y
	SetPenColor(hdc,MapRGB(hdc,0,60,80));
	SetTextColor(hdc,MapRGB(hdc,0,255,255));
	i=0;

	x =rc_wav.x;
	y =rc_wav.y+rc_wav.h-1;

	rc.x =0;
	rc.y =y-8;
	rc.w =LEFT_OFFSET-2;
	rc.h =20;

	while(y >= rc_wav.y)
	{
		HLine(hdc,x,y,x+rc_wav.w);
		if(i>0)
		{
			x_wsprintf(wbuf,L"%.1f",i*y_step[y_step_cur]*20);
			DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_VCENTER|DT_RIGHT);
		}


		i++;
		y -= 20;
		rc.y -= 20;

	}

	////Draw X
	SetPenColor(hdc,MapRGB(hdc,0,60,80));
	SetTextColor(hdc,MapRGB(hdc,0,255,255));
	i=0;
	x=rc_wav.x;
	y=rc_wav.y;

	rc.x =x-8;
	rc.y =rc_wav.y+rc_wav.h+1;
	rc.w =20;
	rc.h =20;

	while(x < (rc_wav.x+rc_wav.w))
	{
		VLine(hdc,x,y,y+rc_wav.h);
		if(i>0)
		{
			x_wsprintf(wbuf,L"%d",i*x_step[x_step_cur]*20/1000);
			DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_VCENTER|DT_CENTER);
		}

		i++;
		x+=20;
		rc.x+=20;
	}

	/////DrawFrame
	if(count>0)
	{
		int x1,y1,x2,y2;
		SetPenColor(hdc,color);

		for(i=1;i<count;i++)
		{
			x1 = pt[i-1].x + rc_wav.x;
			y1 =-pt[i-1].y + rc_wav.y+rc_wav.h-1;
			x1 =MAX(x1,rc_wav.x);
			x1 =MIN(x1,rc_wav.x+rc_wav.w-1);
			y1 =MAX(y1,rc_wav.y);
			y1 =MIN(y1,rc_wav.y+rc_wav.h-1);

			x2 = pt[i].x + rc_wav.x;
			y2 =-pt[i].y + rc_wav.y+rc_wav.h-1;
			x2 =MAX(x2,rc_wav.x);
			x2 =MIN(x2,rc_wav.x+rc_wav.w-1);
			y2 =MAX(y2,rc_wav.y);
			y2 =MIN(y2,rc_wav.y+rc_wav.h-1);

//			Line(hdc,x1,y1,x2,y2);
			//AA_DrawLine(hdc,x1,y1,x2,y2);
			AA_DrawBoldLine(hdc,x1,y1,x2,y2);
		}
	}

	////Cursor X1
	SetPenColor(hdc,MapRGB(hdc,100,255,200));

	x=x1_cur+rc_wav.x;
	VLine(hdc,x,rc_wav.y-2,rc_wav.y+rc_wav.h-1);

	////Cursor X2
	x=x2_cur+rc_wav.x;
	VLine(hdc,x,rc_wav.y-2,rc_wav.y+rc_wav.h-1);

	////Cursor Y1
	y=rc_wav.y+rc_wav.h-y1_cur;
	HLine(hdc,rc_wav.x,y,rc_wav.x+rc_wav.w-1+2);

	////Cursor Y2
	y=rc_wav.y+rc_wav.h-y2_cur;
	HLine(hdc,rc_wav.x,y,rc_wav.x+rc_wav.w-1+2);



	////button obj
	x_obj_draw(hdc,button_item);
//	x_obj_draw(hdc,text_item);

}



static void wave_owner_draw(HDC hdc)
{
	int x_cur,y_cur;

	x_cur=0;
	while(x_cur < rc_wav.w)
	{
		float mV;

		mV = (1-sin( 2*3.14*(x_cur*1.0/(rc_wav.w/10)) ))*((rc_wav.h)/2);
		y_cur =(int)(mV/y_step[y_step_cur]);

		//y_cur =x_rand()%200;

		pt_wav[x_cur].x =x_cur;
		pt_wav[x_cur].y =y_cur;
		x_cur++;
	}

	DrawFrame(hdc,pt_wav,x_cur,MapRGB(hdc,255,0,0));
}

//波形显示的窗口过稿.
static	LRESULT	WaveWinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;
	switch(msg)
	{
		case	WM_CREATE:
		{
			GetClientRect(hwnd,&rc);
			CopyRect(&rc_wav,&rc);
			InflateRectEx(&rc_wav,-LEFT_OFFSET,-TOP_OFFSET,-RIGHT_OFFSET,-BOTTOM_OFFSET);

			x1_cur=15;
			x2_cur=50;
			y1_cur=30;
			y2_cur=70;
			// x_step=50; //50ms
			y_step_cur=2;
      x_step_cur=0;

			focus_obj =NULL;

			rc_x1_cur.x =rc_wav.x+x1_cur-(CUR_BOX_SIZE/2);
			rc_x1_cur.y =rc_wav.y-CUR_BOX_SIZE;
			rc_x1_cur.w =CUR_BOX_SIZE;
			rc_x1_cur.h =CUR_BOX_SIZE;

			rc_x2_cur.x =rc_wav.x+x2_cur-(CUR_BOX_SIZE/2);
			rc_x2_cur.y =rc_wav.y-CUR_BOX_SIZE;
			rc_x2_cur.w =CUR_BOX_SIZE;
			rc_x2_cur.h =CUR_BOX_SIZE;

			rc_y1_cur.x =rc_wav.x+rc_wav.w;
			rc_y1_cur.y =rc_wav.y+rc_wav.h-y1_cur-(CUR_BOX_SIZE/2);
			rc_y1_cur.w =CUR_BOX_SIZE;
			rc_y1_cur.h =CUR_BOX_SIZE;

			rc_y2_cur.x =rc_wav.x+rc_wav.w;
			rc_y2_cur.y =rc_wav.y+rc_wav.h-y2_cur-(CUR_BOX_SIZE/2);
			rc_y2_cur.w =CUR_BOX_SIZE;
			rc_y2_cur.h =CUR_BOX_SIZE;


			button_item =x_obj_create_class(L"ButtonItem",	0xFFFFFFFF,	&rc,X_OBJ_VISIBLE,0,btn_draw);

			x1_cur_obj	=x_obj_create(L"X1",	ID_X1_CUR,	&rc_x1_cur,	X_OBJ_VISIBLE,0,button_item);
			x2_cur_obj	=x_obj_create(L"X2",	ID_X2_CUR,	&rc_x2_cur,	X_OBJ_VISIBLE,0,button_item);
			y1_cur_obj	=x_obj_create(L"Y1",	ID_Y1_CUR,	&rc_y1_cur,	X_OBJ_VISIBLE,0,button_item);
			y2_cur_obj	=x_obj_create(L"Y2",	ID_Y2_CUR,	&rc_y2_cur,	X_OBJ_VISIBLE,0,button_item);
		}
		return TRUE;
		////

	case    WM_LBUTTONDOWN: //
			{
				int x,y;

				x=GET_LPARAM_X(lParam);
				y=GET_LPARAM_Y(lParam);

				focus_obj =x_obj_get_from_pos(button_item,x,y); //从x,y坐标值获徿 x_obj对象.
				if(focus_obj==NULL)
				{ //没有
					return DefWindowProc(hwnd,msg,wParam,lParam);
				}

				if(focus_obj->id == ID_X1_CUR)
				{
					focus_cur =CUR_X1;
					InvalidateRect(hwnd,&x1_cur_obj->rc,FALSE);
				}

				if(focus_obj->id == ID_X2_CUR)
				{
					focus_cur =CUR_X2;
					InvalidateRect(hwnd,&x2_cur_obj->rc,FALSE);
				}

				if(focus_obj->id == ID_Y1_CUR)
				{
					focus_cur =CUR_Y1;
					InvalidateRect(hwnd,&y1_cur_obj->rc,FALSE);
				}

				if(focus_obj->id == ID_Y2_CUR)
				{
					focus_cur =CUR_Y2;
					InvalidateRect(hwnd,&y2_cur_obj->rc,FALSE);
				}

			}
			return DefWindowProc(hwnd,msg,wParam,lParam);
			////

	case WM_NOTIFY:
	{
		u16 code,id;
		NMHDR *nm=(NMHDR*)lParam;
		HWND wnd;


		code =HIWORD(wParam);
		id =LOWORD(wParam);
		wnd =nm->hwndFrom;


	}
	return DefWindowProc(hwnd,msg,wParam,lParam);
	/////

	case WM_MOUSELEAVE:
	case WM_LBUTTONUP:
			{
				/////
				if(focus_obj!=NULL)
				{
					InvalidateRect(hwnd,&focus_obj->rc,FALSE);
				}
				focus_cur =0;
				focus_obj =NULL;


			}
			return DefWindowProc(hwnd,msg,wParam,lParam);
			////

	case	WM_MOUSEMOVE:
			{
				HWND wnd;

				int x,y;

				x=GET_LPARAM_X(lParam);
				y=GET_LPARAM_Y(lParam);


				x =x-rc_wav.x;
				x=MAX(x,0);
				x=MIN(x,rc_wav.w-1);

				y =y-rc_wav.y;
				y=MAX(y,0);
				y=MIN(y,rc_wav.h-1);

				if(focus_cur == CUR_X1) // X1光标.
				{

					////移动前的位置刷新.
					rc.x =x1_cur + rc_wav.x;
					rc.y =rc_wav.y;
					rc.w =1;
					rc.h =rc_wav.h;
					InvalidateRect(hwnd,&rc,FALSE);
					InvalidateRect(hwnd,&x1_cur_obj->rc,FALSE);

					////移动到新的位罿.
					x1_cur =x;
					x_obj_move_to(x1_cur_obj,(x1_cur+rc_wav.x)-(x1_cur_obj->rc.w/2),x1_cur_obj->rc.y);

					//移动后的位置刷新.
					rc.x =x1_cur + rc_wav.x;
					rc.y =rc_wav.y;
					rc.w =1;
					rc.h =rc_wav.h;
					InvalidateRect(hwnd,&rc,FALSE);
					InvalidateRect(hwnd,&x1_cur_obj->rc,FALSE);


					////给父窗口叿 MSG_CUR_X1_CAHNGLE 消息.
					PostMessage(GetParent(hwnd),MSG_CUR_X1_CHANGE,x1_cur,0);
				}

				if(focus_cur == CUR_X2) // X1光标.
				{


					rc.x =x2_cur + rc_wav.x;
					rc.y =rc_wav.y;
					rc.w =1;
					rc.h =rc_wav.h;
					InvalidateRect(hwnd,&rc,FALSE);
					InvalidateRect(hwnd,&x2_cur_obj->rc,FALSE);

					x2_cur =x;
					x_obj_move_to(x2_cur_obj,(x+rc_wav.x)-(x2_cur_obj->rc.w/2),x2_cur_obj->rc.y);

					rc.x =x2_cur + rc_wav.x;
					rc.y =rc_wav.y;
					rc.w =1;
					rc.h =rc_wav.h;
					InvalidateRect(hwnd,&rc,FALSE);
					InvalidateRect(hwnd,&x2_cur_obj->rc,FALSE);

					////
					PostMessage(GetParent(hwnd),MSG_CUR_X2_CHANGE,x2_cur,0);

				}

				if(focus_cur == CUR_Y1)
				{

					rc.x =rc_wav.x;
					rc.y =rc_wav.y+rc_wav.h-y1_cur;
					rc.w =rc_wav.w;
					rc.h =1;
					InvalidateRect(hwnd,&rc,FALSE);
					InvalidateRect(hwnd,&y1_cur_obj->rc,FALSE);

					y1_cur =rc_wav.h-y;
					x_obj_move_to(y1_cur_obj,y1_cur_obj->rc.x,(y+rc_wav.y)-(y1_cur_obj->rc.h/2));

					rc.x =rc_wav.x;
					rc.y =rc_wav.y+rc_wav.h-y1_cur;
					rc.w =rc_wav.w;
					rc.h =1;
					InvalidateRect(hwnd,&rc,FALSE);
					InvalidateRect(hwnd,&y1_cur_obj->rc,FALSE);


					////
					PostMessage(GetParent(hwnd),MSG_CUR_Y1_CHANGE,y1_cur,0);
				}

				if(focus_cur == CUR_Y2)
				{
					rc.x =rc_wav.x;
					rc.y =rc_wav.y+rc_wav.h-y2_cur;
					rc.w =rc_wav.w;
					rc.h =1;
					InvalidateRect(hwnd,&rc,FALSE);
					InvalidateRect(hwnd,&y2_cur_obj->rc,FALSE);

					y2_cur =rc_wav.h-y;
					x_obj_move_to(y2_cur_obj,y2_cur_obj->rc.x,(y+rc_wav.y)-(y2_cur_obj->rc.h/2));

					rc.x =rc_wav.x;
					rc.y =rc_wav.y+rc_wav.h-y2_cur;
					rc.w =rc_wav.w;
					rc.h =1;
					InvalidateRect(hwnd,&rc,FALSE);
					InvalidateRect(hwnd,&y2_cur_obj->rc,FALSE);


					////
					PostMessage(GetParent(hwnd),MSG_CUR_Y2_CHANGE,y2_cur,0);


				}

			}
			return DefWindowProc(hwnd,msg,wParam,lParam);
			////

	case WM_ERASEBKGND:

		return TRUE;
		////

		case WM_PAINT:
		{
			HDC hdc;
			PAINTSTRUCT ps;
			hdc =BeginPaint(hwnd,&ps);
			wave_owner_draw(hdc);
			EndPaint(hwnd,&ps);
		}
		return TRUE;
		////

		default:
		return DefWindowProc(hwnd,msg,wParam,lParam);
	}
	return WM_NULL;
}


/*============================================================================*/


static	LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;
	HWND wnd;

	switch(msg)
	{

	case WM_CREATE:

			hFont20 =XFT_CreateFont(ASCII_20_4BPP);
			hFont24 =XFT_CreateFont(ASCII_24_4BPP);

			GetClientRect(hwnd,&rc);
			rc_main =rc;


      rc.w =60;
      rc.h =60;
      rc.x = rc_main.w - rc.w;
      rc.y =0;   
            
      /* 关闭按钮 */  
      wnd=CreateWindow(BUTTON,L"C",	BS_FLAT|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_EXIT,NULL,NULL); //创建一个按钿.
      SetWindowFont(wnd,controlFont); //设置控件窗口字体.

      rc.x =rc_main.w-rc.w-20;
     	rc.y =60;
			rc.w =150;
			rc.h =rc_main.h-80;

      CreateWindow(TEXTBOX,L"模拟通道数据",TBS_FLAT|TBS_CENTER|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_TEXT1,NULL,NULL);

      OffsetRect(&rc,0,rc.h+20);
      CreateWindow(TEXTBOX,L"频率，单位Hz",TBS_FLAT|TBS_CENTER|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_TEXT2,NULL,NULL);

      OffsetRect(&rc,0,rc.h);
      {
        SCROLLINFO sif;
        sif.cbSize		=sizeof(sif);
        sif.fMask		=SIF_ALL;
        sif.nMin		=100;
        sif.nMax		=1000;
        //sif.nMin		=-(1020<<20);
        //sif.nMax		=+(1020<<20);
        sif.nValue		=500;
        //sif.nTrackValue	=0;
        sif.TrackSize		=30;
        sif.ArrowSize		=0;//20;

        wnd = CreateWindow(SCROLLBAR,L"HScroll1",WS_OWNERDRAW|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_SCROLL1,NULL,NULL);
        SendMessage(wnd,SBM_SETSCROLLINFO,TRUE,(LPARAM)&sif);
  
        OffsetRect(&rc,0,rc.h+10);
        CreateWindow(TEXTBOX,L"幅值，单位mV",TBS_FLAT|TBS_CENTER|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_TEXT3,NULL,NULL);

        OffsetRect(&rc,0,rc.h);
        wnd = CreateWindow(SCROLLBAR,L"HScroll2",WS_OWNERDRAW|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_SCROLL2,NULL,NULL);
        SendMessage(wnd,SBM_SETSCROLLINFO,TRUE,(LPARAM)&sif);
      }
				
      OffsetRect(&rc,0,rc.h+20);
      rc.x +=5;
      rc.h -=5;
      rc.w = rc.h-2;

      //创建单选按钿(GroupID号为1).
      CreateWindow(BUTTON,L"",BS_RADIOBOX|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,(1<<16)|ID_RB1,NULL,NULL);

      OffsetRect(&rc,rc.w+7,0);
      CreateWindow(BUTTON,L"",BS_RADIOBOX|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,(1<<16)|ID_RB2,NULL,NULL);

      OffsetRect(&rc,rc.w+7,0);
      CreateWindow(BUTTON,L"",BS_RADIOBOX|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,(1<<16)|ID_RB3,NULL,NULL);
      
      OffsetRect(&rc,rc.w+7,0);
      CreateWindow(BUTTON,L"",BS_RADIOBOX|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,(1<<16)|ID_RB4,NULL,NULL);
  
      /* 设置默认按钮 */
      SendMessage(GetDlgItem(hwnd,ID_RB1),BM_SETSTATE,BST_CHECKED,0);

      OffsetRect(&rc,0,rc.h+2);
      CreateWindow(TEXTBOX,L" A  B  C  D",TBS_FLAT|TBS_LEFT|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_TEXT4,NULL,NULL);
     
      rc.w =30;
      rc.h =58;
      rc.x =rc_main.w-rc.w-20;
      rc.y =rc_main.h - rc.h-2;
      x_obj_create(L"+",	ID_Y_STEP_ADD,	&rc,	X_OBJ_VISIBLE,0,button_item);

      rc.w =80;
      OffsetRect(&rc,-(rc.w+2),0);
      //x_obj_create(L"mV",	ID_Y_STEP,		&rc,	X_OBJ_VISIBLE,0,button_item);
      x_obj_create(y_step_str[y_step_cur],ID_Y_STEP,	&rc,	X_OBJ_VISIBLE,0,button_item);

      rc.w =30;
      OffsetRect(&rc,-(rc.w+2),0);
      x_obj_create(L"-",	ID_Y_STEP_SUB,	&rc,	X_OBJ_VISIBLE,0,button_item);
      
      rc.w =30;
      rc.h =58;
      rc.x =rc_main.w-rc.w-20;
      rc.y =rc_main.h - rc.h-2;
      OffsetRect(&rc,0,rc.h+2);
      x_obj_create(L"+",	ID_X_STEP_ADD,	&rc,	X_OBJ_VISIBLE,0,button_item);

      rc.w =80;
      OffsetRect(&rc,-(rc.w+2),0);
      //x_obj_create(L"mV",	ID_Y_STEP,		&rc,	X_OBJ_VISIBLE,0,button_item);
      x_obj_create(x_step_str[x_step_cur],ID_X_STEP,	&rc,	X_OBJ_VISIBLE,0,button_item);

      rc.w =30;
      OffsetRect(&rc,-(rc.w+2),0);
      x_obj_create(L"-",	ID_X_STEP_SUB,	&rc,	X_OBJ_VISIBLE,0,button_item);
      OffsetRect(&rc,-(rc.w+2),0);

			if(1)
			{

				// rc.w =100;
				// rc.h =40;
				// rc.x =rc_main.w-rc.w-2;
				// rc.y =40;

				// //OffsetRect(&rc,-100,0);
				// CreateWindow(BUTTON,L"Exit",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_EXIT,NULL,NULL);

				// OffsetRect(&rc,0,rc.h+4);
				// CreateWindow(BUTTON,L"ON",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_SENSOR_ON,NULL,NULL);

				// OffsetRect(&rc,0,rc.h+4);
				// CreateWindow(BUTTON,L"OFF",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_SENSOR_OFF,NULL,NULL);

				// OffsetRect(&rc,0,rc.h+4);
				// CreateWindow(BUTTON,L"Pump",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_PUMP,NULL,NULL);

				// OffsetRect(&rc,0,rc.h+4);
				// CreateWindow(BUTTON,L"Restart",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_START,NULL,NULL);

				// OffsetRect(&rc,0,rc.h+4);
				// CreateWindow(BUTTON,L"Y+",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_Y_STEP_ADD,NULL,NULL);

				// OffsetRect(&rc,0,rc.h+4);
				// CreateWindow(BUTTON,L"Y-",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_Y_STEP_SUB,NULL,NULL);

				// rc =rc_wav;

				if(1)
				{
					WNDCLASS wcex;

					wcex.Tag	 		= WNDCLASS_TAG;

					wcex.Style			= CS_HREDRAW | CS_VREDRAW;
					wcex.lpfnWndProc	= (WNDPROC)WaveWinProc;
					wcex.cbClsExtra		= 0;
					wcex.cbWndExtra		= 0;
					wcex.hInstance		= NULL;
					wcex.hIcon			= NULL;
					wcex.hCursor		= NULL;

					rc.w =600;
					rc.h =360;
					rc.x =10;
					rc.y =40;
					////创建"波形显示"的控仿.
					CreateWindow(&wcex,L"---",WS_CLIPCHILDREN|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_WAVE,NULL,NULL);

					////创建一丿 TEXTBOX.
					OffsetRect(&rc,0,rc.h+4);
					rc.h =24;
					CreateWindow(TEXTBOX,L"---",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_X_STR,NULL,NULL);

					////创建一丿 TEXTBOX.
					OffsetRect(&rc,0,rc.h);
					CreateWindow(TEXTBOX,L"---",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_Y_STR,NULL,NULL);
				}

			}

			//SetTimer(hwnd,1,100,TMR_START,NULL);
			return TRUE;
			////


	case	WM_NOTIFY:
	{
		U16 code,id;
		code =HIWORD(wParam);
		id   =LOWORD(wParam);

		if(id==ID_EXIT && code==BN_CLICKED)
		{
			PostCloseMessage(hwnd);
		}

		if(id==ID_Y_STEP_ADD && code==BN_CLICKED)
		{
			if(y_step_cur < (Y_STEP_NUM-1))
			{
				y_step_cur++;

				InvalidateRect(GetDlgItem(hwnd,ID_WAVE),NULL,FALSE);
				InvalidateRect(GetDlgItem(hwnd,ID_Y_STR),NULL,FALSE);

			}
		}

		if(id==ID_Y_STEP_SUB && code==BN_CLICKED)
		{

			if(y_step_cur > 0)
			{
				y_step_cur--;

				InvalidateRect(GetDlgItem(hwnd,ID_WAVE),NULL,FALSE);
				InvalidateRect(GetDlgItem(hwnd,ID_Y_STR),NULL,FALSE);
			}

		}

	}
	break;
	/////

	case MSG_CUR_X1_CHANGE:
	case MSG_CUR_X2_CHANGE:
	{
		WCHAR wbuf[128];

		x_wsprintf(wbuf,L"X1: %04dmS, X2: %04dmS, X2-X1: %04dmS",x1_cur*x_step[x_step_cur],x2_cur*x_step[x_step_cur],(x2_cur-x1_cur)*x_step[x_step_cur]);
		SetWindowText(GetDlgItem(hwnd,ID_X_STR),wbuf);
	}
	break;
	////

	case MSG_CUR_Y1_CHANGE:
	case MSG_CUR_Y2_CHANGE:
	{
		WCHAR wbuf[128];

		x_wsprintf(wbuf,L"Y1: %.1fmV, Y2: %.1fmV, Y2-Y1: %.1fmV",y1_cur*y_step[y_step_cur],y2_cur*y_step[y_step_cur],(y2_cur-y1_cur)*y_step[y_step_cur]);
		SetWindowText(GetDlgItem(hwnd,ID_Y_STR),wbuf);
	}
	break;
	////

	case WM_ERASEBKGND:
	{
		HDC hdc =(HDC)wParam;
		RECT rc =*(RECT*)lParam;

		SetBrushColor(hdc,MapRGB(hdc,100,100,150));
		FillRect(hdc,&rc);
	}
	return TRUE;
	////

	case WM_CTLCOLOR:
	{
		int id;
		CTLCOLOR *cr=(CTLCOLOR*)lParam;
		id =wParam;

		if(id==ID_X_STR)
		{
			cr->BackColor   =RGB888(10,20,50);
			cr->BorderColor =RGB888(10,255,50);
			cr->TextColor   =RGB888(10,255,50);
			return TRUE;
		}

		if(id==ID_Y_STR)
		{
			cr->BackColor   =RGB888(10,20,50);
			cr->BorderColor =RGB888(255,200,10);
			cr->TextColor   =RGB888(255,200,10);
			return TRUE;
		}

	}
	return FALSE;
	//////

	case WM_PAINT:
	{
		HDC hdc;
		PAINTSTRUCT ps;
		RECT rc;
		WCHAR wbuf[128];

		hdc =BeginPaint(hwnd,&ps);

	//	GetClientRect(hwnd,&rc);

		EndPaint(hwnd,&ps);
	}
	break;
	////

	case WM_DESTROY:
	{
		x_obj_del(button_item);

		DeleteFont(hFont20);
		DeleteFont(hFont24);

		return DefWindowProc(hwnd,msg,wParam,lParam);
	}
	////

		default:
			return DefWindowProc(hwnd,msg,wParam,lParam);
	}

	return WM_NULL;
}

/*============================================================================*/


extern "C" void	GUI_DEMO_ShowWave2(void)
{
	HWND hwnd;
	MSG	msg;
	WNDCLASS wcex;
	RECT rc;
	////

	ShowCursor(FALSE); //不显示系统光栿.


	wcex.Tag	 		= WNDCLASS_TAG;

	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WinProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;


	rc.x =0;
	rc.y =0;
	rc.w =GUI_XSIZE;
	rc.h =GUI_YSIZE;


	hwnd	=CreateWindowEx(	WS_EX_NODRAG|WS_EX_FRAMEBUFFER,&wcex,
							L"Show Wave-2",
							WS_CLIPCHILDREN,
							rc.x,rc.y,rc.w,rc.h,
							NULL,0,NULL,NULL);


	ShowWindow(hwnd,SW_SHOW);
	UpdateWindow(hwnd);


	ShowWindow(hwnd,SW_SHOW);
	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	ShowCursor(TRUE); //恢复系统光标.

}

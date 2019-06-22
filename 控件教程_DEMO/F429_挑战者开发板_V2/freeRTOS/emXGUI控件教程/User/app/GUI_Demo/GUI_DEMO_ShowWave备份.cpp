

/*============================================================================*/
#include <math.h>


#include "emXGUI.h"
#include "GUI_Font_XFT.h"
#include "x_libc.h"
#include "x_obj.h"

#include "GUI_AppDef.h"

/*============================================================================*/

extern const char ASCII_20_4BPP[];
extern const char ASCII_24_4BPP[];

/*============================================================================*/
#define	BK_W	rc_main.w
#define	BK_H	rc_main.h

#define	LEFT_OFFSET	 84
#define	TOP_OFFSET	 64
#define	RIGHT_OFFSET  (28+180+4)
#define BOTTOM_OFFSET (90)

#define	CUR_BOX_SIZE 30

static POINT pt[800];
static int x_cur,y_cur;

static SURFACE *pSurfTop=NULL;
static HDC hdcTop=NULL;

static HFONT hFont20,hFont24;

static RECT rc_main,rc_wav,rc_button,rc_label,rc_x1_cur,rc_x2_cur,rc_y1_cur,rc_y2_cur;
static int wave_Hz,wave_mV;

enum WAVE_FORMAT{
  SINE_WAVE,
  SQUARE_WAVE,
  TRAINGULAR_WAVE,
  SCATTER,
};

static WAVE_FORMAT wave_format = SINE_WAVE;
// static int x_step;

#define	Y_STEP_NUM	9
#define	X_STEP_NUM	9

static int y_step_cur=0;
static int x_step_cur=0;

// static const float y_step[Y_STEP_NUM]={0.1,0.2,0.5,1.0,2.0,5.0,10.0,20.0,50.0};

// static const WCHAR *y_step_str[Y_STEP_NUM]={
// 	L"0.1mV",
// 	L"0.2mV",
// 	L"0.5mV",
// 	L"1mV",
// 	L"2mV",
// 	L"5mV",
// 	L"10mV",
// 	L"20mV",
// 	L"50mV",

// };

static const float y_step[Y_STEP_NUM]={1.1,1.2,1.3,1.4,1.5,2.0,5.0,10.0,20.0};

static const WCHAR *y_step_str[Y_STEP_NUM]={
	L"1.1mV",
	L"1.2mV",
	L"1.3mV",
	L"1.4mV",
	L"1.5mV",
	L"2.0mV",
	L"5.0mV",
	L"10mV",
	L"20mV",

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

struct	__alco_cfg{
	char tag[8]; //"ALCO.CFG"
	u32 B2_ref;
	u32 B10_ref;
	u32 B2_max;
	u32 B10_max;
};


/*============================================================================*/

enum eID{

	ID_BTN_BEGIN,////////////

	ID_EXIT =1,
	ID_START,
	ID_SENSOR_ON,
	ID_SENSOR_OFF,
	ID_Y_STEP,
  ID_X_STEP,
	ID_PUMP,

	ID_BTN_END,//////////////

	ID_LISTBOX,

	ID_X1_CUR,
	ID_X2_CUR,
	ID_Y1_CUR,
	ID_Y2_CUR,

	ID_Y_STEP_SUB,
	ID_Y_STEP_ADD,

  ID_X_STEP_SUB,
	ID_X_STEP_ADD,

	ID_TEST,

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
static struct __x_obj_item *text_item =NULL;

static struct __x_obj_item *x1_cur_obj =NULL;
static struct __x_obj_item *x2_cur_obj =NULL;
static struct __x_obj_item *y1_cur_obj =NULL;
static struct __x_obj_item *y2_cur_obj =NULL;

/*============================================================================*/
/**
  * @brief  主函数
  * @param  back_c 背景颜色
  * @param  Page_c 滚动条颜色
  * @param  fore_c 滑块颜色  * 
  * @retval 无
  */
static void draw_scrollbar(HWND hwnd, HDC hdc, COLOR_RGB32 back_c, COLOR_RGB32 Page_c,  COLOR_RGB32 fore_c)
{
   RECT rc;
   GetClientRect(hwnd,&rc);

  /* 背景 */
   SetBrushColor(hdc,MapRGB888(hdc, back_c)); 
   FillRect(hdc,&rc);

  /* 滚动条 */
  /* 边框 */
  InflateRect(&rc,0,-rc.h>>2);
	SetBrushColor(hdc,MapRGB(hdc,169,169,169));  
	FillRoundRect(hdc,&rc,MIN(rc.w,rc.h)>>1);
  
  InflateRect(&rc,-2,-2);  
	SetBrushColor(hdc,MapRGB888(hdc,Page_c));  
	FillRoundRect(hdc,&rc,MIN(rc.w,rc.h)>>1);
    
  /* 滑块 */
   SendMessage(hwnd,SBM_GETTRACKRECT,0,(LPARAM)&rc);
	// sif.fMask =SIF_ALL;
	// SendMessage(hwnd,SBM_GETSCROLLINFO,0,(LPARAM)&sif);
	SetBrushColor(hdc,MapRGB(hdc,169,169,169));
    rc.y += (rc.h>>2)>>1;
    rc.h -= rc.h>>2;
    /* 边框 */
   FillRoundRect(hdc,&rc,MIN(rc.w,rc.h)>>2); 
   InflateRect(&rc,-2,-2);
   
   SetBrushColor(hdc,MapRGB888(hdc,	fore_c));   
   FillRoundRect(hdc,&rc,MIN(rc.w,rc.h)>>2);
}


static void scrollbar_owner_draw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
   HDC hdc_mem;
   HDC hdc_mem1;
	RECT rc;
   RECT rc_cli;
//	int i;
   
	hwnd =ds->hwnd;
	hdc =ds->hDC;
	GetClientRect(hwnd,&rc_cli);
   
   hdc_mem = CreateMemoryDC(SURF_SCREEN, rc_cli.w, rc_cli.h);
   hdc_mem1 = CreateMemoryDC(SURF_SCREEN, rc_cli.w, rc_cli.h);
   
   //右
	draw_scrollbar(hwnd, hdc_mem1, RGB888(0,0,0), RGB888(255,255,255),RGB888(255,255,255));
   //左
   draw_scrollbar(hwnd, hdc_mem, RGB888(0,0,0), RGB888(90,174,22), RGB888(90,174,22));
   
	SendMessage(hwnd,SBM_GETTRACKRECT,0,(LPARAM)&rc);

   //右
   BitBlt(hdc, rc_cli.x,rc_cli.y,rc.x, rc_cli.h, hdc_mem, 0, 0,SRCCOPY); 
   //左
   BitBlt(hdc, rc.x+rc.w ,0, rc_cli.w-(rc.x+rc.w), rc_cli.h, hdc_mem1, rc.x+rc.w, 0,SRCCOPY); 
   
   
      
	if(ds->State & SST_THUMBTRACK)
	{	
    	BitBlt(hdc, rc.x ,0, rc.w, rc_cli.h, hdc_mem1, rc.x, 0,SRCCOPY);
	}
	else
	{	
    	BitBlt(hdc, rc.x ,0, rc.w, rc_cli.h, hdc_mem, rc.x, 0,SRCCOPY);
	}
   
   
   
   DeleteDC(hdc_mem1);
   DeleteDC(hdc_mem);
}

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
    /* 阴影 */
		CopyRect(&rc,&obj->rc);
		OffsetRect(&rc,2,2);
		SetBrushColor(hdc,MapRGB(hdc,20,20,20));
		FillRect(hdc,&rc);

		SetTextColor(hdc,MapRGB(hdc,255,255,255));
	  SetPenColor(hdc,MapRGB(hdc,255,255,255));
	  SetBrushColor(hdc,MapRGB(hdc,90,174,22));
		DrawText(hdc,obj->pszText,-1,&obj->rc,DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_BKGND|DT_BORDER);
	}

	SetFont(hdc,hFont);

}

static	void text_draw(HDC hdc,struct __x_obj_item * obj)
{
	HDC hdcTop;

	RECT rc;


	hdcTop =CreateMemoryDC(SURF_ARGB4444,obj->rc.w,obj->rc.h);
	rc =obj->rc;

	SetFont(hdcTop,hFont20);
	SetPenColor(hdcTop,MapARGB(hdcTop,200,200,255,200));
	SetBrushColor(hdcTop,MapARGB(hdcTop,80,50,255,50));
	SetTextColor(hdcTop,MapARGB(hdcTop,250,200,255,200));

	rc.x =0;
	rc.y =0;
	DrawText(hdcTop,obj->pszText,-1,&rc,DT_VCENTER|DT_CENTER|DT_BKGND|DT_BORDER);
	BitBlt(hdc,obj->rc.x,obj->rc.y,obj->rc.w,obj->rc.h,hdcTop,0,0,SRCCOPY);
	DeleteDC(hdcTop);
}

static void button_owner_draw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	hwnd =ds->hwnd; //button的窗口句柄.
	hdc =ds->hDC;   //button的绘图上下文句柄.
	rc =ds->rc;     //button的绘制矩形区.

	SetFont(hdc,hFont24);

	if(ds->State & BST_PUSHED)
	{ //按钮是按下状态
		SetTextColor(hdc,MapARGB(hdc,255,255,0,255));
		SetPenColor(hdc,MapARGB(hdc,255,200,0,200));
		SetBrushColor(hdc,MapARGB(hdc,150,100,0,100));
	}
	else
	{ //按钮是弹起状态
		SetBrushColor(hdc,MapARGB(hdc,80,0,150,150));
		SetPenColor(hdc,MapARGB(hdc,200,0,200,250));
		SetTextColor(hdc,MapARGB(hdc,250,200,255,255));
	}

	FillRect(hdc,&rc); //用矩形填充背景
	DrawRect(hdc,&rc); //画矩形外框

	GetWindowText(hwnd,wbuf,128); //获得按钮控件的文字
	DrawText(hdc,wbuf,-1,&rc,DT_VCENTER|DT_CENTER);//绘制文字(居中对齐方式)

}

static void listbox_owner_draw_1(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	int i,count,cursel;
	WCHAR wbuf[128];

	hwnd =ds->hwnd;
	hdc =ds->hDC;

	SetBrushColor(hdc,MapARGB(hdc,90,50,100,200)); //用单色填充整个listbox背景.
	FillRect(hdc,&ds->rc);

	i=SendMessage(hwnd,LB_GETTOPINDEX,0,0);    //获得第一个可见Item项索引。
	count=SendMessage(hwnd,LB_GETCOUNT,0,0);   //获得总的Item项总数。
	cursel=SendMessage(hwnd,LB_GETCURSEL,0,0); //获得当前选中的Item项索引。

	while(i < count) //循环绘制.
	{
		SendMessage(hwnd,LB_GETITEMRECT,i,(LPARAM)&rc); //获得指定Item项的矩形位置参数.
		if(rc.y > ds->rc.h) //如果是超出listbox客户区的Item项不绘制了，直接完成退出.
		{
			break;
		}

		if(i==cursel)
		{ // 绘制当前选中的Item项。

			SetBrushColor(hdc,MapARGB(hdc,200,240,100,0));
			FillRect(hdc,&rc);

			SetTextColor(hdc,MapRGB(hdc,255,255,255));
			SetFont(hdc,hFont24);
		}
		else
		{// 绘制未选中的Item项。

			SetTextColor(hdc,MapRGB(hdc,100,200,255));
			SetFont(hdc,hFont20);
		}

		SendMessage(hwnd,LB_GETTEXT,i,(LPARAM)wbuf);
		DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_LEFT|DT_VCENTER);

		i++; //下一个Item项索引值
	}
}

/**
  * @brief  绘制示波器中间的窗口
  */
static void DrawFrame(HDC hdc,const POINT *pt,int count,COLORREF color)
{
	int i,x,y;
	WCHAR wbuf[128];
	RECT rc,m_rc[8];

	ClrDisplay(hdc,NULL,MapRGB(hdc,0,0,0));

	ClrDisplay(hdc,&rc_wav,MapRGB(hdc,0,40,40));

	rc.x =0;
	rc.y =0;
	rc.w =rc_main.w;
	rc.h =30;
//	SetBrushColor(hdc,MapRGB(hdc,0,128,0));
//	FillRect(hdc,&rc);
	SetTextColor(hdc,MapRGB(hdc,255,255,255));
	DrawText(hdc,L"波形显示",-1,&rc,DT_SINGLELINE|DT_VCENTER|DT_CENTER);


//	SetPenColor(hdc,MapRGB(hdc,255,255,0));
//	DrawRect(hdc,&rc_wav);

	////Draw Y
	SetPenColor(hdc,MapRGB(hdc,0,60,80));
	SetTextColor(hdc,MapRGB(hdc,255,255,255));
	i=0;

	x =rc_wav.x;
	y =rc_wav.y+rc_wav.h-1;

	rc.x =0;
	rc.y =y-8;
	rc.w =LEFT_OFFSET-2;
	rc.h =40;

	while(y >= rc_wav.y)
	{
    /* 不画第一条线，看起来更好看 */
    if(i!=0)  
		  HLine(hdc,x,y,x+rc_wav.w);

		if(i>0)
		{
			x_wsprintf(wbuf,L"%.1f",i*y_step[y_step_cur]*40);
			DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_TOP|DT_RIGHT);
		}


		i++;
		y -= 40;
		rc.y -= 40;

	}

	////Draw X
	SetPenColor(hdc,MapRGB(hdc,0,60,80));
	SetTextColor(hdc,MapRGB(hdc,255,255,255));
	i=0;
	x=rc_wav.x;
	y=rc_wav.y;

	rc.x =x-8;
	rc.y =rc_wav.y+rc_wav.h+1;
	rc.w =40;
	rc.h =20;

	while(x < (rc_wav.x+rc_wav.w))
	{
     /* 不画第一条线，看起来更好看 */
    if(i!=0)
		  VLine(hdc,x,y,y+rc_wav.h);

		if(i>0)
		{
			x_wsprintf(wbuf,L"%d",i*x_step[x_step_cur]*40/1000);
			DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_VCENTER|DT_LEFT);
		}

		i++;
		x+=40;
		rc.x+=40;
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

			//Line(hdc,x1,y1,x2,y2);
			//AA_DrawLine(hdc,x1,y1,x2,y2);
			AA_DrawBoldLine(hdc,x1,y1,x2,y2);
		}
	}

	////Cursor X1
  SetPenColor(hdc,MapRGB(hdc,90,174,22));

	x=x1_cur+rc_wav.x;
	VLine(hdc,x,rc_wav.y-2,rc_wav.y+rc_wav.h-1);

	////Cursor X2
	x=x2_cur+rc_wav.x;
	VLine(hdc,x,rc_wav.y-2,rc_wav.y+rc_wav.h-1);

	////Cursor Y1
  SetPenColor(hdc,MapRGB(hdc,238,206,31));

	y=rc_wav.y+rc_wav.h-y1_cur;
	HLine(hdc,rc_wav.x,y,rc_wav.x+rc_wav.w-1+2);

	////Cursor Y2
	y=rc_wav.y+rc_wav.h-y2_cur;
	HLine(hdc,rc_wav.x,y,rc_wav.x+rc_wav.w-1+2);


	MakeMatrixRect(m_rc,&rc_label,20,2,3,2);

	////X Text
	SetTextColor(hdc,MapRGB(hdc,255,255,255));
	SetPenColor(hdc,MapRGB(hdc,255,255,255));
	SetBrushColor(hdc,MapRGB(hdc,90,174,22));

	x_wsprintf(wbuf,L"X1:%dmS",x1_cur*x_step[x_step_cur]);
	DrawText(hdc,wbuf,-1,&m_rc[0],DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_BKGND|DT_BORDER);

	x_wsprintf(wbuf,L"X2:%dmS",x2_cur*x_step[x_step_cur]);
	DrawText(hdc,wbuf,-1,&m_rc[1],DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_BKGND|DT_BORDER);

	x_wsprintf(wbuf,L"X2-X1:%dmS",(x2_cur-x1_cur)*x_step[x_step_cur]);
	DrawText(hdc,wbuf,-1,&m_rc[2],DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_BKGND|DT_BORDER);

	////Y Text
	SetTextColor(hdc,MapRGB(hdc,255,255,255));
	SetPenColor(hdc,MapRGB(hdc,255,255,255));
	SetBrushColor(hdc,MapRGB(hdc,90,174,22));

	x_wsprintf(wbuf,L"Y1:%.1fmV",y1_cur*y_step[y_step_cur]);
	DrawText(hdc,wbuf,-1,&m_rc[3],DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_BKGND|DT_BORDER);

	x_wsprintf(wbuf,L"Y2:%.1fmV",y2_cur*y_step[y_step_cur]);
	DrawText(hdc,wbuf,-1,&m_rc[4],DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_BKGND|DT_BORDER);

	x_wsprintf(wbuf,L"Y2-Y1:%.1fmV",(y2_cur-y1_cur)*y_step[y_step_cur]);
	DrawText(hdc,wbuf,-1,&m_rc[5],DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_BKGND|DT_BORDER);

	////button obj
	x_obj_draw(hdc,button_item);
	x_obj_draw(hdc,text_item);

}

/*============================================================================*/
static float ADS1120_GetVoltage_mV(u16 addata);

static int move_id[5]=
{
		ID_EXIT ,
		ID_START,
		ID_SENSOR_ON,
		ID_SENSOR_OFF,
		ID_PUMP,

};


static int move_speed[5]=
{
		1*15 ,
		2*15,
		3*15,
		4*15,
		5*15,

};


int bMove=0;

static	LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;

	switch(msg)
	{
	case WM_CREATE:
			x_cur=0;
			y_cur=0;

			x1_cur=15;
			x2_cur=50;
			y1_cur=30;
			y2_cur=70;
			// x_step=50; //50ms
			y_step_cur=2;
      x_step_cur=0;

			bMove=0;

			focus_obj =NULL;

			hFont20 =XFT_CreateFont(ASCII_20_4BPP);
			hFont24 =XFT_CreateFont(ASCII_24_4BPP);

			GetClientRect(hwnd,&rc);
			// pSurfTop =CreateSurface(SURF_ARGB4444,rc.w,rc.h,NULL,0);
			// hdcTop =CreateDC(pSurfTop,NULL);
			// ClrDisplay(hdcTop,NULL,MapARGB(hdcTop,0,0,0,0));


			GetClientRect(hwnd,&rc_main);
			CopyRect(&rc_wav,&rc_main);
			InflateRectEx(&rc_wav,-LEFT_OFFSET,-TOP_OFFSET,-RIGHT_OFFSET,-BOTTOM_OFFSET);

			rc_x1_cur.x =rc_wav.x+x1_cur-CUR_BOX_SIZE/2;
			rc_x1_cur.y =rc_wav.y-CUR_BOX_SIZE;
			rc_x1_cur.w =CUR_BOX_SIZE;
			rc_x1_cur.h =CUR_BOX_SIZE;

			rc_x2_cur.x =rc_wav.x+x2_cur-CUR_BOX_SIZE/2;
			rc_x2_cur.y =rc_wav.y-CUR_BOX_SIZE;
			rc_x2_cur.w =CUR_BOX_SIZE;
			rc_x2_cur.h =CUR_BOX_SIZE;

			rc_y1_cur.x =rc_wav.x+rc_wav.w;
			rc_y1_cur.y =rc_wav.y+rc_wav.h-y1_cur-CUR_BOX_SIZE/2;
			rc_y1_cur.w =CUR_BOX_SIZE;
			rc_y1_cur.h =CUR_BOX_SIZE;

			rc_y2_cur.x =rc_wav.x+rc_wav.w;
			rc_y2_cur.y =rc_wav.y+rc_wav.h-y2_cur-CUR_BOX_SIZE/2;
			rc_y2_cur.w =CUR_BOX_SIZE;
			rc_y2_cur.h =CUR_BOX_SIZE;
      
      /* 底部的标签栏 */
			rc_label.x =LEFT_OFFSET-20;
			rc_label.y =rc_wav.y+rc_wav.h+26;
			rc_label.w =rc_main.w-LEFT_OFFSET-RIGHT_OFFSET+40;
			rc_label.h =BOTTOM_OFFSET-30-2;

			rc_button.w =150;
			rc_button.h =rc_main.h-80;
			rc_button.x =rc_main.w-RIGHT_OFFSET+10;
			rc_button.y =60;

			if(1)
			{
				int i=0;
				RECT rc,m_rc[8],cb_rc;
				HWND wnd;

        rc.w =60;
        rc.h =60;
        rc.x = rc_main.w - rc.w;
        rc.y =0;   
             
        /* 关闭按钮 */  
        wnd=CreateWindow(BUTTON,L"C",	BS_FLAT|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_EXIT,NULL,NULL); //创建一个按钮.
        SetWindowFont(wnd,controlFont); //设置控件窗口字体.

				MakeMatrixRect(m_rc,&rc_button,2,20,1,7);
        
        /* 飞出的按钮 */
				button_item =x_obj_create_class(L"ButtonItem",	0xFFFFFFFF,	&rc_button,X_OBJ_VISIBLE,0,btn_draw);
				text_item =x_obj_create_class(L"TextItem",	0xFFFFFFFF,	&rc_button,X_OBJ_VISIBLE,0,text_draw);

				rc =m_rc[0];
				//OffsetRect(&rc,-100,0);

				rc.x =rc_main.w-rc.w-20;

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
        CopyRect(&cb_rc,&rc);
        cb_rc.x +=5;
        cb_rc.h -=5;
        cb_rc.w = cb_rc.h-2;

      //创建单选按钮(GroupID号为1).
				CreateWindow(BUTTON,L"",BS_RADIOBOX|WS_VISIBLE,cb_rc.x,cb_rc.y,cb_rc.w,cb_rc.h,hwnd,(1<<16)|ID_RB1,NULL,NULL);

        OffsetRect(&cb_rc,cb_rc.w+7,0);
        CreateWindow(BUTTON,L"",BS_RADIOBOX|WS_VISIBLE,cb_rc.x,cb_rc.y,cb_rc.w,cb_rc.h,hwnd,(1<<16)|ID_RB2,NULL,NULL);

        OffsetRect(&cb_rc,cb_rc.w+7,0);
        CreateWindow(BUTTON,L"",BS_RADIOBOX|WS_VISIBLE,cb_rc.x,cb_rc.y,cb_rc.w,cb_rc.h,hwnd,(1<<16)|ID_RB3,NULL,NULL);
        
        OffsetRect(&cb_rc,cb_rc.w+7,0);
        CreateWindow(BUTTON,L"",BS_RADIOBOX|WS_VISIBLE,cb_rc.x,cb_rc.y,cb_rc.w,cb_rc.h,hwnd,(1<<16)|ID_RB4,NULL,NULL);
	  
        /* 设置默认按钮 */
    		SendMessage(GetDlgItem(hwnd,ID_RB1),BM_SETSTATE,BST_CHECKED,0);

        OffsetRect(&rc,0,rc.h+2);
				CreateWindow(TEXTBOX,L" A  B  C  D",TBS_FLAT|TBS_LEFT|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_TEXT4,NULL,NULL);


#if 0
				CreateWindow(BUTTON,L"Exit",WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_EXIT,NULL,NULL);

				OffsetRect(&rc,0,rc.h+4);
				CreateWindow(BUTTON,L"ON",WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_SENSOR_ON,NULL,NULL);

				OffsetRect(&rc,0,rc.h+4);
				CreateWindow(BUTTON,L"OFF",WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_SENSOR_OFF,NULL,NULL);

				OffsetRect(&rc,0,rc.h+4);
				CreateWindow(BUTTON,L"Pump",WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_PUMP,NULL,NULL);

				OffsetRect(&rc,0,rc.h+4);
				CreateWindow(BUTTON,L"Restart",WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_START,NULL,NULL);
#endif 
/* 列表框 */
#if 0
				rc.x =500;
				rc.y =100;
				rc.w =150;
				rc.h =200;
				wnd=CreateWindow(LISTBOX,_T("Listbox2"),WS_TRANSPARENT|WS_OWNERDRAW|LBS_LINE|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_LISTBOX,NULL,NULL);
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"10HZ");
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"15HZ");
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"20HZ");
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"25HZ");
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"30HZ");
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"35HZ");
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"40HZ");
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"45HZ");
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"50HZ");
#endif
				x1_cur_obj	=x_obj_create(L"X1",	ID_X1_CUR,	&rc_x1_cur,	X_OBJ_VISIBLE,0,button_item);
				x2_cur_obj	=x_obj_create(L"X2",	ID_X2_CUR,	&rc_x2_cur,	X_OBJ_VISIBLE,0,button_item);
				y1_cur_obj	=x_obj_create(L"Y1",	ID_Y1_CUR,	&rc_y1_cur,	X_OBJ_VISIBLE,0,button_item);
				y2_cur_obj	=x_obj_create(L"Y2",	ID_Y2_CUR,	&rc_y2_cur,	X_OBJ_VISIBLE,0,button_item);

	    //   MakeMatrixRect(m_rc,&rc_label,20,2,3,2);


				rc.w =30;
				rc.h =rc_label.h>>1;
				rc.x =rc_main.w-rc.w-20;
				rc.y =rc_label.y;
				x_obj_create(L"+",	ID_Y_STEP_ADD,	&rc,	X_OBJ_VISIBLE,0,button_item);

				rc.w =80;
				OffsetRect(&rc,-(rc.w+2),0);
				//x_obj_create(L"mV",	ID_Y_STEP,		&rc,	X_OBJ_VISIBLE,0,button_item);
				x_obj_create(y_step_str[y_step_cur],ID_Y_STEP,	&rc,	X_OBJ_VISIBLE,0,button_item);

				rc.w =30;
				OffsetRect(&rc,-(rc.w+2),0);
				x_obj_create(L"-",	ID_Y_STEP_SUB,	&rc,	X_OBJ_VISIBLE,0,button_item);
				
				rc.w =30;
				rc.h =rc_label.h>>1;
				rc.x =rc_main.w-rc.w-20;
				rc.y =rc_label.y;
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
/* 界面文字 */
#if 0
				/////
				rc.x =100;
				rc.y =100;
				rc.w =150;
				rc.h =80;
				x_obj_create(L"Text-1",	ID_TEST+1,	&rc,	X_OBJ_VISIBLE,0,text_item);

				OffsetRect(&rc,0,rc.h+8);
				x_obj_create(L"Text-2",	ID_TEST+2,	&rc,	X_OBJ_VISIBLE,0,text_item);
#endif
			}


			// SetTimer(hwnd,1,10,TMR_START,NULL);
			return TRUE;
			////

	case	WM_TIMER:
	{
		HWND wnd;
		RECT rc;
		int i;

		if(bMove == 1)
		{
			for(i=0;i<5;i++)
			{
				wnd =GetDlgItem(hwnd,move_id[i]);
				if(wnd!=NULL)
				{

					GetWindowRect(wnd,&rc);

					if(rc.x > 250)
					{
						ScreenToClient(hwnd,(POINT*)&rc,1);
						SetBrushColor(hdcTop,MapARGB(hdcTop,0,0,0,0));
						FillRect(hdcTop,&rc);

						OffsetRect(&rc,-move_speed[i],0);
						MoveWindow(wnd,rc.x,rc.y,rc.w,rc.h,TRUE);
						UpdateWindow(wnd);

					}
					else if(rc.x > 200)
					{
						ScreenToClient(hwnd,(POINT*)&rc,1);
						SetBrushColor(hdcTop,MapARGB(hdcTop,0,0,0,0));
						FillRect(hdcTop,&rc);

						OffsetRect(&rc,-1,0);
						MoveWindow(wnd,rc.x,rc.y,rc.w,rc.h,TRUE);
						UpdateWindow(wnd);
					}

				}
				////
			}
			//InvalidateRect(hwnd,NULL,FALSE);
		}

		if(bMove == -1)
		{
			for(i=0;i<5;i++)
			{
				wnd =GetDlgItem(hwnd,move_id[i]);
				if(wnd!=NULL)
				{

					GetWindowRect(wnd,&rc);
					if(rc.x < 700)
					{
						ScreenToClient(hwnd,(POINT*)&rc,1);
						SetBrushColor(hdcTop,MapARGB(hdcTop,0,0,0,0));
						FillRect(hdcTop,&rc);

						OffsetRect(&rc,move_speed[i],0);
						MoveWindow(wnd,rc.x,rc.y,rc.w,rc.h,TRUE);
						UpdateWindow(wnd);
					}
					else if(rc.x < 750)
					{
						ScreenToClient(hwnd,(POINT*)&rc,1);
						SetBrushColor(hdcTop,MapARGB(hdcTop,0,0,0,0));
						FillRect(hdcTop,&rc);

						OffsetRect(&rc,1,0);
						MoveWindow(wnd,rc.x,rc.y,rc.w,rc.h,TRUE);
						UpdateWindow(wnd);
					}


				}
				////
			}
			//InvalidateRect(hwnd,NULL,FALSE);
		}


/*
		if(x_cur < rc_wav.w)
		{
			float mV;
			mV=ADS1120_GetVoltage_mV(0);

			//mV =sin(2*3.14*(x_cur/rc_wave.w))*10+50;


			mV = (1-sin( 2*3.14*(x_cur*1.0/(rc_wav.w/10)) ))*((rc_wav.h)/2);
			y_cur =(int)(mV/y_step[y_step_cur]);

			//y_cur =x_rand()%200;

			pt[x_cur].x =x_cur;
			pt[x_cur].y =y_cur;
			x_cur++;
			InvalidateRect(hwnd,NULL,FALSE);
		}
		*/

	}
	break;
	////

  case WM_CTLCOLOR:
		{
			u16 id;

			id =LOWORD(wParam);			
      if( id == ID_EXIT)
      {
        CTLCOLOR *cr;
        cr =(CTLCOLOR*)lParam;

        if(	SendMessage(GetDlgItem(hwnd,id),BM_GETSTATE,0,0)&BST_PUSHED)
        {
          cr->TextColor =RGB888(255,255,255);
          cr->BackColor =RGB888_COLOR_INVALID;
          cr->BorderColor =RGB888(50,50,50);
        }          
        else
        {
          cr->TextColor =RGB888(255,255,255);
          cr->BackColor =RGB888_COLOR_DESKTOP_BACK_GROUND;
          cr->BorderColor =RGB888(50,50,50);
        }
				return TRUE;
      }
      else if(id >= ID_TEXT1 && id <= ID_TEXT4)
      {
        CTLCOLOR *cr;
        cr =(CTLCOLOR*)lParam;

        cr->TextColor =RGB888(255,255,255);
        cr->BackColor =RGB888(0,0,0);
        cr->BorderColor =RGB888(0,0,0);
         
				return TRUE;
      }
      else if(id >= ID_RB1 && id <= ID_RB4)
      {
        CTLCOLOR *cr;
        cr =(CTLCOLOR*)lParam;

				if(SendMessage(GetDlgItem(hwnd,id),BM_GETSTATE,0,0)&BST_CHECKED)
				{
					cr->TextColor =RGB888(0,0,0);      //文字颜色（RGB32颜色格式)
					cr->BackColor =RGB888(220,200,200);    //背景颜色（RGB32颜色格式)
					cr->BorderColor =RGB888(30,30,230);  //边框颜色（RGB32颜色格式)
					cr->ForeColor =RGB888(90,174,22);  //前景颜色（RGB32颜色格式)
				}
				else
				{
					cr->TextColor =RGB888(0,0,0);
					cr->BackColor =RGB888(200,220,200);
					cr->BorderColor =RGB888(50,50,50);
					cr->ForeColor =RGB888(180,200,230);
				}
         
				return TRUE;
      }
			else
			{
				//其它按钮使用系统默认的颜色进行绘制，所以直接返回FALSE.
				return FALSE;
			}
		}

	

	case	WM_NOTIFY:
	{
		U16 code,id;
    NMHDR *nr;

		code =HIWORD(wParam);
		id   =LOWORD(wParam);
    nr =(NMHDR*)lParam; //lParam参数，是以NMHDR结构体开头.

		if(id==ID_EXIT && code==BN_CLICKED)
		{
			PostCloseMessage(hwnd);
		}

		else if(id==ID_SENSOR_ON && code==BN_CLICKED)
		{
			bMove = 1;
		}

		else if(id==ID_SENSOR_OFF && code==BN_CLICKED)
		{
			bMove = -1;
		}
    else if(id == ID_SCROLL1 || id == ID_SCROLL2 )
    {
      NM_SCROLLBAR *sb_nr;
      int i;

      sb_nr =(NM_SCROLLBAR*)nr; //Scrollbar的通知消息实际为 NM_SCROLLBAR扩展结构,里面附带了更多的信息.
      switch(nr->code)
      {
        // case SBN_CLICKED: //单击
        // {
        //   if(sb_nr->cmd==SB_TRACK) //NM_SCROLLBAR结构体成员cmd指明了单击发生的位置
        //   {//在滑块内单击.
        //     GUI_DEBUG("SCROLLBAR CLICK In Track.\r\n");
        //   }
        //   else
        //   {
        //     GUI_DEBUG("SCROLLBAR CLICK :%d.\r\n",sb_nr->cmd);
        //   }
        // }
        // break;
        case SBN_THUMBTRACK: //滑块移动
        {

          i =sb_nr->nTrackValue; //获得滑块当前位置值
          if(id == ID_SCROLL1)
            wave_Hz = i;
          else if(id == ID_SCROLL2)
            wave_mV = i;

          SendMessage(nr->hwndFrom,SBM_SETVALUE,TRUE,i); //设置位置值
          // GUI_DEBUG("SCROLLBAR TRACK :%d.\r\n",i);
        }
        break;
        default:
          break;
      }
    }
    else if(id >= ID_RB1 && id<= ID_RB4)
		{
			if(code == BN_CLICKED) //被点击了
			{
        switch(id)
        {
          case ID_RB1:
            wave_format = SINE_WAVE;
            break;
          case ID_RB2:
            wave_format = SQUARE_WAVE;
            break;
          case ID_RB3:
            wave_format = TRAINGULAR_WAVE;
            break;
          case ID_RB4:
            wave_format = SCATTER;
            break;
          default:
            wave_format = SINE_WAVE;
            break;
        }	
                //单选框选中.
        GUI_DEBUG("wave_format:%d",wave_format);		
	
			}
		}
	}
	break;
	/////

	case    WM_LBUTTONDOWN: //
			{
				int x,y;


				x=GET_LPARAM_X(lParam);
				y=GET_LPARAM_Y(lParam);

				focus_obj =x_obj_get_from_pos(button_item,x,y); //从x,y坐标值获得 x_obj对象.
				if(focus_obj==NULL)
				{ //没有
					return DefWindowProc(hwnd,msg,wParam,lParam);
				}

				else if(focus_obj->id == ID_X1_CUR)
				{
					focus_cur =CUR_X1;
				}

				else if(focus_obj->id == ID_X2_CUR)
				{
					focus_cur =CUR_X2;
				}   

				else if(focus_obj->id == ID_Y1_CUR)
				{
					focus_cur =CUR_Y1;
				}

				else if(focus_obj->id == ID_Y2_CUR)
				{
					focus_cur =CUR_Y2;
				}

				else if(focus_obj->id == ID_START)
				{////重新开始
					x_cur =0;
				}

				else if(focus_obj->id == ID_Y_STEP)
				{
					y_step_cur++;
					if(y_step_cur>=Y_STEP_NUM)
					{
						y_step_cur=0;
					}
					x_obj_set_text(x_obj_get_from_id(button_item,ID_Y_STEP),y_step_str[y_step_cur]);
					////重新开始
					x_cur =0;

				}

				else if(focus_obj->id == ID_SENSOR_ON)
				{

				}

				else if(focus_obj->id == ID_SENSOR_OFF)
				{

				}

				else if(focus_obj->id == ID_Y_STEP_SUB)
				{
					y_step_cur = MAX(y_step_cur-1,0);
					x_obj_set_text(x_obj_get_from_id(button_item,ID_Y_STEP),y_step_str[y_step_cur]);
					////重新开始
					x_cur =0;
				}

				else if(focus_obj->id == ID_Y_STEP_ADD)
				{
					y_step_cur = MIN(y_step_cur+1,Y_STEP_NUM-1);
					x_obj_set_text(x_obj_get_from_id(button_item,ID_Y_STEP),y_step_str[y_step_cur]);
					////重新开始
					x_cur =0;
				}

        else if(focus_obj->id == ID_X_STEP_SUB)
				{
					x_step_cur = MAX(x_step_cur-1,0);
					x_obj_set_text(x_obj_get_from_id(button_item,ID_X_STEP),x_step_str[x_step_cur]);
					////重新开始
					x_cur =0;
				}

				else if(focus_obj->id == ID_X_STEP_ADD)
				{
					x_step_cur = MIN(x_step_cur+1,X_STEP_NUM-1);
					x_obj_set_text(x_obj_get_from_id(button_item,ID_X_STEP),x_step_str[x_step_cur]);
					////重新开始
					x_cur =0;
				}
   

				InvalidateRect(hwnd,NULL,TRUE);
			}
			return DefWindowProc(hwnd,msg,wParam,lParam);
			////

	case WM_LBUTTONUP:
			{
				int x,y;

				struct __x_obj_item *obj;

				x=GET_LPARAM_X(lParam);
				y=GET_LPARAM_Y(lParam);

				obj =x_obj_get_from_pos(button_item,x,y);
				if(obj!=NULL)
				{
					if(obj->id == focus_obj->id)
					{
						if(obj->id == ID_PUMP)
						{
							//sensor_pump_on();
						}

						if(obj->id == ID_EXIT)
						{
							SendMessage(hwnd,WM_CLOSE,0,0);
						}
					}

				}

				/////
				focus_cur =0;
				focus_obj =NULL;
				InvalidateRect(hwnd,NULL,TRUE);

			}
			return DefWindowProc(hwnd,msg,wParam,lParam);
			////
     

	case	WM_MOUSEMOVE:
			{
				int x,y;

				x=GET_LPARAM_X(lParam);
				y=GET_LPARAM_Y(lParam);


				x =x-rc_wav.x;
				x=MAX(x,0);
				x=MIN(x,rc_wav.w-1);

				y =y-rc_wav.y;
				y=MAX(y,0);
				y=MIN(y,rc_wav.h-1);

				if(focus_cur == CUR_X1)
				{

					x1_cur =x;

					x_obj_move_to(x1_cur_obj,(x1_cur+rc_wav.x)-(x1_cur_obj->rc.w/2),x1_cur_obj->rc.y);

				}

				if(focus_cur == CUR_X2)
				{

					x2_cur =x;

					x_obj_move_to(x2_cur_obj,(x+rc_wav.x)-(x2_cur_obj->rc.w/2),x2_cur_obj->rc.y);

				}

				if(focus_cur == CUR_Y1)
				{

					y1_cur =rc_wav.h-y;

					x_obj_move_to(y1_cur_obj,y1_cur_obj->rc.x,(y+rc_wav.y)-(y1_cur_obj->rc.h/2));

				}

				if(focus_cur == CUR_Y2)
				{

					y2_cur =rc_wav.h-y;
					x_obj_move_to(y2_cur_obj,y2_cur_obj->rc.x,(y+rc_wav.y)-(y2_cur_obj->rc.h/2));

				}

				InvalidateRect(hwnd,NULL,TRUE);

			}
			return DefWindowProc(hwnd,msg,wParam,lParam);
			////

		case	WM_ERASEBKGND:
		{
			HDC hdc=(HDC)wParam;
			RECT rc;

			GetClientRect(hwnd,&rc);
			SetBrushColor(hdc,MapRGB(hdc,0,0,0));
			FillRect(hdc,&rc);
			return TRUE;
		}
	////
#if 0
	case	WM_DRAWITEM:
	{
		/*　当控件指定了WS_OWNERDRAW风格，则每次在绘制前都会给父窗口发送WM_DRAWITEM消息。
		 *  wParam参数指明了发送该消息的控件ID;lParam参数指向一个DRAWITEM_HDR的结构体指针，
		 *  该指针成员包含了一些控件绘制相关的参数.
	     */

		DRAWITEM_HDR *ds;

		ds =(DRAWITEM_HDR*)lParam;


			HDC hdc;
			RECT rc;

			rc =ds->rc;

			//把控件的客户区坐标转换成主窗口的客户区坐标。
			ClientToScreen(ds->hwnd,(POINT*)&rc,1);
			ScreenToClient(hwnd,(POINT*)&rc,1);

			hdc =CreateDC(pSurfTop,&rc); //在“顶层”Surface中创建一个DC。
			ds->hDC =hdc;

			if(ds->ID> ID_BTN_BEGIN && ds->ID<ID_BTN_END)
			{

				button_owner_draw(ds); //执行自绘制按钮
			}

			if(ds->ID == ID_LISTBOX)
			{
				listbox_owner_draw_1(ds);
			}

			DeleteDC(hdc);

			InvalidateRect(hwnd,&rc,FALSE); //使主窗口重绘.

		return TRUE;
	}
	break;
  
#else
   case	WM_DRAWITEM:
		{
			DRAWITEM_HDR *ds;

			ds =(DRAWITEM_HDR*)lParam;
      if(ds->ID == ID_SCROLL1 || ds->ID == ID_SCROLL2)
      {
         scrollbar_owner_draw(ds);
         return TRUE;
      }
		}
    break;
#endif  
	////

	case WM_PAINT:
	{
		HDC hdc,hdc_bk;
		PAINTSTRUCT ps;
		RECT rc;
		WCHAR wbuf[128];

		hdc =BeginPaint(hwnd,&ps);




		///////
		x_cur=0;
		while(x_cur < rc_wav.w)
		{
				float mV;
				//mV=ADS1120_GetVoltage_mV(0);

				//mV =sin(2*3.14*(x_cur/rc_wave.w))*10+50;

				mV = (1-sin( 2*3.14*(x_cur*1.0/(rc_wav.w/10)) ))*((rc_wav.h)/2);
				y_cur =(int)(mV/y_step[y_step_cur]);

				//y_cur =x_rand()%200;

				pt[x_cur].x =x_cur;
				pt[x_cur].y =y_cur;
				x_cur++;
								//InvalidateRect(hwnd,NULL,FALSE);
		}
#if 0
		if(1)
		{

			x_wsprintf(wbuf,L"Y[abs]:%.1fmV",(y2_cur-y1_cur)*y_step[y_step_cur]);
			x_obj_set_text(x_obj_get_from_id(text_item,ID_TEST+1),wbuf);
		}

		rc.x=200;
		rc.y=200;
		rc.w=200;
		rc.h=40;
		SetTextColor(hdcTop,MapARGB(hdcTop,250,10,10,10));
		SetPenColor(hdcTop,MapARGB(hdcTop,150,100,200,255));
		SetBrushColor(hdcTop,MapARGB(hdcTop,100,250,250,250));
		DrawText(hdcTop,wbuf,-1,&rc,DT_VCENTER|DT_CENTER|DT_BKGND|DT_BORDER);
#endif
	/*
		rc.x =700;
		rc.y =100;
		rc.w=200;
		rc.h=50;
		SetBrushColor(hdcTop,MapARGB(hdcTop,200,255,100,100));
		FillRect(hdcTop,&rc);

		OffsetRect(&rc,30,30);
		SetBrushColor(hdcTop,MapARGB(hdcTop,200,100,255,100));
		FillRect(hdcTop,&rc);
*/
#if 0

		GetClientRect(hwnd,&rc);
	  hdc_bk =CreateMemoryDC(SURF_SCREEN,rc_wav.w,rc_wav.h);
		DrawFrame(hdcTop,pt,x_cur,MapRGB(hdcTop,255,0,0));


		////////////////
		 BitBlt(hdc_bk,0,0,BK_W,BK_H,hdcTop,0,0,SRCCOPY);
		BitBlt(hdc,0,0,rc_wav.w,rc_wav.h,hdc_bk,0,0,SRCCOPY);
  	DeleteDC(hdc_bk);
#else
    DrawFrame(hdc,pt,x_cur,MapRGB(hdc,255,0,0));


		////////////////
		// BitBlt(hdc_bk,0,0,BK_W,BK_H,hdcTop,0,0,SRCCOPY);
		// BitBlt(hdc,0,0,BK_W,BK_H,hdc_bk,0,0,SRCCOPY);
#endif


		EndPaint(hwnd,&ps);
	}
	break;
	////

	case WM_DESTROY:
	{
		x_obj_del(button_item);
    x_obj_del(text_item);

		DeleteDC(hdcTop);
		DeleteSurface(pSurfTop);
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

static void ADS1120_Init(void)
{
//	sensor_enable();
}

static u16 ADS1120_GetData(void)
{
	//return get_alco_data();
	return x_rand()%0x7FFF;
}

static float ADS1120_GetVoltage_mV(u16 addata)
{
	float val;

	addata=ADS1120_GetData();
	val=(float)addata*(float)2048/32768;
	return val;
}


extern "C" void	GUI_DEMO_ShowWave(void)
{
	HWND hwnd;
	MSG	msg;
	WNDCLASS wcex;
	RECT rc;
	////

	ShowCursor(FALSE);
	ADS1120_Init();

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
							L"Show Wave",
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
	ShowCursor(TRUE);


}



#include <emXGUI.h>
/*============================================================================*/
//定义控件ID
#define	ID_OK		         0x1000
//空心图形颜色
#define  DrawRect_col       RGB888(250,250,250)  //矩形
#define  DrawCir_col        RGB888(79,129,189)   //圆
#define  DrawEllipse_col    RGB888(247,186,0)    //椭圆
#define  DrawLine_col       RGB888(146,208,80)   //线
//填充图形颜色
#define  FillRTri_col       RGB888(0,176,240)    //直角三角形
#define  FillEqTri_col      RGB888(146,208,80)   //等边三角形
#define  FillCir_col        RGB888(255,0,0)      //圆
#define  FillRECT_col       RGB888(255,192,0)    //矩形
#define  FillRoundRECT_col  RGB888(79,129,189)   //圆角矩形
#define  FillEllipse_col    RGB888(112,48,160)   //椭圆
//渐变色矩形
#define  Fillsquare_col0    RGB888(50,100,250)   
#define  Fillsquare_col1    RGB888(234,236,198)
//正方体
#define  FillCube_col       RGB888(50,150,250)   //正面
#define  FillCubeside_col   RGB888(40,121,201)   //侧面
#define  FillCubetop_col    RGB888(90,171,251)   //顶部
//长方体
#define  Fillcuboid_col     RGB888(255,0,0)      //正面
#define  Fillcuboidside_col RGB888(205,0,0)      //侧面
#define  Fillcuboidtop_col  RGB888(255,50,50)    //顶部


#define  FillRect_borcol    RGB888(220,220,220)   //外边框
#define  FillborRect_col    RGB888(79,99,40)   //矩形颜色
/*============================================================================*/
void Draw_Func(HWND hwnd, HDC hdc)
{
   RECT rc; 
   //设置矩形参数
   rc.x =50;
   rc.y =80;
   rc.w =140;
   rc.h =75;
   SetPenColor(hdc,MapRGB888(hdc, DrawRect_col)); //设置画笔颜色(用于画线，框).
   DrawRect(hdc,&rc); //绘制一个空心矩形
   //绘制一个空心圆.

   SetPenColor(hdc,MapRGB888(hdc,DrawCir_col));
   DrawCircle(hdc,80,100,50); 
   //画斜线
   SetPenColor(hdc,MapRGB888(hdc,DrawLine_col));
   Line(hdc,30,160,200,230); 
   //画椭圆  
   SetPenColor(hdc,MapRGB888(hdc,DrawEllipse_col));
   DrawEllipse(hdc, 150, 155, 65, 40);
}

void Fill_Func(HWND hwnd, HDC hdc)
{
   POINT pt_RTri[3]={{35, 200},
                     {35, 300},
                     {235, 300}
                    };
   //绘制直角三角形
   SetBrushColor(hdc, MapRGB888(hdc, FillRTri_col)); //设置颜色， 使用BrushColor。
   FillPolygon(hdc, 0, 0, &pt_RTri[0],3); //填充多边形。  
   //绘制等边三角形
   POINT pt_EqTri[3]={{220, 120},{320, 120},{270, 13}};   
   SetBrushColor(hdc, MapRGB888(hdc, FillEqTri_col)); //设置颜色， 使用BrushColor。
   FillPolygon(hdc, 0, 0, &pt_EqTri[0],3); //填充多边形。
   //绘制圆
   SetBrushColor(hdc, MapRGB888(hdc, FillCir_col));
   FillCircle(hdc, 320, 250, 60);
   //绘制矩形
   RECT rc_rect = {280, 320, 165, 55};
   SetBrushColor(hdc, MapRGB888(hdc, FillRECT_col));
   FillRect(hdc, &rc_rect);  
   //绘制圆角矩形
   RECT rc_Roundrect = {390, 160, 55, 150};
   SetBrushColor(hdc, MapRGB888(hdc, FillRoundRECT_col));
   FillRoundRect(hdc, &rc_Roundrect,14);
   //绘制椭圆
   SetBrushColor(hdc, MapRGB888(hdc, FillEllipse_col));
   FillEllipse(hdc, 500, 80, 90, 55);   
   
   //绘制渐变色正方形
   RECT rc_square = {470, 235, 140, 140};  
   GradientFillRect(hdc, &rc_square, MapRGB888(hdc, Fillsquare_col0), 
                    MapRGB888(hdc, Fillsquare_col1), FALSE);
   //绘制正方体顶部
   RECT Cube = {630, 120, 100, 100};
   SetBrushColor(hdc, MapRGB888(hdc, FillCube_col));
   FillRect(hdc, &Cube);
   //绘制正方体侧面
   POINT pt_SidePolygon[4]={{730, 120}, {730, 220},
                            {765, 185}, {765, 85}};
   SetBrushColor(hdc, MapRGB888(hdc, FillCubeside_col));
   FillPolygon(hdc, 0, 0, &pt_SidePolygon[0],4); //填充多边形。
   //绘制正方体正面                     
   POINT pt_TopPolygon[4]={{665, 85},  {630, 120},
                           {730, 120}, {765, 85}};
   SetBrushColor(hdc, MapRGB888(hdc, FillCubetop_col));
   FillPolygon(hdc, 0, 0, &pt_TopPolygon[0],4); //填充多边形。
                           
   //绘制长方体
   RECT cuboid = {650, 280, 60, 165};
   SetBrushColor(hdc, MapRGB888(hdc, Fillcuboid_col));
   FillRect(hdc, &cuboid);
   //绘制长方体侧面
   POINT pt_cuboidSide[4]={{710, 280}, {710, 445},
                           {731, 424}, {731, 259}};
   SetBrushColor(hdc, MapRGB888(hdc, Fillcuboidside_col));
   FillPolygon(hdc, 0, 0, &pt_cuboidSide[0],4); //填充多边形。
   //绘制长方体正面                  
   POINT pt_cuboidTop[4]={{671, 259},  {650, 280},
                           {710, 280}, {731, 259}};
   SetBrushColor(hdc, MapRGB888(hdc, Fillcuboidtop_col));
   FillPolygon(hdc, 0, 0, &pt_cuboidTop[0],4); //填充多边形。  


   RECT rc_borrect = {25, 330, 220, 100};
   SetBrushColor(hdc, MapRGB888(hdc, FillRect_borcol));//设置矩形的外边框颜色
   FillRect(hdc, &rc_borrect);    
   
   InflateRect(&rc_borrect, -5, -5);//将矩形四条边的位置减少5个坐标值
   SetBrushColor(hdc, MapRGB888(hdc, FillborRect_col));//设置矩形的颜色
   FillRect(hdc, &rc_borrect);    
}

static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;
	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{
			GetClientRect(hwnd,&rc); //获得窗口的客户区矩形.
			CreateWindow(BUTTON,L"OK",WS_VISIBLE,rc.w-80,8,68,32,hwnd,ID_OK,NULL,NULL); //创建一个按钮(示例).
         return TRUE;
		}
		case WM_NOTIFY: //WM_NOTIFY消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
		{
			u16 code,id;

			code =HIWORD(wParam); //获得通知码类型.
			id   =LOWORD(wParam); //获得产生该消息的控件ID.

			if(id==ID_OK && code==BN_CLICKED) // 按钮“单击”了.
			{
				PostCloseMessage(hwnd); //使产生WM_CLOSE消息关闭窗口.
			}
         break;
		}
		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{
			PAINTSTRUCT ps;
			HDC hdc;
         RECT rc;
			hdc =BeginPaint(hwnd,&ps); //开始绘图
         GetClientRect(hwnd,&rc);
         SetBrushColor(hdc, MapRGB(hdc, 0, 0, 0));
         FillRect(hdc, &rc);
         Draw_Func(hwnd, hdc);
         Fill_Func(hwnd, hdc);        
			EndPaint(hwnd,&ps); //结束绘图
         break;
		}
		default: //用户不关心的消息,由系统处理.
		{
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}

	}
	return WM_NULL;
}

/*============================================================================*/
void	GUI_DEMO_DrawTest(void)
{
	HWND	hwnd;
	WNDCLASS	wcex;
	MSG msg;
	wcex.Tag 		    = WNDCLASS_TAG;

	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WinProc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;

	//创建主窗口
	hwnd	=CreateWindowEx(NULL,
								&wcex,
								_T("emXGUI Window"), //窗口名称
								WS_CLIPCHILDREN,
								0,0,GUI_XSIZE,GUI_YSIZE,    //窗口位置和大小
								NULL,NULL,NULL,NULL);

	//显示主窗口
	ShowWindow(hwnd,SW_SHOW);

	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

/*============================================================================*/

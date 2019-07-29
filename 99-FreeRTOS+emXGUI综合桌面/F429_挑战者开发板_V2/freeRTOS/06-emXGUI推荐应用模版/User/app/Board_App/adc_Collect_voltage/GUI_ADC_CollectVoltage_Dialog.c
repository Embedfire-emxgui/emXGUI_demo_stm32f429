#include <emXGUI.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "ff.h"
#include "x_libc.h"
#include "GUI_AppDef.h"
#include "emXGUI_JPEG.h"
#include "emxgui_png.h"
#include  "GUI_ADC_CollectVoltage_Dialog.h"


/* 窗口 ID */
#define ID_ADV_WIN         0x01    // 中间显示窗口ID
#define SCROLLBAR_Brigh_ID 0x02    // 滑动条的ID
#define ID_TEXTBOX_Title   0x03    // 标题栏
#define ID_TEXTBOX_Brigh   0x04    // 亮度百分比

#define CircleCenter_1    (79)     // 三角形旋转半径
#define CircleCenter_2    (100)    // 圆弧进度条半径（小）
#define CircleCenter_3    (CircleCenter_2 + 10)    //  不大于 CircleSize / 2

/* 移动方向标志 */
#define LeftToRight    0
#define RightToLeft    1
#define MOVE_WIN       1

#define CircleSize    240    // 圆形显示区域的大小
#define Circle_X      500    // 圆形显示区域的位置
#define Circle_Y      (50)   // 圆形显示区域的位置

#define GUI_ADC_BACKGROUNG_PIC      "musicdesktop.jpg"

#define TitleHeight    70    // 标题栏的高度

#define TriangleLen    20    // 三角形的边长

uint8_t AovingDirection = 0;
double count = 0.0;
HWND MAIN_Handle;
HWND Title_Handle;
HWND Brigh_Handle;
HWND ADC_Handle;

HDC bk_hdc;

static COLORREF color_bg;//透明控件的背景颜色

// 局部变量，用于保存转换计算后的电压值 	 
double ADC_Vol; 

static void	X_MeterPointer(HDC hdc, int cx, int cy, int r, u32 color, double dat_val)
{
  double angle;
  int midpoint_x,midpoint_y;
  POINT pt[4];

  angle = (dat_val * 1.427 + 0.785);    // 计算角度
  
  /* 计算与切线平行的一边的中点坐标 */
  midpoint_x =cx - sin(angle) * (r - TriangleLen * 0.866);    // 0.866 = sqrt(3) / 2
  midpoint_y =cy + cos(angle) * (r - TriangleLen * 0.866);

  /* 最远的一个点 */
  pt[0].x = cx - r * sin(angle);
  pt[0].y = cy + r * cos(angle);

  /* 最远一点的左边一点 */
  pt[1].x = midpoint_x - (TriangleLen / 2) * sin(angle - 1.57);    // 1.57 = 3.14/2 = π/2 = 90°
  pt[1].y = midpoint_y + (TriangleLen / 2) * cos(angle - 1.57);

  /* 最远一点的右边一点 */
  pt[2].x = midpoint_x - (TriangleLen / 2) * sin(angle + 1.57);
  pt[2].y = midpoint_y + (TriangleLen / 2) * cos(angle + 1.57);

  pt[3].x = pt[0].x;
  pt[3].y = pt[0].y;


  /* 画三角形 */
  SetBrushColor(hdc,color);
  EnableAntiAlias(hdc, TRUE);
  FillPolygon(hdc,0,0,pt,4);
  EnableAntiAlias(hdc, FALSE);
}

//退出按钮重绘制
static void CollectVoltage_ExitButton_OwnerDraw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
  HDC hdc;
	RECT rc;
 // RECT rc_top={0,0,800,70};
	WCHAR wbuf[128];

	hwnd = ds->hwnd; 
	hdc = ds->hDC;   
	rc = ds->rc; 

	SetBrushColor(hdc, MapRGB(hdc, COLOR_DESKTOP_BACK_GROUND));
   
  FillCircle(hdc, rc.x+rc.w, rc.y, rc.w);
	//FillRect(hdc, &rc); //用矩形填充背景

  if (ds->State & BST_PUSHED)
	{ //按钮是按下状态
		SetTextColor(hdc, MapRGB(hdc, 105, 105, 105));      //设置文字色
	}
	else
	{ //按钮是弹起状态

		SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
	}

	  /* 使用控制图标字体 */
	SetFont(hdc, controlFont_64);

	GetWindowText(hwnd, wbuf, 128); //获得按钮控件的文字
  rc.y = -10;
  rc.x = 16;
	DrawText(hdc, wbuf, -1, &rc, NULL);//绘制文字(居中对齐方式)

  /* 恢复默认字体 */
	SetFont(hdc, defaultFont);

}

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
	RECT rc,rc_tmp;
   RECT rc_scrollbar;

	/* 背景 */
   GetClientRect(hwnd, &rc_tmp);//得到控件的位置
   GetClientRect(hwnd, &rc);//得到控件的位置
   WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换
   
   BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, bk_hdc, rc_tmp.x, rc_tmp.y, SRCCOPY);

   rc_scrollbar.x = rc.x;
   rc_scrollbar.y = rc.h/2-15;
   rc_scrollbar.w = rc.w;
   rc_scrollbar.h = 30;
   
	SetBrushColor(hdc, MapRGB888(hdc, Page_c));
	FillRect(hdc, &rc_scrollbar);
}

/*
 * @brief  绘制滚动条
 * @param  hwnd:   滚动条的句柄值
 * @param  hdc:    绘图上下文
 * @param  back_c：背景颜色
 * @param  Page_c: 滚动条Page处的颜色
 * @param  fore_c：滚动条滑块的颜色
 * @retval NONE
*/
static void draw_gradient_scrollbar(HWND hwnd, HDC hdc, COLOR_RGB32 back_c, COLOR_RGB32 Page_c, COLOR_RGB32 fore_c)
{
	RECT rc,rc_tmp;
   RECT rc_scrollbar;
	GetClientRect(hwnd, &rc);
	/* 背景 */
   GetClientRect(hwnd, &rc_tmp);//得到控件的位置
   GetClientRect(hwnd, &rc);//得到控件的位置
   WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换
   
   BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, bk_hdc, rc_tmp.x, rc_tmp.y, SRCCOPY);

   rc_scrollbar.x = rc.x;
   rc_scrollbar.y = rc.h/2-15;
   rc_scrollbar.w = rc.w;
   rc_scrollbar.h = 30;
   
//	SetBrushColor(hdc, MapRGB888(hdc, Page_c));
	GradientFillRect(hdc, &rc_scrollbar, RGB888(175, 150, 150), RGB888( 255, 255, 255), FALSE);

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
	draw_scrollbar(hwnd, hdc_mem1, color_bg, RGB888( 50, 50, 50), RGB888( 255, 255, 255));
	//绘制渐变类型的滚动条
	draw_gradient_scrollbar(hwnd, hdc_mem, color_bg, RGB888(	50, 50, 50), RGB888(50, 205, 50));
  SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc);   

	//左
	BitBlt(hdc, rc_cli.x, rc_cli.y, rc.x + rc.w / 2, rc_cli.h, hdc_mem, 0, 0, SRCCOPY);
	//右
	BitBlt(hdc, rc.x + rc.w/2, rc_cli.y, rc_cli.w - (rc.x + rc.w/2) , rc_cli.h, hdc_mem1, rc.x + rc.w/2, 0, SRCCOPY);

	//绘制滑块
	if (ds->State & SST_THUMBTRACK)//按下
	{
    /* 滑块 */
    SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc);

    SetBrushColor(hdc, MapRGB(hdc, 169, 169, 169));

    /* 边框 */
    FillCircle(hdc, rc.x + rc.w / 2, rc.y + rc.h / 2, rc.h / 2 - 1);
    InflateRect(&rc, -2, -2);

    SetBrushColor(hdc, MapRGB888(hdc, RGB888(200, 200, 200)));
    FillCircle(hdc, rc.x + rc.w / 2, rc.y + rc.h / 2, rc.h / 2 - 1);
	}
	else//未选中
	{
		/* 滑块 */
    SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc);

    SetBrushColor(hdc, MapRGB(hdc, 169, 169, 169));

    /* 边框 */
    FillCircle(hdc, rc.x + rc.w / 2, rc.y + rc.h / 2, rc.h / 2 - 1);
    InflateRect(&rc, -2, -2);

    SetBrushColor(hdc, MapRGB888(hdc, RGB888( 255, 255, 255)));
    FillCircle(hdc, rc.x + rc.w / 2, rc.y + rc.h / 2, rc.h / 2 - 1);
	}
  
	//释放内存MemoryDC
	DeleteDC(hdc_mem1);
	DeleteDC(hdc_mem);
}

/*
 * @brief  重绘透明文本
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void Textbox_OwnerDraw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
	HWND hwnd;
	HDC hdc;
	RECT rc, rc_tmp;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button的窗口句柄.
	hdc = ds->hDC;   //button的绘图上下文句柄.
  GetClientRect(hwnd, &rc_tmp);//得到控件的位置
  GetClientRect(hwnd, &rc);//得到控件的位置
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换

  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, bk_hdc, rc_tmp.x, rc_tmp.y, SRCCOPY);
  SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));

  GetWindowText(hwnd, wbuf, 128); //获得按钮控件的文字

  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//绘制文字(居中对齐方式)
}

/* 重绘圆形显示区域 */
void Circle_Paint(HWND hwnd, HDC hdc)
{
  char  cbuf[128];
  WCHAR wbuf[128];
  RECT rc = {0, 0, CircleSize, CircleSize};

  EnableAntiAlias(hdc, TRUE);

  SetBrushColor(hdc, MapRGB(hdc, 65, 65, 65));
  FillArc(hdc, CircleSize/2, CircleSize/2, CircleCenter_2, CircleCenter_3, -45, 225);

  SetBrushColor(hdc, MapRGB(hdc, 200, 200, 200));
  FillArc(hdc, CircleSize/2, CircleSize/2, CircleCenter_2+1, CircleCenter_3-1, -45, ((225 - (-45))) * ADC_Vol / 3.3  - 45);

  EnableAntiAlias(hdc, FALSE);
  /* 画三角形 */
  X_MeterPointer(hdc, CircleSize/2, CircleSize/2, CircleCenter_1, MapRGB(hdc,250,20,20), ADC_Vol);

  /* 使用默认字体 */
	SetFont(hdc, defaultFont);

  rc.w = 24*4;
  rc.h = 30;
  rc.x = CircleSize/2 - rc.w/2;
  rc.y = CircleSize/2 - rc.h/2;

  /* 显示电压百分比 */
  x_sprintf(cbuf, "%d%%", (int)(ADC_Vol/3.3*100));
  x_mbstowcs_cp936(wbuf, cbuf, 128);
  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);    // 绘制文字(居中对齐方式)

  /* 显示采集到的电压值 */
  rc.y = CircleSize/2 - rc.h/2 + CircleCenter_3;
  x_sprintf(cbuf, "%.2fV", ADC_Vol);
  x_mbstowcs_cp936(wbuf, cbuf, 128);
  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);    // 绘制文字(居中对齐方式)
}

/*
 * @brief  重绘两个表示亮度的圆
 * @param  HDC:	自定义绘制结构体
 * @retval NONE
*/
#define CircleCenter_r    25
#define CircleCenter_R    35
void BrighCircle_Paint(HDC hdc)
{
  /* 先画左边 */
  SetBrushColor(hdc, MapRGB(hdc, 200, 200, 200));
  EnableAntiAlias(hdc, TRUE);
  FillArc(hdc, GUI_XSIZE + 100, 105, 15, 18, 0, 360); 
  // EnableAntiAlias(hdc, FALSE);

  /* 右边 */
  SetBrushColor(hdc, MapRGB(hdc, 255, 255, 255));
  // EnableAntiAlias(hdc, TRUE);
  FillArc(hdc, GUI_XSIZE + 700, 105, 16, 19, 0, 360); 
  EnableAntiAlias(hdc, FALSE);

  int Little_x, Little_y;
  int Big_x,    Big_y;

  SetPenColor(hdc, MapRGB(hdc, 255, 255, 255));

  for(int i=0; i<8; i++)
  {
    /* 靠近圆的点 */
    Little_x = GUI_XSIZE + 700 + CircleCenter_r * sin(0.785 * i);    // 0.785 = 3.14/4 = π/4 = 45°
    Little_y = 105 - CircleCenter_r * cos(0.785 * i);

    /* 远离圆的点 */
    Big_x = GUI_XSIZE + 700 + CircleCenter_R * sin(0.785 * i);    // 0.785 = 3.14/4 = π/4 = 45°
    Big_y = 105 - CircleCenter_R * cos(0.785 * i);

    /* 画直线 */
    Line(hdc, Little_x, Little_y, Big_x, Big_y);
  }
}

static LRESULT	ADCWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_CREATE:
    {
      RECT rc;
      HWND hwnd_scrolbar;
      SCROLLINFO sif;/*设置滑动条的参数*/
      GetClientRect(hwnd, &rc);
      Rheostat_Init();    // 初始化 ADC

      /*********************亮度调节滑动条******************/
      sif.cbSize = sizeof(sif);
      sif.fMask = SIF_ALL;
      sif.nMin = 0;
      sif.nMax = 100;
      sif.nValue = 50;//初始值
      sif.TrackSize = 60;//滑块值
      sif.ArrowSize = 0;//两端宽度为0（水平滑动条）          
      hwnd_scrolbar = CreateWindow(SCROLLBAR, L"SCROLLBAR_Brigh", WS_OWNERDRAW | WS_VISIBLE,//  
                      GUI_XSIZE + 100, (GUI_YSIZE - TitleHeight * 2) / 2, 600, 60, hwnd, SCROLLBAR_Brigh_ID, NULL, NULL);
      SendMessage(hwnd_scrolbar, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif); 

      rc.w = 24*5;
      rc.h = TitleHeight;
      rc.x = GUI_XSIZE + GUI_XSIZE / 2 - rc.w / 2;
      rc.y = TitleHeight;

      Brigh_Handle = CreateWindow(TEXTBOX, L"50%", WS_VISIBLE | WS_OWNERDRAW, rc.x, rc.y, rc.w, rc.h, hwnd, ID_TEXTBOX_Brigh, NULL, NULL);//
      SendMessage(Brigh_Handle, TBM_SET_TEXTFLAG, 0, DT_VCENTER | DT_CENTER | DT_BKGND);   

      SetTimer(hwnd, 2, 10, TMR_START, NULL);

      break;
    } 

    case WM_TIMER:
    {
      RECT rc;
      int tmr_id;
      
      tmr_id = wParam;
      GetWindowRect(hwnd, &rc);
      SetForegroundWindow(hwnd);

      if (tmr_id == MOVE_WIN)
      {
        if (AovingDirection == LeftToRight)
        {
          if (rc.x < 0)
          {
            OffsetRect(&rc, (rc.w >> 3), 0);
            rc.x = MIN(rc.x, 0);
            MoveWindow(hwnd, rc.x, rc.y, rc.w, rc.h, TRUE);
          }
          else
          {
            SetWindowText(Title_Handle,L"ADC一电位器电压显示");
            KillTimer(hwnd, 1);
          }
        }
        else if (AovingDirection == RightToLeft)
        {
          if (rc.x > -800)
          {
            OffsetRect(&rc, -(rc.w >> 3), 0);
            rc.x = MAX(rc.x, -800);
            MoveWindow(hwnd, rc.x, rc.y, rc.w, rc.h, TRUE);
          }
          else
          {
            SetWindowText(Title_Handle,L"屏幕亮度调节");
            KillTimer(hwnd, 1);
          }
        }
      }
      else if (tmr_id == 2)
      {
        RECT rc;
        static double ADC_buff = 0.0;
        double vol_buff = 0.0;
        static uint8_t xC = 0;

        vol_buff =(double) ADC_ConvertedValue/4096*(double)3.3; // 读取转换的AD值
//        GUI_DEBUG("电压值前为：%f", ADC_Vol);
        #if 1

          if (xC++ < 10)
          {
            ADC_buff += vol_buff;
            break;
          }
          else
          {
            ADC_Vol = ADC_buff / ((double)(xC-1));
            ADC_buff = 0;
            xC = 0;
          }

        #else
          
        ADC_Vol = (double)(((int)(vol_buff * 10)) / 10.0);

        #endif
//        GUI_DEBUG("电压值后为：%f", ADC_Vol);
        
        rc.x = Circle_X;
        rc.y = Circle_Y;
        rc.w = CircleSize * 2;
        rc.h = CircleSize * 2;

        InvalidateRect(hwnd, &rc, FALSE);
      }
      
      
      break;
    }

    case WM_ERASEBKGND:
    {
      
      HDC hdc =(HDC)wParam;
      RECT rc = {0, TitleHeight, GUI_XSIZE, GUI_YSIZE - TitleHeight};

      ScreenToClient(hwnd, (POINT *)&rc, 1);
      BitBlt(hdc, rc.x, rc.y, GUI_XSIZE, rc.h, bk_hdc, 0, TitleHeight, SRCCOPY);

      return TRUE;
    }

    case WM_PAINT:
    {
      HDC hdc, hdc_mem;
      PAINTSTRUCT ps;
      RECT rc = {Circle_X, Circle_Y, 2, 2};

      ClientToScreen(hwnd, (POINT *)&rc, 1);

      hdc_mem = CreateMemoryDC(SURF_SCREEN, CircleSize, CircleSize);

      hdc = BeginPaint(hwnd, &ps);

      BitBlt(hdc_mem, 0, 0, CircleSize, CircleSize, bk_hdc, rc.x, rc.y, SRCCOPY);

      Circle_Paint(hwnd, hdc_mem);
      BrighCircle_Paint(hdc);
      
      BitBlt(hdc, Circle_X, Circle_Y, CircleSize, CircleSize, hdc_mem, 0, 0, SRCCOPY);
      DeleteDC(hdc_mem);
      EndPaint(hwnd, &ps);

      break;
    } 

    case WM_NOTIFY:
    {
//      u16 code;
      u16 ctr_id;
      NMHDR *nr;
//      code=HIWORD(wParam);//获取消息的类型
      ctr_id = LOWORD(wParam); //wParam低16位是发送该消息的控件ID.
      nr = (NMHDR*)lParam; //lParam参数，是以NMHDR结构体开头.      

      if (ctr_id == SCROLLBAR_Brigh_ID)
      {
        NM_SCROLLBAR *sb_nr;
        int i = 0;
        WCHAR wbuf[128];
        sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar的通知消息实际为 NM_SCROLLBAR扩展结构,里面附带了更多的信息.
        switch (nr->code)
        {
          case SBN_THUMBTRACK: //R滑块移动
          {
            i = sb_nr->nTrackValue; //获得滑块当前位置值                
            SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, i); //设置进度值
            x_wsprintf(wbuf, L"%d%%", i);
            SetWindowText(Brigh_Handle, wbuf);
          }
          break;
        }
      }   
      break;
    } 
 
    
    static int x_move;

    case WM_LBUTTONDOWN:
    {
      int x;
      x = LOWORD(lParam);
      RECT rc;
	  	GetWindowRect(hwnd, &rc);
      rc.x = x;
      ClientToScreen(hwnd, (POINT *)&rc, 1);
      x_move = rc.x;
//      GUI_DEBUG("按下：x = %d",x);
      break;
    }

    static int x_old;
    case WM_MOUSEMOVE:
    {
      int x;
      RECT rc;
      RECT Client_rc;
	  	GetWindowRect(hwnd, &rc);
      GetWindowRect(hwnd, &Client_rc);
      x = LOWORD(lParam);
      
      rc.x = x;
      ClientToScreen(hwnd, (POINT *)&rc, 1);
      OffsetRect(&Client_rc, rc.x - x_move, 0);

      Client_rc.x = MIN(Client_rc.x, 30);
      Client_rc.x = MAX(Client_rc.x, -800 - 30);
      
      MoveWindow(hwnd, Client_rc.x, Client_rc.y, Client_rc.w, Client_rc.h, TRUE);

      x_old = x_move;
      x_move = rc.x;

//      GUI_DEBUG("移动：x = %d", x_old);
      break;
    }

    case WM_LBUTTONUP:
    { 
      int x;
      RECT Client_rc;
      RECT rc;
      
      x = LOWORD(lParam);
      rc.x = x;
      GetWindowRect(hwnd, &Client_rc);
      ClientToScreen(hwnd, (POINT *)&rc, 1);
//      GUI_DEBUG("抬起：x = %d",rc.x);

      if (rc.x - x_old > 0)    // 从左往右滑
      {
        if (Client_rc.x > 0)
        {
          MoveWindow(hwnd, 0, Client_rc.y, Client_rc.w, Client_rc.h, TRUE);
        }
        else 
        {
          SetTimer(hwnd, 1, 5, TMR_START, NULL);
          AovingDirection = LeftToRight;
        }
      }
      else    // 从右往左滑
      {
        if (Client_rc.x < -800)
        {
          MoveWindow(hwnd, -800, Client_rc.y, Client_rc.w, Client_rc.h, TRUE);
        }
        else 
        {
          SetTimer(hwnd, 1, 5, TMR_START, NULL);
          AovingDirection = RightToLeft;
        }
      }
      
      break;
    } 

    case WM_DRAWITEM:    // 按钮重绘
    {
       DRAWITEM_HDR *ds;
       ds = (DRAWITEM_HDR*)lParam;
       switch(ds->ID)
       {
          case eID_ADC_EXIT:
          {
            CollectVoltage_ExitButton_OwnerDraw(ds);
            return TRUE;             
          }   

          case SCROLLBAR_Brigh_ID:
          {
            scrollbar_owner_draw(ds);
            return TRUE;             
          } 

          case ID_TEXTBOX_Brigh:
          {
            Textbox_OwnerDraw(ds);
            return TRUE;             
          } 
       }
       break;
    }

    case WM_DESTROY:
    {
      

      return PostQuitMessage(hwnd);	
    } 

    default:
      return	DefWindowProc(hwnd, msg, wParam, lParam);   
  }
  
  return WM_NULL;
}
 
static LRESULT	CollectVoltage_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_CREATE:
    {
      RECT rc;
      GetClientRect(hwnd, &rc); 
            
      CreateWindow(BUTTON, L"O", WS_TRANSPARENT|BS_FLAT | BS_NOTIFY |WS_OWNERDRAW|WS_VISIBLE,
                  730, 0, 70, 70, hwnd, eID_ADC_EXIT, NULL, NULL); 

      WNDCLASS wcex;

      wcex.Tag	 		    = WNDCLASS_TAG;
      wcex.Style			  = CS_HREDRAW | CS_VREDRAW;
      wcex.lpfnWndProc	= (WNDPROC)ADCWinProc;
      wcex.cbClsExtra		= 0;
      wcex.cbWndExtra		= 0;
      wcex.hInstance		= NULL;
      wcex.hIcon			  = NULL;
      wcex.hCursor		  = NULL;
      
      rc.x = 0;
      rc.y = TitleHeight;
      rc.w = GUI_XSIZE*2;
      rc.h = GUI_YSIZE - TitleHeight * 2;
      ////创建"ADC采集窗口"的控件.
      ADC_Handle = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER ,&wcex,L"---",WS_CLIPCHILDREN|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_ADV_WIN,NULL,NULL);

      rc.w = GUI_XSIZE / 2;
      rc.h = TitleHeight;
      rc.x = GUI_XSIZE / 2 - rc.w / 2;
      rc.y = 0;

      Title_Handle = CreateWindow(TEXTBOX, L"ADC一电位器电压显示", WS_VISIBLE | WS_OWNERDRAW, rc.x, rc.y, rc.w, rc.h, hwnd, ID_TEXTBOX_Title, NULL, NULL);//
      SendMessage(Title_Handle, TBM_SET_TEXTFLAG, 0, DT_VCENTER | DT_CENTER | DT_BKGND);   

      BOOL res;
      u8 *jpeg_buf;
      u32 jpeg_size;
      JPG_DEC *dec;
      res = RES_Load_Content(GUI_ADC_BACKGROUNG_PIC, (char**)&jpeg_buf, &jpeg_size);
      bk_hdc = CreateMemoryDC(SURF_SCREEN, GUI_XSIZE, GUI_YSIZE);
      if(res)
      {
        /* 根据图片数据创建JPG_DEC句柄 */
        dec = JPG_Open(jpeg_buf, jpeg_size);

        /* 绘制至内存对象 */
        JPG_Draw(bk_hdc, 0, 0, dec);

        /* 关闭JPG_DEC句柄 */
        JPG_Close(dec);
      }
      /* 释放图片内容空间 */
      RES_Release_Content((char **)&jpeg_buf);

    //  SetTimer(hwnd,2,0,TMR_SINGLE,NULL);

      break;
    } 
    case WM_TIMER:
    {
      
      
      break;
    }
    // case WM_ERASEBKGND:
    // {
      
    //   HDC hdc =(HDC)wParam;
    //   RECT rc =*(RECT*)lParam;
      
    //   SetBrushColor(hdc, MapRGB(hdc, 255, 0, 0));;
    //   FillRect(hdc, &rc);

    //   return TRUE;
    //   break;
    // }

    case WM_PAINT:
    {
      HDC hdc;
      PAINTSTRUCT ps;
      //  RECT rc = {0,0,800,70};
      //  hdc_mem = CreateMemoryDC(SURF_ARGB4444, 800,70);
       
      hdc = BeginPaint(hwnd, &ps);

      
       
      //  SetBrushColor(hdc_mem, MapARGB(hdc_mem,100,105, 105, 105));
      //  FillRect(hdc_mem, &rc);

      BitBlt(hdc, 0, 0, GUI_XSIZE, GUI_YSIZE, bk_hdc, 0, 0, SRCCOPY);
      
      //  BitBlt(hdc, 0,0,800,70,hdc_mem,0,0,SRCCOPY);
      //  DeleteDC(hdc_mem);
      EndPaint(hwnd, &ps);

      break;
    } 
    case WM_DRAWITEM:
    {
       DRAWITEM_HDR *ds;
       ds = (DRAWITEM_HDR*)lParam;
       switch(ds->ID)
       {
          case eID_ADC_EXIT:
          {
            CollectVoltage_ExitButton_OwnerDraw(ds);
            return TRUE;             
          }    

          case ID_TEXTBOX_Title:
          {
            Textbox_OwnerDraw(ds);
            return TRUE;             
          } 
       }

       break;
    }
    case WM_NOTIFY:
    {
      u16 code, id;
      id  =LOWORD(wParam);//获取消息的ID码
      code=HIWORD(wParam);//获取消息的类型    
      if(code == BN_CLICKED && id == eID_ADC_EXIT)
      {
        PostCloseMessage(hwnd);
        break;
      }

      break;
    } 

    case WM_LBUTTONUP:
    {
      return	DefWindowProc(hwnd, msg, wParam, lParam);   
    }

    case WM_MOUSEMOVE:
    {
      return	DefWindowProc(hwnd, msg, wParam, lParam);
    }

    case WM_LBUTTONDOWN:
    {
      return	DefWindowProc(hwnd, msg, wParam, lParam);   
    }

    case WM_DESTROY:
    {
      Rheostat_DISABLE();    // 停止ADC的采集
      DeleteDC(bk_hdc);
      return PostQuitMessage(hwnd);	
    } 

    default:
      return	DefWindowProc(hwnd, msg, wParam, lParam);   
  }
  
  return WM_NULL;
  
}

void GUI_ADC_CollectVoltage_Dialog(void)
{
	
	WNDCLASS	wcex;
	MSG msg;

	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = CollectVoltage_proc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);
   
	//创建主窗口
	MAIN_Handle = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,
                              &wcex,
                              L"GUI_ADC_CollectVoltage_Dialog",
                              WS_VISIBLE|WS_CLIPCHILDREN,
                              0, 0, GUI_XSIZE, GUI_YSIZE,
                              NULL, NULL, NULL, NULL);
   //显示主窗口
	ShowWindow(MAIN_Handle, SW_SHOW);
	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while (GetMessage(&msg, MAIN_Handle))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}  
}



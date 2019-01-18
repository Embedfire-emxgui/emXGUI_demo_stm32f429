#include "emXGUI.h"
#include "x_libc.h"

#include "./led/bsp_led.h"  
#include "GUI_AppDef.h"
#include "GUI_RGBLED_DIALOG.h"
#include "Widget.h"
#include "emXGUI_JPEG.h"
/* 硬件控制接口 */
extern void TIM_GPIO_Config(void);
extern void TIM_Mode_Config(void);
extern void TIM_RGBLED_Close(void);
extern void SetRGBColor(uint32_t rgb);
extern void SetColorValue(uint8_t r,uint8_t g,uint8_t b);
extern const unsigned char RGBdesktop[];
static BITMAP RGBdesktop_0;
/**********************分界线*********************/
struct leddlg
{
	char col_R;  //R分量值
	char col_G;  //G分量值
	char col_B;  //B分量值
   char led_R;//硬件RGB灯颜色分量值
   char led_G;//硬件RGB灯颜色分量值
   char led_B;//硬件RGB灯颜色分量值
   int colR_ctr;//硬件RGB灯控制位
   int colG_ctr;//硬件RGB灯控制位
   int colB_ctr;//硬件RGB灯控制位
}leddlg_S={255, 165, 0, 255, 165, 0, 1, 1, 1};

icon_S GUI_RGBLED_Icon[18] = 
{
      {"tuichu",           {730,0,70,70},       FALSE},//退出按键
      {"biaotilan",        {100,0,600,80},      FALSE},//APP标题栏
      {"APPHouse",         {40,80,175,275},      FALSE},//APP房子图标
      {"hongdeng",         {250, 80, 72, 72},  FALSE},//红灯图标
      {"lvdeng",           {379, 80, 72, 72},  FALSE},//绿灯图标
      {"landeng",          {507, 80, 72, 72},  FALSE},//蓝灯图标
      {"hongdengscrollbar",{270, 155, 35, 200},  FALSE},//红色滚动条
      {"lvdengscrollbar",  {399, 155, 35, 200},  FALSE},//绿色滚动条
      {"landengscrollbar", {527, 155, 35, 200},  FALSE},//蓝色滚动条 
      {"kongzhikaiguan",   {675, 355,160,160},  FALSE},//控制开关
      {"hongdengwenzi",    {253, 420, 72, 50}, FALSE},//文字-红灯
      {"lvdengwenzi",      {383, 420, 72, 50}, FALSE},//文字-绿灯
      {"landengwenzi",     {513, 420, 72, 50}, FALSE},//文字-蓝灯
      {"Rshuzhi",          {253, 355, 72, 72}, FALSE},//文字-R数值
      {"Gshuzhi",          {383, 355, 72, 72}, FALSE},//文字-G数值
      {"Bshuzhi",          {513, 355, 72, 72}, FALSE},//文字-B数值
};

RGBLED_DIALOG_s RGBLED_DIALOG =
{
   .RGBLED_Hwnd = NULL,
   .State = TRUE,
   .exit_bt_draw = home_owner_draw,
   .col_R = 255,
   .col_G = 165,
   .col_B = 0,
   
};

static void Delete_DlALOG()
{
   RGBLED_DIALOG.col_R = 255;
   RGBLED_DIALOG.col_G = 165;
   RGBLED_DIALOG.col_B = 0;
   RGBLED_DIALOG.State = 1;
   leddlg_S.colB_ctr = 1;
   leddlg_S.colG_ctr = 1;
   leddlg_S.colR_ctr = 1;
   leddlg_S.led_R = 255;
   leddlg_S.led_G = 165;
   leddlg_S.led_B = 0;
   leddlg_S.col_R = 255;
   leddlg_S.col_G = 165;
   leddlg_S.col_B = 0;
   TIM_RGBLED_Close();
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
static void GUI_RGBLED_drawscrollbar_V(HWND hwnd, HDC hdc, COLOR_RGB32 back_c, COLOR_RGB32 Page_c, COLOR_RGB32 fore_c)
{
	RECT rc;
   RECT rc_scrollbar;
	GetClientRect(hwnd, &rc);
	/* 背景 */
	SetBrushColor(hdc, MapRGB888(hdc, back_c));
	FillRect(hdc, &rc);

   rc_scrollbar.x = rc.w/2;
   rc_scrollbar.y = rc.y;
   rc_scrollbar.w = 2;
   rc_scrollbar.h = rc.h;
   
	SetBrushColor(hdc, MapRGB888(hdc, Page_c));
	FillRect(hdc, &rc_scrollbar);

	/* 滑块 */
	SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc);

	SetBrushColor(hdc, MapRGB(hdc, 169, 169, 169));
//	rc.x += (rc.w >> 2) >> 1;
//	rc.w -= rc.w >> 2;
	/* 边框 */
	FillCircle(hdc, rc.x + rc.h / 2+1, rc.y + rc.h / 2, rc.h / 2);
   InflateRect(&rc, -2, -2);

	SetBrushColor(hdc, MapRGB888(hdc, fore_c));
	FillCircle(hdc, rc.x + rc.h / 2+1, rc.y + rc.h / 2, rc.h / 2);
}
/*
 * @brief  自定义回调函数
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void GUI_RGBLED_ScrollbarOwnerDraw_V(DRAWITEM_HDR *ds)
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
	GUI_RGBLED_drawscrollbar_V(hwnd, hdc_mem1, ID_SCROLLBAR_BackColor, RGB888(250, 250, 250), RGB888(255, 255, 255));
	//绘制绿色类型的滚动条
	switch (ds->ID)
	{
		case ID_SCROLLBAR_R:
		{
			GUI_RGBLED_drawscrollbar_V(hwnd, hdc_mem, ID_SCROLLBAR_BackColor, RGB888(leddlg_S.col_R, 0, 0), RGB888(leddlg_S.col_R, 0, 0));
			break;
		}
		case ID_SCROLLBAR_G:
		{
			GUI_RGBLED_drawscrollbar_V(hwnd, hdc_mem, ID_SCROLLBAR_BackColor, RGB888(0, leddlg_S.col_G, 0), RGB888(0, leddlg_S.col_G, 0));
			break;
		}
		case ID_SCROLLBAR_B:
		{
			GUI_RGBLED_drawscrollbar_V(hwnd, hdc_mem, ID_SCROLLBAR_BackColor, RGB888(0, 0, leddlg_S.col_B), RGB888(0, 0, leddlg_S.col_B));
			break;
		}
	}
   
   
   SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc);
	//绘制滑块
	if (ds->State & SST_THUMBTRACK)//按下
	{
		BitBlt(hdc, rc.x, 0, rc.w, rc_cli.h, hdc_mem, rc.x, 0, SRCCOPY);
	}
	else//未选中
	{
		BitBlt(hdc, rc.x, 0, rc.w, rc_cli.h, hdc_mem1, rc.x, 0, SRCCOPY);
	}	
	//上
	BitBlt(hdc, rc_cli.x, rc_cli.y, rc_cli.w, rc.y, hdc_mem, 0, 0, SRCCOPY);
	//下
	BitBlt(hdc, rc_cli.x, rc.y+rc.h, rc_cli.w , rc_cli.h-(rc.y+rc.h), hdc_mem1, 0, rc.y + rc.h, SRCCOPY);

	//释放内存MemoryDC
	DeleteDC(hdc_mem1);
	DeleteDC(hdc_mem);
}

void GUI_RGBLED_HomeOwnerDraw(DRAWITEM_HDR *ds) 
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button的窗口句柄.
	hdc = ds->hDC;   //button的绘图上下文句柄.
	rc = ds->rc;     //button的绘制矩形区.

   SetBrushColor(hdc, MapRGB(hdc, 169,169,169));
   FillRect(hdc, &rc);   
   
	SetBrushColor(hdc, MapRGB(hdc, COLOR_DESKTOP_BACK_GROUND));
   FillCircle(hdc, rc.x+rc.w, rc.y, rc.w);
   
   //按钮按下状态
   if (ds->State & BST_PUSHED)
	{ 
		SetTextColor(hdc, MapRGB(hdc, 105, 105, 105));      //设置文字色
	}
	else//按钮弹起状态
	{ 
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
/**
  * @brief  CheckBox按钮控件的重绘制
  * @param  ds:DRAWITEM_HDR结构体
  * @retval NULL
  */
static void GUI_RGBLED_CheckBoxOwnerDraw(DRAWITEM_HDR *ds)
{
   HDC hdc; //控件窗口HDC
   HWND hwnd; //控件句柄 
   RECT rc_cli;//控件的位置大小矩形
   WCHAR wbuf[128];
	hwnd = ds->hwnd;
	hdc = ds->hDC; 
   GetClientRect(hwnd, &rc_cli);
   //创建缓冲层，格式为SURF_ARGB4444
   
	GetWindowText(ds->hwnd,wbuf,128); //获得按钮控件的文字  
   
   SetBrushColor(hdc,MapRGB(hdc, 169,169,169));
   FillRect(hdc, &rc_cli);
   //NEXT键、BACK键和LIST键按下时，改变颜色
	if((ds->State & BST_PUSHED))//按钮是按下状态
	{ 
      EnableAlpha(hdc,TRUE);
      EnableAntiAlias(hdc, TRUE);
      SetAlpha(hdc, 128);
      SetBrushColor(hdc,MapRGB(hdc, 185,218,251));
      FillCircle(hdc, rc_cli.x + rc_cli.w/2, rc_cli.y + rc_cli.w/2, rc_cli.w/2);       
      EnableAlpha(hdc,FALSE);
      SetBrushColor(hdc,MapRGB(hdc,  185,218,251));
      FillCircle(hdc, rc_cli.x + rc_cli.w/2, rc_cli.y + rc_cli.w/2, rc_cli.w/2-5);      
      EnableAntiAlias(hdc, FALSE);
	}
   else
   {
      EnableAntiAlias(hdc, TRUE);//抗锯齿
      if(RGBLED_DIALOG.State == TRUE)
      {
         EnableAlpha(hdc,TRUE);
         SetAlpha(hdc, 128);
         SetBrushColor(hdc,MapRGB(hdc, 50,205,50));
         FillCircle(hdc, rc_cli.x + rc_cli.w/2, rc_cli.y + rc_cli.w/2, rc_cli.w/2); 
         
         EnableAlpha(hdc,FALSE);
         SetBrushColor(hdc,MapRGB(hdc,  50,205,50));
         
         FillCircle(hdc, rc_cli.x + rc_cli.w/2, rc_cli.y + rc_cli.w/2, rc_cli.w/2-5);                 
      }
      else
      {
         EnableAlpha(hdc,TRUE);
         SetAlpha(hdc, 128);
         
         SetBrushColor(hdc,MapRGB(hdc, 220,20,60));
         FillCircle(hdc, rc_cli.x + rc_cli.w/2, rc_cli.y + rc_cli.w/2, rc_cli.w/2); 
         
         EnableAlpha(hdc,FALSE);
         SetBrushColor(hdc,MapRGB(hdc,  220,20,60));
         FillCircle(hdc, rc_cli.x + rc_cli.w/2, rc_cli.y + rc_cli.w/2, rc_cli.w/2-5);          
      }
      EnableAntiAlias(hdc, FALSE);
   }
   SetFont(hdc, controlFont_32);
   SetTextColor(hdc, MapRGB(hdc, 255,255,255));      //设置文字色
   rc_cli.x = 35;
   rc_cli.y = 55;
   if(RGBLED_DIALOG.State == TRUE)
   { 
      DrawText(hdc, L"op",-1,&rc_cli,NULL);//绘制文字(居中对齐方式)           
   }
   else
   {     
      DrawText(hdc, L"np",-1,&rc_cli,NULL);//绘制文字(居中对齐方式)         
   }      
}

static	LRESULT	win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//static RECT rc_R, rc_G, rc_B;//RGB分量指示框
   //HDC hdc_mem2pic;
   static HDC hdc_mem;
	switch (msg)
	{
      case WM_CREATE: 
      {
         WCHAR wbuf[128];
         CreateWindow(BUTTON, L"O",WS_OWNERDRAW|WS_VISIBLE,
                      GUI_RGBLED_Icon[0].rc.x, GUI_RGBLED_Icon[0].rc.y, 
                      GUI_RGBLED_Icon[0].rc.w, GUI_RGBLED_Icon[0].rc.h, 
                      hwnd, ID_EXIT, NULL, NULL); 
         
         CreateWindow(TEXTBOX, L"全彩LED灯", WS_VISIBLE, 
                      GUI_RGBLED_Icon[1].rc.x, GUI_RGBLED_Icon[1].rc.y, 
                      GUI_RGBLED_Icon[1].rc.w, GUI_RGBLED_Icon[1].rc.h, 
                      hwnd, ID_TEXTBOX_Title, NULL, NULL);
         SendMessage(GetDlgItem(hwnd, ID_TEXTBOX_Title),TBM_SET_TEXTFLAG,0,
                        DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND); 
         
         CreateWindow(TEXTBOX, L"k", WS_VISIBLE, 
                      GUI_RGBLED_Icon[2].rc.x, GUI_RGBLED_Icon[2].rc.y, 
                      GUI_RGBLED_Icon[2].rc.w, GUI_RGBLED_Icon[2].rc.h, 
                      hwnd, ID_TEXTBOX_APP, NULL, NULL);
         SetWindowFont(GetDlgItem(hwnd, ID_TEXTBOX_APP), iconFont_252);
         SendMessage(GetDlgItem(hwnd, ID_TEXTBOX_APP),TBM_SET_TEXTFLAG,0,
                        DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND);

         CreateWindow(TEXTBOX, L"k", WS_VISIBLE, 
                      GUI_RGBLED_Icon[3].rc.x, GUI_RGBLED_Icon[3].rc.y, 
                      GUI_RGBLED_Icon[3].rc.w, GUI_RGBLED_Icon[3].rc.h, 
                      hwnd, ID_TEXTBOX_R_LED, NULL, NULL);
         SetWindowFont(GetDlgItem(hwnd, ID_TEXTBOX_R_LED), controlFont_72);
         SendMessage(GetDlgItem(hwnd, ID_TEXTBOX_R_LED),TBM_SET_TEXTFLAG,0,
                        DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND); 
                        
         CreateWindow(TEXTBOX, L"k", WS_VISIBLE, 
                      GUI_RGBLED_Icon[4].rc.x, GUI_RGBLED_Icon[4].rc.y, 
                      GUI_RGBLED_Icon[4].rc.w, GUI_RGBLED_Icon[4].rc.h, 
                      hwnd, ID_TEXTBOX_G_LED, NULL, NULL);
         SetWindowFont(GetDlgItem(hwnd, ID_TEXTBOX_G_LED), controlFont_72);
         SendMessage(GetDlgItem(hwnd, ID_TEXTBOX_G_LED),TBM_SET_TEXTFLAG,0,
                        DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND); 
                        
         CreateWindow(TEXTBOX, L"k", WS_VISIBLE, 
                      GUI_RGBLED_Icon[5].rc.x, GUI_RGBLED_Icon[5].rc.y, 
                      GUI_RGBLED_Icon[5].rc.w, GUI_RGBLED_Icon[5].rc.h, 
                      hwnd, ID_TEXTBOX_B_LED, NULL, NULL);
         SetWindowFont(GetDlgItem(hwnd, ID_TEXTBOX_B_LED), controlFont_72);         
         SendMessage(GetDlgItem(hwnd, ID_TEXTBOX_B_LED),TBM_SET_TEXTFLAG,0,
                        DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND); 



         RGBLED_DIALOG.sif_R.cbSize = sizeof(RGBLED_DIALOG.sif_R);
         RGBLED_DIALOG.sif_R.fMask = SIF_ALL;
         RGBLED_DIALOG.sif_R.nMin = 0;
         RGBLED_DIALOG.sif_R.nMax = 255;
         RGBLED_DIALOG.sif_R.nValue = RGBLED_DIALOG.col_R;
         RGBLED_DIALOG.sif_R.TrackSize = 35;
         RGBLED_DIALOG.sif_R.ArrowSize = 0;

         /*创建滑动条--R*/
         CreateWindow(SCROLLBAR, L"SCROLLBAR_R", SBS_VERT|WS_OWNERDRAW | WS_VISIBLE, 
                      GUI_RGBLED_Icon[6].rc.x, GUI_RGBLED_Icon[6].rc.y, 
                      GUI_RGBLED_Icon[6].rc.w, GUI_RGBLED_Icon[6].rc.h, 
                      hwnd, ID_SCROLLBAR_R, NULL, NULL);
         SendMessage(GetDlgItem(hwnd, ID_SCROLLBAR_R), SBM_SETSCROLLINFO, TRUE, (LPARAM)&RGBLED_DIALOG.sif_R);

         RGBLED_DIALOG.sif_G.cbSize = sizeof(RGBLED_DIALOG.sif_G);
         RGBLED_DIALOG.sif_G.fMask = SIF_ALL;
         RGBLED_DIALOG.sif_G.nMin = 0;
         RGBLED_DIALOG.sif_G.nMax = 255;
         RGBLED_DIALOG.sif_G.nValue = RGBLED_DIALOG.col_G;
         RGBLED_DIALOG.sif_G.TrackSize = 35;
         RGBLED_DIALOG.sif_G.ArrowSize = 0;
         /*创建滑动条--G*/
         CreateWindow(SCROLLBAR, L"SCROLLBAR_G", SBS_VERT|WS_OWNERDRAW | WS_VISIBLE, 
                      GUI_RGBLED_Icon[7].rc.x, GUI_RGBLED_Icon[7].rc.y, 
                      GUI_RGBLED_Icon[7].rc.w, GUI_RGBLED_Icon[7].rc.h, 
                      hwnd, ID_SCROLLBAR_G, NULL, NULL);
         SendMessage(GetDlgItem(hwnd, ID_SCROLLBAR_G), SBM_SETSCROLLINFO, TRUE, (LPARAM)&RGBLED_DIALOG.sif_G);
         RGBLED_DIALOG.sif_B.cbSize = sizeof(RGBLED_DIALOG.sif_B);
         RGBLED_DIALOG.sif_B.fMask = SIF_ALL;
         RGBLED_DIALOG.sif_B.nMin = 0;
         RGBLED_DIALOG.sif_B.nMax = 255;
         RGBLED_DIALOG.sif_B.nValue = RGBLED_DIALOG.col_B;
         RGBLED_DIALOG.sif_B.TrackSize = 35;
         RGBLED_DIALOG.sif_B.ArrowSize = 0;
         /*创建滑动条--B*/
         CreateWindow(SCROLLBAR, L"SCROLLBAR_B", SBS_VERT|WS_OWNERDRAW | WS_VISIBLE, 
                      GUI_RGBLED_Icon[8].rc.x, GUI_RGBLED_Icon[8].rc.y, 
                      GUI_RGBLED_Icon[8].rc.w, GUI_RGBLED_Icon[8].rc.h,
                      hwnd, ID_SCROLLBAR_B, NULL, NULL);
         SendMessage(GetDlgItem(hwnd, ID_SCROLLBAR_B), SBM_SETSCROLLINFO, TRUE, (LPARAM)&RGBLED_DIALOG.sif_B);

			CreateWindow(BUTTON,L"Checkbox5",WS_OWNERDRAW|WS_VISIBLE,
                      GUI_RGBLED_Icon[9].rc.x, GUI_RGBLED_Icon[9].rc.y, 
                      GUI_RGBLED_Icon[9].rc.w, GUI_RGBLED_Icon[9].rc.h,
                      hwnd,ID_CHECKBOX_SW,NULL,NULL);         
         /*创建文本框--红灯*/
         CreateWindow(TEXTBOX, L"qp", WS_VISIBLE, 
                      GUI_RGBLED_Icon[10].rc.x, GUI_RGBLED_Icon[10].rc.y, 
                      GUI_RGBLED_Icon[10].rc.w, GUI_RGBLED_Icon[10].rc.h,
                      hwnd, ID_TEXTBOX_R, NULL, NULL);
         SendMessage(GetDlgItem(hwnd, ID_TEXTBOX_R),TBM_SET_TEXTFLAG,0,
                        DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND); 
         SetWindowFont(GetDlgItem(hwnd, ID_TEXTBOX_R), controlFont_32);                        
         //R的分量值 
         x_wsprintf(wbuf, L"%d", leddlg_S.col_R);
         CreateWindow(TEXTBOX, wbuf, WS_VISIBLE, 
                      GUI_RGBLED_Icon[13].rc.x, GUI_RGBLED_Icon[13].rc.y, 
                      GUI_RGBLED_Icon[13].rc.w, GUI_RGBLED_Icon[13].rc.h,
                      hwnd, ID_TEXTBOX_R_NUM, NULL, NULL);
         SendMessage(GetDlgItem(hwnd, ID_TEXTBOX_R_NUM),TBM_SET_TEXTFLAG,0,
                        DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND);  
         SetWindowFont(GetDlgItem(hwnd, ID_TEXTBOX_R_NUM), controlFont_32);               
         //G的分量值 
         x_wsprintf(wbuf, L"%d", leddlg_S.col_G);
         CreateWindow(TEXTBOX, wbuf, WS_VISIBLE, 
                      GUI_RGBLED_Icon[14].rc.x, GUI_RGBLED_Icon[14].rc.y, 
                      GUI_RGBLED_Icon[14].rc.w, GUI_RGBLED_Icon[14].rc.h,          
                      hwnd, ID_TEXTBOX_G_NUM, NULL, NULL);
         SendMessage(GetDlgItem(hwnd, ID_TEXTBOX_G_NUM),TBM_SET_TEXTFLAG,0,
                        DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND); 
         SetWindowFont(GetDlgItem(hwnd, ID_TEXTBOX_G_NUM), controlFont_32);                         
         //创建文本框--蓝灯         
         CreateWindow(TEXTBOX, L"rp", WS_VISIBLE, 
                      GUI_RGBLED_Icon[11].rc.x, GUI_RGBLED_Icon[11].rc.y, 
                      GUI_RGBLED_Icon[11].rc.w, GUI_RGBLED_Icon[11].rc.h,         
                      hwnd, ID_TEXTBOX_G, NULL, NULL);
         SendMessage(GetDlgItem(hwnd, ID_TEXTBOX_G),TBM_SET_TEXTFLAG,0,
                        DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND);   
         SetWindowFont(GetDlgItem(hwnd, ID_TEXTBOX_G), controlFont_32);  
         //创建文本框--绿灯
         CreateWindow(TEXTBOX, L"sp", WS_VISIBLE, 
                      GUI_RGBLED_Icon[12].rc.x, GUI_RGBLED_Icon[12].rc.y, 
                      GUI_RGBLED_Icon[12].rc.w, GUI_RGBLED_Icon[12].rc.h,          
                      hwnd, ID_TEXTBOX_B, NULL, NULL);
         SendMessage(GetDlgItem(hwnd, ID_TEXTBOX_B),TBM_SET_TEXTFLAG,0,
                        DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND);   
         SetWindowFont(GetDlgItem(hwnd, ID_TEXTBOX_B), controlFont_32);
         //B的分量值         
         x_wsprintf(wbuf, L"%d", leddlg_S.col_B);         
         CreateWindow(TEXTBOX, wbuf, WS_VISIBLE, 
                      GUI_RGBLED_Icon[15].rc.x, GUI_RGBLED_Icon[15].rc.y, 
                      GUI_RGBLED_Icon[15].rc.w, GUI_RGBLED_Icon[15].rc.h,
                      hwnd, ID_TEXTBOX_B_NUM, NULL, NULL);
         SendMessage(GetDlgItem(hwnd, ID_TEXTBOX_B_NUM),TBM_SET_TEXTFLAG,0,
                        DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND);    
         SetWindowFont(GetDlgItem(hwnd, ID_TEXTBOX_B_NUM), controlFont_32);   

			//设置位图结构参数
			RGBdesktop_0.Format	= BM_RGB888;     //位图格式
			RGBdesktop_0.Width  = 240;              //宽度
			RGBdesktop_0.Height = 158;             //高度
			RGBdesktop_0.WidthBytes =RGBdesktop_0.Width*3; //每行字节数
			RGBdesktop_0.LUT =NULL;                //查找表(RGB/ARGB格式不使用该参数)
			RGBdesktop_0.Bits =(void*)RGBdesktop;    //位图数据
         hdc_mem = CreateMemoryDC(SURF_SCREEN, RGBdesktop_0.Width, RGBdesktop_0.Height);
         DrawBitmap(hdc_mem,0, 0,&RGBdesktop_0,NULL);

         SetColorValue(leddlg_S.led_R, leddlg_S.led_G, leddlg_S.led_B);
         //GUI_DEBUG("%x%x", leddlg_S.led_R/16, leddlg_S.led_R%16);
         break;
      }

      case WM_NOTIFY: 
      {
         NMHDR *nr;
         WCHAR wbuf[128];
         u16 ctr_id; 
         u16 code,  id;
         id  =LOWORD(wParam);//获取消息的ID码
         code=HIWORD(wParam);//获取消息的类型
         ctr_id = LOWORD(wParam); //wParam低16位是发送该消息的控件ID.
         nr = (NMHDR*)lParam; //lParam参数，是以NMHDR结构体开头.
         if(id == ID_EXIT && code == BN_CLICKED)
         {
            PostCloseMessage(hwnd);
         }
         
         if (ctr_id == ID_SCROLLBAR_R)
         {
            NM_SCROLLBAR *sb_nr;		
            sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar的通知消息实际为 NM_SCROLLBAR扩展结构,里面附带了更多的信息.
            switch (nr->code)
            {
               case SBN_THUMBTRACK: //R滑块移动
               {
                  leddlg_S.col_R = sb_nr->nTrackValue; //获得滑块当前位置值
                  SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, leddlg_S.col_R); //设置位置值
                  x_wsprintf(wbuf, L"%d", leddlg_S.col_R);
                  SetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_R_NUM), wbuf);
                  RedrawWindow(GetDlgItem(hwnd, ID_TEXTBOX_R_LED), NULL, RDW_ALLCHILDREN|RDW_INVALIDATE);
               }
               break;
            }
         }
         
         if (ctr_id == ID_SCROLLBAR_G)
         {
            NM_SCROLLBAR *sb_nr;
            sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar的通知消息实际为 NM_SCROLLBAR扩展结构,里面附带了更多的信息.
            switch (nr->code)
            {
               case SBN_THUMBTRACK: //G滑块移动
               {
                  leddlg_S.col_G = sb_nr->nTrackValue; //获得滑块当前位置值
                  SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, leddlg_S.col_G); //设置位置值
                  x_wsprintf(wbuf, L"%d", leddlg_S.col_G);
                  SetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_G_NUM), wbuf);
                  RedrawWindow(GetDlgItem(hwnd, ID_TEXTBOX_G_LED), NULL, RDW_ALLCHILDREN|RDW_INVALIDATE);
               }
               break;
            }
         }

         if(ctr_id == ID_SCROLLBAR_B)
         {
            NM_SCROLLBAR *sb_nr;
            sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar的通知消息实际为 NM_SCROLLBAR扩展结构,里面附带了更多的信息.
            switch (nr->code)
            {
               case SBN_THUMBTRACK: //B滑块移动
               {
                  leddlg_S.col_B = sb_nr->nTrackValue; //获得B滑块当前位置值
                  SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, leddlg_S.col_B); //设置B滑块的位置
                  x_wsprintf(wbuf, L"%d", leddlg_S.col_B);
                  SetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_B_NUM), wbuf);
                  RedrawWindow(GetDlgItem(hwnd, ID_TEXTBOX_B_LED), NULL, RDW_ALLCHILDREN|RDW_INVALIDATE);
               }
               break;
            }
         }
			if(id == ID_CHECKBOX_SW)
			{
				if(code == BN_CLICKED) //被点击了
				{
               RGBLED_DIALOG.State = ~RGBLED_DIALOG.State;

				}

			}     
         
         RedrawWindow(GetDlgItem(hwnd, ID_TEXTBOX_APP), NULL, RDW_ALLCHILDREN|RDW_INVALIDATE);
         if(RGBLED_DIALOG.State == TRUE)
         {
            leddlg_S.led_R=leddlg_S.col_R;
            leddlg_S.led_G=leddlg_S.col_G;
            leddlg_S.led_B=leddlg_S.col_B;
            SetColorValue(leddlg_S.led_R, leddlg_S.led_G, leddlg_S.led_B);
         }
         else
         {
            SetColorValue(0, 0, 0);
         }
         break;
      }
      case WM_DRAWITEM:
      {
         DRAWITEM_HDR *ds;
         ds = (DRAWITEM_HDR*)lParam;
         switch(ds->ID)
         {
            case ID_EXIT:
            {
               GUI_RGBLED_HomeOwnerDraw(ds);
               return TRUE;              
            }
            case ID_SCROLLBAR_R:
            {
               GUI_RGBLED_ScrollbarOwnerDraw_V(ds);
               return TRUE;
            }
            case ID_SCROLLBAR_G:
            {
               GUI_RGBLED_ScrollbarOwnerDraw_V(ds);
               return TRUE;
            }
            case ID_SCROLLBAR_B:
            {
               GUI_RGBLED_ScrollbarOwnerDraw_V(ds);
               return TRUE;
            }
            case ID_CHECKBOX_SW:
            {
               GUI_RGBLED_CheckBoxOwnerDraw(ds);
               return TRUE;
            }
            
         }
         break;

      }
      case WM_CTLCOLOR:
      {
         u16 id;
         id =LOWORD(wParam);         
         CTLCOLOR *cr;
         cr =(CTLCOLOR*)lParam;
         
         switch(id)
         {
            case ID_TEXTBOX_Title:
            {
               cr->TextColor = ID_TEXTBOX_Title_TextColor;
               cr->BackColor = ID_TEXTBOX_Title_BackColor;            
               break;               
            }
            case ID_TEXTBOX_APP:
            {
               cr->BackColor = ID_TEXTBOX_APP_BackColor; 
               //cr->BorderColor = ID_TEXTBOX_APP_BackColor;
               cr->TextColor = RGB888(leddlg_S.col_R, leddlg_S.col_G, leddlg_S.col_B);
               break;
            }
            case ID_TEXTBOX_R_LED:
            {
               cr->TextColor = RGB888(leddlg_S.col_R, 0, 0);
               cr->BackColor = ID_TEXTBOX_R_LED_BackColor;     
               break;
            }
            case ID_TEXTBOX_G_LED:
            {
               cr->TextColor = RGB888(0, leddlg_S.col_G, 0);
               cr->BackColor = ID_TEXTBOX_G_LED_BackColor;     
               break;
            }
            case ID_TEXTBOX_B_LED:
            {
               cr->TextColor = RGB888(0, 0, leddlg_S.col_B);
               cr->BackColor = ID_TEXTBOX_B_LED_BackColor;     
               break;
            }     
            case ID_TEXTBOX_R_NUM:
            {
               cr->BackColor = ID_TEXTBOX_R_NUM_BackColor;   
               cr->TextColor = RGB888(0, 0, 0);               
               break;               
            }  
            case ID_TEXTBOX_R:
            {
               cr->BackColor = ID_TEXTBOX_R_BackColor;   
               cr->TextColor = RGB888(0, 0, 0);               
               break;               
            }
            case ID_TEXTBOX_G:
            {
               cr->BackColor = ID_TEXTBOX_G_BackColor;   
               cr->TextColor = RGB888(0, 0, 0);               
               break;               
            }  
            case ID_TEXTBOX_B:
            {
               cr->BackColor = ID_TEXTBOX_B_BackColor;   
               cr->TextColor = RGB888(0, 0, 0);               
               break;               
            }              
            
            case ID_TEXTBOX_G_NUM:
            {
               cr->BackColor = ID_TEXTBOX_R_NUM_BackColor;   
               cr->TextColor = RGB888(0, 0, 0);               
               break;               
            }  
            case ID_TEXTBOX_B_NUM:
            {
               cr->BackColor = ID_TEXTBOX_R_NUM_BackColor;   
               cr->TextColor = RGB888(0, 0, 0);               
               break;               
            }              
            default:
               return FALSE;
         }
         #if 0
   //      if(id== ID_TEXTBOX_R_NUM || id== ID_TEXTBOX_G_NUM || id== ID_TEXTBOX_B_NUM)
   //      {

   //         cr->TextColor =RGB888(255,255,255);//文字颜色（RGB888颜色格式)
   //         cr->BackColor =RGB888(0,0,0);//背景颜色（RGB888颜色格式)
   //         cr->BorderColor =RGB888(255,0,0);//边框颜色（RGB888颜色格式)
   //         return TRUE;
   //      }
   //      switch(id)
   //      {
   //         case ID_TEXTBOX_R:
   //         {
   //            cr->TextColor =RGB888(255,0,0);//文字颜色（RGB888颜色格式)
   //            cr->BackColor =RGB888(0,0,0);//背景颜色（RGB888颜色格式)
   //            cr->BorderColor =RGB888(255,0,0);//边框颜色（RGB888颜色格式)            
   //            break;
   //         }
   //         case ID_TEXTBOX_G:
   //         {
   //            cr->TextColor =RGB888(0,255,0);//文字颜色（RGB888颜色格式)
   //            cr->BackColor =RGB888(0,0,0);//背景颜色（RGB888颜色格式)
   //            cr->BorderColor =RGB888(255,0,0);//边框颜色（RGB888颜色格式)              
   //            break;
   //         }
   //         case ID_TEXTBOX_B:
   //         {
   //            cr->TextColor =RGB888(0,0,255);//文字颜色（RGB888颜色格式)
   //            cr->BackColor =RGB888(0,0,0);//背景颜色（RGB888颜色格式)
   //            cr->BorderColor =RGB888(255,0,0);//边框颜色（RGB888颜色格式)                       
   //            break;
   //         }
   //         case ID_TEXTBOX_R_NUM:
   //         {
   //            cr->TextColor =RGB888(255,255,255);//文字颜色（RGB888颜色格式)
   //            cr->BackColor =RGB888(0,0,0);//背景颜色（RGB888颜色格式)
   //            cr->BorderColor =RGB888(255,0,0);//边框颜色（RGB888颜色格式)            
   //         }
   //         case ID_TEXTBOX_G_NUM:
   //         {
   //            cr->TextColor =RGB888(255,255,255);//文字颜色（RGB888颜色格式)
   //            cr->BackColor =RGB888(0,0,0);//背景颜色（RGB888颜色格式)
   //            cr->BorderColor =RGB888(255,0,0);//边框颜色（RGB888颜色格式)          
   //         }
   //         case ID_TEXTBOX_B_NUM:
   //         {
   //            cr->TextColor =RGB888(255,255,255);//文字颜色（RGB888颜色格式)
   //            cr->BackColor =RGB888(0,0,0);//背景颜色（RGB888颜色格式)
   //            cr->BorderColor =RGB888(255,0,0);//边框颜色（RGB888颜色格式)     
   //            break;
   //         }
   //         default:
   //            return FALSE;
   //         
   //      }
   #endif
         
         
         
         return TRUE;
         
      } 
      case WM_ERASEBKGND:
      {
         HDC hdc =(HDC)wParam;
         RECT rc =*(RECT*)lParam;;

         //GetClientRect(hwnd, &rc);
//         SetBrushColor(hdc, MapRGB(hdc, 169,169,169));
//         FillRect(hdc, &rc);         
         #if 0  //图片背景 
         static JPG_DEC *dec;//JPG句柄         
         BOOL res;
         U16 width, height;
         res = RES_Load_Content(GUI_RGB_BACKGROUNG_PIC, (char **)&RGBLED_DIALOG.jpg_buf, &RGBLED_DIALOG.jpg_size);
         
         if(res)
         {

            /* 根据图片数据创建JPG_DEC句柄 */
            dec = JPG_Open(RGBLED_DIALOG.jpg_buf, RGBLED_DIALOG.jpg_size);
            JPG_GetImageSize(&width, &height, dec);
            HDC hdc_mem = CreateMemoryDC(SURF_SCREEN, width, height);
            /* 绘制至内存对象 */
            JPG_Draw(hdc, 0, 0, dec);
         
            /* 关闭JPG_DEC句柄 */
            JPG_Close(dec);
         }         
         /* 释放图片内容空间 */
         RES_Release_Content((char **)&RGBLED_DIALOG.jpg_buf);
         #endif

         StretchBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_mem, 0, 0,RGBdesktop_0.Width, RGBdesktop_0.Height, SRCCOPY);          


         return TRUE;

      } 
      case WM_DESTROY:
      {        
         Delete_DlALOG();
         DeleteDC(hdc_mem);
         return PostQuitMessage(hwnd);	
      }          
      default:
         return	DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return	WM_NULL;
}


void	GUI_LED_DIALOG(void)
{
	HWND	hwnd;
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
   //初始化定时器
   //LED_GPIO_Config();
   TIM_GPIO_Config();
   TIM_Mode_Config();
   //SetColorValue(leddlg_S.col_R, leddlg_S.col_R, leddlg_S.col_R);
   
	//创建主窗口
	hwnd = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,
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

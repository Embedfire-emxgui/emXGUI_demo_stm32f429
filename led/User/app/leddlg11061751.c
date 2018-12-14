#include "emXGUI.h"
#include "x_libc.h"
#include "./led/bsp_led.h"  


#define PWM_LEDR_GPIO_PORT         GPIOH
#define PWM_LEDR_GPIO_PIN          GPIO_Pin_10
#define PWM_LEDR_GPIO_CLK          RCC_AHB1Periph_GPIOH
#define PWM_LEDR_PINSOURCE         GPIO_PinSource10
#define PWM_LEDR_AF                GPIO_AF_TIM5

#define PWM_LEDG_GPIO_PORT         GPIOH
#define PWM_LEDG_GPIO_PIN          GPIO_Pin_11
#define PWM_LEDG_GPIO_CLK          RCC_AHB1Periph_GPIOH
#define PWM_LEDG_PINSOURCE         GPIO_PinSource11
#define PWM_LEDG_AF                GPIO_AF_TIM5

#define PWM_LEDB_GPIO_PORT         GPIOH
#define PWM_LEDB_GPIO_PIN          GPIO_Pin_12
#define PWM_LEDB_GPIO_CLK          RCC_AHB1Periph_GPIOH
#define PWM_LEDB_PINSOURCE         GPIO_PinSource12
#define PWM_LEDB_AF                GPIO_AF_TIM5

#define PWM_LEDRGB_TIM             TIM5
#define PWM_LEDRGB_TIM_CLK         RCC_APB1Periph_TIM5

 /**
  * @brief  配置TIM3复用输出PWM时用到的I/O
  * @param  无
  * @retval 无
  */
static void TIM_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* GPIOB clock enable */
  RCC_AHB1PeriphClockCmd(PWM_LEDR_GPIO_CLK|PWM_LEDG_GPIO_CLK|PWM_LEDB_GPIO_CLK, ENABLE); 
  
  GPIO_PinAFConfig(PWM_LEDR_GPIO_PORT,PWM_LEDR_PINSOURCE,PWM_LEDR_AF); 
  GPIO_PinAFConfig(PWM_LEDG_GPIO_PORT,PWM_LEDG_PINSOURCE,PWM_LEDG_AF); 
  GPIO_PinAFConfig(PWM_LEDB_GPIO_PORT,PWM_LEDB_PINSOURCE,PWM_LEDB_AF); 
  
  /* 配置呼吸灯用到的PB0引脚 */
  GPIO_InitStructure.GPIO_Pin =  PWM_LEDR_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		    // 复用推挽输出
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;   //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
  GPIO_Init(PWM_LEDR_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin =  PWM_LEDG_GPIO_PIN;
  GPIO_Init(PWM_LEDG_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin =  PWM_LEDB_GPIO_PIN;
  GPIO_Init(PWM_LEDB_GPIO_PORT, &GPIO_InitStructure);
}

/**
  * @brief  配置TIM3输出的PWM信号的模式，如周期、极性
  * @param  无
  * @retval 无
  */
/*
 * TIMxCLK/CK_PSC --> TIMxCNT --> TIMx_ARR --> 中断 & TIMxCNT 重新计数
 *                    TIMx_CCR(电平发生变化)
 *
 * 信号周期=(TIMx_ARR +1 ) * 时钟周期
 * 
 */
/*    _______    ______     _____      ____       ___        __         _
 * |_|       |__|      |___|     |____|    |_____|   |______|  |_______| |________|
 */
static void TIM_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;																				
	
	/* 设置TIM3CLK 时钟为72MHZ */
   RCC_APB1PeriphClockCmd(PWM_LEDRGB_TIM_CLK, ENABLE); 					//使能TIM5时钟

   /* 基本定时器配置 */		 
   TIM_TimeBaseStructure.TIM_Period = 255;       							  //当定时器从0计数到255，即为266次，为一个定时周期
   TIM_TimeBaseStructure.TIM_Prescaler = 2499;	    							//设置预分频：
   TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;			//设置时钟分频系数：不分频(这里用不到)
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	//向上计数模式
   TIM_TimeBaseInit(PWM_LEDRGB_TIM, &TIM_TimeBaseStructure);

   /* PWM模式配置 */
   TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    				//配置为PWM模式1
   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//使能输出
   TIM_OCInitStructure.TIM_Pulse = 0;										  			//设置初始PWM脉冲宽度为0	
   TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;  	  //当定时器计数值小于CCR1_Val时为低电平
 
   TIM_OC1Init(PWM_LEDRGB_TIM, &TIM_OCInitStructure);	 									//使能通道3
   TIM_OC1PreloadConfig(PWM_LEDRGB_TIM, TIM_OCPreload_Enable);						//使能预装载	

   TIM_OC2Init(PWM_LEDRGB_TIM, &TIM_OCInitStructure);	 									//使能通道3
   TIM_OC2PreloadConfig(PWM_LEDRGB_TIM, TIM_OCPreload_Enable);						//使能预装载	
  
   TIM_OC3Init(PWM_LEDRGB_TIM, &TIM_OCInitStructure);	 									//使能通道3
   TIM_OC3PreloadConfig(PWM_LEDRGB_TIM, TIM_OCPreload_Enable);						//使能预装载	
  
   TIM_ARRPreloadConfig(PWM_LEDRGB_TIM, ENABLE);			 										//使能TIM5重载寄存器ARR
 
   /* TIM5 enable counter */
   TIM_Cmd(PWM_LEDRGB_TIM, ENABLE);                   										//使能定时器5
	
}

//RGBLED显示颜色
void SetRGBColor(uint32_t rgb)
{
	uint8_t r=0,g=0,b=0;
	r=(uint8_t)(rgb>>16);
	g=(uint8_t)(rgb>>8);
	b=(uint8_t)rgb;
	PWM_LEDRGB_TIM->CCR1 = r;	//根据PWM表修改定时器的比较寄存器值
	PWM_LEDRGB_TIM->CCR2 = g;	//根据PWM表修改定时器的比较寄存器值        
	PWM_LEDRGB_TIM->CCR3 = b;	//根据PWM表修改定时器的比较寄存器值
}

//RGBLED显示颜色
void SetColorValue(uint8_t r,uint8_t g,uint8_t b)
{
	PWM_LEDRGB_TIM->CCR1 = r;	//根据PWM表修改定时器的比较寄存器值
	PWM_LEDRGB_TIM->CCR2 = g;	//根据PWM表修改定时器的比较寄存器值        
	PWM_LEDRGB_TIM->CCR3 = b;	//根据PWM表修改定时器的比较寄存器值
}

//停止pwm输出
void TIM_RGBLED_Close(void)
{
	SetColorValue(0,0,0);
	TIM_ForcedOC1Config(PWM_LEDRGB_TIM,TIM_ForcedAction_InActive);
	TIM_ForcedOC2Config(PWM_LEDRGB_TIM,TIM_ForcedAction_InActive);
	TIM_ForcedOC3Config(PWM_LEDRGB_TIM,TIM_ForcedAction_InActive);
	TIM_ARRPreloadConfig(PWM_LEDRGB_TIM, DISABLE);
	TIM_Cmd(PWM_LEDRGB_TIM, DISABLE);                   							//失能定时器3						
	RCC_APB1PeriphClockCmd(PWM_LEDRGB_TIM_CLK, DISABLE); 	//失能定时器3时钟
	LED_GPIO_Config();
}




/**********************分界线*********************/
struct leddlg
{
	int col_R;  //R分量值
	int col_G;  //G分量值
	int col_B;  //B分量值
   int led_R;
   int led_G;
   int led_B;
   int colR_ctr;//控制参数
   int colG_ctr;//控制参数
   int colB_ctr;//控制参数
}leddlg_S;
/*滑动条ID宏定义*/
#define ID_SCROLLBAR_R  0x1100
#define ID_SCROLLBAR_G  0x1101
#define ID_SCROLLBAR_B  0x1102

/*复选框ID宏定义*/
#define ID_CHECKBOX_R	0x1010
#define ID_CHECKBOX_G	0x1011
#define ID_CHECKBOX_B	0x1012
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
	GetClientRect(hwnd, &rc);
	/* 背景 */
	SetBrushColor(hdc, MapRGB888(hdc, back_c));
	FillRect(hdc, &rc);

	/* 滚动条 */
	/* 边框 */
	InflateRect(&rc, 0, -rc.h >> 2);
	SetBrushColor(hdc, MapRGB(hdc, 169, 169, 169));
	FillRoundRect(hdc, &rc, MIN(rc.w, rc.h) >> 1);

	InflateRect(&rc, -2, -2);
	SetBrushColor(hdc, MapRGB888(hdc, Page_c));
	FillRoundRect(hdc, &rc, MIN(rc.w, rc.h) >> 1);

	/* 滑块 */
	SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc);

	SetBrushColor(hdc, MapRGB(hdc, 169, 169, 169));
	rc.y += (rc.h >> 2) >> 1;
	rc.h -= rc.h >> 2;
	/* 边框 */
	FillRoundRect(hdc, &rc, MIN(rc.w, rc.h) >> 2);
	InflateRect(&rc, -2, -2);

	SetBrushColor(hdc, MapRGB888(hdc, fore_c));
	FillRoundRect(hdc, &rc, MIN(rc.w, rc.h) >> 2);
}
/*
 * @brief  自定义回调函数
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
	draw_scrollbar(hwnd, hdc_mem1, RGB888(0, 0, 0), RGB888(250, 250, 250), RGB888(255, 255, 255));
	//绘制绿色类型的滚动条
	switch (ds->ID)
	{
		case ID_SCROLLBAR_R:
		{
			draw_scrollbar(hwnd, hdc_mem, RGB888(0, 0, 0), RGB888(leddlg_S.col_R, 0, 0), RGB888(leddlg_S.col_R, 0, 0));
			break;
		}
		case ID_SCROLLBAR_G:
		{
			draw_scrollbar(hwnd, hdc_mem, RGB888(0, 0, 0), RGB888(0, leddlg_S.col_G, 0), RGB888(0, leddlg_S.col_G, 0));
			break;
		}
		case ID_SCROLLBAR_B:
		{
			draw_scrollbar(hwnd, hdc_mem, RGB888(0, 0, 0), RGB888(0, 0, leddlg_S.col_B), RGB888(0, 0, leddlg_S.col_B));
			break;
		}
	}
	SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc);
	//右
	BitBlt(hdc, rc_cli.x, rc_cli.y, rc.x, rc_cli.h, hdc_mem, 0, 0, SRCCOPY);
	//左
	BitBlt(hdc, rc.x + rc.w, 0, rc_cli.w - (rc.x + rc.w), rc_cli.h, hdc_mem1, rc.x + rc.w, 0, SRCCOPY);

	//绘制滑块
	if (ds->State & SST_THUMBTRACK)//按下
	{
		BitBlt(hdc, rc.x, 0, rc.w, rc_cli.h, hdc_mem, rc.x, 0, SRCCOPY);
	}
	else//未选中
	{
		BitBlt(hdc, rc.x, 0, rc.w, rc_cli.h, hdc_mem1, rc.x, 0, SRCCOPY);
	}
	//释放内存MemoryDC
	DeleteDC(hdc_mem1);
	DeleteDC(hdc_mem);
}

static	LRESULT	win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
//	RECT rc;
	HWND wnd;

	static RECT rc_R, rc_G, rc_B;//RGB分量指示框

   //HDC hdc_mem2pic;
	switch (msg)
	{
	case WM_CREATE: {
		/*设置滑动条的参数*/
		SCROLLINFO sif;
      
		sif.cbSize = sizeof(sif);
		sif.fMask = SIF_ALL;
		sif.nMin = 0;
		sif.nMax = 255;
		sif.nValue = 0;
		sif.TrackSize = 30;
		sif.ArrowSize = 0;//20;

		/*创建滑动条--R*/
		wnd = CreateWindow(SCROLLBAR, L"SCROLLBAR_R", WS_OWNERDRAW | WS_VISIBLE, 255, 120, 255, 40, hwnd, ID_SCROLLBAR_R, NULL, NULL);
		SendMessage(wnd, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif);
		/*配置R分量指示框的位置大小*/
		rc_R.x = 160;
		rc_R.y = 120;
		rc_R.w = 40;
		rc_R.h = 40;
		/*创建滑动条--G*/
		wnd = CreateWindow(SCROLLBAR, L"SCROLLBAR_G", WS_OWNERDRAW | WS_VISIBLE, 255, 240, 255, 40, hwnd, ID_SCROLLBAR_G, NULL, NULL);
		SendMessage(wnd, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif);
		/*配置G分量指示框的位置大小*/
		rc_G.x = 160;
		rc_G.y = 240;
		rc_G.w = 40;
		rc_G.h = 40;
		/*创建滑动条--B*/
		wnd = CreateWindow(SCROLLBAR, L"SCROLLBAR_B", WS_OWNERDRAW | WS_VISIBLE, 255, 360, 255, 40, hwnd, ID_SCROLLBAR_B, NULL, NULL);
		SendMessage(wnd, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif);
		/*配置B分量指示框的位置大小*/
		rc_B.x = 160;
		rc_B.y = 360;
		rc_B.w = 40;
		rc_B.h = 40;  
      /*创建复选框--R(on/off)*/
		CreateWindow(BUTTON, L"", BS_CHECKBOX |BS_NOTIFY| WS_VISIBLE, 80, 138, 30, 30, hwnd, ID_CHECKBOX_R, NULL, NULL);
		/*创建复选框--G(on/off)*/
		CreateWindow(BUTTON, L"", BS_CHECKBOX | WS_VISIBLE, 80, 258, 30, 30, hwnd, ID_CHECKBOX_G, NULL, NULL);
		/*创建复选框--B(on/off)*/
		CreateWindow(BUTTON, L"", BS_CHECKBOX | WS_VISIBLE, 80, 378, 30, 30, hwnd, ID_CHECKBOX_B, NULL, NULL);             
	}
	case WM_NOTIFY: {
		NMHDR *nr;
		u16 ctr_id, code, id;;
		ctr_id = LOWORD(wParam); //wParam低16位是发送该消息的控件ID.
		nr = (NMHDR*)lParam; //lParam参数，是以NMHDR结构体开头.
      id  =LOWORD(wParam);
		code=HIWORD(wParam);
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
					InvalidateRect(hwnd, NULL, FALSE );//窗口重绘，触发R分量指示框改变
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
					InvalidateRect(hwnd, NULL, FALSE);//窗口重绘，触发G分量指示框改变
				}
				break;
			}
		}

		if (ctr_id == ID_SCROLLBAR_B)
		{
			NM_SCROLLBAR *sb_nr;
			sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar的通知消息实际为 NM_SCROLLBAR扩展结构,里面附带了更多的信息.
			switch (nr->code)
			{
			case SBN_THUMBTRACK: //B滑块移动
			{
				leddlg_S.col_B = sb_nr->nTrackValue; //获得B滑块当前位置值
				SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, leddlg_S.col_B); //设置B滑块的位置
				InvalidateRect(hwnd, NULL, FALSE);//窗口重绘，触发B分量指示框改变
			}
			break;
			}
		}
      if(code == BN_CLICKED && id == ID_CHECKBOX_R) //被点击了
		{     
         if(SendMessage(nr->hwndFrom,BM_GETSTATE,0,0)&BST_CHECKED) //获取当前状态
         {            
            leddlg_S.colR_ctr = 1;
         }
         else
         {
            leddlg_S.colR_ctr = 0;
         }         
		}
      if(code == BN_CLICKED && id == ID_CHECKBOX_G) //被点击了
		{
         if(SendMessage(nr->hwndFrom,BM_GETSTATE,0,0)&BST_CHECKED) //获取当前状态
          {            
            leddlg_S.colG_ctr = 1;
         }
         else
         {
            leddlg_S.colG_ctr = 0;
         } 
		}
      if(code == BN_CLICKED && id == ID_CHECKBOX_B) //被点击了
		{
         if(SendMessage(nr->hwndFrom,BM_GETSTATE,0,0)&BST_CHECKED) //获取当前状态
         {            
            leddlg_S.colB_ctr = 1;
         }
         else
         {
            leddlg_S.colB_ctr = 0;
         } 
		}   
      leddlg_S.led_R=(leddlg_S.colR_ctr == 0)? 0:leddlg_S.col_R;
      leddlg_S.led_G=(leddlg_S.colG_ctr == 0)? 0:leddlg_S.col_G;
      leddlg_S.led_B=(leddlg_S.colB_ctr == 0)? 0:leddlg_S.col_B;
      SetColorValue(leddlg_S.led_R, leddlg_S.led_G, leddlg_S.led_B);
		break;
	}
	case WM_DRAWITEM:
	{
		DRAWITEM_HDR *ds;
		ds = (DRAWITEM_HDR*)lParam;
		if(ds->ID == ID_SCROLLBAR_R || ds->ID == ID_SCROLLBAR_G || ds->ID == ID_SCROLLBAR_B)
		{
			scrollbar_owner_draw(ds);
			return TRUE;
		}
	}
   
/*由文字框，颜色指示框，复选框（开关），颜色分量值，滑动条控件组成；
 *位置关系：
 *	  文字框     颜色      颜色			滑
 *										      动
 *	  复选框    指示框	    分量值	   条
*/ 
	case	WM_PAINT: //窗口需要重绘制时，会自动收到该消息.
	{	
		PAINTSTRUCT ps;
      RECT rc_cli;
		WCHAR wbuf[128];		
		RECT rc_text = {210, 127, 40, 25 };//颜色分量值
		RECT rc_sign = {30, 110, 120, 25};//文字框
      HDC hdc_mem;
		hdc = BeginPaint(hwnd, &ps);
      GetClientRect(hwnd, &rc_cli);//获取客户端坐标
      
      hdc_mem = CreateMemoryDC(SURF_SCREEN, 800, 480);//创建MemoryDC
      
      SetBrushColor(hdc_mem, MapRGB(hdc_mem, 240, 250, 0));
      FillRect(hdc_mem, &rc_cli);
      
		SetTextColor(hdc_mem, MapRGB(hdc_mem, 0, 0, 0));
		/*修改R指示框的颜色*/
		SetBrushColor(hdc_mem, MapRGB(hdc_mem, leddlg_S.col_R, 0, 0));
		FillRect(hdc_mem, &rc_R);
		/*显示R分量值*/
		x_wsprintf(wbuf, L"%d", leddlg_S.col_R);		
		DrawTextEx(hdc_mem, wbuf, -1, &rc_text,
				     DT_CENTER, NULL);
      
		/*修改G指示框的颜色*/
		SetBrushColor(hdc_mem, MapRGB(hdc_mem, 0, leddlg_S.col_G, 0));
		FillRect(hdc_mem, &rc_G);
      
      DrawTextEx(hdc_mem, L"R(ON/OFF):", -1, &rc_sign, DT_CENTER, NULL);
		rc_sign.y = 230;
		DrawTextEx(hdc_mem, L"G(ON/OFF):", -1, &rc_sign, DT_CENTER, NULL);
		rc_sign.y = 350;
		DrawTextEx(hdc_mem, L"B(ON/OFF):", -1, &rc_sign, DT_CENTER, NULL);
      
		/*显示G分量值*/
		x_wsprintf(wbuf, L"%d", leddlg_S.col_G);
		rc_text.y = 247;	
		DrawTextEx(hdc_mem, wbuf, -1, &rc_text,
				   DT_CENTER, NULL);
		/*修改B指示框的颜色*/
		SetBrushColor(hdc_mem, MapRGB(hdc_mem, 0, 0,  leddlg_S.col_B));
		FillRect(hdc_mem, &rc_B);
		/*显示G分量值*/
		x_wsprintf(wbuf, L"%d", leddlg_S.col_B);
		rc_text.y = 367;	
		DrawTextEx(hdc_mem, wbuf, -1, &rc_text,
			       DT_CENTER, NULL);
		/*RGB灯的颜色*/
		SetBrushColor(hdc_mem, MapRGB(hdc_mem, leddlg_S.col_R, leddlg_S.col_G, leddlg_S.col_B));
		FillCircle(hdc_mem, 660, 260, 100);
      
      BitBlt(hdc, 0, 0, 800, 480, hdc_mem, 0, 0, SRCCOPY);
      
      DeleteDC(hdc_mem);
		EndPaint(hwnd, &ps);
		return	TRUE;
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
   leddlg_S.colR_ctr = 0;
   leddlg_S.colG_ctr = 0;
   leddlg_S.colB_ctr = 0;
   //关闭PWM输出
// TIM_ForcedOC1Config(PWM_LEDRGB_TIM,TIM_ForcedAction_InActive);
//	TIM_ForcedOC2Config(PWM_LEDRGB_TIM,TIM_ForcedAction_InActive);
//	TIM_ForcedOC3Config(PWM_LEDRGB_TIM,TIM_ForcedAction_InActive);
	//创建主窗口
	hwnd = CreateWindowEx(NULL,
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

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
#define ID_TEXTBOX_R	      0x1010
#define ID_TEXTBOX_R_NUM	0x1013
#define ID_TEXTBOX_G	      0x1011
#define ID_TEXTBOX_G_NUM   0x1014
#define ID_TEXTBOX_B	      0x1012
#define ID_TEXTBOX_B_NUM   0x1015


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
	SetBrushColor(hdc, MapRGB888(hdc, back_c));
	FillRect(hdc, &rc);

//	/* 滚动条 */
//	/* 边框 */
//	InflateRect(&rc, -rc.w >> 2, 0);
//	SetBrushColor(hdc, MapRGB(hdc, 169, 169, 169));
//	FillRoundRect(hdc, &rc, MIN(rc.w, rc.h) >> 1);

//	InflateRect(&rc, -2, -2);
//	SetBrushColor(hdc, MapRGB888(hdc, Page_c));
//	FillRoundRect(hdc, &rc, MIN(rc.w, rc.h) >> 1);
   rc_scrollbar.x = rc.w/2;
   rc_scrollbar.y = rc.y;
   rc_scrollbar.w = 2;
   rc_scrollbar.h = rc.h;
   
	SetBrushColor(hdc, MapRGB888(hdc, Page_c));
	FillRect(hdc, &rc_scrollbar);

	/* 滑块 */
	SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc);

	SetBrushColor(hdc, MapRGB(hdc, 169, 169, 169));
	rc.x += (rc.w >> 2) >> 1;
	rc.w -= rc.w >> 2;
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

static	LRESULT	win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
//	RECT rc;
	HWND wnd;

	//static RECT rc_R, rc_G, rc_B;//RGB分量指示框

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
		sif.TrackSize = 40;
		sif.ArrowSize = 0;//20;

		/*创建滑动条--R*/
		wnd = CreateWindow(SCROLLBAR, L"SCROLLBAR_R", SBS_VERT|WS_OWNERDRAW | WS_VISIBLE, 215, 120, 50, 255, hwnd, ID_SCROLLBAR_R, NULL, NULL);
		SendMessage(wnd, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif);

		/*创建滑动条--G*/
		wnd = CreateWindow(SCROLLBAR, L"SCROLLBAR_G", SBS_VERT|WS_OWNERDRAW | WS_VISIBLE, 375, 120, 50, 255, hwnd, ID_SCROLLBAR_G, NULL, NULL);
		SendMessage(wnd, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif);

		/*创建滑动条--B*/
		wnd = CreateWindow(SCROLLBAR, L"SCROLLBAR_B", SBS_VERT|WS_OWNERDRAW | WS_VISIBLE, 535, 120, 50, 255, hwnd, ID_SCROLLBAR_B, NULL, NULL);
		SendMessage(wnd, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif);

 
//    /*创建文本框--R*/
		CreateWindow(TEXTBOX, L"R", WS_VISIBLE, 215, 80, 50, 40, hwnd, ID_TEXTBOX_R, NULL, NULL);
      SendMessage(GetDlgItem(hwnd, ID_TEXTBOX_R),TBM_SET_TEXTFLAG,0,
                     DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND); 
      //R的分量值               
		CreateWindow(TEXTBOX, L"0", WS_VISIBLE, 215, 375, 60, 40, hwnd, ID_TEXTBOX_R_NUM, NULL, NULL);
      SendMessage(GetDlgItem(hwnd, ID_TEXTBOX_R_NUM),TBM_SET_TEXTFLAG,0,
                     DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND);                      
		/*创建复选框--G(on/off)*/
		CreateWindow(TEXTBOX, L"G", WS_VISIBLE, 375, 80, 50, 40, hwnd, ID_TEXTBOX_G, NULL, NULL);
      SendMessage(GetDlgItem(hwnd, ID_TEXTBOX_G),TBM_SET_TEXTFLAG,0,
                     DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND);   
      //G的分量值               
		CreateWindow(TEXTBOX, L"0", WS_VISIBLE, 375, 375, 60, 40, hwnd, ID_TEXTBOX_G_NUM, NULL, NULL);
      SendMessage(GetDlgItem(hwnd, ID_TEXTBOX_G_NUM),TBM_SET_TEXTFLAG,0,
                     DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND);                       
		/*创建复选框--B(on/off)*/
		CreateWindow(TEXTBOX, L"B", WS_VISIBLE, 535, 80, 50, 40, hwnd, ID_TEXTBOX_B, NULL, NULL);     
      SendMessage(GetDlgItem(hwnd, ID_TEXTBOX_B),TBM_SET_TEXTFLAG,0,
                     DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND);
      //BG的分量值               
		CreateWindow(TEXTBOX, L"0", WS_VISIBLE, 535, 375, 60, 40, hwnd, ID_TEXTBOX_B_NUM, NULL, NULL);
      SendMessage(GetDlgItem(hwnd, ID_TEXTBOX_B_NUM),TBM_SET_TEXTFLAG,0,
                     DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND);      

      break;
	}
   case WM_LBUTTONDOWN:
   {
      POINT pt;
      pt.x =GET_LPARAM_X(lParam); //获得X坐标
      pt.y =GET_LPARAM_Y(lParam); //获得Y坐标
      RECT rc = {720, 0, 80, 80};
      if(PtInRect(&rc, &pt))
      {
         PostCloseMessage(hwnd);
         //产生WM_CLOSE消息关闭主窗口
      }
      break;         
   }   
	case WM_NOTIFY: {
		NMHDR *nr;
      WCHAR wbuf[128];
		u16 ctr_id; 
		ctr_id = LOWORD(wParam); //wParam低16位是发送该消息的控件ID.
		nr = (NMHDR*)lParam; //lParam参数，是以NMHDR结构体开头.
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
               x_wsprintf(wbuf, L"%d", leddlg_S.col_B);
               SetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_B_NUM), wbuf);
            }
            break;
			}
		}
  
      leddlg_S.led_R=(leddlg_S.colR_ctr != 0)? 0:leddlg_S.col_R;
      leddlg_S.led_G=(leddlg_S.colG_ctr != 0)? 0:leddlg_S.col_G;
      leddlg_S.led_B=(leddlg_S.colB_ctr != 0)? 0:leddlg_S.col_B;
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
      if(id== ID_TEXTBOX_R_NUM || id== ID_TEXTBOX_G_NUM || id== ID_TEXTBOX_B_NUM)
      {

         cr->TextColor =RGB888(255,255,255);//文字颜色（RGB888颜色格式)
         cr->BackColor =RGB888(0,0,0);//背景颜色（RGB888颜色格式)
         cr->BorderColor =RGB888(255,0,0);//边框颜色（RGB888颜色格式)
         return TRUE;
      }
      switch(id)
      {
         case ID_TEXTBOX_R:
         {
            cr->TextColor =RGB888(255,0,0);//文字颜色（RGB888颜色格式)
            cr->BackColor =RGB888(0,0,0);//背景颜色（RGB888颜色格式)
            cr->BorderColor =RGB888(255,0,0);//边框颜色（RGB888颜色格式)            
            break;
         }
         case ID_TEXTBOX_G:
         {
            cr->TextColor =RGB888(0,255,0);//文字颜色（RGB888颜色格式)
            cr->BackColor =RGB888(0,0,0);//背景颜色（RGB888颜色格式)
            cr->BorderColor =RGB888(255,0,0);//边框颜色（RGB888颜色格式)              
            break;
         }
         case ID_TEXTBOX_B:
         {
            cr->TextColor =RGB888(0,0,255);//文字颜色（RGB888颜色格式)
            cr->BackColor =RGB888(0,0,0);//背景颜色（RGB888颜色格式)
            cr->BorderColor =RGB888(255,0,0);//边框颜色（RGB888颜色格式)                       
            break;
         }
         case ID_TEXTBOX_R_NUM:
         {
            cr->TextColor =RGB888(255,255,255);//文字颜色（RGB888颜色格式)
            cr->BackColor =RGB888(0,0,0);//背景颜色（RGB888颜色格式)
            cr->BorderColor =RGB888(255,0,0);//边框颜色（RGB888颜色格式)            
         }
         case ID_TEXTBOX_G_NUM:
         {
            cr->TextColor =RGB888(255,255,255);//文字颜色（RGB888颜色格式)
            cr->BackColor =RGB888(0,0,0);//背景颜色（RGB888颜色格式)
            cr->BorderColor =RGB888(255,0,0);//边框颜色（RGB888颜色格式)          
         }
         case ID_TEXTBOX_B_NUM:
         {
            cr->TextColor =RGB888(255,255,255);//文字颜色（RGB888颜色格式)
            cr->BackColor =RGB888(0,0,0);//背景颜色（RGB888颜色格式)
            cr->BorderColor =RGB888(255,0,0);//边框颜色（RGB888颜色格式)     
            break;
         }
         default:
            return FALSE;
         
      }
      return TRUE;
      
   }   
   case WM_ERASEBKGND:
   {
      HDC hdc =(HDC)wParam;
      RECT rc;
      GetClientRect(hwnd, &rc);
      
      SetBrushColor(hdc, MapRGB(hdc, 0, 0, 0));
      FillRect(hdc, &rc);
      
      
      return TRUE;
      
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
      HDC hdc, hdc_mem, hdc_mem1;//屏幕hdc
      RECT rc = {0,0,72,72};
      RECT rc_cli = {0,0,72,72};
      GetClientRect(hwnd, &rc_cli);
      hdc = BeginPaint(hwnd, &ps); 
      hdc_mem = CreateMemoryDC(SURF_SCREEN, 72, 72);
      hdc_mem1 = CreateMemoryDC(SURF_SCREEN, 72, 72);
      
      
      /****************返回主界面按钮******************/
      SetBrushColor(hdc, MapRGB(hdc, 0,0,0));
      FillCircle(hdc, rc_cli.w, 0, 80);  
      
      SetBrushColor(hdc, MapRGB(hdc, 250,0,0));
      FillCircle(hdc, rc_cli.w, 0, 76); 
      //字体层
      SetBrushColor(hdc_mem1, MapRGB(hdc, 250,0,0));
      FillRect(hdc_mem1, &rc);        
      
//      SetFont(hdc_mem1, hFont_SDCARD);
      SetTextColor(hdc_mem1, MapRGB(hdc_mem1, 250, 250,250));
      TextOut(hdc_mem1, 0, 0, L"O", -1);

      StretchBlt(hdc, 755, 12, 40, 40, 
                 hdc_mem1, 0, 0, 72, 72, SRCCOPY);

      DeleteDC(hdc_mem);
      DeleteDC(hdc_mem1);
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

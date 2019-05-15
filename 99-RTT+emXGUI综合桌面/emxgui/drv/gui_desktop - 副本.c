/**
  *********************************************************************
  * @file    desktop.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   emXGUI 桌面
  *********************************************************************
  * 实验平台:野火 F429-挑战者 STM32 开发板
  * 官网    :www.embedfire.com
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  **********************************************************************
  */ 


#include	"rtthread.h"
#include "emXGUI.h"





/*===================================================================================*/
extern HWND	slideWin_hwnd;

extern void GUI_AppMain(void);
extern void	GUI_DEMO_SlideWindow(void);

#define INFO_HEAD_HEIGHT   70

#define WM_MSG_SLIDE  (WM_USER + 10)


static	void	gui_app_thread(void *p)
{
    #if(GUI_TOUCHSCREEN_EN & GUI_TOUCHSCREEN_CALIBRATE)
    {
        int i=0;
        while(TouchPanel_IsPenDown())
    	{
    		GUI_msleep(100);
    		if(i++>10)
    		{
    			ShowCursor(FALSE);
    			TouchScreenCalibrate(NULL);
    			ShowCursor(TRUE);
    			break;
    		}
    	}
    }
    #endif
		
  /* 调用APP函数 */  
	GUI_AppMain();
 //   GUI_UserAppStart();
//   	ShellWindowStartup();
  //  return 0;
}

/*===================================================================================*/

/**
  * @brief  桌面背景绘制函数，触发背景绘制时会调用本函数，
            通过修改本函数的内容可更改桌面的背景
  * @param  hdc 绘图上下文
  * @param  lprc 要绘制的矩形区域，为NULL时会自动绘制hwnd的客户区
  * @param  hwnd 窗口对象句柄
  * @retval 无
  */
static	void	_EraseBackgnd(HDC hdc,const RECT *lprc,HWND hwnd)
{
	RECT rc;

	if(lprc==NULL)
	{
		GetClientRect(hwnd,&rc);
	}
	else
	{
		CopyRect(&rc,lprc);
	}

	SetBrushColor(hdc,MapRGB(hdc,215,61,50));
	FillRect(hdc,&rc);
  	
  SetTextColor(hdc,MapRGB(hdc,255,255,255));
  
  SetTextColor(hdc,MapRGB(hdc,250,250,250));
  DrawText(hdc,L"emXGUI@Embedfire STM32F429 ",-1,&rc,DT_CENTER);

  
//  SetFont(hdc, iconFont);
//	DrawText(hdc,L" A B C D E \r\n F G H I J",-1,&rc,DT_LEFT|DT_VCENTER);

//  SetFont(hdc, defaultFont);
  
  GetClientRect(hwnd,&rc);
  SetBrushColor(hdc,MapRGB(hdc,82,85,82));
  rc.y = GUI_YSIZE - INFO_HEAD_HEIGHT;
  rc.h = INFO_HEAD_HEIGHT;
  FillRect(hdc,&rc);
  
  /* 首栏 */ 
  SetFont(hdc, logoFont); 
  /* 显示logo */
//  GetClientRect(hwnd,&rc);
  rc.y += 5;        
  SetTextColor(hdc,MapRGB(hdc,250,0,0)); 
  DrawText(hdc,L" B",-1,&rc,DT_LEFT);
  
  /* 恢复默认字体 */
  SetFont(hdc, defaultFont);

  GetClientRect(hwnd,&rc);
  rc.y = GUI_YSIZE - INFO_HEAD_HEIGHT;
    rc.h = INFO_HEAD_HEIGHT;
     
  SetTextColor(hdc,MapRGB(hdc,250,250,250)); 
  DrawText(hdc,L"▲",-1,&rc,DT_CENTER);
  
  rc.x +=60;
  rc.y +=10;
  DrawText(hdc,L" 野火\r\n emXGUI",-1,&rc,DT_LEFT);

  GetClientRect(hwnd,&rc);
  rc.y = GUI_YSIZE - INFO_HEAD_HEIGHT;
  rc.h = INFO_HEAD_HEIGHT;

  rc.y += 10;
  DrawText(hdc,L"\r\n详细",-1,&rc,DT_CENTER);
  DrawText(hdc,L"\r\nwww.embedFire.com  ",-1,&rc,DT_RIGHT);  

  
}

#if 0
static	int	gui_input_thread(void *p)
{
	SYS_thread_set_priority(NULL,+4);
	while(1)
	{
		GUI_InputHandler(); //处理输入设备
		GUI_msleep(20);
	}
}
#endif


/**
  * @brief  桌面回调函数
  * @param  hwnd 当前处理该消息的窗口对象句柄
  * @param  msg 消息类型值，用以标识和区分当前产生的消息
  * @param  wParam 消息参数值，根据msg消息代码值不同
  * @param  lParam 消息参数值，根据msg消息代码值不同
  * @retval 返回给SendMessage的值
  */
static 	 LRESULT  	desktop_proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    static int win_pos=0;

	switch(msg)
	{
    /* 桌面创建时,会产生该消息,可以在这里做一些初始化工作. */
		case	WM_CREATE:	


			   ////创建1个20ms定时器，处理循环事件.
				 SetTimer(hwnd,1,20,TMR_START,NULL);

				//创建App线程						
				if(1)
				{
						rt_thread_t h;
					
						h=rt_thread_create("GUI_APP",gui_app_thread,NULL,8192,5,5);
						rt_thread_startup(h);		

						h=rt_thread_create("GUI_slide",GUI_DEMO_SlideWindow,NULL,8192,5,5);
						rt_thread_startup(h);				
          
				}

				break;

		/* 定时处理输入设备的信息 */
		case	WM_TIMER:
      #if(GUI_INPUT_DEV_EN)
        {
          u16 id;

          id =LOWORD(wParam);
          if(id==1)
          {
            GUI_InputHandler(); //处理输入设备
          }
        }
      #endif
		break;

    /* 客户区背景需要被擦除 */
		case	WM_ERASEBKGND:
		{
			HDC hdc =(HDC)wParam;
			_EraseBackgnd(hdc,NULL,hwnd);
		}
		return TRUE;
    
    case WM_LBUTTONDOWN:
		{

			POINT pt;
			RECT rc;

			pt.x =GET_LPARAM_X(lParam);
			pt.y =GET_LPARAM_Y(lParam);

			GetClientRect(hwnd,&rc);
      GUI_DEBUG("pt.x =%d,pt.y=%d",pt.x,pt.y);

      if(win_pos == 0)
      {
        rc.y = GUI_YSIZE - INFO_HEAD_HEIGHT;
        rc.h = INFO_HEAD_HEIGHT;
      }

			if(PtInRect(&rc,&pt))
			{
//        KillTimer(hwnd,2);
//        SetTimer(hwnd,2,5,TMR_START,NULL);

        /* 发送消息给滑动窗口 */
				if(win_pos==0)
				{
					win_pos =1;          
				}
				else
				{
					win_pos =0;
				}
        
        if(slideWin_hwnd)
        {
          SendMessage(slideWin_hwnd,WM_MSG_SLIDE,win_pos,0); 
          GUI_DEBUG("SendMesage");

        }
			}
		}
		break;

    /* 用户不关心的信息，由系统处理 */
		default:
				return	DefDesktopProc(hwnd,msg,wParam,lParam);
	}

	return WM_NULL;

}

/**
  * @brief  桌面启动函数
  * @param  无
  * @retval 无
  */
void GUI_DesktopStartup(void)
{
	WNDCLASS	wcex;
	HWND hwnd;
	MSG msg;

	wcex.Tag 		    = WNDCLASS_TAG;

	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= desktop_proc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;//hInst;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;//LoadCursor(NULL, IDC_ARROW);

	//创建桌面窗口.
	hwnd = GUI_CreateDesktop(	WS_EX_LOCKPOS,
                              &wcex,
                              L"DESKTOP",
                              WS_VISIBLE|WS_CLIPCHILDREN,
                              0,0,GUI_XSIZE,GUI_YSIZE,
                              NULL,0,NULL,NULL);

	GUI_Printf("HWND_Desktop=%08XH\r\n",	hwnd);

	//显示桌面窗口.
	ShowWindow(hwnd,SW_SHOW);

	//设置系统打开光标显示(可以按实际情况看是否需要).
//	ShowCursor(TRUE);

	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

/********************************END OF FILE****************************/


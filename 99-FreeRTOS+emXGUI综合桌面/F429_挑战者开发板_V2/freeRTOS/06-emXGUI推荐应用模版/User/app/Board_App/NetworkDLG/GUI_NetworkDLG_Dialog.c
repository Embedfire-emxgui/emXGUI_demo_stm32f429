#include <emXGUI.h>
#include <string.h>
#include <stdio.h>
#include "x_libc.h"
#include "GUI_AppDef.h"
#include "netconf.h"
#include "tcp_echoclient.h"
#include "tcp_echoserver.h"
#include "udp_echoclient.h"

/* 单选框 ID */
#define ID_RB1    (0x1100 | (1<<16))
#define ID_RB2    (0x1101 | (1<<16))
#define ID_RB3    (0x1102 | (1<<16))


/* 窗口 ID */
#define ID_TEXTBOX_Title       0x00     // 标题栏
#define ID_TEXTBOX_Send        0x01     // 发送显示
#define ID_TEXTBOX_Receive     0x02     // 接收显示
#define ID_TEXTBOX_RemoteIP1   0x07     // 远端IP
#define ID_TEXTBOX_RemoteIP2   0x08     // 远端IP
#define ID_TEXTBOX_RemoteIP3   0x09     // 远端IP
#define ID_TEXTBOX_RemoteIP4   0x0A     // 远端IP
#define ID_TEXTBOX_RemotePort  0x0B     // 远端端口

/* 按钮 ID */
#define eID_Network_EXIT    0x03
#define eID_LINK_STATE      0x04
#define eID_Network_Send    0x05
#define eID_Receive_Clear   0x06

#define TitleHeight     70

int8_t NetworkTypeSelection = 0;

HWND Send_Handle;
HWND Receive_Handle;
static HWND Network_Main_Handle;

extern struct netif gnetif;
extern __IO uint8_t EthLinkStatus;
__IO uint32_t LocalTime = 0; /* this variable is used to create a time reference incremented by 10ms */
DRV_NETWORK drv_network;
uint16_t bsp_result=0;

/**
  * @brief  通用定时器3中断初始化
  * @param  period : 自动重装值。
  * @param  prescaler : 时钟预分频数
  * @retval 无
  * @note   定时器溢出时间计算方法:Tout=((period+1)*(prescaler+1))/Ft us.
  *          Ft=定时器工作频率,为SystemCoreClock/2=90,单位:Mhz
  */
static void TIM3_Config(uint16_t period,uint16_t prescaler)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///使能TIM3时钟
	
	TIM_TimeBaseInitStructure.TIM_Prescaler=prescaler;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_Period=period;   //自动重装载值
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //允许定时器3更新中断
	TIM_Cmd(TIM3,ENABLE); //使能定时器3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

#ifdef   AAAA// 在Backend_vidoplayer.c中实现
/**
  * @brief  定时器3中断服务函数
  * @param  无
  * @retval 无
  */
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
	{
		LocalTime+=10;//10ms增量
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位
}
#endif
void Network_Dispose_Task(void *p) 
{
  static uint8_t startflag=0;

  if(startflag==0)
  {
    /* Configure ethernet (GPIOs, clocks, MAC, DMA) */
    if(ETH_BSP_Config()==1)
    {
      bsp_result |=1;
    }
    else
    {
      bsp_result &=~ 1;        
    }
    startflag=1;
  }
  if((drv_network.net_init==0)&&((bsp_result&1)==0))
  {     
    /* Initilaize the LwIP stack */
    LwIP_Init(); 

    drv_network.net_local_ip1=(uint8_t)(gnetif.ip_addr.addr&0xFF);
    drv_network.net_local_ip2=(uint8_t)((gnetif.ip_addr.addr>>8)&0xFF);
    drv_network.net_local_ip3=(uint8_t)((gnetif.ip_addr.addr>>16)&0xFF);
    drv_network.net_local_ip4=(uint8_t)((gnetif.ip_addr.addr>>24)&0xFF);
    drv_network.net_local_port=LOCAL_PORT;
    
    drv_network.net_remote_ip1=DEST_IP_ADDR0;
    drv_network.net_remote_ip2=DEST_IP_ADDR1;
    drv_network.net_remote_ip3=DEST_IP_ADDR2;
    drv_network.net_remote_ip4=DEST_IP_ADDR3;
    drv_network.net_remote_port=DEST_PORT;     
    
    drv_network.net_init=1;
  }

  //// WM_DeleteWindow(WM_GetClientWindow(htext));
  if(bsp_result&1)
  {		
    char str[30];
    if(startflag==2)
    {
      /* Configure ethernet (GPIOs, clocks, MAC, DMA) */
      if(ETH_BSP_Config()==1)
      {
        bsp_result |= 1;
        sprintf(str," ");  
      }
      else
      {
        bsp_result &=~ 1;    
        sprintf(str,"< must be restart Safari >"); 
      }
    }
    else
    {
      sprintf(str," ");  
    }
    startflag=2;
    // ErrorDialog(hWin,UTF8_NETWORKERROR,str);

  }
  // DROPDOWN_SetSel(WM_GetDialogItem(hWin, GUI_ID_DROPDOWN0),drv_network.net_type);
  // EDIT_SetValue(WM_GetDialogItem(hWin, GUI_ID_EDIT0),drv_network.net_remote_ip1);
  // EDIT_SetValue(WM_GetDialogItem(hWin, GUI_ID_EDIT1),drv_network.net_remote_ip2);
  // EDIT_SetValue(WM_GetDialogItem(hWin, GUI_ID_EDIT2),drv_network.net_remote_ip3);
  // EDIT_SetValue(WM_GetDialogItem(hWin, GUI_ID_EDIT3),drv_network.net_remote_ip4);
  // EDIT_SetValue(WM_GetDialogItem(hWin, GUI_ID_EDIT4),drv_network.net_remote_port);
  // WM_InvalidateWindow(hWin);
  InvalidateRect(Network_Main_Handle, NULL, TRUE);
  drv_network.net_connect=0;
  drv_network.net_type=0; 
  TIM3_Config(999,899);//10ms定时器 
  LocalTime=0;
  TIM_SetCounter(TIM3,0);
  EthLinkStatus=0;
  while(1)
  {
    // if(EthLinkStatus)
    // {
    //   ErrorDialog(hWin,UTF8_NETWORKERROR,UTF8_NETWORKERROR1);
    //   while(1)
    //   {
    //     if(tpad_flag)WM_DeleteWindow(hWin);
    //      if(!Flag_ICON6)return;
    //      GUI_Delay(10);
    //   }
    // }
    /* check if any packet received */
    if (ETH_CheckFrameReceived())
    { 
      /* process received ethernet packet */
      LwIP_Pkt_Handle();
    }
    /* handle periodic timers for LwIP */
    LwIP_Periodic_Handle(LocalTime);

    GUI_msleep(3);//WM_Exec();//
  }
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
	// //FillRect(hdc, &rc); //用矩形填充背景

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
 * @brief  重绘显示亮度的透明文本
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void Brigh_Textbox_OwnerDraw(DRAWITEM_HDR *ds) //绘制一个按钮外观
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

//  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, bk_hdc, rc_tmp.x, rc_tmp.y, SRCCOPY);
  SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
  rc.w -= 45;
  GetWindowText(hwnd, wbuf, 128); //获得按钮控件的文字
  SetFont(hdc, defaultFont);
  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//绘制文字(居中对齐方式)
}
extern void TCP_Echo_Init(void);

static LRESULT	CollectVoltage_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_CREATE:
    {
      RECT rc;
      GetClientRect(hwnd, &rc); 
      HWND Temp_Handle;
      
      GUI_Thread_Create(Network_Dispose_Task,  /* 任务入口函数 */
                              "Network Dispose Task",/* 任务名字 */
                              3*1024,  /* 任务栈大小 */
                              NULL, /* 任务入口函数参数 */
                              5,    /* 任务的优先级 */
                              10); /* 任务时间片，部分任务不支持 */

      CreateWindow(BUTTON, L"O", WS_TRANSPARENT|BS_FLAT | BS_NOTIFY |WS_OWNERDRAW|WS_VISIBLE,
                  730, 0, 70, 70, hwnd, eID_Network_EXIT, NULL, NULL); 

      rc.x = 5;
      rc.y = 35;
      rc.w = 160;
      rc.h = 30;
      CreateWindow(BUTTON,L"TCP Server",BS_RADIOBOX|WS_VISIBLE,
      rc.x,rc.y,rc.w,rc.h,hwnd,ID_RB1,NULL,NULL);
      
      OffsetRect(&rc, rc.w + 10, 0);
      CreateWindow(BUTTON,L"TCP Client",BS_RADIOBOX|WS_VISIBLE,
      rc.x,rc.y,rc.w,rc.h,hwnd,ID_RB2,NULL,NULL);

      //创建第2组单选按钮(GroupID号为2,使用常规按钮风格(BS_PUSHLIKE)).
      OffsetRect(&rc, rc.w + 10, 0);
      rc.w = 82;
      CreateWindow(BUTTON,L"UDP",BS_RADIOBOX|WS_VISIBLE,
      rc.x,rc.y,rc.w,rc.h,hwnd,ID_RB3,NULL,NULL);
      
      
      OffsetRect(&rc, rc.w + 10, 0);
      CreateWindow(BUTTON, L"未连接", WS_TRANSPARENT | BS_NOTIFY|WS_VISIBLE|BS_3D,
                  rc.x,rc.y,rc.w,rc.h, hwnd, eID_LINK_STATE, NULL, NULL); 
                  
      rc.w = GUI_XSIZE / 2;
      rc.h = TitleHeight-2;
      rc.x = GUI_XSIZE / 2 - rc.w / 2;
      rc.y = 0;

      // Title_Handle = CreateWindow(TEXTBOX, L"温湿度显示", WS_VISIBLE, rc.x, rc.y, rc.w, rc.h, hwnd, ID_TEXTBOX_Title, NULL, NULL);//
      // SendMessage(Title_Handle, TBM_SET_TEXTFLAG, 0, DT_VCENTER | DT_CENTER | DT_BKGND);   
      
      /* 数据发送文本窗口 */
      rc.w = GUI_XSIZE/2-10;
      rc.h = GUI_YSIZE/2-80;
      rc.x = GUI_XSIZE/2;
      rc.y = 70;
      Send_Handle = CreateWindow(TEXTBOX, L"你好！这里是野火开发板 ^_^", WS_VISIBLE, rc.x, rc.y, rc.w, rc.h, hwnd, ID_TEXTBOX_Send, NULL, NULL);//
      SendMessage(Send_Handle, TBM_SET_TEXTFLAG, 0, DT_VCENTER | DT_CENTER | DT_BKGND); 
      
      rc.x = GUI_XSIZE/2;
      rc.y = 70;
      rc.w = 80;
      rc.h = 30;
      OffsetRect(&rc, -rc.w-5, 0);
      CreateWindow(BUTTON, L"发送", WS_TRANSPARENT | BS_NOTIFY|WS_VISIBLE|BS_3D,
                         rc.x,rc.y,rc.w,rc.h, hwnd, eID_Network_Send, NULL, NULL); 
                         
      rc.x = GUI_XSIZE/2;
      rc.h = 30;
      rc.w = 120;
      rc.y = 70+GUI_YSIZE/2-80-rc.h;
      OffsetRect(&rc, -rc.w-5, 0);
      CreateWindow(BUTTON, L"清空接收", WS_TRANSPARENT | BS_NOTIFY|WS_VISIBLE|BS_3D,
                         rc.x,rc.y,rc.w,rc.h, hwnd, eID_Receive_Clear, NULL, NULL); 
      
      rc.w = GUI_XSIZE - 20;
      rc.h = 220;
      rc.x = 10;
      rc.y = 240;
      Receive_Handle = CreateWindow(TEXTBOX, L"", WS_VISIBLE|WS_DISABLED, rc.x, rc.y, rc.w, rc.h, hwnd, ID_TEXTBOX_Receive, NULL, NULL);//
      SendMessage(Receive_Handle, TBM_SET_TEXTFLAG, 0, DT_LEFT | DT_TOP | DT_BKGND);   

      /* 数据发送文本窗口 */
      rc.w = 45;
      rc.h = 30;
      rc.x = 5;
      rc.y = 160;
      Temp_Handle = CreateWindow(TEXTBOX, L"192", WS_VISIBLE, rc.x, rc.y, rc.w, rc.h, hwnd, ID_TEXTBOX_RemoteIP1, NULL, NULL);//
      SendMessage(Temp_Handle, TBM_SET_TEXTFLAG, 0, DT_VCENTER | DT_CENTER | DT_BKGND);

      OffsetRect(&rc, rc.w+15, 0);
      Temp_Handle = CreateWindow(TEXTBOX, L"168", WS_VISIBLE, rc.x, rc.y, rc.w, rc.h, hwnd, ID_TEXTBOX_RemoteIP2, NULL, NULL);//
      SendMessage(Temp_Handle, TBM_SET_TEXTFLAG, 0, DT_VCENTER | DT_CENTER | DT_BKGND);

      OffsetRect(&rc, rc.w+15, 0);
      Temp_Handle = CreateWindow(TEXTBOX, L"000", WS_VISIBLE, rc.x, rc.y, rc.w, rc.h, hwnd, ID_TEXTBOX_RemoteIP3, NULL, NULL);//
      SendMessage(Temp_Handle, TBM_SET_TEXTFLAG, 0, DT_VCENTER | DT_CENTER | DT_BKGND);

      OffsetRect(&rc, rc.w+15, 0);
      Temp_Handle = CreateWindow(TEXTBOX, L"122", WS_VISIBLE, rc.x, rc.y, rc.w, rc.h, hwnd, ID_TEXTBOX_RemoteIP4, NULL, NULL);//
      SendMessage(Temp_Handle, TBM_SET_TEXTFLAG, 0, DT_VCENTER | DT_CENTER | DT_BKGND);

      OffsetRect(&rc, rc.w+15, 0);
      rc.w = 50;
      Temp_Handle = CreateWindow(TEXTBOX, L"5000", WS_VISIBLE, rc.x, rc.y, rc.w, rc.h, hwnd, ID_TEXTBOX_RemotePort, NULL, NULL);//
      SendMessage(Temp_Handle, TBM_SET_TEXTFLAG, 0, DT_VCENTER | DT_CENTER | DT_BKGND);
      
      // /* 本地IP&端口显示文本框 */
      // rc.x = 441;
      // RH_Handle = CreateWindow(TEXTBOX, L"66", WS_VISIBLE|WS_OWNERDRAW, rc.x, rc.y, rc.w, rc.h, hwnd, ID_TEXTBOX_RH, NULL, NULL);//
      // SendMessage(RH_Handle, TBM_SET_TEXTFLAG, 0, DT_VCENTER | DT_CENTER | DT_BKGND);   

      SetTimer(hwnd, 0, 2000, TMR_START, NULL);

      break;
    } 
    case WM_TIMER:
    {
      
      break;
    }

    case WM_PAINT:
    {
      HDC hdc;
      PAINTSTRUCT ps;
      WCHAR tempstr[30];
      
      RECT rc =  {0, 0, GUI_XSIZE, GUI_YSIZE};
      // hdc_pointer = CreateMemoryDC(SURF_SCREEN, PANEL_W, PANEL_H);
      hdc = BeginPaint(hwnd, &ps);
      
      SetBrushColor(hdc, MapRGB(hdc, 120, 120, 120));
      FillRect(hdc, &rc);
      
      SetFont(hdc, defaultFont);
      SetTextColor(hdc, MapRGB(hdc, 0x80, 0xFF, 0x80));

      rc.x = 5;
      rc.y = 5;
      rc.w = 160;
      rc.h = 30;
      DrawText(hdc, L"通讯协议：", -1, &rc, DT_LEFT|DT_TOP);

      rc.x = 550;
      rc.y = 36;
      rc.w = 60;
      rc.h = 30;
      DrawText(hdc, L"数据发送：", -1, &rc, DT_LEFT|DT_TOP);
      
      rc.w = 400;
      rc.h = 30;
      rc.x = 5;
      rc.y = 65;
      DrawText(hdc, L"本地IP地址&端口：", -1, &rc, DT_LEFT|DT_TOP);
      
      SetTextColor(hdc, MapRGB(hdc, 200, 200, 200));
      x_wsprintf(tempstr, L"[%d.%d.%d.%d:%d]",drv_network.net_local_ip1,drv_network.net_local_ip2,\
                                       drv_network.net_local_ip3,drv_network.net_local_ip4,\
                                       drv_network.net_local_port);
      rc.w = 400;
      rc.h = 30;
      rc.x = 5;
      rc.y = 95;
      DrawText(hdc, tempstr, -1, &rc, DT_LEFT|DT_TOP);
      
      SetTextColor(hdc, MapRGB(hdc, 0x80, 0xFF, 0x80));

      rc.w = 120;
      rc.h = 30;
      rc.x = 10;
      rc.y = 210;
      DrawText(hdc, L"数据接收：", -1, &rc, DT_LEFT|DT_TOP);

      rc.w = 20;
      rc.h = 30;
      rc.x = 49;
      rc.y = 160;
      DrawText(hdc, L".", -1, &rc, DT_LEFT|DT_BOTTOM);
      
      rc.x = 110;
      DrawText(hdc, L".", -1, &rc, DT_LEFT|DT_BOTTOM);
      
      rc.x = 170;
      DrawText(hdc, L".", -1, &rc, DT_LEFT|DT_BOTTOM);
      
      rc.x = 229;
      DrawText(hdc, L":", -1, &rc, DT_LEFT|DT_BOTTOM);
      
      EndPaint(hwnd, &ps);
      break;
    }

    case WM_DRAWITEM:
    {
       DRAWITEM_HDR *ds;
       ds = (DRAWITEM_HDR*)lParam;
       switch(ds->ID)
       {
          case eID_Network_EXIT:
          {
            CollectVoltage_ExitButton_OwnerDraw(ds);
            return TRUE;             
          }  

          case ID_TEXTBOX_Send:
          case ID_TEXTBOX_Receive:
          {
            Brigh_Textbox_OwnerDraw(ds);
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
      if(code == BN_CLICKED && id == eID_Network_EXIT)
      {
        PostCloseMessage(hwnd);
        break;
      }
      if(code == BN_CLICKED && id == eID_Receive_Clear)
      {
        SetWindowText(Receive_Handle, L"");
        break;
      }
      
      if(code == BN_CLICKED && id == eID_LINK_STATE)
      {
        if((bsp_result&1)||EthLinkStatus)
          {
            break;
          }
          if(drv_network.net_connect==0)
          {
            uint8_t connectflag;
            
            drv_network.net_remote_ip1=192;//EDIT_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT0));
            drv_network.net_remote_ip2=168;//EDIT_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT1));
            drv_network.net_remote_ip3=0;//EDIT_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT2));
            drv_network.net_remote_ip4=122;//EDIT_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT3));
            drv_network.net_remote_port=8080;//EDIT_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT4));
            drv_network.net_type=NetworkTypeSelection;//DROPDOWN_GetSel(WM_GetDialogItem(pMsg->hWin, GUI_ID_DROPDOWN0));          
            switch(drv_network.net_type)
            {
              case 0:
                /*create tcp server */ 
                connectflag=tcp_echoserver_init(drv_network);
                break;
              case 1:
                /*connect to tcp server */
                connectflag=tcp_echoclient_connect(drv_network);
                break;
              case 2:
                /* Connect to tcp server */ 
                connectflag=udp_echoclient_connect(drv_network);		
                break;            
            }
            if(connectflag==0)
            {
              drv_network.net_connect=1; 
              SetWindowText(GetDlgItem(hwnd, eID_LINK_STATE), L"已连接");
            }      
          }
          else
          {
            SetWindowText(GetDlgItem(hwnd, eID_LINK_STATE), L"未连接");
            switch(drv_network.net_type)
            {
              case 0:
                tcp_echoserver_close();
                break;
              case 1:
                tcp_echoclient_disconnect();
                break;
              case 2:
                udp_echoclient_disconnect();	
                break;            
            }
            drv_network.net_connect=0;
          }
        }
        if(code == BN_CLICKED && id == eID_Network_Send)
        {
          if(drv_network.net_connect==1)          
          {
            WCHAR wbuf[128];
            char comdata[128];

            GetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_Send), wbuf, 128);
            x_wcstombs_cp936(comdata, wbuf, 128);
            switch(drv_network.net_type)
            {
              case 0:
                network_tcpserver_send((char *)comdata);
                break;
              case 1:
                network_tcpclient_send((char *)comdata);
                break;
              case 2:
                udp_echoclient_send((char *)comdata);
                break;            
            }
          }
        }
      
        if( (id >= (ID_RB1 & ~(1<<16))) && (id <= (ID_RB3 & ~(1<<16))))
        {
          if (code == BN_CLICKED)
          {
            NetworkTypeSelection = id & 3;
            GUI_DEBUG("NetworkTypeSelection = %d", NetworkTypeSelection);
          }
        }

      break;
    } 

    case WM_DESTROY:
    {
      
      // DeleteDC(bk_hdc);
      return PostQuitMessage(hwnd);	
    } 

    default:
      return	DefWindowProc(hwnd, msg, wParam, lParam);   
  }
  
  return WM_NULL;
  
}

void GUI_NetworkDLG_Dialog(void)
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
	Network_Main_Handle = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,
                              &wcex,
                              L"GUI_ADC_CollectVoltage_Dialog",
                              WS_VISIBLE|WS_CLIPCHILDREN,
                              0, 0, GUI_XSIZE, GUI_YSIZE,
                              NULL, NULL, NULL, NULL);
   //显示主窗口
	ShowWindow(Network_Main_Handle, SW_SHOW);
	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while (GetMessage(&msg, Network_Main_Handle))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}  
}



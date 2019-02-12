#include "emXGUI.h"
#include "Cdlg_MUSCIPLAYER.h"
#include "emXGUI_JPEG.h"
#include "Widget.h"
Cdlg_Master_Struct sCdlg_Master;//播放器APP管理结构体

//图标管理数组
icon_S music_icon[12] = {
   {"yinliang",         {20,400,48,48},       FALSE},//音量
   {"yinyueliebiao",    {668,404,72,72},      FALSE},//音乐列表
   {"geci",             {728,404,72,72},      FALSE},//歌词栏
   {"NULL",             {0,0,0,0},            FALSE},//无
   {"NULL",             {0,0,0,0},            FALSE},//无
   {"shangyishou",      {294, 404, 72, 72},   FALSE},//上一首
   {"zanting/bofang",   {364, 406, 72, 72},   FALSE},//播放
   {"xiayishou",        {448, 404, 72, 72},   FALSE},//下一首
  
};


SCROLLINFO g_sif_power;//音量滑动条
SCROLLINFO g_sif;//歌曲进度
static void MUSICPLAYER_OwnerDraw_BTN(DRAWITEM_HDR *ds)
{
  HDC hdc; //控件窗口HDC
  HDC hdc_mem;//内存HDC，作为缓冲区
  HWND hwnd; //控件句柄 
  RECT rc_cli;//控件的位置大小矩形
  WCHAR wbuf[128];
  hwnd = ds->hwnd;
  hdc = ds->hDC; 

  //获取控件的位置大小信息
  GetClientRect(hwnd, &rc_cli);
  //创建缓冲层，格式为SURF_ARGB4444
  hdc_mem = CreateMemoryDC(SURF_ARGB4444, rc_cli.w, rc_cli.h);

  GetWindowText(ds->hwnd,wbuf,128); //获得按钮控件的文字  


  SetBrushColor(hdc_mem,MapARGB(hdc_mem, 0, 255, 250, 250));
  FillRect(hdc_mem, &rc_cli);
  //播放键使用100*100的字体
  if(ds->ID == eID_BUTTON_START)
  SetFont(hdc_mem, controlFont_72);
  else if(ds->ID == eID_BUTTON_NEXT || ds->ID == eID_BUTTON_BACK)
  SetFont(hdc_mem, controlFont_64);
  else
  SetFont(hdc_mem, controlFont_48);
  //设置按键的颜色
  SetTextColor(hdc_mem, MapARGB(hdc_mem, 250,250,250,250));
  //NEXT键、BACK键和LIST键按下时，改变颜色
  if((ds->State & BST_PUSHED) )
  { //按钮是按下状态
    SetTextColor(hdc_mem, MapARGB(hdc_mem, 250,105,105,105));      //设置文字色     
  }

  DrawText(hdc_mem, wbuf,-1,&rc_cli,DT_VCENTER);//绘制文字(居中对齐方式)

  BitBlt(hdc, rc_cli.x, rc_cli.y, rc_cli.w, rc_cli.h, hdc_mem, 0, 0, SRCCOPY);

  DeleteDC(hdc_mem);  
}

static void MUSICPLAYER_DrawScrollBar(HWND hwnd, HDC hdc, COLOR_RGB32 back_c, COLOR_RGB32 Page_c, COLOR_RGB32 fore_c)
{
  RECT rc,rc_tmp;
  RECT rc_scrollbar;
  GetClientRect(hwnd, &rc);
  /* 背景 */
  GetClientRect(hwnd, &rc_tmp);//得到控件的位置
  GetClientRect(hwnd, &rc);//得到控件的位置
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换

  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, sCdlg_Master.mhdc_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);

  rc_scrollbar.x = rc.x;
  rc_scrollbar.y = rc.h/2-1;
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
  FillCircle(hdc, rc.x + rc.w / 2, rc.y + rc.h / 2, rc.h / 2 - 1);
  InflateRect(&rc, -2, -2);

  SetBrushColor(hdc, MapRGB888(hdc, fore_c));
  FillCircle(hdc, rc.x + rc.w / 2, rc.y + rc.h / 2, rc.h / 2 - 1);

}
/*
 * @brief  自定义滑动条绘制函数
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void MUSICPLAYER_OwnerDraw_ScrollBar(DRAWITEM_HDR *ds)
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
	MUSICPLAYER_DrawScrollBar(hwnd, hdc_mem1, RGB888( 250, 250, 250), RGB888( 250, 250, 250), RGB888( 255, 255, 255));
	//绘制绿色类型的滚动条
	MUSICPLAYER_DrawScrollBar(hwnd, hdc_mem, RGB888( 250, 250, 250), RGB888(	50, 205, 50), RGB888(50, 205, 50));
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
		BitBlt(hdc, rc.x, 0, rc.w, rc_cli.h, hdc_mem, rc.x, 0, SRCCOPY);
	}
	//释放内存MemoryDC
	DeleteDC(hdc_mem1);
	DeleteDC(hdc_mem);
}
//透明文本
static void Music_Button_OwnerDraw(DRAWITEM_HDR *ds) //绘制一个按钮外观
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

  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, sCdlg_Master.mhdc_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);
  SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));


  GetWindowText(hwnd, wbuf, 128); //获得按钮控件的文字

  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//绘制文字(居中对齐方式)

}


static void Cdlg_Init(void)
{
  //Step1:初始化背景
  u8 *jpeg_buf;
  u32 jpeg_size;
  JPG_DEC *dec;
  sCdlg_Master.LoadPic_State = RES_Load_Content(GUI_RGB_BACKGROUNG_PIC, (char**)&jpeg_buf, &jpeg_size);
  sCdlg_Master.mhdc_bk = CreateMemoryDC(SURF_SCREEN, 800, 480);
  if(sCdlg_Master.LoadPic_State)
  {
    /* 根据图片数据创建JPG_DEC句柄 */
    dec = JPG_Open(jpeg_buf, jpeg_size);

    /* 绘制至内存对象 */
    JPG_Draw(sCdlg_Master.mhdc_bk, 0, 0, dec);

    /* 关闭JPG_DEC句柄 */
    JPG_Close(dec);
  }
  /* 释放图片内容空间 */
  RES_Release_Content((char **)&jpeg_buf);   
  //Step2:
}
static LRESULT win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  { 
    case WM_CREATE:
    {
      
      Cdlg_Init();
      
      music_icon[0].rc.y = 440-music_icon[0].rc.h/2;//居中
      CreateWindow(BUTTON,L"A",WS_OWNERDRAW |WS_VISIBLE,//按钮控件，属性为自绘制和可视
                   music_icon[0].rc.x,music_icon[0].rc.y,//位置坐标和控件大小
                   music_icon[0].rc.w,music_icon[0].rc.h,//由music_icon[0]决定
                   hwnd,eID_BUTTON_Power,NULL,NULL);//父窗口hwnd,ID为ID_BUTTON_Power，附加参数为： NULL      
      //音乐列表icon
      CreateWindow(BUTTON,L"G",WS_OWNERDRAW |WS_VISIBLE, //按钮控件，属性为自绘制和可视
                   music_icon[1].rc.x,music_icon[1].rc.y,//位置坐标
                   music_icon[1].rc.w,music_icon[1].rc.h,//控件大小
                   hwnd,eID_BUTTON_List,NULL,NULL);//父窗口hwnd,ID为ID_BUTTON_List，附加参数为： NULL
      //歌词icon
      CreateWindow(BUTTON,L"W",WS_OWNERDRAW |WS_VISIBLE,
                   music_icon[2].rc.x,music_icon[2].rc.y,
                   music_icon[2].rc.w,music_icon[2].rc.h,
                   hwnd,eID_BUTTON_Equa,NULL,NULL);

      //上一首
      CreateWindow(BUTTON,L"S",WS_OWNERDRAW |WS_VISIBLE,
                   music_icon[5].rc.x,music_icon[5].rc.y,
                   music_icon[5].rc.w,music_icon[5].rc.h,
                   hwnd,eID_BUTTON_BACK,NULL,NULL);
      //下一首 
      CreateWindow(BUTTON,L"V",WS_OWNERDRAW |WS_VISIBLE,
                   music_icon[7].rc.x,music_icon[7].rc.y,
                   music_icon[7].rc.w,music_icon[7].rc.h,
                   hwnd,eID_BUTTON_NEXT,NULL,NULL);
      //播放键
      CreateWindow(BUTTON,L"U",WS_OWNERDRAW |WS_VISIBLE,
                   music_icon[6].rc.x,music_icon[6].rc.y,
                   music_icon[6].rc.w,music_icon[6].rc.h,
                   hwnd,eID_BUTTON_START,NULL,NULL);       
      /*********************歌曲进度条******************/
      g_sif.cbSize = sizeof(g_sif);
      g_sif.fMask = SIF_ALL;
      g_sif.nMin = 0;
      g_sif.nMax = 255;
      g_sif.nValue = 0;//初始值
      g_sif.TrackSize = 30;//滑块值
      g_sif.ArrowSize = 0;//两端宽度为0（水平滑动条）          
      sCdlg_Master.mMusic_wnd_time = CreateWindow(SCROLLBAR, L"SCROLLBAR_Time",  WS_OWNERDRAW| WS_VISIBLE, 
                                    80, 370, 640, 35, 
                                    hwnd, eID_SCROLLBAR_TIMER, NULL, NULL);
      SendMessage(sCdlg_Master.mMusic_wnd_time, SBM_SETSCROLLINFO, TRUE, (LPARAM)&g_sif);         

      /*********************音量值滑动条******************/
      g_sif_power.cbSize = sizeof(g_sif_power);
      g_sif_power.fMask = SIF_ALL;
      g_sif_power.nMin = 0;
      g_sif_power.nMax = 63;//音量最大值为63
      g_sif_power.nValue = sCdlg_Master.power;//初始音量值
      g_sif_power.TrackSize = 30;//滑块值
      g_sif_power.ArrowSize = 0;//两端宽度为0（水平滑动条）

      CreateWindow(SCROLLBAR, L"SCROLLBAR_R", WS_OWNERDRAW|WS_TRANSPARENT, 
                         70, 440-31/2, 150, 31, 
                         hwnd, eID_SCROLLBAR_POWER, NULL, NULL);
      SendMessage(GetDlgItem(hwnd, eID_SCROLLBAR_POWER), SBM_SETSCROLLINFO, TRUE, (LPARAM)&g_sif_power);


      CreateWindow(BUTTON,L"歌曲文件名",WS_OWNERDRAW|WS_TRANSPARENT|WS_VISIBLE,
                  100,0,600,80,hwnd,eID_TB5,NULL,NULL);


      CreateWindow(BUTTON,L"00:00",WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE,
                  720,387-15,80,30,hwnd,eID_TB1,NULL,NULL);


      CreateWindow(BUTTON,L"00:00",WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE,
                   0,387-15,80,30,hwnd,eID_TB2,NULL,NULL);
      break;
    }
    case WM_NOTIFY:
    {
      u16 code,  id, ctr_id;
      NMHDR *nr;
      id  =LOWORD(wParam);//获取消息的ID码
      code=HIWORD(wParam);//获取消息的类型
      ctr_id = LOWORD(wParam); //wParam低16位是发送该消息的控件ID.   
      if(code == BN_CLICKED)
      { 
        switch(id)
        {
          case eID_BUTTON_Power:
          {
            music_icon[0].state = ~music_icon[0].state;
            //当音量icon未被按下时
            if(music_icon[0].state == FALSE)
            {
              RedrawWindow(hwnd, NULL, RDW_ALLCHILDREN|RDW_INVALIDATE);
              ShowWindow(GetDlgItem(hwnd, eID_SCROLLBAR_POWER), SW_HIDE); //窗口隐藏
            }
            //当音量icon被按下时，设置为静音模式
            else
            {                
              ShowWindow(GetDlgItem(hwnd, eID_SCROLLBAR_POWER), SW_SHOW); //窗口显示
            }
            break;
          }           
        }

      }//end of if(code == BN_CLICKED)        
      nr = (NMHDR*)lParam; //lParam参数，是以NMHDR结构体开头.
      if (ctr_id == eID_SCROLLBAR_TIMER)
      {
        NM_SCROLLBAR *sb_nr;
        int i = 0;
        sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar的通知消息实际为 NM_SCROLLBAR扩展结构,里面附带了更多的信息.
        switch (nr->code)
        {
          case SBN_THUMBTRACK: //R滑块移动
          {
            i = sb_nr->nTrackValue; //获得滑块当前位置值                
            SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, i); //设置进度值
            break;
          }
            
        }
      }
      nr = (NMHDR*)lParam; //lParam参数，是以NMHDR结构体开头.
      //音量条处理case
      if (ctr_id == eID_SCROLLBAR_POWER)
      {
        NM_SCROLLBAR *sb_nr;
        sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar的通知消息实际为 NM_SCROLLBAR扩展结构,里面附带了更多的信息.
        static int NoVol_flag = 0;
        switch (nr->code)
        {
        case SBN_THUMBTRACK: //R滑块移动
        {
          sCdlg_Master.power= sb_nr->nTrackValue; //得到当前的音量值
          if(sCdlg_Master.power == 0) 
          {
            //wm8978_OutMute(1);//静音
            SetWindowText(GetDlgItem(hwnd, eID_BUTTON_Power), L"J");
            NoVol_flag = 1;
          }
          else
          {
            if(NoVol_flag == 1)
            {
              NoVol_flag = 0;
              SetWindowText(GetDlgItem(hwnd, eID_BUTTON_Power), L"A");
            }
//            wm8978_OutMute(0);
//            wm8978_SetOUT1Volume(power);//设置WM8978的音量值
          } 
          SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, sCdlg_Master.power); //发送SBM_SETVALUE，设置音量值
        }
        break;
        }
      }


      
      break;   
    }

    case WM_ERASEBKGND:
    {
      HDC hdc =(HDC)wParam;
      RECT rc =*(RECT*)lParam;
   
      if(sCdlg_Master.LoadPic_State!=FALSE)
        BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, sCdlg_Master.mhdc_bk, rc.x, rc.y, SRCCOPY);         

      return TRUE;
    }   
    case WM_DRAWITEM:
    {
      DRAWITEM_HDR *ds;
      ds = (DRAWITEM_HDR*)lParam;        
      if (ds->ID >= eID_BUTTON_Power && ds->ID<= eID_BUTTON_START)
      {
        MUSICPLAYER_OwnerDraw_BTN(ds);
        return TRUE;
      }
      if (ds->ID == eID_SCROLLBAR_POWER || ds->ID == eID_SCROLLBAR_TIMER)
      {
        MUSICPLAYER_OwnerDraw_ScrollBar(ds);
        return TRUE;
      }
      if(ds->ID == eID_TB1 || ds->ID == eID_TB2 || ds->ID == eID_TB5)
      {
        Music_Button_OwnerDraw(ds);
        return TRUE;
      }      
    }    
    default:
      return DefWindowProc(hwnd, msg, wParam, lParam);
  }
  return WM_NULL;
}


void	GUI_MUSICPLAYER_DIALOG(void)
{
	
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

	//创建主窗口
	sCdlg_Master.mMusicPlayer_hwnd= CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,
		&wcex,
		L"GUI_MUSICPLAYER_DIALOG",
		WS_VISIBLE,
		0, 0, GUI_XSIZE, GUI_YSIZE,
		NULL, NULL, NULL, NULL);

	//显示主窗口
	ShowWindow(sCdlg_Master.mMusicPlayer_hwnd, SW_SHOW);

	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while (GetMessage(&msg, sCdlg_Master.mMusicPlayer_hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}



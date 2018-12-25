#include "emXGUI.h"
#include "x_libc.h"

#include "GUI_MUSICPLAYER_DIALOG.h"
#include "GUI_MusicList_DIALOG.h"
#include "./Bsp/wm8978/bsp_wm8978.h" 
#include "./mjpegplayer/vidoplayer.h"
#include "emXGUI_JPEG.h"

void	GUI_MusicList_DIALOG(void);
int avi_chl = 0;
COLORREF color_bg;//透明控件的背景颜色
extern int Play_index;
extern char playlist[FILE_MAX_NUM][FILE_NAME_LEN];//播放List
//图标管理数组
icon_S music_icon[13] = {
   {"yinliang",         {30,402,72,72},        FALSE},
   {"yinyueliebiao",    {724,404,72,72},      FALSE},
   {"junhengqi",        {652,404,72,72},      FALSE},
   {"wenjianjia",       {724,404,72,72},      FALSE},
   {"zuoshangjiaolist", {20,20,40,40},        FALSE},
   {"shangyishou",      {128, 200, 72, 72},   FALSE},
   {"zanting/bofang",   {300, 140, 200, 200}, FALSE},
   {"xiayishou",        {600, 200, 72, 72},   FALSE},    
   {"mini_next",        {580, 4, 72, 72},     FALSE},
   {"mini_Stop",        {652, 4, 72, 72},     FALSE},
   {"mini_back",        {724, 3, 72, 72},     FALSE},  
   {"上边栏",           {0 ,0, 800, 80},     FALSE},
   {"下边栏",           {0 ,400, 800, 80},     FALSE},   
};
int power;//音量值
int showmenu_flag = 0;//显示菜单栏
/****************************控件重绘函数***********************/
/**
  * @brief  button_owner_draw 按钮控件的重绘制
  * @param  ds:DRAWITEM_HDR结构体
  * @retval NULL
  */
static void button_owner_draw(DRAWITEM_HDR *ds)
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

   SetBrushColor(hdc, color_bg);
   FillRect(hdc, &rc_cli);
   
   SetBrushColor(hdc_mem,MapARGB(hdc_mem, 0, 255, 250, 250));
   FillRect(hdc_mem, &rc_cli);
   //设置按键的颜色
   SetTextColor(hdc_mem, MapARGB(hdc_mem, 250,250,250,250));
   //设置按钮字体
   SetFont(hdc_mem, hFont_SDCARD);
 
   DrawText(hdc_mem, wbuf,-1,&rc_cli,DT_VCENTER);//绘制文字(居中对齐方式)
   
   BitBlt(hdc, rc_cli.x, rc_cli.y, rc_cli.w, rc_cli.h, hdc_mem, 0, 0, SRCCOPY);
   
   DeleteDC(hdc_mem);  
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
	RECT rc;
   RECT rc_scrollbar;
	GetClientRect(hwnd, &rc);
	/* 背景 */
	SetBrushColor(hdc, color_bg);
	FillRect(hdc, &rc);

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
   //FillRoundRect(hdc, &rc, MIN(rc.w, rc.h) >> 2);
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
	draw_scrollbar(hwnd, hdc_mem1, color_bg, RGB888( 250, 250, 250), RGB888( 255, 255, 255));
	//绘制绿色类型的滚动条
	draw_scrollbar(hwnd, hdc_mem, color_bg, RGB888( 250, 0, 0), RGB888( 250, 0, 0));
   SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc);   

	//左
	BitBlt(hdc, rc_cli.x, rc_cli.y, rc.x, rc_cli.h, hdc_mem, 0, 0, SRCCOPY);
	//右
	BitBlt(hdc, rc.x + rc.w, 0, rc_cli.w - (rc.x + rc.w) , rc_cli.h, hdc_mem1, rc.x + rc.w, 0, SRCCOPY);

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

/**
  * @brief  播放视频进程
  * @param  hwnd：屏幕窗口的句柄
  * @retval 无
  * @notes  
  */
rt_thread_t h_music;//音乐播放进程
static void App_PlayMusic(HWND hwnd)
{
	static int thread=0;
	static int app=0;
   //HDC hdc;
   
	if(thread==0)
	{  
      h_music=rt_thread_create("App_PlayMusic",(void(*)(void*))App_PlayMusic,NULL,5*1024,1,5);
      thread =1;
      rt_thread_startup(h_music);//启动线程				
      return;
	}
	while(thread) //线程已创建了
	{     
		if(app==0)
		{
         //hdc = GetDC(hwnd);
			app=1;
         AVI_play(playlist[Play_index], hwnd);         
			app=0;
        // ReleaseDC(hwnd, hdc);
         GUI_msleep(20);
		}
	}
}
#if 0
rt_thread_t h_decode;//音乐解码进程
void App_DecodeMusic(HWND hwnd, const void *dat, int cbSize, JPG_DEC *dec)
{
	static int thread=0;
	static int app=0;
   HDC hdc_mem;
   static U16 pic_width,pic_height;
	if(thread==0)
	{  
      h_decode=rt_thread_create("App_PlayMusic",(void(*)(void*))App_PlayMusic,NULL,5*1024,5,5);
      thread =1;
      rt_thread_startup(h_music);//启动线程				
      return;
	}
   hdc_mem = GetDC(hwnd);
	while(thread) //线程已创建了
	{     
		if(app==0)
		{
         
			app=1;
         dec = JPG_Open(dat, cbSize);
         JPG_GetImageSize(&pic_width, &pic_height,dec);
         //hdc_mem = CreateMemoryDC(SURF_SCREEN,pic_width,pic_height); 
         if(cbSize>10)
           //JPG_Draw(hdc_mem, 400, 0, dec);    
			app=0;
         ReleaseDC(hwnd, hdc_mem);
         GUI_msleep(20);
		}
	}
}
#endif

/**
  * @brief  创建音乐列表进程
  * @param  无
  * @retval 无
  * @notes  
  */
static void App_MusicList()
{
	static int thread=0;
	static int app=0;
   rt_thread_t h1;
	if(thread==0)
	{  
      h1=rt_thread_create("App_MusicList",(void(*)(void*))App_MusicList,NULL,4096,5,5);
      rt_thread_startup(h1);				
      thread =1;
      return;
	}
	if(thread==1) //线程已创建了
	{
		if(app==0)
		{
			app=1;
			GUI_MusicList_DIALOG();
			app=0;
			thread=0;
		}
	}
}


static SCROLLINFO sif_time;/*设置进度条的参数*/
HWND wnd_time;

static SCROLLINFO sif;/*设置音量条的参数*/
static HWND wnd;

static HWND wnd_power;//音量icon句柄
static HWND wnd_list;//音量icon句柄

HDC hdc_AVI=NULL;
HWND hwnd_AVI=NULL;

static int t0=0;
static int frame=0;
volatile int win_fps=0;
extern volatile int avi_fps;
extern UINT      BytesRD;
extern uint8_t   Frame_buf[];

static LRESULT win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   
   switch(msg)
   {
      case WM_CREATE:
      {
					t0 =GUI_GetTickCount();
				  frame =0;
					win_fps =0;
				
			    hwnd_AVI =hwnd;
					hdc_AVI =CreateMemoryDC(SURF_SCREEN,480,272);
#if 1 
         //音量icon（切换静音模式），返回控件句柄值
         wnd_power = CreateWindow(BUTTON,L"A",WS_OWNERDRAW|WS_VISIBLE,//按钮控件，属性为自绘制和可视
                                  music_icon[0].rc.x,music_icon[0].rc.y,//位置坐标和控件大小
                                  music_icon[0].rc.w,music_icon[0].rc.h,//由music_icon[0]决定
                                  hwnd,ID_BUTTON_Power,NULL,NULL);//父窗口hwnd,ID为ID_BUTTON_Power，附加参数为： NULL
         //播放列表icon
         wnd_list = CreateWindow(BUTTON,L"D",WS_OWNERDRAW|WS_VISIBLE, //按钮控件，属性为自绘制和可视
                      music_icon[1].rc.x,music_icon[1].rc.y,//位置坐标
                      music_icon[1].rc.w,music_icon[1].rc.h,//控件大小
                      hwnd,ID_BUTTON_List,NULL,NULL);//父窗口hwnd,ID为ID_BUTTON_List，附加参数为： NULL

         /*********************歌曲进度条******************/
         sif_time.cbSize = sizeof(sif_time);
         sif_time.fMask = SIF_ALL;
         sif_time.nMin = 0;
         sif_time.nMax = 255;
         sif_time.nValue = 0;//初始值
         sif_time.TrackSize = 30;//滑块值
         sif_time.ArrowSize = 0;//两端宽度为0（水平滑动条）          
         wnd_time = CreateWindow(SCROLLBAR, L"SCROLLBAR_Time",  WS_OWNERDRAW|WS_VISIBLE, 
                         120, 370, 560, 30, hwnd, ID_SCROLLBAR_TIMER, NULL, NULL);
         SendMessage(wnd_time, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif_time);
         /*********************音量值滑动条******************/
         sif.cbSize = sizeof(sif);
         sif.fMask = SIF_ALL;
         sif.nMin = 0;
         sif.nMax = 63;//音量最大值为63
         sif.nValue = 20;//初始音量值
         sif.TrackSize = 31;//滑块值
         sif.ArrowSize = 0;//两端宽度为0（水平滑动条）
         wnd = CreateWindow(SCROLLBAR, L"SCROLLBAR_R", WS_OWNERDRAW|WS_TRANSPARENT|WS_VISIBLE, 
                            120, 431, 150, 30, hwnd, ID_SCROLLBAR_POWER, NULL, NULL);
         SendMessage(wnd, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif);         
 #endif   
			 App_PlayMusic(hwnd);
         break;
      }

      case WM_DRAWITEM:
      {
         
         
         DRAWITEM_HDR *ds;
         ds = (DRAWITEM_HDR*)lParam;
         if (ds->ID == ID_SCROLLBAR_POWER || ds->ID == ID_SCROLLBAR_TIMER)
         {
            scrollbar_owner_draw(ds);
            return TRUE;
         }
         if (ds->ID >= 0x1000 && ds->ID<= 0x1099)
         {
            button_owner_draw(ds);
            return TRUE;
         }
      }  
#if 0
      case WM_ERASEBKGND:
      {
         if(showmenu_flag){
         HDC hdc = (HDC)wParam;
         HDC hdc_mem;//缓冲区
         RECT rc_top = {0 ,0, 800, 80};//上边栏
         RECT rc_bot = {0 ,400, 800, 80};//下边栏
         RECT rc_cli;//客户区矩形        
         GetClientRect(hwnd, &rc_cli);//获取客户区位置信息
         
         hdc_mem = CreateMemoryDC(SURF_ARGB4444, rc_cli.w, rc_cli.h);//分配空间
         SetBrushColor(hdc, MapRGB(hdc, 250, 250, 250));
         FillRect(hdc, &rc_cli);
         
            /*上边栏目*/
            SetBrushColor(hdc_mem, MapARGB(hdc_mem, 50, 0, 0, 0));
            FillRect(hdc_mem, &rc_top);
            BitBlt(hdc, rc_top.x, rc_top.y, rc_top.w, rc_top.h, 
                  hdc_mem, rc_top.x, rc_top.y, SRCCOPY);         
            /*下边栏目*/ 
            SetBrushColor(hdc_mem, MapARGB(hdc_mem, 50, 0, 0, 0));
            FillRect(hdc_mem, &rc_bot);
            BitBlt(hdc, rc_bot.x, rc_bot.y, rc_bot.w, rc_bot.h, 
                  hdc_mem, rc_bot.x, rc_bot.y, SRCCOPY);
         
         
         DeleteDC(hdc_mem);
         }        
         break;
      }
#endif
			
      //绘制窗口界面消息
      case WM_PAINT:
      {
         PAINTSTRUCT ps;
         HDC hdc;//屏幕hdc
//				WCHAR wbuf[40];
				RECT rc;
				int t1;
				GetClientRect(hwnd, &rc);
            hdc = BeginPaint(hwnd, &ps);   
               
            SetBrushColor(hdc, MapRGB(hdc, 0,0,0));
            FillRect(hdc, &rc);
				frame++;
				t1 =GUI_GetTickCount();
				if((t1-t0)>=1000)
				{
					win_fps =frame;
					t0 =t1;
					frame =0;
				}
				
         //获取屏幕点（385，404）的颜色，作为透明控件的背景颜色
         color_bg = GetPixel(hdc, 385, 404);
         EndPaint(hwnd, &ps);
         break;
      }
      case WM_NOTIFY:
      {
         u16 code, id, ctr_id;
         id  =LOWORD(wParam);//获取消息的ID码
         code=HIWORD(wParam);//获取消息的类型
         //发送单击
         if(code == BN_CLICKED)
         { 
            switch(id)
            {
               //音量icon处理case
               case ID_BUTTON_Power:
               {
                  RECT rc_cli = {80, 431, 150, 30};
                  music_icon[0].state = ~music_icon[0].state;
                  //InvalidateRect(hwnd, &music_icon[0].rc, TRUE);
                  //当音量icon未被按下时
                  if(music_icon[0].state == FALSE)
                  {
                     wm8978_OutMute(0);
                     //更新进度条的值
                     sif.nValue = power;
                     SendMessage(wnd, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif);     
                     EnableWindow(wnd, ENABLE);//启用音量进度条
                     SetWindowText(wnd_power, L"A");
                  }
                  //当音量icon被按下时，设置为静音模式
                  else
                  {                
                     wm8978_OutMute(1);//静音
                     power = SendMessage(wnd, SBM_GETVALUE, TRUE, TRUE);//获取当前音量值
                     sif.nValue = 0;//设置音量为0
                     SendMessage(wnd, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif);
                     EnableWindow(wnd, DISABLE); //禁用音量进度条               
                     SetWindowText(wnd_power, L"J");
                  }
                  InvalidateRect(hwnd, &rc_cli, TRUE);
                  break;
               }              
            
               case ID_BUTTON_List:
               {
                  App_MusicList();
                  break;
               }
            }

         }


         
         NMHDR *nr;  
         ctr_id = LOWORD(wParam); //wParam低16位是发送该消息的控件ID. 
         nr = (NMHDR*)lParam; //lParam参数，是以NMHDR结构体开头.
         //进度条处理case
         if (ctr_id == ID_SCROLLBAR_TIMER)
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
                  //置位进度条变更位置
                  avi_chl = 1;//滑动标志
               }
               break;
            }
         }
         //音量条处理case
         if (ctr_id == ID_SCROLLBAR_POWER)
         {
            NM_SCROLLBAR *sb_nr;
            sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar的通知消息实际为 NM_SCROLLBAR扩展结构,里面附带了更多的信息.
            switch (nr->code)
            {
               case SBN_THUMBTRACK: //R滑块移动
               {
                  power= sb_nr->nTrackValue; //得到当前的音量值
                  //设置WM8978的音量值
                  wm8978_SetOUT1Volume(power); 
                  SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, power); //发送SBM_SETVALUE，设置音量值
               }
               break;
            }
         }   
         break;
      }       
      case WM_CLOSE:
      {
         showmenu_flag = 0;
				DeleteDC(hdc_AVI);
         DestroyWindow(hwnd); //调用DestroyWindow函数来销毁窗口（该函数会产生WM_DESTROY消息）。
         return TRUE; //关闭窗口返回TRUE。
      }
      default :
         return DefWindowProc(hwnd, msg, wParam, lParam);
   }  
   return WM_NULL;
}



//音乐播放器句柄
HWND	VideoPlayer_hwnd;
void	GUI_VideoPlayer_DIALOG(void)
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
	VideoPlayer_hwnd = CreateWindowEx(WS_EX_NOFOCUS,
                                    &wcex,
                                    L"GUI_MUSICPLAYER_DIALOG",
                                    WS_VISIBLE,
                                    0, 0, GUI_XSIZE, GUI_YSIZE,
                                    NULL, NULL, NULL, NULL);

	//显示主窗口
	ShowWindow(VideoPlayer_hwnd, SW_SHOW);

	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while (GetMessage(&msg, VideoPlayer_hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}



#include "emXGUI.h"
#include "x_libc.h"
#include "string.h"
#include "ff.h"
#include "./mp3_player/Backend_mp3Player.h"
#include "./CDlg_MusicPlayer/CDlg_MusicPlayer.h"
#include "GUI_AppDef.h"
#include "ExitButton.h"


icon_S CDlg_MusicPlayerICON[7] = {
   {"yinliang",         {10,402,72,72},       FALSE},//音量
   {"yinyueliebiao",    {668,404,50,72},      FALSE},//音乐列表
   {"geci",             {728,404,72,72},      FALSE},//歌词栏
   {"shangyishou",      {294, 404, 72, 72},   FALSE},//上一首
   {"zanting/bofang",   {364, 406, 72, 72},   FALSE},//播放
   {"xiayishou",        {448, 404, 72, 72},   FALSE},//下一首
   {"tuichu",           {730, 0, 70, 70},     FALSE},//退出键
  
};
CDlg_MusicPlayer_s CDlg_MusicPlayer_Init = 
{
   .h_MusicPlayer = NULL,
   .h_MusicLrc = NULL,
   .pCDlg_MusicPlayerICON = CDlg_MusicPlayerICON,
   .power = 20,
   .back_col = RGB888(0, 0, 0),
   .bt_pushed_col = RGB888(105, 105, 105),
   .bt_default_col = RGB888(255, 255, 255),
   .exit_bt_draw = home_owner_draw,
   .ScrollBar_parm1 = {dir_H, 0, 2,  
                      RGB888(0, 0, 0),
                      RGB888(255, 255, 255),
                      RGB888(255, 255, 255)}, 

   .ScrollBar_parm2 = {dir_H, 0, 2,  
                      RGB888(0, 0, 0),
                      RGB888(50, 205, 50),
                      RGB888(50, 205, 50)},
   .power_state = FALSE,   
};

static void _EraseBackgnd(HWND hwnd, HDC hdc)
{
   SetBrushColor(hdc, CDlg_MusicPlayer_Init.back_col);
   FillRect(hdc, &CDlg_MusicPlayer_Init.rc_cli);
}

static void MusicPlayer_BUTTON_ownerdraw(DRAWITEM_HDR *ds)
{
   HDC hdc; //控件窗口HDC
   HWND hwnd; //控件句柄 
   RECT rc_cli;//控件的位置大小矩形
   WCHAR wbuf[128];
	hwnd = ds->hwnd;
	hdc = ds->hDC; 
   //获取控件的位置大小信息
   GetClientRect(hwnd, &rc_cli);
   
	GetWindowText(ds->hwnd,wbuf,128); //获得按钮控件的文字  
   
   SetBrushColor(hdc,MapRGB888(hdc, CDlg_MusicPlayer_Init.back_col));
   FillRect(hdc, &rc_cli);

   //设置按键的颜色
   SetTextColor(hdc, MapRGB888(hdc, CDlg_MusicPlayer_Init.bt_default_col));
   //NEXT键、BACK键和LIST键按下时，改变颜色
	if((ds->State & BST_PUSHED) )
	{ //按钮是按下状态
		SetTextColor(hdc, MapRGB888(hdc, CDlg_MusicPlayer_Init.bt_pushed_col));      //设置文字色     
	}
 
   DrawText(hdc, wbuf,-1,&rc_cli,DT_VCENTER);//绘制文字(居中对齐方式)
 
}


static LRESULT MusicPlayer_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch(msg)
   {
      
      case WM_CREATE:
      {
         CreateWindow(BUTTON,L"A",WS_OWNERDRAW |WS_VISIBLE,//按钮控件，属性为自绘制和可视
                                  CDlg_MusicPlayer_Init.pCDlg_MusicPlayerICON[0].rc.x,
                                  CDlg_MusicPlayer_Init.pCDlg_MusicPlayerICON[0].rc.y,//位置坐标和控件大小
                                  CDlg_MusicPlayer_Init.pCDlg_MusicPlayerICON[0].rc.w,
                                  CDlg_MusicPlayer_Init.pCDlg_MusicPlayerICON[0].rc.h,//由music_icon[0]决定
                                  hwnd,ID_BUTTON_Power,NULL,NULL);//父窗口hwnd,ID为ID_BUTTON_Power，附加参数为： NULL
         SetWindowFont(GetDlgItem(hwnd, ID_BUTTON_Power), controlFont_48);
         //音乐列表icon
         CreateWindow(BUTTON,L"G",WS_OWNERDRAW |WS_VISIBLE, //按钮控件，属性为自绘制和可视
                                  CDlg_MusicPlayer_Init.pCDlg_MusicPlayerICON[1].rc.x,
                                  CDlg_MusicPlayer_Init.pCDlg_MusicPlayerICON[1].rc.y,//位置坐标和控件大小
                                  CDlg_MusicPlayer_Init.pCDlg_MusicPlayerICON[1].rc.w,
                                  CDlg_MusicPlayer_Init.pCDlg_MusicPlayerICON[1].rc.h,
                                  hwnd,ID_BUTTON_List,NULL,NULL);//父窗口hwnd,ID为ID_BUTTON_List，附加参数为： NULL
         SetWindowFont(GetDlgItem(hwnd, ID_BUTTON_List), controlFont_48);
         //歌词icon
         CreateWindow(BUTTON,L"W",WS_OWNERDRAW |WS_VISIBLE,
                                  CDlg_MusicPlayer_Init.pCDlg_MusicPlayerICON[2].rc.x,
                                  CDlg_MusicPlayer_Init.pCDlg_MusicPlayerICON[2].rc.y,//位置坐标和控件大小
                                  CDlg_MusicPlayer_Init.pCDlg_MusicPlayerICON[2].rc.w,
                                  CDlg_MusicPlayer_Init.pCDlg_MusicPlayerICON[2].rc.h,
                                  hwnd,ID_BUTTON_LRC,NULL,NULL);
         SetWindowFont(GetDlgItem(hwnd, ID_BUTTON_LRC), controlFont_48);
         //上一首
         CreateWindow(BUTTON,L"S",WS_OWNERDRAW |WS_VISIBLE,
                                  CDlg_MusicPlayer_Init.pCDlg_MusicPlayerICON[3].rc.x,
                                  CDlg_MusicPlayer_Init.pCDlg_MusicPlayerICON[3].rc.y,//位置坐标和控件大小
                                  CDlg_MusicPlayer_Init.pCDlg_MusicPlayerICON[3].rc.w,
                                  CDlg_MusicPlayer_Init.pCDlg_MusicPlayerICON[3].rc.h,
                                  hwnd,ID_BUTTON_BACK,NULL,NULL);
         SetWindowFont(GetDlgItem(hwnd, ID_BUTTON_BACK), controlFont_64);                         
         //播放键
         CreateWindow(BUTTON,L"U",WS_OWNERDRAW |WS_VISIBLE,
                                  CDlg_MusicPlayer_Init.pCDlg_MusicPlayerICON[4].rc.x,
                                  CDlg_MusicPlayer_Init.pCDlg_MusicPlayerICON[4].rc.y,//位置坐标和控件大小
                                  CDlg_MusicPlayer_Init.pCDlg_MusicPlayerICON[4].rc.w,
                                  CDlg_MusicPlayer_Init.pCDlg_MusicPlayerICON[4].rc.h,
                                  hwnd,ID_BUTTON_NEXT,NULL,NULL);
         SetWindowFont(GetDlgItem(hwnd, ID_BUTTON_NEXT), controlFont_72); 
         //下一首
         CreateWindow(BUTTON,L"V",WS_OWNERDRAW |WS_VISIBLE,
                                  CDlg_MusicPlayer_Init.pCDlg_MusicPlayerICON[5].rc.x,
                                  CDlg_MusicPlayer_Init.pCDlg_MusicPlayerICON[5].rc.y,//位置坐标和控件大小
                                  CDlg_MusicPlayer_Init.pCDlg_MusicPlayerICON[5].rc.w,
                                  CDlg_MusicPlayer_Init.pCDlg_MusicPlayerICON[5].rc.h,
                                  hwnd,ID_BUTTON_START,NULL,NULL);
         SetWindowFont(GetDlgItem(hwnd, ID_BUTTON_START), controlFont_64);             

         CreateWindow(BUTTON, L"O",WS_OWNERDRAW|WS_VISIBLE,
                                  CDlg_MusicPlayer_Init.pCDlg_MusicPlayerICON[6].rc.x,
                                  CDlg_MusicPlayer_Init.pCDlg_MusicPlayerICON[6].rc.y,//位置坐标和控件大小
                                  CDlg_MusicPlayer_Init.pCDlg_MusicPlayerICON[6].rc.w,
                                  CDlg_MusicPlayer_Init.pCDlg_MusicPlayerICON[6].rc.h,
                                  hwnd, ID_BUTTON_EXIT, NULL, NULL);

         GetClientRect(hwnd, &CDlg_MusicPlayer_Init.rc_cli);
         
         CDlg_MusicPlayer_Init.sif_time.cbSize = sizeof(CDlg_MusicPlayer_Init.sif_time);
         CDlg_MusicPlayer_Init.sif_time.fMask = SIF_ALL;
         CDlg_MusicPlayer_Init.sif_time.nMin = 0;
         CDlg_MusicPlayer_Init.sif_time.nMax = 255;
         CDlg_MusicPlayer_Init.sif_time.nValue = 0;//初始值
         CDlg_MusicPlayer_Init.sif_time.TrackSize = 35;//滑块值
         CDlg_MusicPlayer_Init.sif_time.ArrowSize = 0;//两端宽度为0（水平滑动条）          
         CreateWindow(SCROLLBAR, L"SCROLLBAR_Time",  WS_OWNERDRAW| WS_VISIBLE, 
                         80, 370, 640, 35, hwnd, ID_SCROLLBAR_TIMER, NULL, NULL);
         SendMessage(GetDlgItem(hwnd, ID_SCROLLBAR_TIMER), 
                     SBM_SETSCROLLINFO, TRUE, (LPARAM)&CDlg_MusicPlayer_Init.sif_time);          

         /*********************音量值滑动条******************/
         CDlg_MusicPlayer_Init.sif_power.cbSize = sizeof(CDlg_MusicPlayer_Init.sif_power);
         CDlg_MusicPlayer_Init.sif_power.fMask = SIF_ALL;
         CDlg_MusicPlayer_Init.sif_power.nMin = 0;
         CDlg_MusicPlayer_Init.sif_power.nMax = 63;//音量最大值为63
         CDlg_MusicPlayer_Init.sif_power.nValue = CDlg_MusicPlayer_Init.power;//初始音量值
         CDlg_MusicPlayer_Init.sif_power.TrackSize = 30;//滑块值
         CDlg_MusicPlayer_Init.sif_power.ArrowSize = 0;//两端宽度为0（水平滑动条）
         CreateWindow(SCROLLBAR, L"SCROLLBAR_Power", WS_OWNERDRAW|WS_TRANSPARENT|WS_VISIBLE, 
                            70, 425, 150, 31, hwnd, ID_SCROLLBAR_POWER, NULL, NULL);
         SendMessage(GetDlgItem(hwnd, ID_SCROLLBAR_POWER), 
                     SBM_SETSCROLLINFO, TRUE, (LPARAM)&CDlg_MusicPlayer_Init.sif_power);

         break;
      }
      case WM_ERASEBKGND:
      {
         HDC hdc =(HDC)wParam;
         _EraseBackgnd(hwnd, hdc);
         return TRUE;
      }
      case WM_NOTIFY:
      {
         u16 code,  id, ctr_id;
         id  =LOWORD(wParam);//获取消息的ID码
         code=HIWORD(wParam);//获取消息的类型
         ctr_id = LOWORD(wParam); //wParam低16位是发送该消息的控件ID. 
         NMHDR *nr;
         nr = (NMHDR*)lParam;
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
                  //chgsch = 1;
               }
               break;
            }
         }

         if (ctr_id == ID_SCROLLBAR_POWER)
         {
            NM_SCROLLBAR *sb_nr;
            sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar的通知消息实际为 NM_SCROLLBAR扩展结构,里面附带了更多的信息.
            switch (nr->code)
            {
               case SBN_THUMBTRACK: //R滑块移动
               {
                  CDlg_MusicPlayer_Init.power= sb_nr->nTrackValue; //得到当前的音量值
                  if(CDlg_MusicPlayer_Init.power == 0) 
                  {
                     wm8978_OutMute(1);//静音
                     SetWindowText(GetDlgItem(hwnd, ID_SCROLLBAR_POWER), L"J");
                     CDlg_MusicPlayer_Init.power_state = 1;
                  }
                  else
                  {
                     if(CDlg_MusicPlayer_Init.power_state == 1)
                     {
                        CDlg_MusicPlayer_Init.power_state = 0;
                        SetWindowText(GetDlgItem(hwnd, ID_SCROLLBAR_POWER), L"A");
                     }
                     wm8978_OutMute(0);
                     wm8978_SetOUT1Volume(CDlg_MusicPlayer_Init.power);//设置WM8978的音量值
                  } 
                  SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, CDlg_MusicPlayer_Init.power); //发送SBM_SETVALUE，设置音量值
               }
               break;
            }
         }         
         break;
      }         
      case WM_DRAWITEM:
      {
         DRAWITEM_HDR *ds;
         ds = (DRAWITEM_HDR*)lParam;        
         if(ds->ID == ID_BUTTON_EXIT)
         {
            CDlg_MusicPlayer_Init.exit_bt_draw(ds);
            return TRUE;
         }
         if (ds->ID == ID_SCROLLBAR_POWER || ds->ID == ID_SCROLLBAR_TIMER)
         {
            scrollbar_owner_draw(ds, CDlg_MusicPlayer_Init.ScrollBar_parm1, CDlg_MusicPlayer_Init.ScrollBar_parm2);
            return TRUE;
         }
         if (ds->ID >= ID_BUTTON_Power && ds->ID<= ID_BUTTON_START)
         {
            MusicPlayer_BUTTON_ownerdraw(ds);
            return TRUE;
         }
      }      
      default:
         return DefWindowProc(hwnd, msg, wParam, lParam);      
   }
}




void	GUI_CDlg_MusicPlayer(void)
{
	
	WNDCLASS	wcex;
	MSG msg;


	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = MusicPlayer_proc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);

	//创建主窗口
	CDlg_MusicPlayer_Init.h_MusicPlayer = CreateWindowEx(WS_EX_NOFOCUS,
                                                      &wcex,
                                                      L"CDlg_MusicPlayer",
                                                      WS_VISIBLE,
                                                      0, 0, GUI_XSIZE, GUI_YSIZE,
                                                      NULL, NULL, NULL, NULL);

	//显示主窗口
	ShowWindow(CDlg_MusicPlayer_Init.h_MusicPlayer, SW_SHOW);

	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while (GetMessage(&msg, CDlg_MusicPlayer_Init.h_MusicPlayer))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}


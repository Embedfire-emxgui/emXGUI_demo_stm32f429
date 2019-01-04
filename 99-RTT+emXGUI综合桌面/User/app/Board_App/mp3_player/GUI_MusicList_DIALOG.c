#include "emXGUI.h"
#include "./mp3_player/mp3Player.h"
#include "./mp3_player/musiclist.h"
#include "x_libc.h"
#include "./mp3_player/GUI_MUSICPLAYER_DIALOG.h"


/******************按键ID值********************/
#define ID_BUTTON_EXIT 0x2000  
/******************列表ID值********************/
#define ID_LISTBOX1    0x2100
#define ID_LISTBOX2    0x2101




/**********************变量****************************/
char music_playlist[MUSIC_MAX_NUM][FILE_NAME_LEN];//播放List
char music_lcdlist[MUSIC_MAX_NUM][MUSIC_NAME_LEN];//显示list
uint8_t  music_file_num = 0;//文件个数
int play_index = 0;   //播放歌曲的编号值


static BITMAP bm;//位图结构体

/*******************控件重绘代码************************/
//按键
static void button_owner_draw(DRAWITEM_HDR *ds)
{
   HDC hdc; 
   HDC hdc_mem;//内存DC
   HWND hwnd;//按键的句柄  
   RECT rc_cli;//按键的位置信息
   WCHAR wbuf[128];
	hwnd = ds->hwnd;
	hdc = ds->hDC; 
   //获取按键位置信息
   GetClientRect(hwnd, &rc_cli);
   //创建内存DC
   hdc_mem = CreateMemoryDC(SURF_ARGB4444, rc_cli.w, rc_cli.h);
   
	GetWindowText(ds->hwnd,wbuf,128); //获得按钮控件的文字  
   //设置控件背景颜色为透明（透明度A：0为全透明）
   SetBrushColor(hdc_mem,MapARGB(hdc_mem, 0, 255, 250, 250));
   FillRect(hdc_mem, &rc_cli);
   //设置字体类型
   SetFont(hdc_mem, Music_Player_hFont72);
   //设置字体颜色为白色
   SetTextColor(hdc_mem, MapARGB(hdc_mem, 250,250,250,250));
   //按钮是按下状态
	if((ds->State & BST_PUSHED) && ds->ID == ID_BUTTON_EXIT)
	{ 
		SetTextColor(hdc_mem, MapARGB(hdc_mem, 250,70,130,180));      //设置文字色     
	}
   
   DrawText(hdc_mem, wbuf,-1,&rc_cli,DT_VCENTER);//绘制文字(居中对齐方式)
   //复制HDC_MEM到HDC中
   BitBlt(hdc, rc_cli.x, rc_cli.y, rc_cli.w, rc_cli.h, hdc_mem, 0, 0, SRCCOPY);
   //释放HDC
   DeleteDC(hdc_mem);  
}
//LIST
static HDC hdc_mem; //内存DC，用于缓冲数据
static HDC hdc_pic; //内存DC，用来保存图片的信息
static void _listbox_owner_draw_x(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
	RECT rc;//List的栏目位置信息
   RECT rc_cli;//List控件的位置信息
	int i, count;//
   int item = 0;//曲目编号
	hwnd = ds->hwnd;
	hdc = ds->hDC;	
   RECT rc1;//曲目编号位置及大小
	WCHAR wbuf[128];
   //获取控件的位置信息
	GetClientRect(hwnd, &rc_cli);
   HFONT font_old;
   //设置背景颜色（黑色）
	SetBrushColor(hdc_mem, MapRGB(hdc_mem, 0, 0, 0));
   //填充背景
	FillRect(hdc_mem, &rc_cli);
				
	//获取当前的第一项
	i = SendMessage(hwnd, LB_GETTOPINDEX, 0, 0);
  
	//获取列表项目总数
	count = SendMessage(hwnd, LB_GETCOUNT, 0, 0);
   
	while (i < count)
	{
      //设置字体颜色（白色）
      SetTextColor(hdc_mem, MapRGB(hdc_mem, 255, 0, 0));
      //获取栏目的位置信息
		SendMessage(hwnd, LB_GETITEMRECT, i, (LPARAM)&rc);
      font_old = SetFont(hdc_mem, Music_Player_hFont48);
      
		//BitBlt(hdc_mem, 50, (rc.y + 2), 72, 58, hdc_pic, 0, 0, SRCCOPY);//复制图标
      rc1.x = 50;
      rc1.y = (rc.y + 2);
      rc1.w = 72;
      rc1.h = 58;

      DrawText(hdc_mem, L"X", -1, &rc1, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
      SetFont(hdc_mem, font_old);
      SetTextColor(hdc_mem, MapRGB(hdc_mem, 255, 0, 0));
      //歌曲编号的位置
		rc1.x = rc.x + 5;
		rc1.y = rc.y + 2;
		rc1.w = 40;
		rc1.h = 58;
		if (rc.y > rc_cli.h)//超过list的高，则退出，不进行绘制
		{
			break;
		}
      //获取栏目i的文本内容
		SendMessage(hwnd, LB_GETTEXT, i, (LPARAM)wbuf);
      //绘制文本内容
		DrawText(hdc_mem, wbuf, -1, &rc1, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
      //设置歌曲名字的位置信息
  		rc1.x = 50+72+5;
		rc1.y = rc.y;          
		rc1.w = 200;  
      SetTextColor(hdc_mem, MapRGB(hdc_mem, 255, 255, 255));
      //根据List的ID值来绘制两个栏目（歌曲编号是music_lcdlist数组的内容决定的）
      //单数的歌曲绘制在LIST1
      //双数的歌曲绘制在LIST2
      switch(ds->ID)
      {
         case ID_LISTBOX1:
         {   
            item = 2*i;
            if(item == play_index)
            {
               SetTextColor(hdc_mem, MapRGB(hdc_mem, 250, 0, 0));
            }
            else
            {
               SetTextColor(hdc_mem, MapRGB(hdc_mem, 255, 255, 255));
            }
            
                        
            x_mbstowcs_cp936(wbuf, music_lcdlist[item], FILE_NAME_LEN);            
                       
            DrawText(hdc_mem, wbuf, 7, &rc1, DT_VCENTER);
            
            break;
         }
         case ID_LISTBOX2:
         {
            item = 2*i+1;        
            if(item == play_index)
            {
               SetTextColor(hdc_mem, MapRGB(hdc_mem, 250, 0, 0));
            }
            else
            {
               SetTextColor(hdc_mem, MapRGB(hdc_mem, 255, 255, 255));
            }   
                    
            x_mbstowcs_cp936(wbuf, music_lcdlist[item], FILE_NAME_LEN);
            DrawText(hdc_mem, wbuf, -1, &rc1, DT_SINGLELINE| DT_VCENTER);                       
            break;
         }
      
      }
      i++;
	}
	
	BitBlt(hdc, 0, 0, rc_cli.w, rc_cli.h, hdc_mem, 0, 0, SRCCOPY);
}



static LRESULT win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   static HWND wnd_list1;//左边歌曲list的句柄
   static HWND wnd_list2;//右边歌曲list的句柄
   switch(msg)
   {
      case WM_CREATE:
      { 
         int i;
         int item1=0, item2=0;//记录列表项ID标号
         int item_tmp=0;
         HDC hdc_tmp;
         HWND wnd_tmp;
         WCHAR wbuff[128];
         RECT rc_tmp ={ 0,0,72,58 };//
         RECT rc_button_exit = {5, 5, 72, 72};//返回键位置
         CreateWindow(BUTTON,L"F",WS_OWNERDRAW |WS_VISIBLE,
                      rc_button_exit.x, rc_button_exit.y, 
                      rc_button_exit.w, rc_button_exit.h,
                      hwnd,ID_BUTTON_EXIT,NULL,NULL);
  
         wnd_list1 = CreateWindow(LISTBOX, _T("Listbox"), WS_OWNERDRAW |  LBS_LINE | LBS_NOTIFY | WS_VISIBLE,
            0, 80, 400, 400, hwnd, ID_LISTBOX1, NULL, NULL);
         wnd_list2 = CreateWindow(LISTBOX, _T("Listbox"), WS_OWNERDRAW | LBS_LINE | LBS_NOTIFY | WS_VISIBLE ,
            400, 80, 400, 400, hwnd, ID_LISTBOX2, NULL, NULL);         
         //根据文件分栏：单数第一栏，双数放第二栏
         if(music_file_num)
         {
            for(i = 0; i < music_file_num; i++)
            {
               wnd_tmp = (i%2==0)? wnd_list1:wnd_list2;
               item_tmp = (i%2==0)? item1:item2;
               x_wsprintf(wbuff, L"%d",i+1);
               SendMessage(wnd_tmp, LB_ADDSTRING, item_tmp, (LPARAM)wbuff);
               if(wnd_tmp == wnd_list1) item1++;
               if(wnd_tmp == wnd_list2) item2++;
            }
            
         }
         for(i = 0; i < item1; i++)
            SendMessage(wnd_list1, LB_SETITEMHEIGHT, i, 58);
         for (i = 0; i < item2; i++)
            SendMessage(wnd_list2, LB_SETITEMHEIGHT, i, 58);   
         //设置位图结构参数
//         bm.Format = BM_RGB888;     //位图格式
//         bm.Width = 72;              //宽度
//         bm.Height = 58;             //高度
//         bm.WidthBytes = bm.Width * 3; //每行字节数
//         bm.LUT = NULL;                //查找表(RGB/ARGB格式不使用该参数)
//         bm.Bits = (void*)timg;    //位图数据

         hdc_mem = CreateMemoryDC(SURF_SCREEN, 800, 480);
         //hdc_pic = CreateMemoryDC(SURF_SCREEN, bm.Width, bm.Height);
//         DrawBitmap(hdc_pic, 0, 0, &bm, NULL);
//         //以下代码为抠圆形的pic       
//         hdc_tmp = CreateMemoryDC(SURF_ARGB4444, bm.Width, bm.Height);
//         
//         SetBrushColor(hdc_tmp, MapARGB(hdc_tmp, 255, 0, 0, 0));
//         FillRect(hdc_tmp, &rc_tmp);

//         SetBrushColor(hdc_tmp, MapARGB(hdc_tmp, 0, 0, 0, 0));
//         FillCircle(hdc_tmp, 36, 29, 25);

//         BitBlt(hdc_pic, 0, 0, 72, 58, hdc_tmp, 0, 0, SRCCOPY);
         //释放缓冲DC,结束抠图
//         DeleteDC(hdc_tmp);            
         
         break;
      }
      case WM_PAINT:
      {
         PAINTSTRUCT ps;
         HDC hdc;
         RECT rc;//窗口大小
         GetClientRect(hwnd, &rc); //获得客户区矩形.
         hdc = BeginPaint(hwnd, &ps);
         //背景
         SetBrushColor(hdc, MapRGB(hdc, 0,0,0));
         FillRect(hdc, &rc);  
         //DrawBitmap(hdc,0,0,&bm_0,NULL);   
               
         EndPaint(hwnd, &ps);
         break;
      }
		case WM_NOTIFY: //WM_NOTIFY消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
		{
			u16 code,id;

			code =HIWORD(wParam); //获得通知码类型.
			id   =LOWORD(wParam); //获得产生该消息的控件ID.

			if(id==ID_BUTTON_EXIT && code==BN_CLICKED) // 按钮“单击”了.
			{
				PostCloseMessage(hwnd); //使产生WM_CLOSE消息关闭窗口.
			}
         //获取列表的相关消息
         NMHDR *nr;        
         nr = (NMHDR*)lParam;
         switch(nr->code)
         {
            case LBN_CLICKED:
            {
               switch(nr->idFrom)
               {
                  case ID_LISTBOX1:
                  {
                     int i = 0;
                     i = SendMessage(nr->hwndFrom, LB_GETCURSEL, 0, 0);
                     play_index = 2*i;//切换至下一首
                     mp3player.ucStatus = STA_SWITCH;	
                     InvalidateRect(wnd_list1, NULL, TRUE);            
                     InvalidateRect(wnd_list2, NULL, TRUE);
                     break;
                  }//end of case ID_LISTBOX1
                  case ID_LISTBOX2:
                  {
                     int i = 0;
                     i = SendMessage(nr->hwndFrom, LB_GETCURSEL, 0, 0);
                     play_index = 2*i+1;//切换至下一首
                     mp3player.ucStatus = STA_SWITCH;		
                     InvalidateRect(wnd_list1, NULL, TRUE);
                     InvalidateRect(wnd_list2, NULL, TRUE);                     
                        
                  }//end of case ID_LISTBOX2
               }//end of switch (nr->idFrom)
               break;
            }//end of case LBN_CLICKED
            case LBN_POSCHANGE:
            {
               switch (nr->idFrom)
               {
                  static int flag_A = 0;
                  static int flag_B = 0;
                  
                  case ID_LISTBOX1:
                  {
                     int i;
                     flag_A = 1;
                     if (flag_B == 0 && flag_A == 1 )
                     {
                        i = SendMessage(nr->hwndFrom, LB_GETPOS, 0, 0);
                        SendMessage(wnd_list2, LB_SETPOS, 0, i);
                     }// end of if (flag_B == 0 && flag_A == 1 )
                     else if(flag_B == 1)
                     {
                        i = SendMessage(wnd_list1, LB_GETPOS, 0, 0);
                       
                        flag_B = 0;		
                        flag_A = 0;
                        InvalidateRect(wnd_list1, NULL, TRUE);
                     }//end of else if(flag_B == 1)
                     break;
                  }//end of case ID_LISTBOX1
                  case ID_LISTBOX2:
                  {
                     int t;
                     flag_B = 1;
                     if (flag_A == 0 && flag_B == 1) {
                        t = SendMessage(nr->hwndFrom, LB_GETPOS, 0, 0);
                        SendMessage(wnd_list1, LB_SETPOS, 0, t);
                     }//end of if (flag_A == 0 && flag_B == 1)
                     else if(flag_A == 1)
                     {
                        t = SendMessage(nr->hwndFrom, LB_GETPOS, 0, 0);
                        InvalidateRect(wnd_list2, NULL, TRUE);
                        flag_A = 0;	
                        flag_B = 0;
                     }//end of else if(flag_A == 1)
                     break;
                  }//end of case ID_LISTBOX2
               default:
                  break;
               }               
               
               
               break;
            }
         
         }
         break;
      }         
      //自定义绘制控件
      case WM_DRAWITEM:
      {
         DRAWITEM_HDR *ds;
         ds = (DRAWITEM_HDR*)lParam;
         if (ds->ID == ID_BUTTON_EXIT)
         {
            button_owner_draw(ds);
            return TRUE;
         }
         if(ds->ID == ID_LISTBOX1 || wParam == ID_LISTBOX2)
         {
            _listbox_owner_draw_x(ds);
            return TRUE;
         }
      }      
      case WM_CLOSE: //窗口关闭时，会自动产生该消息.
		{
         DeleteDC(hdc_mem);//释放
         //DeleteDC(hdc_pic);
         enter_flag = 0;
         SetForegroundWindow(MusicPlayer_hwnd);//设置前台窗口为MusicPlayer_hwnd，否则的话会触发重绘
			return DestroyWindow(hwnd); //调用DestroyWindow函数销毁窗口，该函数会使主窗口结束并退出消息循环;否则窗口将继续运行.
		}
      default:
         return DefWindowProc(hwnd, msg, wParam, lParam);
      
   }
   return WM_NULL;
}

void GUI_MusicList_DIALOG(void)
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

	//创建主窗口
	hwnd = CreateWindowEx(WS_EX_NOFOCUS,
                         &wcex,
                         L"GUI_MusicList_DIALOG",
                         WS_CLIPSIBLINGS,
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

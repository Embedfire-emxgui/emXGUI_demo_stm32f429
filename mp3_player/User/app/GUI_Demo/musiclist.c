#include "emXGUI.h"
#include "string.h"
#include "ff.h"
#include "x_libc.h"
#include "musiclist.h"
#include "mp3Player.h"

#define ID_BUTTON_EXIT 0x2000
#define ID_LISTBOX1    0x2100
#define ID_LISTBOX2    0x2101




																		
uint8_t  file_num = 0;																	
unsigned char playlist[MUSIC_MAX_NUM][FILE_NAME_LEN];
unsigned char lcdlist[MUSIC_MAX_NUM][MUSIC_NAME_LEN];

int play_index = 0;//记录播放曲目

extern char timg[];
extern int enter_flag;
static BITMAP bm;

//按键重绘
static void button_owner_draw(DRAWITEM_HDR *ds)
{
   HDC hdc; 
   HDC hdc_mem;
   HWND hwnd;  
   RECT rc_cli;
   WCHAR wbuf[128];
	hwnd = ds->hwnd;
	hdc = ds->hDC; 
   GetClientRect(hwnd, &rc_cli);
   hdc_mem = CreateMemoryDC(SURF_ARGB4444, rc_cli.w, rc_cli.h);
   
	GetWindowText(ds->hwnd,wbuf,128); //获得按钮控件的文字  
  
   SetBrushColor(hdc_mem,MapARGB(hdc_mem, 0, 255, 250, 250));
   FillRect(hdc_mem, &rc_cli);
   
   SetFont(hdc_mem, hFont_SDCARD);
   SetTextColor(hdc_mem, MapARGB(hdc_mem, 250,250,250,250));

	if((ds->State & BST_PUSHED) && ds->ID == ID_BUTTON_EXIT)
	{ //按钮是按下状态
		SetTextColor(hdc_mem, MapARGB(hdc_mem, 250,70,130,180));      //设置文字色     
	}
 
   DrawText(hdc_mem, wbuf,-1,&rc_cli,DT_VCENTER);//绘制文字(居中对齐方式)
   
   BitBlt(hdc, rc_cli.x, rc_cli.y, rc_cli.w, rc_cli.h, hdc_mem, 0, 0, SRCCOPY);
   
   DeleteDC(hdc_mem);  
}






static HDC hdc_mem;
static HDC hdc_pic;
static void _listbox_owner_draw_x(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
	RECT rc, rc_cli;
	int i, count;
   int item = 0;
	hwnd = ds->hwnd;
	hdc = ds->hDC;
	GetClientRect(hwnd, &rc_cli);
	RECT rc1 = { 5, 0, 40,58 };//文字
	WCHAR wbuf[128];
	GetClientRect(hwnd, &rc_cli);

	SetBrushColor(hdc_mem, MapRGB(hdc_mem, 0, 0, 0));
	FillRect(hdc_mem, &rc_cli);
				
	//获取当前的第一项
	i = SendMessage(hwnd, LB_GETTOPINDEX, 0, 0);
  
	//获取列表项目总数
	count = SendMessage(hwnd, LB_GETCOUNT, 0, 0);
   
	while (i < count )//第一栏目ID_LISTBOX1
	{
      SetTextColor(hdc_mem, MapRGB(hdc_mem, 255, 255, 255));
		SendMessage(hwnd, LB_GETITEMRECT, i, (LPARAM)&rc);
		BitBlt(hdc_mem, 50, (rc.y + 2), 72, 58, hdc_pic, 0, 0, SRCCOPY);//图标

		rc1.x = rc.x + 5;
		rc1.y = rc.y + 2;
		rc1.w = 40;
		rc1.h = 58;
		if (rc.y > rc_cli.h)
		{
			break;
		}
		SendMessage(hwnd, LB_GETTEXT, i, (LPARAM)wbuf);
		DrawText(hdc_mem, wbuf, -1, &rc1, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
  		rc1.x = 50+72+5;
		rc1.y = rc.y;          
		rc1.w = 200;   
      switch(ds->ID)
      {
         case ID_LISTBOX1:
         {   
            item = 2*i;
            if(item == play_index)
            {
               SetTextColor(hdc_mem, MapRGB(hdc_mem, 0, 191, 255));
            }
            else
            {
               SetTextColor(hdc_mem, MapRGB(hdc_mem, 255, 255, 255));
            }
            
                        
            x_mbstowcs_cp936(wbuf, lcdlist[item], strlen(lcdlist[item]));            
                       
            DrawText(hdc_mem, wbuf, 7, &rc1, DT_VCENTER);
            
            break;
         }
         case ID_LISTBOX2:
         {
            item = 2*i+1;        
            if(item == play_index)
            {
               SetTextColor(hdc_mem, MapRGB(hdc_mem, 0, 191, 255));
            }
            else
            {
               SetTextColor(hdc_mem, MapRGB(hdc_mem, 255, 255, 255));
            }   
                    
            x_mbstowcs_cp936(wbuf, lcdlist[item], strlen(lcdlist[item]));
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
   static HWND wnd1, wnd2;
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
         RECT rc_tmp = { 0,0,72,58 };
         RECT rc_button_exit = {5, 5, 72, 72};
         CreateWindow(BUTTON,L"F",WS_OWNERDRAW |WS_VISIBLE,
         rc_button_exit.x, rc_button_exit.y, rc_button_exit.w, rc_button_exit.h,hwnd,ID_BUTTON_EXIT,NULL,NULL);
         
         
         
         wnd1 = CreateWindow(LISTBOX, _T("Listbox"), WS_OWNERDRAW |  LBS_LINE | LBS_NOTIFY | WS_VISIBLE,
            0, 80, 400, 400, hwnd, ID_LISTBOX1, NULL, NULL);
         wnd2 = CreateWindow(LISTBOX, _T("Listbox"), WS_OWNERDRAW | LBS_LINE | LBS_NOTIFY | WS_VISIBLE ,
            400, 80, 400, 400, hwnd, ID_LISTBOX2, NULL, NULL);         
         
         if(file_num)
         {
            for(i = 0; i < file_num; i++)
            {
               wnd_tmp = (i%2==0)? wnd1:wnd2;
               item_tmp = (i%2==0)? item1:item2;
               x_wsprintf(wbuff, L"%d",i+1);
               SendMessage(wnd_tmp, LB_ADDSTRING, item_tmp, (LPARAM)wbuff);
               if(wnd_tmp == wnd1) item1++;
               if(wnd_tmp == wnd2) item2++;
            }
            
         }
         
//            //设置列表项的内容
//            SendMessage(wnd1, LB_ADDSTRING, 0, (LPARAM)L"01");
//            SendMessage(wnd1, LB_ADDSTRING, 1, (LPARAM)L"03");
//            SendMessage(wnd1, LB_ADDSTRING, 2, (LPARAM)L"05");
//            SendMessage(wnd1, LB_ADDSTRING, 3, (LPARAM)L"07");
//            SendMessage(wnd1, LB_ADDSTRING, 4, (LPARAM)L"09");
//            SendMessage(wnd1, LB_ADDSTRING, 5, (LPARAM)L"11");
//            SendMessage(wnd1, LB_ADDSTRING, 6, (LPARAM)L"13");
         for(i = 0; i < 7; i++)
            SendMessage(wnd1, LB_SETITEMHEIGHT, i, 58);

//         //设置列表项的内容
//         SendMessage(wnd2, LB_ADDSTRING, 0, (LPARAM)L"02");
//         SendMessage(wnd2, LB_ADDSTRING, 1, (LPARAM)L"04");
//         SendMessage(wnd2, LB_ADDSTRING, 2, (LPARAM)L"06");
//         SendMessage(wnd2, LB_ADDSTRING, 3, (LPARAM)L"08");
//         SendMessage(wnd2, LB_ADDSTRING, 4, (LPARAM)L"10");
//         SendMessage(wnd2, LB_ADDSTRING, 5, (LPARAM)L"12");
//         SendMessage(wnd2, LB_ADDSTRING, 6, (LPARAM)L"14");
         for (i = 0; i < 7; i++)
            SendMessage(wnd2, LB_SETITEMHEIGHT, i, 58);
            
            
            
         //设置位图结构参数
         bm.Format = BM_RGB888;     //位图格式
         bm.Width = 72;              //宽度
         bm.Height = 58;             //高度
         bm.WidthBytes = bm.Width * 3; //每行字节数
         bm.LUT = NULL;                //查找表(RGB/ARGB格式不使用该参数)
         bm.Bits = (void*)timg;    //位图数据

         hdc_mem = CreateMemoryDC(SURF_SCREEN, 800, 480);
         hdc_pic = CreateMemoryDC(SURF_SCREEN, bm.Width, bm.Height);
         DrawBitmap(hdc_pic, 0, 0, &bm, NULL);

         hdc_tmp = CreateMemoryDC(SURF_ARGB4444, bm.Width, bm.Height);

         SetBrushColor(hdc_tmp, MapARGB(hdc_tmp, 255, 0, 0, 0));
         FillRect(hdc_tmp, &rc_tmp);

         SetBrushColor(hdc_tmp, MapARGB(hdc_tmp, 0, 0, 0, 0));
         FillCircle(hdc_tmp, 36, 29, 25);

         BitBlt(hdc_pic, 0, 0, 72, 58, hdc_tmp, 0, 0, SRCCOPY);
         DeleteDC(hdc_tmp);            
            
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
         NMHDR *nr;        
         nr = (NMHDR*)lParam;
         if (nr->idFrom == ID_LISTBOX2 && nr->code == LBN_SELCHANGE)
         {
            int i = 0;
            i = SendMessage(nr->hwndFrom, LB_GETCURSEL, 0, 0);
            play_index = 2*i+1;
            mp3player.ucStatus = STA_SWITCH;		/* 放音状态 */
            InvalidateRect(wnd1, NULL, TRUE);
         }
         if (nr->idFrom == ID_LISTBOX1 && nr->code == LBN_SELCHANGE)
         {
            int i = 0;
            i = SendMessage(nr->hwndFrom, LB_GETCURSEL, 0, 0);
            play_index = 2*i;
            mp3player.ucStatus = STA_SWITCH;		/* 放音状态 */
            InvalidateRect(wnd2, NULL, TRUE);
         }		
         if (nr->code == LBN_POSCHANGE)
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
                  SendMessage(wnd2, LB_SETPOS, 0, i);
               }
               else if(flag_B == 1)
               {
                  i = SendMessage(wnd1, LB_GETPOS, 0, 0);
                 
                  flag_B = 0;		
                  flag_A = 0;
                  InvalidateRect(wnd1, NULL, TRUE);
               }
            
               break;
            }
            case ID_LISTBOX2:
            {
               int t;
               flag_B = 1;
               if (flag_A == 0 && flag_B == 1) {
                  t = SendMessage(nr->hwndFrom, LB_GETPOS, 0, 0);
                  SendMessage(wnd1, LB_SETPOS, 0, t);
               }
               else if(flag_A == 1)
               {
                  t = SendMessage(nr->hwndFrom, LB_GETPOS, 0, 0);
                  InvalidateRect(wnd2, NULL, TRUE);
                  flag_A = 0;	
                  flag_B = 0;
               }
               break;
            }
            default:
               break;
            }
         }         
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
         DeleteDC(hdc_mem);
         DeleteDC(hdc_pic);
         enter_flag = 0;
         
			return DestroyWindow(hwnd); //调用DestroyWindow函数销毁窗口，该函数会使主窗口结束并退出消息循环;否则窗口将继续运行.
		}
      default:
         return DefWindowProc(hwnd, msg, wParam, lParam);
   }
   
   return WM_NULL;
}



void	GUI_MusicList_DIALOG(void)
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
      if(mp3player.ucStatus == STA_SWITCH)
      {
         printf("我要走了\n");
         //mp3player.ucStatus = STA_PLAYING;
         GUI_msleep(20);
      }
	}
}


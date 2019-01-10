#include "emXGUI.h"
#include "./mp3_player/Backend_mp3Player.h"
#include "./mp3_player/Backend_musiclist.h"
#include "x_libc.h"
#include "./mp3_player/GUI_MUSICPLAYER_DIALOG.h"
#include	"CListMenu.h"
#include "GUI_AppDef.h"
/******************按键ID值********************/
#define ID_BUTTON_EXIT 0x2000  
/******************列表ID值********************/
#define ID_LISTBOX1    0x2100
#define ID_LISTBOX2    0x2101

#define ID_LIST_1             0x2200
#define ICON_VIEWER_ID_PREV   0x2201
#define ICON_VIEWER_ID_NEXT   0x2202

#define ID_EXIT        0x3000

/**********************变量****************************/
char music_playlist[MUSIC_MAX_NUM][FILE_NAME_LEN];//播放List
char music_lcdlist[MUSIC_MAX_NUM][MUSIC_NAME_LEN];//显示list
uint8_t  music_file_num = 0;//文件个数
int play_index = 0;   //播放歌曲的编号值


static BITMAP bm;//位图结构体

/*******************控件重绘代码************************/
//按键
static void button_owner_draw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button的窗口句柄.
	hdc = ds->hDC;   //button的绘图上下文句柄.
	rc = ds->rc;     //button的绘制矩形区.

	SetBrushColor(hdc, MapRGB(hdc, 0,0,0));
	FillRect(hdc, &rc); //用矩形填充背景

	if (IsWindowEnabled(hwnd) == FALSE)
	{
		SetTextColor(hdc, MapRGB(hdc, COLOR_INVALID));
	}
	else if (ds->State & BST_PUSHED)
	{ //按钮是按下状态
//    GUI_DEBUG("ds->ID=%d,BST_PUSHED",ds->ID);
//		SetBrushColor(hdc,MapRGB(hdc,150,200,250)); //设置填充色(BrushColor用于所有Fill类型的绘图函数)
//		SetPenColor(hdc,MapRGB(hdc,250,0,0));        //设置绘制色(PenColor用于所有Draw类型的绘图函数)
		SetTextColor(hdc, MapRGB(hdc, 105, 105, 105));      //设置文字色
	}
	else
	{ //按钮是弹起状态
//		SetBrushColor(hdc,MapRGB(hdc,255,255,255));
//		SetPenColor(hdc,MapRGB(hdc,0,250,0));
		SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
	}


	//	SetBrushColor(hdc,COLOR_BACK_GROUND);

	//	FillRect(hdc,&rc); //用矩形填充背景
	//	DrawRect(hdc,&rc); //画矩形外框
	//  
	//  FillCircle(hdc,rc.x+rc.w/2,rc.x+rc.w/2,rc.w/2); //用矩形填充背景FillCircle
	//	DrawCircle(hdc,rc.x+rc.w/2,rc.x+rc.w/2,rc.w/2); //画矩形外框

	  /* 使用控制图标字体 */
	SetFont(hdc, controlFont_64);
	//  SetTextColor(hdc,MapRGB(hdc,255,255,255));

	GetWindowText(ds->hwnd, wbuf, 128); //获得按钮控件的文字
   
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER | DT_CENTER);//绘制文字(居中对齐方式)


  /* 恢复默认字体 */
	SetFont(hdc, defaultFont);

}
static void exit_owner_draw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button的窗口句柄.
	hdc = ds->hDC;   //button的绘图上下文句柄.
	rc = ds->rc;     //button的绘制矩形区.

	SetBrushColor(hdc, MapRGB(hdc, COLOR_DESKTOP_BACK_GROUND));
   
   FillCircle(hdc, rc.x, rc.y, rc.w);
	//FillRect(hdc, &rc); //用矩形填充背景

	if (IsWindowEnabled(hwnd) == FALSE)
	{
		SetTextColor(hdc, MapRGB(hdc, COLOR_INVALID));
	}
	else if (ds->State & BST_PUSHED)
	{ //按钮是按下状态
//    GUI_DEBUG("ds->ID=%d,BST_PUSHED",ds->ID);
//		SetBrushColor(hdc,MapRGB(hdc,150,200,250)); //设置填充色(BrushColor用于所有Fill类型的绘图函数)
//		SetPenColor(hdc,MapRGB(hdc,250,0,0));        //设置绘制色(PenColor用于所有Draw类型的绘图函数)
		SetTextColor(hdc, MapRGB(hdc, 250, 0, 0));      //设置文字色
	}
	else
	{ //按钮是弹起状态
//		SetBrushColor(hdc,MapRGB(hdc,255,255,255));
//		SetPenColor(hdc,MapRGB(hdc,0,250,0));
		SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
	}


	//	SetBrushColor(hdc,COLOR_BACK_GROUND);

	//	FillRect(hdc,&rc); //用矩形填充背景
	//	DrawRect(hdc,&rc); //画矩形外框
	//  
	//  FillCircle(hdc,rc.x+rc.w/2,rc.x+rc.w/2,rc.w/2); //用矩形填充背景FillCircle
	//	DrawCircle(hdc,rc.x+rc.w/2,rc.x+rc.w/2,rc.w/2); //画矩形外框

	  /* 使用控制图标字体 */
	SetFont(hdc, controlFont_64);
	//  SetTextColor(hdc,MapRGB(hdc,255,255,255));

	GetWindowText(ds->hwnd, wbuf, 128); //获得按钮控件的文字
   rc.x = 5;
   rc.y = 0;
	DrawText(hdc, wbuf, -1, &rc, NULL);//绘制文字(居中对齐方式)


  /* 恢复默认字体 */
	SetFont(hdc, defaultFont);

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
     //SetTextColor(hdc_mem, MapRGB(hdc_mem, 250, 250, 250));
      
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
                                       
            break;
         }
      
      }      
      
      //获取栏目的位置信息
		SendMessage(hwnd, LB_GETITEMRECT, i, (LPARAM)&rc);
      font_old = SetFont(hdc_mem, controlFont_48);
      
		//BitBlt(hdc_mem, 50, (rc.y + 2), 72, 58, hdc_pic, 0, 0, SRCCOPY);//复制图标
      rc1.x = 50;
      rc1.y = (rc.y + 2);
      rc1.w = 72;
      rc1.h = 58;

      DrawText(hdc_mem, L"X", -1, &rc1, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
      SetFont(hdc_mem, font_old);
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
                                       
            break;
         }
      
      }
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




static LRESULT Win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   static struct __obj_list *menu_list = NULL;
   static WCHAR (*wbuf)[128];
   switch(msg)
   {
      case WM_CREATE:
      {
         
         HWND wnd;
         int i = 0;
         list_menu_cfg_t cfg;
	   	   RECT rc;
         GetClientRect(hwnd, &rc);
         /* 需要分配N+1项，最后一项为空 */
         menu_list = (struct __obj_list *)GUI_VMEM_Alloc(sizeof(struct __obj_list)*(music_file_num+1));
         wbuf = (WCHAR (*)[128])GUI_VMEM_Alloc(sizeof(WCHAR *)*music_file_num);
         if(menu_list == NULL) 
            return 0;
         for(;i < music_file_num; i++){
            //printf("%s\n", lcdlist[i]);
            
            
            char p[128] ;
            strcpy(p, music_lcdlist[i]);
            printf("%s\n",p);
            int t, L;
            L = (int)strlen(p);
            if (L > 13)
            {
               for (t = L; t > 13; t --)
               {
                  p[t] = p[t - 1];
               }
               p[13] = '\0';
               p[L + 1] = '\0';
            }            
            
            
            x_mbstowcs_cp936(wbuf[i], p, FILE_NAME_LEN);
            menu_list[i].pName = wbuf[i];
            menu_list[i].cbStartup = NULL;
            menu_list[i].icon = L"a";
            menu_list[i].bmp = NULL;
            menu_list[i].color = RGB_WHITE;
         } 
          /* 最后一项为空 */
          menu_list[i].pName = NULL;
          menu_list[i].cbStartup = NULL;
          menu_list[i].icon = NULL;
          menu_list[i].bmp = NULL;
          menu_list[i].color = NULL;         
         
         cfg.list_objs = menu_list; 
         cfg.x_num = 3;
         cfg.y_num = 2; 
         cfg.bg_color = 0;
         wnd = CreateWindow(&wcex_ListMenu,
                      L"ListMenu1",
                      WS_VISIBLE | LMS_ICONFRAME|LMS_PAGEMOVE,
                      rc.x + 100, rc.y + 80, rc.w - 200, rc.h-80,
                      hwnd,
                      ID_LIST_1,
                      NULL,
                      &cfg);         
         
         wnd= CreateWindow(BUTTON, L"L", BS_FLAT | BS_NOTIFY | WS_OWNERDRAW |WS_VISIBLE,
                        0, rc.h * 1 / 2, 70, 70, hwnd, ICON_VIEWER_ID_PREV, NULL, NULL);
         SetWindowFont(wnd, controlFont_48); 
	      wnd = CreateWindow(BUTTON, L"K", BS_FLAT | BS_NOTIFY | WS_OWNERDRAW | WS_VISIBLE,
			rc.w - 65, rc.h * 1 / 2, 70, 70, hwnd, ICON_VIEWER_ID_NEXT, NULL, NULL);
         SetWindowFont(wnd, controlFont_48);    
         
         CreateWindow(BUTTON, L"N", BS_FLAT | BS_NOTIFY|WS_OWNERDRAW |WS_VISIBLE,
                        0, 0, 80, 80, hwnd, ID_EXIT, NULL, NULL);         
         
         
         break;
      } 
      case WM_DRAWITEM:
      {

         DRAWITEM_HDR *ds;

         ds = (DRAWITEM_HDR*)lParam;
  
         if(ds->ID == ID_EXIT)
            exit_owner_draw(ds);
         else
            button_owner_draw(ds); //执行自绘制按钮
         return TRUE;

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
         rc.x = 0;
         rc.y = 0;
         rc.w = 800;
         rc.h = 80;
         SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
         DrawText(hdc, L"播放列表", -1, &rc, DT_VCENTER|DT_CENTER);
         EndPaint(hwnd, &ps);
         break;
      }  
      case WM_NOTIFY:
      {
         u16 code, id;	
         LM_NMHDR *nm;
         code = HIWORD(wParam);
         id = LOWORD(wParam); 

         nm = (LM_NMHDR*)lParam;

         if (code == LMN_CLICKED)
         {
            switch (id)
            {
               case ID_LIST_1:{
                     play_index = nm->idx;//切换至下一首
                     mp3player.ucStatus = STA_SWITCH;	                  
//                  Play_index = ;
//                  sw_flag = 1;
                  //PostCloseMessage(hwnd); //产生WM_CLOSE消息关闭主窗口
                  //menu_list_1[nm->idx].cbStartup(hwnd);
               }

               break;
            }

         }

      
         if (code == BN_CLICKED && id == ICON_VIEWER_ID_PREV)
         {
            SendMessage(GetDlgItem(hwnd, ID_LIST_1), MSG_MOVE_PREV, TRUE, 0);
         }
         ////
         if (code == BN_CLICKED && id == ICON_VIEWER_ID_NEXT)
         {
            SendMessage(GetDlgItem(hwnd, ID_LIST_1), MSG_MOVE_NEXT, TRUE, 0);
         }         
         if (code == BN_CLICKED && id == ID_EXIT)
         {
           
            PostCloseMessage(hwnd);
         }   
         break;
      }      
      case WM_CLOSE: //窗口关闭时，会自动产生该消息.
		{         
         GUI_VMEM_Free(menu_list);
         GUI_VMEM_Free(wbuf);
         enter_flag = 0;
         SetForegroundWindow(MusicPlayer_hwnd);
			return DestroyWindow(hwnd); //调用DestroyWindow函数销毁窗口，该函数会使主窗口结束并退出消息循环;否则窗口将继续运行.
		}      
      default:
         return DefWindowProc(hwnd, msg, wParam, lParam); 
   }

} 


void GUI_MusicList_DIALOG(void)
{
	HWND	hwnd;
	WNDCLASS	wcex;
	MSG msg;

	wcex.Tag = WNDCLASS_TAG;
	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = Win_proc; //设置主窗口消息处理的回调函数.
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

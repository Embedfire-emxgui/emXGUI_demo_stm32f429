#include "emXGUI.h"
#include "ff.h"

#include "./mjpegplayer/GUI_AVIList_DIALOG.h"
#include "./mjpegplayer/GUI_AVIPLAYER_DIALOG.h"
#include "string.h"
#include	"CListMenu.h"
#include "x_libc.h"
#include <stdlib.h>
#include "GUI_AppDef.h"
/**********************变量****************************/
char playlist[FILE_MAX_NUM][FILE_NAME_LEN];//播放List
char lcdlist[FILE_MAX_NUM][FILE_NAME_LEN];//显示list
uint8_t  file_num = 0;//文件个数
uint8_t  file_nums = 0;
char path[100]="0:";//文件根目??
COLORREF color_bg_list;
int flag = 0;//只扫描一次文件目录
int Play_index = 0;
extern HWND	VideoPlayer_hwnd;

int sw_flag;//切换标志
/**
  * @brief  scan_files 递归扫描sd卡内的视频文??
  * @param  path:初始扫描路径
  * @retval result:文件系统的返回??
  */
static FRESULT scan_files (char* path) 
{ 
  FRESULT res; 		//部分在递归过程被修改的变量，不用全局变量	
  FILINFO fno; 
  DIR dir; 
  int i; 
  char *fn; 
  char file_name[FILE_NAME_LEN];	
	
#if _USE_LFN 
  static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1]; 	//长文件名支持
  fno.lfname = lfn; 
  fno.lfsize = sizeof(lfn); 
#endif  
  res = f_opendir(&dir, path); //打开目录
  if (res == FR_OK) 
  { 
    i = strlen(path); 
    for (;;) 
    { 
      res = f_readdir(&dir, &fno); 										//读取目录下的内容
     if (res != FR_OK || fno.fname[0] == 0) break; 	//为空时表示所有项目读取完毕，跳出
#if _USE_LFN 
      fn = *fno.lfname ? fno.lfname : fno.fname; 
#else 
      fn = fno.fname; 
#endif 
      if(strstr(path,"recorder")!=NULL)continue;       //逃过录音文件
      if (*fn == '.') continue; 											//点表示当前目录，跳过			
      if (fno.fattrib & AM_DIR) 
			{ 																							//目录，递归读取
        sprintf(&path[i], "/%s", fn); 							//合成完整目录??
        res = scan_files(path);											//递归遍历 
        if (res != FR_OK) 
					break; 																		//打开失败，跳出循??
        path[i] = 0; 
      } 
      else 
		{ 
				//printf("%s%s\r\n", path, fn);								//输出文件??
				if(strstr(fn,".avi")||strstr(fn,".AVI"))//判断是否AVI文件
				{
					if ((strlen(path)+strlen(fn)<FILE_NAME_LEN)&&(file_num<FILE_MAX_NUM)&&flag == 0)
					{
						sprintf(file_name, "%s/%s", path, fn);						
						memcpy(playlist[file_num],file_name,strlen(file_name));
						memcpy(lcdlist[file_num],fn,strlen(fn));						
						//memcpy(lcdlist1[file_num],fn,strlen(fn));
					}
               file_num++;//记录文件个数
				}//if 
      }//else
     } //for
  } 
  return res; 
}






void Insert(char a, int pos, char *str)
{
    int i, L;
    L = (int)strlen(str);
    if (L < pos)
    {
        return;
    }
    else
    {
        for (i = L; i > pos; i --)
        {
            str[i] = str[i - 1];
        }
        str[pos] = a;
        str[L + 1] = '\0';
    }

}

static BOOL Player_Init(void)
{
   int i = 0;
   scan_files(path);
   if(!flag){
      flag = 1;
      for(; i < file_num; i++)
      {
         Insert('\0', 13, lcdlist[i]);
      }    
   }
   return 0;
}



static void button_owner_draw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];
   
   HDC hdc_tmp;
   
	hwnd = ds->hwnd; //button的窗口句柄.
	hdc = ds->hDC;   //button的绘图上下文句柄.
	rc = ds->rc;     //button的绘制矩形区.
   
   hdc_tmp = CreateMemoryDC(SURF_SCREEN, rc.w, rc.h);


   SetBrushColor(hdc_tmp, MapRGB(hdc_tmp, COLOR_DESKTOP_BACK_GROUND));

   FillRect(hdc_tmp, &rc); //用矩形填充背景
	if (IsWindowEnabled(hwnd) == FALSE)
	{
		SetTextColor(hdc_tmp, MapRGB(hdc_tmp, COLOR_INVALID));
	}
	else if(ds->State & BST_PUSHED)
	{ //按钮是按下状态
//    GUI_DEBUG("ds->ID=%d,BST_PUSHED",ds->ID);
//		SetBrushColor(hdc,MapRGB(hdc,150,200,250)); //设置填充色(BrushColor用于所有Fill类型的绘图函数)
//		SetPenColor(hdc,MapRGB(hdc,250,0,0));        //设置绘制色(PenColor用于所有Draw类型的绘图函数)
		SetTextColor(hdc_tmp, MapRGB(hdc_tmp, 250, 250, 255));      //设置文字色
	}
	else
	{ //按钮是弹起状态
//		SetBrushColor(hdc,MapRGB(hdc,255,255,255));
//		SetPenColor(hdc,MapRGB(hdc,0,250,0));
		SetTextColor(hdc_tmp, MapRGB(hdc_tmp, 250, 250, 255));
	}


	//	SetBrushColor(hdc,COLOR_BACK_GROUND);

	//	FillRect(hdc,&rc); //用矩形填充背景
	//	DrawRect(hdc,&rc); //画矩形外框
	//  
	//  FillCircle(hdc,rc.x+rc.w/2,rc.x+rc.w/2,rc.w/2); //用矩形填充背景FillCircle
	//	DrawCircle(hdc,rc.x+rc.w/2,rc.x+rc.w/2,rc.w/2); //画矩形外框

	  /* 使用控制图标字体 */
	SetFont(hdc_tmp, AVI_Player_hFont72);
	//  SetTextColor(hdc,MapRGB(hdc,255,255,255));
      
	GetWindowText(ds->hwnd, wbuf, 128); //获得按钮控件的文字

	DrawText(hdc_tmp, wbuf, -1, &rc, DT_VCENTER | DT_CENTER);//绘制文字(居中对齐方式)

   BitBlt(hdc, 0, 0, rc.w, rc.h, hdc_tmp, 0, 0, SRCCOPY);

   DeleteDC(hdc_tmp);

}




static LRESULT win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   static struct __obj_list *menu_list = NULL;
   static WCHAR (*wbuf)[128];
   switch(msg)
   {
      case WM_CREATE:
      {
         
         
         int i = 0;
         list_menu_cfg_t cfg;
	   	RECT rc;
         GetClientRect(hwnd, &rc);
         menu_list = (struct __obj_list *)malloc(sizeof(struct __obj_list)*file_num);
         wbuf = (WCHAR (*)[128])malloc(sizeof(WCHAR *)*file_num);
         printf("%d\n", file_num);
         if(menu_list == NULL) 
            return 0;
         for(;i < file_num; i++){
            //printf("%s\n", lcdlist[i]);
            x_mbstowcs_cp936(wbuf[i], lcdlist[i], FILE_NAME_LEN);
            menu_list[i].pName = wbuf[i];
            menu_list[i].cbStartup = NULL;
            menu_list[i].icon = L"N";
            menu_list[i].bmp = NULL;
         }       
         
         cfg.list_objs = menu_list; 
         cfg.x_num = 3;
         cfg.y_num = 2; 
         CreateWindow(&wcex_ListMenu,
                      L"ListMenu1",
                      WS_VISIBLE | LMS_ICONFRAME,
                      rc.x + 100, rc.y + 80, rc.w - 200, rc.h-80,
                      hwnd,
                      ID_LIST_1,
                      NULL,
                      &cfg);

         CreateWindow(BUTTON, L"L", BS_FLAT | BS_NOTIFY | WS_OWNERDRAW |WS_VISIBLE,
                        0, rc.h * 1 / 2, 70, 70, hwnd, ICON_VIEWER_ID_PREV, NULL, NULL);
         SetWindowFont(GetDlgItem(hwnd, ICON_VIEWER_ID_PREV), AVI_Player_hFont48); 
	      CreateWindow(BUTTON, L"K", BS_FLAT | BS_NOTIFY | WS_OWNERDRAW | WS_VISIBLE,
			rc.w - 65, rc.h * 1 / 2, 70, 70, hwnd, ICON_VIEWER_ID_NEXT, NULL, NULL);
         SetWindowFont(GetDlgItem(hwnd, ICON_VIEWER_ID_NEXT), AVI_Player_hFont48);
         
//         CreateWindow(BUTTON, L"Q", BS_FLAT | BS_NOTIFY | WS_OWNERDRAW |WS_VISIBLE,
//			10, 5, 70, 70, hwnd, ICON_VIEWER_ID_LIST, NULL, NULL);         
         
         break;
      }
      case WM_PAINT:
      {
         PAINTSTRUCT ps;
         HDC hdc, hdc_mem, hdc_mem1;//屏幕hdc
         RECT rc = {0,0,72,72};
         RECT rc_cli = {0,0,72,72};
         GetClientRect(hwnd, &rc_cli);
         hdc = BeginPaint(hwnd, &ps); 
         hdc_mem = CreateMemoryDC(SURF_SCREEN, 72, 72);
         hdc_mem1 = CreateMemoryDC(SURF_SCREEN, 72, 72);
         /**************返回上一级按钮***************/
         //边框
         SetBrushColor(hdc, MapRGB(hdc, 0,0,0));
         FillCircle(hdc, 0, 0, 80);  
         
         SetBrushColor(hdc, MapRGB(hdc, 250,0,0));
         FillCircle(hdc, 0, 0, 76);     
         
         SetBrushColor(hdc_mem, MapRGB(hdc, 250,0,0));
         FillRect(hdc_mem, &rc);        
         
         SetFont(hdc_mem, AVI_Player_hFont48);
         SetTextColor(hdc_mem, MapRGB(hdc_mem, 250, 250,250));
         TextOut(hdc_mem, 0, 0, L"R", -1);
         StretchBlt(hdc, 10, 12, 40, 40, 
                    hdc_mem, 0, 0, 72, 72, SRCCOPY);
         /****************返回主界面按钮******************/
         SetBrushColor(hdc, MapRGB(hdc, 0,0,0));
         FillCircle(hdc, rc_cli.w, 0, 80);  
         
         SetBrushColor(hdc, MapRGB(hdc, 250,0,0));
         FillCircle(hdc, rc_cli.w, 0, 76); 
         //字体层
         SetBrushColor(hdc_mem1, MapRGB(hdc, 250,0,0));
         FillRect(hdc_mem1, &rc);        
         
         SetFont(hdc_mem1, AVI_Player_hFont48);
         SetTextColor(hdc_mem1, MapRGB(hdc_mem1, 250, 250,250));
         TextOut(hdc_mem1, 0, 0, L"O", -1);

         StretchBlt(hdc, 755, 12, 40, 40, 
                    hdc_mem1, 0, 0, 72, 72, SRCCOPY);

         DeleteDC(hdc_mem);
         DeleteDC(hdc_mem1);
         EndPaint(hwnd, &ps);
         break;         
      }
      case WM_ERASEBKGND:
      {
         HDC hdc = (HDC)wParam;
         HDC hdc_mem;
         RECT rc_top  = {0, 0, 800, 80};
         RECT rc_text = {200, 0, 400, 80};
         RECT rc_cli;
         GetClientRect(hwnd, &rc_cli);
         
         hdc_mem = CreateMemoryDC(SURF_ARGB4444, rc_cli.w, rc_cli.h);
         SetBrushColor(hdc, MapRGB(hdc, 54, 54, 54));
         FillRect(hdc, &rc_cli);
         
         
         SetBrushColor(hdc_mem, MapARGB(hdc_mem, 50, 0, 0, 0));
         FillRect(hdc_mem, &rc_top);
         SetFont(hdc_mem, defaultFont);
         SetTextColor(hdc_mem, MapARGB(hdc_mem, 250, 250, 250, 250));
         DrawText(hdc_mem, L"播放列表", -1, &rc_text, DT_SINGLELINE| DT_CENTER | DT_VCENTER);
         BitBlt(hdc, rc_top.x, rc_top.y, rc_top.w, rc_top.h, 
                hdc_mem, rc_top.x, rc_top.y, SRCCOPY);  
            
//         StretchBlt(hdc, 755, 12, 40, 40, 
//                    hdc_mem1, 0, 0, 72, 72, SRCCOPY);
         
         color_bg_list = GetPixel(hdc, 700, 0);
         DeleteDC(hdc_mem);         
         break;
      }
      case WM_DRAWITEM:
      {

         DRAWITEM_HDR *ds;

         ds = (DRAWITEM_HDR*)lParam;

         button_owner_draw(ds); //执行自绘制按钮

         return TRUE;

      }
      case WM_LBUTTONDOWN:
      {
         POINT pt;
         pt.x =GET_LPARAM_X(lParam); //获得X坐标
         pt.y =GET_LPARAM_Y(lParam); //获得Y坐标
         RECT rc = {0, 0, 80, 80};
         if(PtInRect(&rc, &pt))
         {
            PostCloseMessage(hwnd);
            //产生WM_CLOSE消息关闭主窗口
         }
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
                  
                  Play_index = nm->idx;
                  sw_flag = 1;
                  PostCloseMessage(hwnd); //产生WM_CLOSE消息关闭主窗口
                  //menu_list_1[nm->idx].cbStartup(hwnd);
               }
               case ICON_VIEWER_ID_LIST:
               {
                  PostCloseMessage(hwnd); //产生WM_CLOSE消息关闭主窗口
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
         break;
      }
      case WM_CLOSE:
      {
         free(menu_list);
         free(wbuf);
         file_nums = file_num;
         file_num = 0;
         SetForegroundWindow(VideoPlayer_hwnd);//设置前台窗口为MusicPlayer_hwnd，否则的话会触发重绘
         //DestroyWindow(hwnd);
         return DestroyWindow(hwnd);	
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
   Player_Init();
	
	hwnd = CreateWindowEx(WS_EX_NOFOCUS,
                         &wcex,
                         L"GUI_MusicList_DIALOG",
                         WS_CLIPSIBLINGS,
                         0, 0, GUI_XSIZE, GUI_YSIZE,
                         NULL, NULL, NULL, NULL);
	//显示主窗??
	ShowWindow(hwnd, SW_SHOW);
	//开始窗口消息循??窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)??
	while (GetMessage(&msg, hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

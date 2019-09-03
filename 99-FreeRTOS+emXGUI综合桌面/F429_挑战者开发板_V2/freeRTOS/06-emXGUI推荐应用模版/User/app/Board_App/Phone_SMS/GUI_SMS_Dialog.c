#include <emXGUI.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "x_libc.h"
#include "GUI_AppDef.h"
#include "emXGUI_JPEG.h"
#include "emxgui_png.h"

#define COLOR_SMS_BACK_GROUND         50,50,50
extern int SelectDialogBox(HWND hwndParent, RECT rc, const WCHAR *pText, const WCHAR *pCaption, const MSGBOX_OPTIONS *ops);
  
/* 控件 ID 定义 */
enum
{   
  /* 按钮 ID */
  eID_SMS_EXIT  = 0,
  eID_SMS_CLEAR,
  eID_SMS_SEND,
  eID_SMS_DEL,
  eID_SMS_CONTENT,
  eID_SMS_NUMBER,
  eID_SMS_LIST,

};

/* 用户自定义消息 */
enum eMSG 
{
	eMSG_READ_TEXT = WM_USER + 1,
	eMSG_INIT_ERROR,
};

/*
 * @brief  读短信内容
 * @param  hListWnd: 列表框句柄
 * @retval NONE
*/
void Read_Text(HWND hListWnd)
{
	u32 i = 0;
	u32 xC = 1;
	WCHAR wbuf[20];
  char messagename[20];
  char *wNumber;
  char *wContent;
  
  sim900a_tx_printf("AT+CNMI=2,1\r");
  SIM900A_DELAY(100);  
  sim900a_tx_printf("AT+CMGF=1\r");           //文本模式
  SIM900A_DELAY(100); 
  sim900a_tx_printf("AT+CSCS=\"UCS2\"\r");     //"GSM"字符集
  SIM900A_DELAY(100); 
  
  wNumber = (char *)GUI_VMEM_Alloc(200);
  wContent = (char *)GUI_VMEM_Alloc(3*1024);

  SIM900A_CLEAN_RX();

	while(readmessage(xC++,(char *)&wNumber[1000],(char *)&wContent[1200]))
	{
    hexuni2gbk((char *)&wNumber[1000], messagename);	
    
    GUI_DEBUG("number->(%s)\n",messagename);
    x_mbstowcs_cp936(wbuf, messagename, sizeof(wbuf));	//将Ansi字符转换成GUI的unicode字符.
    xC++;
		//在Listbox中增加一个Item项，记录文件名和文件属性.
		i = SendMessage(hListWnd, LB_GETCOUNT, 0, 0);
		SendMessage(hListWnd, LB_ADDSTRING, i, (LPARAM)wbuf);
    
		/* 通过设置Item项的DATA值来记录短信的属性(用于区分是发送的还是接收的). */
		//SendMessage(hListWnd, LB_SETITEMDATA, i, (LPARAM)pNextInfo.dwFileAttributes);
    SIM900A_CLEAN_RX();
	}
  
  GUI_VMEM_Free(wNumber);
  GUI_VMEM_Free(wContent);
}

/*
 * @brief  退出按钮重绘
 * @param  ds: 自定义绘制结构体
 * @retval NONE
*/
static void SMS_ExitButton_OwnerDraw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	// RECT rc_top={0,0,800,70};
	WCHAR wbuf[128];

	hwnd = ds->hwnd;
	hdc = ds->hDC;
	rc = ds->rc;

	SetBrushColor(hdc, MapRGB(hdc, COLOR_SMS_BACK_GROUND));
	FillRect(hdc, &rc); //用矩形填充背景

	SetBrushColor(hdc, MapRGB(hdc, COLOR_DESKTOP_BACK_GROUND));

	FillCircle(hdc, rc.x + rc.w, rc.y, rc.w);

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

// 圆角按钮重绘制
static void SMS_FilletButton_OwnerDraw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	hwnd = ds->hwnd;
	hdc = ds->hDC;
	rc = ds->rc;

	SetBrushColor(hdc, MapRGB(hdc, COLOR_SMS_BACK_GROUND));
	FillRect(hdc, &rc);

	SetBrushColor(hdc, MapRGB(hdc, 22, 155, 213));
	EnableAntiAlias(hdc, TRUE);
	FillRoundRect(hdc, &rc, 5);
	EnableAntiAlias(hdc, FALSE);

	if (ds->State & BST_PUSHED)
	{
		/* 按钮是按下状态 */
		SetTextColor(hdc, MapRGB(hdc, 120, 120, 120));       //设置文字色
		OffsetRect(&rc, 1, 1);
	}
	else
	{
		/* 按钮是弹起状态 */
		SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
	}

	GetWindowText(hwnd, wbuf, 128);                          //获得按钮控件的文字
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER | DT_CENTER);    //绘制文字(居中对齐方式)
}

/*
 * @brief  重绘显示亮度的透明文本
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void Fillet_Textbox_OwnerDraw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button的窗口句柄.
	hdc = ds->hDC;   //button的绘图上下文句柄.
	GetClientRect(hwnd, &rc);//得到控件的位置

	SetBrushColor(hdc, MapRGB(hdc, COLOR_SMS_BACK_GROUND));
	FillRect(hdc, &rc);

	SetBrushColor(hdc, MapRGB(hdc, 255, 255, 255));
	EnableAntiAlias(hdc, TRUE);
	FillRoundRect(hdc, &rc, 8);
	EnableAntiAlias(hdc, FALSE);

	SetTextColor(hdc, MapRGB(hdc, 50, 50, 50));
	GetWindowText(hwnd, wbuf, 128); //获得按钮控件的文字
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER | DT_CENTER);//绘制文字(居中对齐方式)
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
	RECT rc;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button的窗口句柄.
	hdc = ds->hDC;   //button的绘图上下文句柄.
	GetClientRect(hwnd, &rc);//得到控件的位置

	SetBrushColor(hdc, MapRGB(hdc, 242, 242, 242));
	FillRect(hdc, &rc);

	SetTextColor(hdc, MapRGB(hdc, 50, 50, 50));
	GetWindowText(hwnd, wbuf, 128); //获得按钮控件的文字
	rc.w -= 45;
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//绘制文字(居中对齐方式)
}

static LRESULT	win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_CREATE:
    {
      RECT rc, m_rc[12];
      HWND wnd;
      
      GetClientRect(hwnd, &rc);

      /* 初始化 GSM 模块 */
      if (sim900a_init() != SIM900A_TRUE)//
      {
        PostAsyncMessage(hwnd, eMSG_INIT_ERROR, 0, 0);     // 初始化失败发送消息
      }

      //InflateRectEx(&rc, -3, -112, -3, -101);
      //MakeMatrixRect(m_rc, &rc, 0, 0, 3, 4);
	    
      CreateWindow(BUTTON, L"O",	WS_VISIBLE|WS_OWNERDRAW, 730, 0, 70, 70, hwnd, eID_SMS_EXIT, NULL, NULL);
      CreateWindow(BUTTON, L"清除",	WS_VISIBLE|WS_OWNERDRAW, 590, 433, 84, 40, hwnd, eID_SMS_CLEAR, NULL, NULL);
      CreateWindow(BUTTON, L"删除全部短信",	WS_VISIBLE|WS_OWNERDRAW, 395, 433, 160, 40, hwnd, eID_SMS_DEL,  NULL, NULL);
      CreateWindow(BUTTON, L"发送", WS_VISIBLE|WS_OWNERDRAW, 710, 433, 84, 40, hwnd, eID_SMS_SEND, NULL, NULL);
      CreateWindow(TEXTBOX, L"这里显示内容", WS_VISIBLE, 390, 70, 410, 354, hwnd, eID_SMS_CONTENT, NULL, NULL);
      SendMessage(GetDlgItem(hwnd, eID_SMS_CONTENT), TBM_SET_TEXTFLAG, 0, DT_TOP | DT_LEFT | DT_BKGND);
      CreateWindow(TEXTBOX, L"15185884286", WS_VISIBLE | WS_OWNERDRAW, 479, 18, 245, 40, hwnd, eID_SMS_NUMBER, NULL, NULL);
      wnd = CreateWindow(LISTBOX, L"SMS LIST", WS_VISIBLE | WS_BORDER, 6, 70, 379, 385, hwnd, eID_SMS_LIST, NULL, NULL);

      SetTimer(hwnd, 0, 1, TMR_START|TMR_SINGLE, NULL);

//	  PostAsyncMessage(hwnd, eMSG_READ_TEXT, 0, 0);     // 执行一次读短信

      break;
    } 

    case WM_TIMER:
    {
      HWND wnd;
      int i;

      wnd = GetDlgItem(hwnd, eID_SMS_LIST);

      SendMessage(wnd, LB_RESETCONTENT, 0, 0);

      Read_Text(wnd);
      InvalidateRect(wnd, NULL, TRUE);

      break;
    }
	

	case eMSG_INIT_ERROR:
	{
		RECT RC;
		MSGBOX_OPTIONS ops;
		const WCHAR *btn[] = { L"确认",L"取消" };      //对话框内按钮的文字

		ops.Flag = MB_ICONERROR;
		ops.pButtonText = btn;
		ops.ButtonCount = 2;
		RC.w = 300;
		RC.h = 200;
		RC.x = (GUI_XSIZE - RC.w) >> 1;
		RC.y = (GUI_YSIZE - RC.h) >> 1;
		SelectDialogBox(hwnd, RC, L"没有检测到GSM模块\n请重新检查连接。", L"错误", &ops);    // 显示错误提示框
		PostCloseMessage(hwnd);
		break;
	}
  
	case eMSG_READ_TEXT:
	{
		HWND wnd;
		int i;

		wnd = GetDlgItem(hwnd, eID_SMS_LIST);

		SendMessage(wnd, LB_RESETCONTENT, 0, 0);

		Read_Text(wnd);
		InvalidateRect(wnd, NULL, TRUE);

		break;
	}
	
    
    case WM_ERASEBKGND:
    {
      HDC hdc = (HDC)wParam;
      RECT rc;

      GetClientRect(hwnd, &rc);
      SetBrushColor(hdc, MapRGB(hdc, COLOR_SMS_BACK_GROUND));
      FillRect(hdc, &rc);
      return TRUE;
    }

    case WM_PAINT:
    {
      HDC hdc;
      PAINTSTRUCT ps;
      RECT rc = {393, 21, 79, 34};
      hdc = BeginPaint(hwnd, &ps);
      
      SetBrushColor(hdc, MapRGB(hdc, 22, 155, 213));
      EnableAntiAlias(hdc, TRUE);
      FillRoundRect(hdc, &rc, 5);
      EnableAntiAlias(hdc, FALSE);

      SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
      DrawText(hdc, L"联系人", -1, &rc, DT_VCENTER | DT_CENTER);    //绘制文字(居中对齐方式)

      rc.x = 143;
      rc.y = 17;
      rc.w = 92;
      rc.h = 38;
      DrawText(hdc, L"短信", -1, &rc, DT_VCENTER | DT_CENTER);    //绘制文字(居中对齐方式)
      
      EndPaint(hwnd, &ps);
      break;
    }
    
    case WM_DRAWITEM:
    {
       DRAWITEM_HDR *ds;
       ds = (DRAWITEM_HDR*)lParam;
       switch(ds->ID)
       {
          case eID_SMS_EXIT:
          {
            SMS_ExitButton_OwnerDraw(ds);
            return TRUE;             
          }    

		  case eID_SMS_CLEAR:
		  case eID_SMS_DEL:
		  case eID_SMS_SEND:
		  {
			  SMS_FilletButton_OwnerDraw(ds);
			  return TRUE;
		  }

		  case eID_SMS_NUMBER:
		  {
			  Fillet_Textbox_OwnerDraw(ds);
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

      switch (code)
      {
        case BN_CLICKED:
        {
          switch (id)
          {
            /* 退出按钮按下 */
            case eID_SMS_EXIT:
            {
              PostCloseMessage(hwnd);    // 发送关闭窗口的消息
            }
            break;

            /* 删除全部短信按钮按下 */
            case eID_SMS_DEL:
            {
              RECT RC;
              MSGBOX_OPTIONS ops;
              const WCHAR *btn[] = { L"确认",L"取消" };      //对话框内按钮的文字

              ops.Flag = MB_ICONERROR;
              ops.pButtonText = btn;
              ops.ButtonCount = 2;
              RC.w = 300;
              RC.h = 200;
              RC.x = (GUI_XSIZE - RC.w) >> 1;
              RC.y = (GUI_YSIZE - RC.h) >> 1;
              
              if (SelectDialogBox(hwnd, RC, L"将会删除全部短信。", L"删除", &ops) == 0)    // 显示确认提示框
              {
                
              }
            }
            break;

            /* 清除按钮按下 */
            case eID_SMS_CLEAR:
            {
              SetWindowText(GetDlgItem(hwnd, eID_SMS_CONTENT), L"");
              SetWindowText(GetDlgItem(hwnd, eID_SMS_NUMBER), L"");
            }
            break;

            /* 发送按钮按下 */
            case eID_SMS_SEND:
            {
              
            }
            break;
          }
        }
        break;
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

void GUI_SMS_Dialog(void)
{
	
	WNDCLASS	wcex;
	MSG msg;
  HWND MAIN_Handle;
	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = win_proc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);
   
	//创建主窗口
	MAIN_Handle = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,
                              &wcex,
                              L"Short Messaging Service",
                              WS_VISIBLE|WS_CLIPCHILDREN,
                              0, 0, GUI_XSIZE, GUI_YSIZE,
                              NULL, NULL, NULL, NULL);
   //显示主窗口
	ShowWindow(MAIN_Handle, SW_SHOW);
	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while (GetMessage(&msg, MAIN_Handle))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}  
}



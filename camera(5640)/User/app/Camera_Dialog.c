#include <emXGUI.h>
#include "./camera/bsp_ov5640.h"
#include "x_libc.h"
#include "./camera/ov5640_AF.h"
uint8_t fps=0;//帧率
OV5640_IDTypeDef OV5640_Camera_ID;
RECT rc_fps = {17,17,80,80};
HWND Cam_hwnd;//主窗口句柄
int state = 0;
U16 *bits;

static LRESULT WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch(msg)
   {
      case  WM_CREATE:
      {
        /* 初始化摄像头GPIO及IIC */
        OV5640_HW_Init();  
        /* 读取摄像头芯片ID，确定摄像头正常连接 */
        OV5640_ReadID(&OV5640_Camera_ID);

        if(OV5640_Camera_ID.PIDH  == 0x56)
        {
          GUI_DEBUG("OV5640 ID:%x %x",OV5640_Camera_ID.PIDH ,OV5640_Camera_ID.PIDL);
        }
        else
        {
          MSGBOX_OPTIONS ops;
          const WCHAR *btn[]={L"确定"};
          int x,y,w,h;

          ops.Flag =MB_BTN_WIDTH(60)|MB_ICONERROR;
          ops.pButtonText =btn;
          ops.ButtonCount =1;
          w =400;
          h =200;
          x =(GUI_XSIZE-w)>>1;
          y =(GUI_YSIZE-h)>>1;
          MessageBox(hwnd,x,y,w,h,L"没有检测到OV5640摄像头，\n请重新检查连接。",L"消息",&ops);           
          break;  
        }     
    
        bits = (U16 *)GUI_VMEM_Alloc(800*480); 
		  SetTimer(hwnd,1,100,TMR_START,NULL);
 
        break;
      }
 		case WM_TIMER:
      {
         switch(state)
         {
            case 0:
            {

               OV5640_Init();
              
               OV5640_RGB565Config();
               OV5640_AUTO_FOCUS();
               //使能DCMI采集数据
               DCMI_Cmd(ENABLE); 
               DCMI_CaptureCmd(ENABLE); 	
               state = 1;
               break;
            }
            case 1:
            {
               InvalidateRect(hwnd,NULL,FALSE);
               state=2;
               break;
            }
            case 2:
               //InvalidateRect(hwnd,NULL,FALSE);
               break;
         }
         break;
      }     
      case WM_PAINT:
      {
         PAINTSTRUCT ps;
         SURFACE *pSurf;
         HDC hdc_mem;
         HDC hdc;
         WCHAR wbuf[128];
         hdc = BeginPaint(hwnd,&ps);
         GUI_DEBUG("1\n");          
         pSurf =CreateSurface(SURF_RGB565,GUI_XSIZE, GUI_YSIZE, 0, bits);
         hdc_mem =CreateDC(pSurf,NULL);
         if(state == 2)
         {
            x_wsprintf(wbuf,L"帧率:%d/s",fps);
            DrawText(hdc_mem, wbuf, -1, &rc_fps, DT_SINGLELINE| DT_VCENTER);
            BitBlt(hdc, 0, 0, 800, 480, 
                   hdc_mem, 0, 0, SRCCOPY); 
         }
         DeleteSurface(pSurf);
         DeleteDC(hdc_mem);
         EndPaint(hwnd,&ps);
         break;
      }
      default:
         return DefWindowProc(hwnd, msg, wParam, lParam);
   }
   return WM_NULL;
}
void	GUI_VideoPlayer_DIALOG(void)
{	
	WNDCLASS	wcex;
	MSG msg;


	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WinProc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);

	//创建主窗口
	Cam_hwnd = CreateWindowEx(WS_EX_NOFOCUS,
                                    &wcex,
                                    L"GUI_Camera_Dialog",
                                    WS_VISIBLE,
                                    0, 0, GUI_XSIZE, GUI_YSIZE,
                                    NULL, NULL, NULL, NULL);

	//显示主窗口
	ShowWindow(Cam_hwnd, SW_SHOW);

	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while (GetMessage(&msg, Cam_hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

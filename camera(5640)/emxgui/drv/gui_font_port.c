/**
  *********************************************************************
  * @file    gui_font_port.c
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   字体管理接口
  *********************************************************************
  * @attention
  * 官网    :www.emXGUI.com
  *
  **********************************************************************
  */ 

#include	"emXGUI.h"
#include  "gui_drv_cfg.h"
#include "board.h" 
#include "gui_mem_port.h"
#include "gui_font_port.h"
#include "gui_resource_port.h"

/*===================================================================================*/
/*===================================================================================*/
#include <GUI_Font_XFT.h>

/**
  * @note  若要把字体加入到内部FLASH，
  * 把相应的字体数组文件添加到工程，然后在此处加入声明
  * 并使用XFT_CreateFont创建字体
  */
extern const char GB2312_16_1BPP[];
extern const char GB2312_16_2BPP[];
extern const char GB2312_20_2BPP[];

extern const char GB2312_16_4BPP[];
extern const char GB2312_20_4BPP[];

extern const char ASCII_16_4BPP[];
extern const char ASCII_20_4BPP[];
extern const char ASCII_24_4BPP[];
extern const char ASCII_32_4BPP[];

HFONT hFont_SDCARD=NULL;
HFONT hFont_SDCARD_100=NULL;
HFONT DEFAULT_FONT  =NULL;

/*===================================================================================*/
#if (GUI_USE_EXTERN_FONT && (!GUI_FONT_LOAD_TO_RAM))

/**
  * @brief  从流媒体加载内容的回调函数
  * @param  buf[out] 存储读取到的数据缓冲区
  * @param  offset 要读取的位置
  * @param  size 要读取的数据大小
  * @param  lParam 调用函数时的自定义参数（用户参数）
  * @retval 读取到的数据大小
  */
static int font_read_data_exFlash(void *buf,int offset,int size,LONG lParam)
{
  /* 本例子中offset是具体字符数据在字体文件中的偏移 
   * lParam 是字体文件在FLASH中的基地址
  */
	offset += lParam;
  
  /* 读取具体的字模数据内容 */
	RES_DevRead(buf,offset,size);
	return size;
}
#endif

#if GUI_USE_SDCARD_FONT
BOOL font_read_data_SDCARD(char *res_name, char** buf, u32 size) 
{	 	
    /* file objects */
    FIL     *file;												
    FRESULT fresult;  
    BOOL result = TRUE;
    UINT br;
    file =(FIL*)GUI_VMEM_Alloc(sizeof(FIL));
     
    fresult = f_open(file, res_name, FA_OPEN_EXISTING | FA_READ );

    size = f_size(file);    
    /* 文件内容空间 */
    *buf = (char *)GUI_VMEM_Alloc(size);
    fresult = f_read(file, *buf, size, &br);
    /* 关闭文件 */
    f_close(file);

    
    /* 释放空间 */
    GUI_VMEM_Free(file);    
    
    return result;
}
#else
FIL *file;												
FRESULT fresult;  
BOOL result = TRUE;
UINT br;
int fsize;
int font_read_data_SDCARD(void *buf, int offset, int size,LONG lParam) 
{	 	     
//    /* 文件内容空间 */
//    buf = (char *)GUI_VMEM_Alloc(size);   
      /* 加载整个图片文件 */
      fresult = f_read(file, buf, size, &br);
       
    
    /* 释放空间 */
    
    return result;
}
#endif
/**
  * @brief  GUI默认字体初始化
  * @param  无
  * @retval 返回默认字体的句柄
  */
BOOL res;
HFONT GUI_Default_FontInit(void)
{

	HFONT hFont=NULL;

#if (GUI_FONT_LOAD_TO_RAM  )
  {  
    /* 整个字体文件加载至RAM */
    
    int font_base;
    
    /* 指向缓冲区的指针 */
    static u8 *pFontData_XFT=NULL;
    CatalogTypeDef dir;
    
    /* RES_GetInfo读取到的dir.offset是资源的绝对地址 */
    font_base =RES_GetInfo_AbsAddr(GUI_DEFAULT_EXTERN_FONT, &dir);

    if(font_base > 0)
    {
    	pFontData_XFT =(u8*)GUI_VMEM_Alloc(dir.size);
      if(pFontData_XFT!=NULL)
      {
        RES_DevRead(pFontData_XFT, font_base, dir.size);

        hFont = XFT_CreateFont(pFontData_XFT);
      }
    }
  }
#elif (GUI_USE_EXTERN_FONT)   
  {
    /* 使用流设备加载字体，按需要读取 */
    if(hFont==NULL)
    { 
    	int font_base;
      CatalogTypeDef dir;

    	font_base =RES_GetInfo_AbsAddr(GUI_DEFAULT_EXTERN_FONT, &dir);
    	if(font_base > 0)
    	{
    		hFont =XFT_CreateFontEx(font_read_data_exFlash,font_base);
    	}
    }
  }
#elif (GUI_USE_SDCARD_FONT)  
  {
     /* 指向缓冲区的指针 */
    static u8 *pFontData_XFT=NULL;
    static u8 *pFontData_XFT_100=NULL;
    static u8 *pFontData_XFT_DEFAULT=NULL;
    u32  	fsize;
 
    if(hFont_SDCARD==NULL)
    { 
      res = font_read_data_SDCARD(GUI_DEFAULT_SDCARD_FONT,(char **)&pFontData_XFT, fsize);   
      hFont_SDCARD = XFT_CreateFont(pFontData_XFT);
    }    
//    if(hFont_SDCARD_100==NULL)
//    { 
//      res = font_read_data_SDCARD(GUI_DEFAULT_SDCARD_100FONT,(char **)&pFontData_XFT_100, fsize);   
//      hFont_SDCARD_100 = XFT_CreateFont(pFontData_XFT_100);
//    }     
    if(DEFAULT_FONT==NULL)
    { 
      res = font_read_data_SDCARD(GUI_DEFAULT_EXTERN_FONT,(char **)&pFontData_XFT_DEFAULT, fsize);   
      DEFAULT_FONT = XFT_CreateFont(pFontData_XFT_DEFAULT);
    }
   
  }   
#endif

    /* 若前面的字体加载失败，使用内部FLASH中的数据（工程中的C语言数组）
    *  添加字体数据时，把数组文件添加到工程，在本文件头添加相应字体数组的声明，
    *  然后调用XFT_CreateFont函数创建字体即可
    */
    if(hFont==NULL)
    { 
      /* 从本地加载(本地数组数据) */    	
      hFont =XFT_CreateFont(GUI_DEFAULT_FONT);  /*ASCii字库,20x20,4BPP抗锯齿*/
      /* 中文字库存储占用空间非常大，不推荐放在内部FLASH */
    	//hFont =XFT_CreateFont(GB2312_16_2BPP); /*GB2312字库,16x16,2BPP抗锯齿*/
    	//hFont =XFT_CreateFont(GB2312_20_4BPP); /*GB2312字库,20x20,4BPP抗锯齿*/
    }
    
	return DEFAULT_FONT;
}

/********************************END OF FILE****************************/


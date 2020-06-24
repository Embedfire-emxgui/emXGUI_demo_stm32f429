#ifndef _YMODEM_APP_H_
#define _YMODEM_APP_H_

#include "stm32f4xx.h"

#ifdef	__cplusplus
extern "C"{
#endif

//SD卡及flash的根目录
#define SD_ROOT       "0:"
#define FLASH_ROOT    "1:"

/* 资源在SD卡中的路径 */
#define RESOURCE_DIR         "0:/srcdata"
/* 资源烧录到的FLASH基地址（目录地址） */
#define RESOURCE_BASE_ADDR    4096
/* 存储在FLASH中的资源目录大小 */
#define CATALOG_SIZE           (8*1024)
  
  
/* 生成的烧录信息文件名 */
#define BURN_INFO_NAME        "burn_info.txt"
/* 完整烧录信息文件名*/  
#define BURN_INFO_NAME_FULL   (RESOURCE_DIR "/" BURN_INFO_NAME)
/* 记录忽略的文件名 */
#define IGNORE_NAME           "ignore.txt"  
/* 完整忽略文件名*/    
#define IGNORE_NAME_FULL      (RESOURCE_DIR "/" IGNORE_NAME)

/*=========================================================================================*/
/* 目录信息类型 */
typedef struct 
{
	char 	name[40];  /* 资源的名字 */
	u32  	size;      /* 资源的大小 */ 
	u32 	offset;    /* 资源相对于基地址的偏移 */
}CatalogTypeDef;

/*=========================================================================================*/

/*信息输出*/
#define BURN_DEBUG_ON         1
#define BURN_DEBUG_FUNC_ON    0

#define BURN_INFO(fmt,arg...)           printf("<<-BURN-INFO->> "fmt"\n",##arg)
#define BURN_ERROR(fmt,arg...)          printf("<<-BURN-ERROR->> "fmt"\n",##arg)
#define BURN_DEBUG(fmt,arg...)          do{\
                                          if(BURN_DEBUG_ON)\
                                          printf("<<-BURN-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)

#define BURN_DEBUG_FUNC()               do{\
                                         if(BURN_DEBUG_FUNC_ON)\
                                         printf("<<-BURN-FUNC->> Func:%s@Line:%d\n",__func__,__LINE__);\
                                       }while(0)

#ifdef	__cplusplus
}
#endif

#endif /* XMODEM_H_ */

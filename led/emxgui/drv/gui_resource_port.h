#ifndef __GUI_RESOURCE_PORT_H
#define	__GUI_RESOURCE_PORT_H

#include "stm32f4xx.h"
#include	"emXGUI.h"
#include	"gui_drv_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif  
  
/* 目录信息类型 */
typedef struct 
{
	char 	name[24];  /* 资源的名字 */
	u32  	size;      /* 资源的大小 */ 
	u32 	offset;    /* 资源相对于基地址的偏移 */
}CatalogTypeDef;


BOOL RES_DevInit(void);
U32 RES_DevGetID(void);
BOOL RES_DevWrite(u8 *buf,u32 addr,u32 size);
BOOL RES_DevRead(u8 *buf,u32 addr,u32 size);
int RES_DevEraseSector(u32 addr);
void RES_DevTest(void);

s32 RES_GetOffset(const char *res_name);
s32 RES_GetInfo_AbsAddr(const char *res_name, CatalogTypeDef *dir);

BOOL FS_Load_Content(char *file_name, char** buf, u32* size); 
BOOL RES_Load_Content(char *file_name, char** buf,u32* size);

 /**
  * @brief  释放动态申请的buf空间
  * @retval 无
  */
static __inline void RES_Release_Content(char **buf)
{
    /* 释放空间 */
    GUI_VMEM_Free(*buf);
}

#ifdef	__cplusplus
}
#endif
#endif /* __GUI_RESOURCE_PORT_H */

#ifndef __GUI_FONT_PORT_H
#define	__GUI_FONT_PORT_H

#include	"emXGUI.h"
#include "stm32f4xx.h"
#include	"gui_drv_cfg.h"


HFONT GUI_Default_FontInit(void);
extern HFONT hFont_SDCARD;
extern HFONT hFont_SDCARD_100;
extern HFONT DEFAULT_FONT;
extern HFONT ICON64_FONT;
#endif /* __GUI_FONT_PORT_H */

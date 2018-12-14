#ifndef __GUI_FONT_PORT_H
#define	__GUI_FONT_PORT_H

#include	"emXGUI.h"
#include "stm32f4xx.h"
#include	"gui_drv_cfg.h"

extern HFONT hZoomFont;
extern HFONT hDefaultFont;

HFONT GUI_Default_FontInit(void);

#endif /* __GUI_FONT_PORT_H */

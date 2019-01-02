
#include "emXGUI.h"

void	GUI_DEMO_RectSpeed(void);
void	GUI_DEMO_Hello(void);

void	GUI_DEMO_Button(void);
void	GUI_DEMO_Checkbox(void);
void	GUI_DEMO_Radiobox(void);
void	GUI_DEMO_Textbox(void);
void	GUI_DEMO_Progressbar(void);
void	GUI_DEMO_Scrollbar(void);
void	GUI_DEMO_Listbox(void);
void	GUI_DEMO_Drawbitmap(void);
void	GUI_DEMO_Drawbitmap_extern(void);
void	GUI_DEMO_TextOut(void);
void	GUI_DEMO_Messagebox(void);
void	GUI_DEMO_Fatfs(void);
void	GUI_DEMO_DrawJPEG_Extern(void);
void	GUI_DEMO_SlideWindow(void);
void	GUI_DEMO_IconViewer(void);
void	GUI_DEMO_Desktop(void);


void GUI_AppMain(void)
{
//	GUI_DEMO_RectSpeed();
	
	while(1)
	{
    Board_App_Desktop();
	}
}

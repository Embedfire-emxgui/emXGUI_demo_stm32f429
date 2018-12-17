
#include "emXGUI.h"
#include "vidoplayer.h"
void	GUI_DEMO_RectSpeed(void);
void	GUI_MUSICPLAYER_DIALOG(void);

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
void	GUI_VideoPlayer_DIALOG(void);
void	GUI_DEMO_Fatfs(void);
void	GUI_DEMO_DrawJPEG_Extern(void);
void	GUI_DEMO_LogoIconFont(void);
void	GUI_MUSICLRC_DIALOG(void);
void	GUI_MusicList_DIALOG(void);
void	GUI_VideoPlayer_DIALOG(void);

//////////////////////////
void* vmalloc(int size)
{
	return GUI_VMEM_Alloc(size);
}
void vfree(void *p)
{
	GUI_VMEM_Free(p);
}
///////////////////////

void GUI_AppMain(void)
{	
	while(1)
	{
     GUI_VideoPlayer_DIALOG();
	}
}

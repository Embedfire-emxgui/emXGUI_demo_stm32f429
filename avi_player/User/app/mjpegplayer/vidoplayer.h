#ifndef __VIDOPLAYER_H
#define __VIDOPLAYER_H

#include <stm32f4xx.h>

#define  T_vids	 0x6463
#define  T_auds	 0x7762


#define FILE_NAME_LEN           100	
#define FILE_MAX_NUM            20

#if 0
void AVI_play(void);
void Prog_ctl(void);
#else
void AVI_play(char *filename, HWND hwnd);
#endif
#endif


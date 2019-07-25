#ifndef	__MP3_GET_Image_H__
#define	__MP3_GET_Image_H__

#include "stm32f4xx.h"

typedef struct{
  uint8_t  Flag;      // 标志是否找到图片
  uint32_t Size;      // 图片的大小
  uint8_t  Type;      // 图片类型 0:jpeg 1:PNG
  uint8_t *Offset;    // 图片的偏移起始地址
} MP3_Image_t;

void Get_Imare_Info(uint8_t *data, MP3_Image_t *Image_Info);

#endif
/*****************************File End*****************************************/

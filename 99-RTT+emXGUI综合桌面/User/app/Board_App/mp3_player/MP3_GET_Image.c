/*******************************************************************************
***************************** 读MP3中的专辑图片 ********************************
*******************************************************************************/

#include "MP3_GET_Image.h"
#include <string.h>

/**
  * @brief  获取MP3ID3V2文件头的大小
  * @param  输入MP3文件开头的数据，至少10个字节
  * @retval ID3V2的大小
  */
static uint32_t mp3_Header_Size(unsigned char *buf)
{
 uint32_t ID3V2_size;
	
 if(buf[0] == 'I' && buf[1] == 'D' && buf[2] =='3')//存在ID3V2
 {
 	 ID3V2_size = (buf[6]<<21) | (buf[7]<<14) | (buf[8]<<7) | buf[9];
 }
 else//不存在ID3V2
	 ID3V2_size = 0;

 return ID3V2_size;
}

/**
  * @brief  获取 MP3ID3V2 文件头的大小
* @param  buf：第一个帧标签 string：帧标识
  * @retval 返回帧标识的指针位置
  */
uint8_t *MP3_FrameId_Get(unsigned char *buf, uint32_t File_Header,char *string)
{
  uint8_t *FrameId;
  uint8_t databuff;
  uint8_t *start = buf;

  while(1)
  {
//    databuff = *buf;
//    *(buf+4) = '\0';
    
    if (strstr((char *)buf, string))
    {
      FrameId = buf;
      
      return FrameId;
    }
    
//    printf("%s\n", buf);
//    *(buf+4) = databuff;
    
    buf += (*(buf+4) << 24) + (*(buf+5) << 16) + (*(buf+6) << 8) + (*(buf+7)) + 10;    // 计算下一帧的标签
    
    if (buf-start > File_Header-10)
    {
      break;
    }
  }

  return NULL;
}

/**
  * @brief  获取MP3 专辑图片的信息
  * @param  输入MP3文件开头的数据，整个文件头
  * @retval 无
  */
void Get_Imare_Info(uint8_t *data, MP3_Image_t *Image_Info)
{
  uint32_t File_Header;
  uint8_t buff;
  uint8_t *APIC;
  
  File_Header = mp3_Header_Size(data);    // 获取文件头的大小
  
  APIC = MP3_FrameId_Get((uint8_t *)(data+10), File_Header, "APIC");
  
  if (APIC != NULL)    // 有图片
  {
    Image_Info->Flag = 1;    // 没有找到图片
    Image_Info->Size = (APIC[4] << 24 | APIC[5] << 24 | APIC[6] << 24 | APIC[7]) - 14;    // 计算图片数据的大小
    Image_Info->Offset = (APIC + 24);                                   // 计算图片的偏移位置
  }
  else
  {
    Image_Info->Flag = NULL;    // 没有找到图片
    
    return;
  }
}

/*****************************File End*****************************************/

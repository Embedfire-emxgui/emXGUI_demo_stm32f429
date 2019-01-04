#include    <string.h>
#include    <math.h>
#include	"gui_drv.h"

/*=========================================================================================*/

/* 8位内存绘图(RGB332) */

/*=========================================================================================*/

#define	COLOR8		U8

#define	__set_addr(pSurf,x,y)		(COLOR8*)((U8*)pSurf->Bits + (x) + ((y)*pSurf->WidthBytes))

/*=========================================================================================*/

void* 	GL8_set_addr(const SURFACE *pSurf,int x,int y)
{
	return (void*)__set_addr(pSurf,x,y);
}

/*=========================================================================================*/

void	GL8_set_pixel(const SURFACE *pSurf,int x,int y,COLORREF c)
{
	COLOR8 *addr;
	
	addr  = __set_addr(pSurf,x,y);
	*addr =c;
}

/*=========================================================================================*/

COLORREF	GL8_get_pixel(const SURFACE *pSurf,int x,int y)
{
	COLOR8 *addr;
	
	addr = __set_addr(pSurf,x,y);
	return *addr;
}
/*=========================================================================================*/

void	GL8_xor_pixel(const SURFACE *pSurf,int x,int y)
{
	COLOR8 *addr;

	addr = __set_addr(pSurf,x,y);

	*addr ^= 0xFF;
}

/*=========================================================================================*/

void	GL8_draw_hline(const SURFACE *pSurf,int x0,int y0,int x1,COLORREF c)
{
	COLOR8 *addr;
	
	addr = __set_addr(pSurf,x0,y0);
	GUI_memset8(addr,c,x1-x0);
}

/*=========================================================================================*/

void	GL8_draw_vline(const SURFACE *pSurf,int x0,int y0,int y1,COLORREF c)
{
	COLOR8 *addr;
	
	addr = __set_addr(pSurf,x0,y0);
	GUI_memset8_step(addr,c,y1-y0,pSurf->WidthBytes);		
}

/*=========================================================================================*/

void	GL8_fill_rect(const SURFACE *pSurf,int x,int y,int w,int h,COLORREF c)
{
	COLOR8 *addr;
	int line_step;
	
	line_step =pSurf->WidthBytes;
	addr = __set_addr(pSurf,x,y);

	for(h=y+h;y<h;y++)
	{	
		GUI_memset8(addr,c,w);
		addr += line_step;
	}
	
}

/*=========================================================================================*/

int GL8_copy_bits(const SURFACE *pSurf,int x,int y,int w,int h,int width_bytes,U8 *dst_bits)
{
	int yy;
	U8 *src_bits;
	////
		
	src_bits = (U8*)__set_addr(pSurf,x,y);
	
	for(yy=0;yy<h;yy++)
	{
		memcpy(dst_bits,src_bits,w);
		dst_bits += width_bytes;
		src_bits += pSurf->WidthBytes;				
	}
	return	pSurf->Format;	
}

/*=========================================================================================*/

void GL8_draw_bitmap_LUT1(const SURFACE *pSurf,int x,int y,int w,int h,int width_bytes, const U8 *bits,int bit_offset,const COLORREF *LUT)
{
	int xx,yy;
	U8 *p,dat;
	COLOR8 color0,color1,*addr;
	////
	
	color0	=LUT[0];
	color1	=LUT[1];
		
	for(yy=0;yy<h;yy++)
	{
		addr =__set_addr(pSurf,x,y+yy);
		 	    
	    p	=(U8*)bits;	    	
		xx	=0;
		if(bit_offset > 0)
		{
			dat	=*p++;

			switch(bit_offset)
			{
				case	1: goto off_1;
				case	2: goto off_2;
				case	3: goto off_3;
				case	4: goto off_4;
				case	5: goto off_5;
				case	6: goto off_6;
				case	7: goto off_7;
			}
		}

		
		while(xx<w)
		{
			dat	=*p++;
//off_0:
			if(xx>=w)	goto next_line;
			if( dat & (1<<7)) { *addr++ =color1; }else{ *addr++ =color0; }	
			xx++;
off_1:
			if(xx>=w)	goto next_line;
			if( dat & (1<<6)) { *addr++ =color1; }else{ *addr++ =color0; }	
			xx++;
off_2:
			if(xx>=w)	goto next_line;
			if( dat & (1<<5)) { *addr++ =color1; }else{ *addr++ =color0; }	
			xx++;
off_3:
			if(xx>=w)	goto next_line;
			if( dat & (1<<4)) { *addr++ =color1; }else{ *addr++ =color0; }	
			xx++;
off_4:
			if(xx>=w)	goto next_line;			
			if( dat & (1<<3)) { *addr++ =color1; }else{ *addr++ =color0; }	
			xx++;
off_5:
			if(xx>=w)	goto next_line;			
			if( dat & (1<<2)) { *addr++ =color1; }else{ *addr++ =color0; }	
			xx++;
off_6:
			if(xx>=w)	goto next_line;			
			if( dat & (1<<1)) { *addr++ =color1; }else{ *addr++ =color0; }	
			xx++;
off_7:
			if(xx>=w)	goto next_line;			
			if( dat & (1<<0)) { *addr++ =color1; }else{ *addr++ =color0; }	
			xx++;

		}
			
next_line:	
		
		bits += width_bytes;
		
	}
	
}

void 	GL8_draw_bitmap_LUT2(const SURFACE *pSurf,int x,int y,int w,int h,int width_bytes,const U8 *bits,int bit_offset,const COLORREF *LUT)
{

}

void 	GL8_draw_bitmap_LUT4(const SURFACE *pSurf,int x,int y,int w,int h,int width_bytes,const U8 *bits,int bit_offset,const COLORREF *LUT)
{
	int xx,yy;
	U8 *p,dat;
	COLOR8 *addr;
	COLORREF color;
	////
	
	for(yy=0;yy<h;yy++)
	{
		addr =__set_addr(pSurf,x,y+yy);
		   	    
	    p	=(U8*)bits;	 
	    xx	=0;

		if(bit_offset!=0)
		{
			dat = *p++;
			goto off_1;
		}

		while(xx<w)
		{
			dat	=*p++;
//off_0:
		    if(xx>=w)	goto next_line;	
			color   = LUT[(dat>>4)&0x0F];
			*addr++ = color;
			xx++;
off_1:
		    if(xx>=w)	goto next_line;
			color   = LUT[dat&0x0F];
			*addr++ = color;
			xx++;
					
		}
			
next_line:	
		bits += width_bytes;
		
	}
	
}

void 	GL8_draw_bitmap_LUT8(const SURFACE *pSurf,int x,int y,int w,int h,int width_bytes,const U8 *bits,const COLORREF *LUT)
{
	int xx,yy;
	U8 *p;
	COLOR8 *addr;
	COLORREF color;
	////
	
	for(yy=0;yy<h;yy++)
	{
		addr =__set_addr(pSurf,x,y+yy);
	   	    
	    p	=(U8*)bits;
	    
		for(xx=0;xx<w;xx++)
		{
			color   = LUT[*p++];
			*addr++ = color;
		}
		
		bits += width_bytes;
		
	}
	
}

void 	GL8_draw_bitmap_RGB332(const SURFACE *pSurf,int x,int y,int w,int h,int width_bytes,const U8 *bits)
{
	int xx,yy;
	U8 *p,color;
	COLOR8 *addr;
	////
		
	for(yy=0;yy<h;yy++)
	{
		addr =__set_addr(pSurf,x,y+yy);

	    p	=(U8*)bits;
	    
		for(xx=0;xx<w;xx++)
		{
			color	=*p++;				
			*addr++ =SURF_MapRGB332(pSurf,color);
					
		}
		
		bits += width_bytes;
		
	}
	
}

void 	GL8_draw_bitmap_RGB565(const SURFACE *pSurf,int x,int y,int w,int h,int width_bytes,const U8 *bits)
{

	int xx,yy;
	U16 *p,color;
	COLOR8 *addr;
	////
	
	for(yy=0;yy<h;yy++)
	{
		addr =__set_addr(pSurf,x,y+yy);
	   	    
	    p	=(U16*)bits;
	    
		for(xx=0;xx<w;xx++)
		{
			color	=*p++;				
			*addr++ =SURF_MapRGB565(pSurf,color);
		}
		
		bits += width_bytes;
		
	}
	
}

void 	GL8_draw_bitmap_XRGB1555(const SURFACE *pSurf,int x,int y,int w,int h,int width_bytes,const U8 *bits)
{
	int xx,yy;
	U16 *p,color;
	COLOR8 *addr;
	////
		
	for(yy=0;yy<h;yy++)
	{
		addr =__set_addr(pSurf,x,y+yy);
	   	    
	    p	=(U16*)bits;
	    
		for(xx=0;xx<w;xx++)
		{
			color	=*p++;		
			*addr++ =SURF_MapXRGB1555(pSurf,color);
					
		}
		
		bits += width_bytes;
		
	}
	
}

void 	GL8_draw_bitmap_ARGB1555(const SURFACE *pSurf,int x,int y,int w,int h,int width_bytes,const U8 *bits)
{
	int xx,yy;
	U16 *p,color;
	COLOR8 *addr;
	////
		
	for(yy=0;yy<h;yy++)
	{
		addr =__set_addr(pSurf,x,y+yy);
	   	    
	    p	=(U16*)bits;
	    
		for(xx=0;xx<w;xx++)
		{
			color	=*p++;
		
			if(color&0x8000)
			{			
				*addr++ =SURF_MapXRGB1555(pSurf,color);
			}
			else
			{	
				addr++;
			}
												
		}
		
		bits += width_bytes;
		
	}
	
}

void 	GL8_draw_bitmap_ARGB4444(const SURFACE *pSurf,int x,int y,int w,int h,int width_bytes,const U8 *bits)
{
	U16 *p,color;
  	U8 r,g,b,a;
	U8 r0,g0,b0;
	int xx,yy;
	COLOR8 *addr;
	////
		
	for(yy=0;yy<h;yy++)
	{
		addr =__set_addr(pSurf,x,y+yy);
	   	    
	    p	=(U16*)bits;
	    
		for(xx=0;xx<w;xx++)
		{
			color = *p++;
			switch(color&0xF0000)
			{
													
				case	0x0000:
						addr++;
						break;
										
				case	0xF000:
						*addr++ =SURF_MapXRGB4444(pSurf,color);
						break;
						////
								
				default:
						{
							a  = (color&0xF000)>>8;
							r	=(color&0x0F00)>>4;
							g	=(color&0x00F0);
							b	=(color&0x000F)<<4;

							color =*addr;
							SURF_GetRGB(pSurf,color,&r0,&g0,&b0);
							
							r =GUI_AlphaBlendValue(r0,r,a);
							g =GUI_AlphaBlendValue(g0,g,a);
							b =GUI_AlphaBlendValue(b0,b,a);
							
							*addr++ = SURF_MapRGB(pSurf,r,g,b);
						}
						break;
						////
			}
			
		}
		
		bits += width_bytes;
		
	}
	
}

void 	GL8_draw_bitmap_RGB888(const SURFACE *pSurf,int x,int y,int w,int h,int width_bytes,const U8 *bits)
{
	U8 r,g,b;
	int xx,yy;
	int i;
	COLOR8 *addr;
	////

	for(yy=0;yy<h;yy++)
	{
		addr =__set_addr(pSurf,x,y+yy);
	    
	    i=0;
		for(xx=0;xx<w;xx++)
		{
			b=bits[i++];
			g=bits[i++];
			r=bits[i++];
			
			*addr++ =SURF_MapRGB(pSurf,r,g,b);
		}
		
		bits += width_bytes;
		
	}
	
}

void 	GL8_draw_bitmap_XRGB8888(const SURFACE *pSurf,int x,int y,int w,int h,int width_bytes,const U8 *bits)
{
	U8 r,g,b;
	int xx,yy;
	int i;
	COLOR8 *addr;
	////
		
	for(yy=0;yy<h;yy++)
	{
		addr =__set_addr(pSurf,x,y+yy);
	    
	    i=0;
		for(xx=0;xx<w;xx++)
		{
			b=bits[i++];
			g=bits[i++];
			r=bits[i++];
			i++;
			
			*addr++ =SURF_MapRGB(pSurf,r,g,b);
		}
		
		bits += width_bytes;
		
	}
	
}

void 	GL8_draw_bitmap_ARGB8888(const SURFACE *pSurf,int x,int y,int w,int h,int width_bytes,const U8 *bits)
{
	U8 r,g,b,a;
	U8 r0,g0,b0;
	int xx,yy;
	int i;
	COLOR8 *addr;
	U32 color;
	////

	for(yy=0;yy<h;yy++)
	{
		
		addr =__set_addr(pSurf,x,y+yy);
	    
	    i=0;
		for(xx=0;xx<w;xx++)
		{
					
			b=bits[i++];
			g=bits[i++];
			r=bits[i++];
			a=bits[i++];
			
			switch(a)
			{
													
				case	0x00:
						addr++;
						break;
										
				case	0xFF:
						*addr++ =SURF_MapRGB(pSurf,r,g,b);
						break;
						////
								
				default:
						{			
							color =*addr;	
							SURF_GetRGB(pSurf,color,&r0,&g0,&b0);
							
							r =GUI_AlphaBlendValue(r0,r,a);
							g =GUI_AlphaBlendValue(g0,g,a);
							b =GUI_AlphaBlendValue(b0,b,a);
							
							*addr++ =SURF_MapRGB(pSurf,r,g,b);
						}
						break;
						////
			}

		}
		
		bits += width_bytes;
		
	}
	
}

void 	GL8_draw_bitmap_AL1(const SURFACE *pSurf,int x,int y,int w,int h,int width_bytes, const U8 *bits,int bit_offset,COLORREF color)
{
	int xx,yy;
	U8 *p,dat;
	COLOR8 *addr;
	////
			
	for(yy=0;yy<h;yy++)
	{
		addr =__set_addr(pSurf,x,y+yy);
		 	    
	    p	=(U8*)bits;	    	
		xx	=0;
		
		if(bit_offset>0)
		{
			dat	=*p++;

			switch(bit_offset)
			{
				case	1: goto off_1;
				case	2: goto off_2;
				case	3: goto off_3;
				case	4: goto off_4;
				case	5: goto off_5;
				case	6: goto off_6;
				case	7: goto off_7;
			}
		}
		
		while(xx < w)
		{
			dat	=*p++;
			
			if(xx>=w)	break;
			if( dat & (1<<7)) { *addr++ =color; }else{ addr++; }	
			xx++;

off_1:
			if(xx>=w)	break;
			if( dat & (1<<6)) { *addr++ =color; }else{ addr++; }	
			xx++;

off_2:
			if(xx>=w)	break;
			if( dat & (1<<5)) { *addr++ =color; }else{ addr++; }	
			xx++;

off_3:
			if(xx>=w)	break;
			if( dat & (1<<4)) { *addr++ =color; }else{ addr++; }	
			xx++;

off_4:
			if(xx>=w)	break;
			if( dat & (1<<3)) { *addr++ =color; }else{ addr++; }	
			xx++;

off_5:
			if(xx>=w)	break;
			if( dat & (1<<2)) { *addr++ =color; }else{ addr++; }	
			xx++;

off_6:
			if(xx>=w)	break;
			if( dat & (1<<1)) { *addr++ =color; }else{ addr++; }	
			xx++;

off_7:
			if(xx>=w)	break;
			if( dat & (1<<0)) { *addr++ =color; }else{ addr++; }	
			xx++;

		}
		
		bits += width_bytes;
		
	}
	
}

void 	GL8_draw_bitmap_AL2(const SURFACE *pSurf,int x,int y,int w,int h,int width_bytes, const U8 *bits,int bit_offset,COLORREF color)
{

}

void 	GL8_draw_bitmap_AL4(const SURFACE *pSurf,int x,int y,int w,int h,int width_bytes, const U8 *bits,int bit_offset,COLORREF color)
{

}

void GL8_draw_bitmap_AL8(const SURFACE *pSurf,int x,int y,int w,int h,int width_bytes,const U8 *bits,COLORREF color)
{
	COLOR8 *addr;
	int xx;
	U8 r,g,b,a;
	U8 r0,g0,b0;
	////

	SURF_GetRGB(pSurf,color,&r,&g,&b);

	for(h+=y;y<h;y++)
	{

		addr	=__set_addr(pSurf,x,y);

		for(xx=0;xx<w;xx++)
		{

			a = bits[xx];

			switch(a)
			{


				case	0x00:
						addr++;
						break;

				case	0xFF:

						*addr++ = color;
						break;
						////

				default:

						SURF_GetRGB(pSurf,*addr,&r0,&g0,&b0);
						r0 =GUI_AlphaBlendValue(r0,r,a);
						g0 =GUI_AlphaBlendValue(g0,g,a);
						b0 =GUI_AlphaBlendValue(b0,b,a);
						*addr++ =SURF_MapRGB(pSurf,r0,g0,b0);
						break;
						////
			}

		}

		bits += width_bytes;

	}

}


BOOL 	GL8_scale_bitmap(const SURFACE *pSurf,int x,int y,int w,int h,const BITMAP *bm)
{
	int	xx,yy,i,x_inc,y_inc,src_x,src_y;
	U8 *dst_p;
//	COLORREF (*pfnMapRGB)(u32 c);
	////

	src_x =0;
	src_y =0;

	//调整x,y缩放系数
	x_inc = (bm->Width<<16)/w;
	y_inc = (bm->Height<<16)/h;

	src_x = src_x<<16;
//	src_y = src_y<<16;

	dst_p = (U8*)__set_addr(pSurf,x,y);

	for(yy=0;yy<h;yy++)
	{
		switch(bm->Format)
		{
			case BM_RGB565:
			{
				U16 *src_p;
				src_p = (U16*)((U8*)bm->Bits + (src_x>>16)*2 + (src_y>>16)*bm->WidthBytes);
				i = 0;
				for(xx=0;xx<w;xx++)
				{
					dst_p[xx] = pSurf->CC->MapRGB565(src_p[i>>16]);
					i += x_inc;
				}
			}
			break;
			////

			case BM_XRGB8888:
			{
				U32 *src_p;
				src_p = (U32*)((U8*)bm->Bits + (src_x>>16)*4 + (src_y>>16)*bm->WidthBytes);
				i = 0;
				for(xx=0;xx<w;xx++)
				{
					dst_p[xx] = pSurf->CC->MapXRGB8888(src_p[i>>16]);
					i += x_inc;
				}
			}
			break;
			////

			default:
				return FALSE;
		}

		src_y += y_inc;
		dst_p += pSurf->WidthBytes;
	}

	return	TRUE;

}

int	GL8_rotate_bitmap(const SURFACE *pSurf,int cx,int cy,const BITMAP *bm,int angle)
{
	//float radians;
	int cos_a,sin_a;
	int minx,miny;
	int	inc_x,inc_y;
	int xx,yy,xx0,yy0,xx1,yy1;
	int dst_x,dst_y,dst_dx,dst_dy;

	int src_x,src_y,src_dx,src_dy;
	///////////

	src_x = 0;
	src_y = 0;
	src_dx = bm->Width;
	src_dy = bm->Height;

	angle %= 360;
	//radians	=(2*3.1416*angle)/360;

	cos_a	=(float)cos((2*3.1416f*angle)/360)*65536.0f;
	sin_a	=(float)sin((2*3.1416f*angle)/360)*65536.0f;

	xx	= (-src_dy*sin_a)>>16;
	yy	= (src_dy*cos_a)>>16;
	xx0	= (src_dx*cos_a-src_dy*sin_a)>>16;
	yy0	= (src_dy*cos_a+src_dx*sin_a)>>16;
	xx1	= (src_dx*cos_a)>>16;
	yy1	= (src_dx*sin_a)>>16;

	minx = MIN((float)0,MIN(xx,MIN(xx0,xx1)));
	miny = MIN((float)0,MIN(yy,MIN(yy0,yy1)));

	xx	= MAX(xx,MAX(xx0,xx1)); //maxx
	yy	= MAX(yy,MAX(yy0,yy1));	//maxy


	dst_dx	= (angle>90&&angle<180)?(-minx):(xx-minx);	//外接矩形宽度
	dst_dy	= (angle>180&&angle<270)?(-miny):(yy-miny);	//外接矩形高度

	dst_x	= cx-(dst_dx>>1);
	dst_y	= cy-(dst_dy>>1);
	////

	src_dx	= src_x+src_dx-1;	//调整src_dx为绝对坐标
	src_dy	= src_y+src_dy-1;	//调整src_dy为绝对坐标
	////

	if((dst_y+dst_dy)>=pSurf->Height)
	{
		dst_dy	= pSurf->Height-dst_y;
	}


	if((dst_x+dst_dx)>=pSurf->Width)
	{
		dst_dx	= pSurf->Width-dst_x;
	}

	////
	if(dst_y<0)
	{
		yy	=0-dst_y;
		yy0	=0;
	}
	else
	{
		yy	=0;
		yy0	=dst_y;
	}

	////
	for(;yy<dst_dy;yy++,yy0++)
	{
		//float f1,f2,f3;

		inc_x = (src_x + ((yy+miny)*sin_a) + (minx*cos_a)) >>16;
		inc_y = (src_y + ((yy+miny)*cos_a) - (minx*sin_a)) >>16;

		////
		if(dst_x<0)
		{
			xx	=0-dst_x;
			xx0	=0;
		}
		else
		{
			xx	=0;
			xx0	=dst_x;
		}
		////

		for(;xx<dst_dx;xx++,xx0++)
		{

			U8 *bits;

			xx1	=inc_x + ((xx*cos_a)>>16);
			if(xx1<src_x || xx1>src_dx)
			{
				continue;
			}

			yy1	=inc_y - ((xx*sin_a)>>16);
			if(yy1<src_y || yy1>src_dy)
			{
				continue;
			}


			bits = (U8*)bm->Bits+yy1*bm->WidthBytes;

			switch(bm->Format)
			{
			case BM_RGB565:
				bits += (xx1<<1);
				GL8_draw_bitmap_RGB565(pSurf,xx0,yy0,1,1,bm->WidthBytes,bits);
				break;
			case BM_ARGB4444:
				bits += (xx1<<1);
				GL8_draw_bitmap_ARGB4444(pSurf,xx0,yy0,1,1,bm->WidthBytes,bits);
				break;
			case BM_ARGB8888:
				bits += (xx1<<2);
				GL8_draw_bitmap_ARGB8888(pSurf,xx0,yy0,1,1,bm->WidthBytes,bits);
				break;
			default:
				return FALSE;

			}

		}


	}

	return	TRUE;
}

/*=========================================================================================*/
#if 0
GL_OP gdraw_8bpp;

void	gdDrawInit_8BPP(void)
{
	gdraw_8bpp.SetPos       =GL8_set_addr;
	gdraw_8bpp.SetPixel		=GL8_set_pixel;
	gdraw_8bpp.GetPixel		=GL8_get_pixel;
	gdraw_8bpp.HLine		=GL8_draw_hline;
	gdraw_8bpp.VLine		=GL8_draw_vline;
	gdraw_8bpp.FillArea		=GL8_fill_rect;
	gdraw_8bpp.CopyBits		=GL8_copy_bits;
	gdraw_8bpp.DrawBitmap_LUT1	=GL8_draw_bitmap_LUT1;
	gdraw_8bpp.DrawBitmap_LUT2	=NULL;
	gdraw_8bpp.DrawBitmap_LUT4	=GL8_draw_bitmap_LUT4;
	gdraw_8bpp.DrawBitmap_LUT8	=GL8_draw_bitmap_LUT8;
	gdraw_8bpp.DrawBitmap_RGB332	=GL8_draw_bitmap_RGB332;
	gdraw_8bpp.DrawBitmap_RGB565	=GL8_draw_bitmap_RGB565;
	gdraw_8bpp.DrawBitmap_XRGB1555	=GL8_draw_bitmap_XRGB1555;
	gdraw_8bpp.DrawBitmap_ARGB1555	=GL8_draw_bitmap_ARGB1555;
	gdraw_8bpp.DrawBitmap_ARGB4444	=GL8_draw_bitmap_ARGB4444;
	gdraw_8bpp.DrawBitmap_RGB888	=GL8_draw_bitmap_RGB888;
	gdraw_8bpp.DrawBitmap_XRGB8888	=GL8_draw_bitmap_XRGB8888;
	gdraw_8bpp.DrawBitmap_ARGB8888	=GL8_draw_bitmap_ARGB8888;
	
	gdraw_8bpp.DrawBitmap_AL1	=GL8_draw_bitmap_ALPHA1;
	gdraw_8bpp.DrawBitmap_AL2	=NULL;
	gdraw_8bpp.DrawBitmap_AL4	=NULL;
	gdraw_8bpp.DrawBitmap_AL8	=NULL;
	
}
#endif

#if 1
void GL_MEM_8BPP_Init(GL_OP *gd)
{
	gd->SetPos       =GL8_set_addr;
	gd->SetPixel	=GL8_set_pixel;
	gd->GetPixel	=GL8_get_pixel;
	gd->XorPixel	=GL8_xor_pixel;
	gd->HLine	    =GL8_draw_hline;
	gd->VLine		=GL8_draw_vline;
	gd->FillArea		=GL8_fill_rect;
	gd->CopyBits		=GL8_copy_bits;

	gd->DrawBitmap_LUT1	=GL8_draw_bitmap_LUT1;
	gd->DrawBitmap_LUT2	=GL8_draw_bitmap_LUT2;
	gd->DrawBitmap_LUT4	=GL8_draw_bitmap_LUT4;
	gd->DrawBitmap_LUT8	=GL8_draw_bitmap_LUT8;
	gd->DrawBitmap_RGB332	=GL8_draw_bitmap_RGB332;
	gd->DrawBitmap_RGB565	=GL8_draw_bitmap_RGB565;
	gd->DrawBitmap_XRGB1555	=GL8_draw_bitmap_XRGB1555;
	gd->DrawBitmap_ARGB1555	=GL8_draw_bitmap_ARGB1555;
	gd->DrawBitmap_ARGB4444	=GL8_draw_bitmap_ARGB4444;
	gd->DrawBitmap_RGB888	=GL8_draw_bitmap_RGB888;
	gd->DrawBitmap_XRGB8888	=GL8_draw_bitmap_XRGB8888;
	gd->DrawBitmap_ARGB8888	=GL8_draw_bitmap_ARGB8888;

	gd->DrawBitmap_AL1	=GL8_draw_bitmap_AL1;
	gd->DrawBitmap_AL2	=GL8_draw_bitmap_AL2;
	gd->DrawBitmap_AL4	=GL8_draw_bitmap_AL4;
	gd->DrawBitmap_AL8	=GL8_draw_bitmap_AL8;
	gd->ScaleBitmap			=GL8_scale_bitmap;
	gd->RotateBitmap        =GL8_rotate_bitmap;

}
#endif

/*=========================================================================================*/
/*=========================================================================================*/
/*=========================================================================================*/

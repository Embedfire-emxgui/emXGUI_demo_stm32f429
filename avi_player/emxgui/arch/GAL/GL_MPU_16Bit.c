#ifdef	MPU_LCD_TYPE

#include    <string.h>
#include    <math.h>

#include	"def.h"
#include	"GUI_Drv.h"

/*=========================================================================================*/
/* 16位MPU接口LCD通用绘图引擎 */

/*=========================================================================================*/

#define	COLOR16 U16

/*=========================================================================================*/

static	 __inline U8	__alpha_blend_value(U8 dst,U8 src,U8 src_a)
{
//	return	( src * src_a + dst * (255-src_a))>>8;
	return	(((src-dst)*src_a)>>8) + dst;
}

/*=========================================================================================*/
void* 	lcd_set_pos(const SURFACE *pSurf,int x,int y)
{
	__lcd_set_window(x,y,LCD_XMAX,LCD_YMAX);
	return NULL;
}

static void	lcd_set_pixel(const SURFACE *pSurf,int x,int y,COLORREF c)
{
	__lcd_set_window(x,y,LCD_XMAX,LCD_YMAX);
	__lcd_write_start();
	__lcd_write_pixel(c);

}

static COLORREF	lcd_get_pixel(const SURFACE *pSurf,int x,int y)
{
	__lcd_set_window(x,y,LCD_XMAX,LCD_YMAX);
	__lcd_read_start();
	return __lcd_read_pixel();
}

static void	lcd_get_pixel_RGB(const SURFACE *pSurf,int x,int y,U8 *r,U8 *g,U8 *b)
{

	__lcd_set_window(x,y,LCD_XMAX,LCD_YMAX);
	__lcd_read_start();
	__lcd_read_rgb(r,g,b);
}

static void	lcd_xor_pixel(const SURFACE *pSurf,int x,int y)
{
	u16 c;
	__lcd_set_window(x,y,LCD_XMAX,LCD_YMAX);
	__lcd_read_start();
	c=__lcd_read_pixel();

	c ^= 0xFFFF;

	__lcd_set_window(x,y,LCD_XMAX,LCD_YMAX);
	__lcd_write_start();
	__lcd_write_pixel(c);
}


static	void	lcd_draw_hline(const SURFACE *pSurf,int x0,int y0,int x1,COLORREF c)
{

	__lcd_set_window(x0,y0,LCD_XMAX,y0);
	__lcd_write_start();
	__lcd_write_pixels(c,x1-x0);

}

static	void	lcd_draw_vline(const SURFACE *pSurf,int x0,int y0,int y1,COLORREF c)
{
	__lcd_set_window(x0,y0,x0,LCD_YMAX);
	__lcd_write_start();

	__lcd_write_pixels(c,y1-y0);

}

static	void	lcd_fill_rect(const SURFACE *pSurf,int x,int y,int w,int h,COLORREF c)
{
	__lcd_set_window(x,y,x+w-1,y+h-1);
	__lcd_write_start();
	__lcd_write_pixels(c,w*h);
}

/*=========================================================================================*/

static	int lcd_copy_bits(const SURFACE *pSurf,int x,int y,int w,int h,int width_bytes,U8 *dst_bits)
{
	int xx,yy;
	U16 *dst;
	////
	for(yy=0;yy<h;yy++)
	{
		U16 c1,c2;

	    dst =(U16*)dst_bits;
	    __lcd_copy_bits(x,y+yy,w,1,dst);
		dst_bits += width_bytes;

	}

/*
	for(yy=0;yy<h;yy++)
	{
		U16 c1,c2;

	    dst =(U16*)dst_bits;
		for(xx=0;xx<w;xx++)
		{
			__lcd_set_window(x+xx,y+yy,LCD_XMAX,LCD_YMAX);
			__lcd_read_start();
			*dst++ =__lcd_read_pixel();

		}

		dst_bits += width_bytes;

	}
*/
	return	BM_RGB565;

}


/*=========================================================================================*/

static	void lcd_draw_bitmap_LUT_1(const SURFACE *pSurf,int x,int y,int w,int h,int width_bytes, const U8 *bits,int bit_offset,const COLORREF *LUT)
{

	int xx,yy;
	U8 *p,dat;
	COLOR16 color0,color1;
	////

	color0 = LUT[0];
	color1 = LUT[1];

	for(yy=0;yy<h;yy++)
	{
		__lcd_set_window(x,y+yy,LCD_XMAX,LCD_YMAX);
		__lcd_write_start();

	    p	=(U8*)bits;

		xx	=0;

		if(bit_offset>0)
		{
			dat	=*p++;
			switch(bit_offset)
			{

		//		case	0:
		//				if(xx++>=w)	goto next_line;
		//				if( dat & (1<<7)==0)		{ __set_data(	dst_p,color_0);} else {	__set_data(	dst_p,color_1);}


				case	1:
						if(xx>=w)	goto next_line;
						if( dat & (1<<6)) { __lcd_write_pixel(color1); }else{ __lcd_write_pixel(color0); }
						xx++;

				case	2:
						if(xx>=w)	goto next_line;
						if( dat & (1<<5)) { __lcd_write_pixel(color1); }else{ __lcd_write_pixel(color0); }
						xx++;

				case	3:
						if(xx>=w)	goto next_line;
						if( dat & (1<<4)) { __lcd_write_pixel(color1); }else{ __lcd_write_pixel(color0); }
						xx++;

				case	4:
						if(xx>=w)	goto next_line;
						if( dat & (1<<3)) { __lcd_write_pixel(color1); }else{ __lcd_write_pixel(color0); }
						xx++;

				case	5:
						if(xx>=w)	goto next_line;
						if( dat & (1<<2)) { __lcd_write_pixel(color1); }else{ __lcd_write_pixel(color0); }
						xx++;

				case	6:
						if(xx>=w)	goto next_line;
						if( dat & (1<<1)) { __lcd_write_pixel(color1); }else{ __lcd_write_pixel(color0); }
						xx++;

				case	7:
						if(xx>=w)	goto next_line;
						if( dat & (1<<0)) { __lcd_write_pixel(color1); }else{ __lcd_write_pixel(color0); }
						xx++;

				default:
						break;


			}
		}

		while(xx<w)
		{
			dat	=*p++;

			if(xx>=w)	goto next_line;
			if( dat & (1<<7)) { __lcd_write_pixel(color1); }else{ __lcd_write_pixel(color0); }
			xx++;

			if(xx>=w)	goto next_line;
			if( dat & (1<<6)) { __lcd_write_pixel(color1); }else{ __lcd_write_pixel(color0); }
			xx++;

			if(xx>=w)	goto next_line;
			if( dat & (1<<5)) { __lcd_write_pixel(color1); }else{ __lcd_write_pixel(color0); }
			xx++;

			if(xx>=w)	goto next_line;
			if( dat & (1<<4)) { __lcd_write_pixel(color1); }else{ __lcd_write_pixel(color0); }
			xx++;

			if(xx>=w)	goto next_line;
			if( dat & (1<<3)) { __lcd_write_pixel(color1); }else{ __lcd_write_pixel(color0); }
			xx++;

			if(xx>=w)	goto next_line;
			if( dat & (1<<2)) { __lcd_write_pixel(color1); }else{ __lcd_write_pixel(color0); }
			xx++;

			if(xx>=w)	goto next_line;
			if( dat & (1<<1)) { __lcd_write_pixel(color1); }else{ __lcd_write_pixel(color0); }
			xx++;

			if(xx>=w)	goto next_line;
			if( dat & (1<<0)) { __lcd_write_pixel(color1); }else{ __lcd_write_pixel(color0); }
			xx++;

		}

next_line:

		bits += width_bytes;

	}

}


static	void lcd_draw_bitmap_LUT_2(const SURFACE *pSurf,int x,int y,int w,int h,int width_bytes,const U8 *bits,int bit_offset,const COLORREF *LUT)
{

}

static	void lcd_draw_bitmap_LUT_4(const SURFACE *pSurf,int x,int y,int w,int h,int width_bytes,const U8 *bits,int bit_offset,const COLORREF *LUT)
{
	int xx,yy;
	U8 *p,dat;
	COLORREF color;
	////

	for(yy=0;yy<h;yy++)
	{
		__lcd_set_window(x,y+yy,LCD_XMAX,LCD_YMAX);
		__lcd_write_start();

	    p	=(U8*)bits;
	    xx	=0;

	    if(bit_offset>0)
	    {
			dat	=*p++;

			if(xx++ >= w)	goto next_line;
			color = LUT[dat&0x0F];
			__lcd_write_pixel(color);

	    }

		while(xx<w)
		{
			dat	=*p++;

		    if(xx >= w)	goto next_line;
			color = LUT[(dat>>4)&0x0F];
			__lcd_write_pixel(color);
			xx++;

		    if(xx >= w)	goto next_line;
			color = LUT[dat&0x0F];
			__lcd_write_pixel(color);
			xx++;

		}

next_line:
		bits += width_bytes;

	}

}

static	void lcd_draw_bitmap_LUT_8(const SURFACE *pSurf,int x,int y,int w,int h,int width_bytes,const U8 *bits,const COLORREF *LUT)
{
	int xx,yy;
	U8 *p;
	COLORREF color;
	////

	for(yy=0;yy<h;yy++)
	{
		__lcd_set_window(x,y+yy,LCD_XMAX,LCD_YMAX);
		__lcd_write_start();

	    p	=(U8*)bits;

		for(xx=0;xx<w;xx++)
		{
			color =LUT[*p++];
			__lcd_write_pixel(color);
		}

		bits += width_bytes;

	}

}

static	void lcd_draw_bitmap_RGB332(const SURFACE *pSurf,int x,int y,int w,int h,int width_bytes,const U8 *bits)
{
	int xx,yy;
	U8 *p,color;
	////

	for(yy=0;yy<h;yy++)
	{
		__lcd_set_window(x,y+yy,LCD_XMAX,LCD_YMAX);
		__lcd_write_start();

	    p	=(U8*)bits;

		for(xx=0;xx<w;xx++)
		{
			color = pSurf->CC->MapRGB332(*p++);
			__lcd_write_pixel(color);
		}

		bits += width_bytes;

	}

}
/*=========================================================================================*/

static	void lcd_draw_bitmap_RGB565(const SURFACE *pSurf,int x,int y,int w,int h,int width_bytes,const U8 *bits)
{

	int xx,yy;
	U16 *p,color;
	////


	for(yy=0;yy<h;yy++)
	{
		__lcd_set_window(x,y+yy,LCD_XMAX,LCD_YMAX);
		__lcd_write_start();

	    p	=(U16*)bits;
	    __lcd_write_bitmap(p,w);
		bits += width_bytes;

	}

}


static	void lcd_draw_bitmap_XRGB1555(const SURFACE *pSurf,int x,int y,int w,int h,int width_bytes,const U8 *bits)
{
	int xx,yy;
	U16 *p,color;
	////

	for(yy=0;yy<h;yy++)
	{
		__lcd_set_window(x,y+yy,LCD_XMAX,LCD_YMAX);
		__lcd_write_start();

	    p	=(U16*)bits;

		for(xx=0;xx<w;xx++)
		{
			color = pSurf->CC->MapXRGB1555(*p++);
			__lcd_write_pixel(color);
		}

		bits += width_bytes;

	}

}



static	void lcd_draw_bitmap_ARGB1555(const SURFACE *pSurf,int x,int y,int w,int h,int width_bytes,const U8 *bits)
{
	int xx,yy;
	U16 *p,color;
	////

	for(yy=0;yy<h;yy++)
	{
		__lcd_set_window(x,y+yy,LCD_XMAX,LCD_YMAX);
		__lcd_write_start();

	    p	=(U16*)bits;

		for(xx=0;xx<w;xx++)
		{
			color =*p++;

			if(color&(1<<15))
			{
				color = pSurf->CC->MapXRGB1555(color);
				__lcd_write_pixel(color);
			}
			else
			{
				__lcd_set_window(x+xx+1,y+yy,LCD_XMAX,LCD_YMAX);
				__lcd_write_start();
			}

		}

		bits += width_bytes;

	}

}

static	void lcd_draw_bitmap_ARGB4444(const SURFACE *pSurf,int x,int y,int w,int h,int width_bytes,const U8 *bits)
{
	U16 *p,color;
  	U8 r,g,b,a;
	U8 r0,g0,b0;
	int xx,yy;
	int i;

	////

	for(yy=0;yy<h;yy++)
	{
		__lcd_set_window(x,y+yy,LCD_XMAX,LCD_YMAX);
		__lcd_write_start();

	    p	=(U16*)bits;

		for(xx=0;xx<w;xx++)
		{
			color = *p++;

			switch(color&0xF000)
			{

				case	0x0000:
						__lcd_set_window(x+xx+1,y+yy,LCD_XMAX,LCD_YMAX);
						__lcd_write_start();
						break;

				case	0xF000:
						color =pSurf->CC->MapXRGB4444(color);
						__lcd_write_pixel(color);
						break;
						////

				default:
						{
							a  = (color&0xF000)>>8;
							r	=(color&0x0F00)>>4;
							g	=(color&0x00F0);
							b	=(color&0x000F)<<4;

							lcd_get_pixel_RGB(pSurf,x+xx,y+yy,&r0,&g0,&b0);

							r0 =GUI_AlphaBlendValue(r0,r,a);
							g0 =GUI_AlphaBlendValue(g0,g,a);
							b0 =GUI_AlphaBlendValue(b0,b,a);

							__lcd_write_start();
							__lcd_write_pixel(pSurf->CC->MapRGB(r0,g0,b0));
						}
						break;
						////
			}

		}

		bits += width_bytes;

	}

}

static	void lcd_draw_bitmap_RGB888(const SURFACE *pSurf,int x,int y,int w,int h,int width_bytes,const U8 *bits)
{
	U8 r,g,b;
	int xx,yy;
	int i;
	////

	for(yy=0;yy<h;yy++)
	{
		__lcd_set_window(x,y+yy,LCD_XMAX,LCD_YMAX);
		__lcd_write_start();

	    i=0;
		for(xx=0;xx<w;xx++)
		{
			b=bits[i++];
			g=bits[i++];
			r=bits[i++];

			__lcd_write_pixel(pSurf->CC->MapRGB(r,g,b));
		}

		bits += width_bytes;

	}

}

static	void lcd_draw_bitmap_XRGB8888(const SURFACE *pSurf,int x,int y,int w,int h,int width_bytes,const U8 *bits)
{
	U8 r,g,b;
	int xx,yy;
	int i;
	////

	for(yy=0;yy<h;yy++)
	{
		__lcd_set_window(x,y+yy,LCD_XMAX,LCD_YMAX);
		__lcd_write_start();

	    i=0;
		for(xx=0;xx<w;xx++)
		{
			b=bits[i++];
			g=bits[i++];
			r=bits[i++];
			i++;

			__lcd_write_pixel(pSurf->CC->MapRGB(r,g,b));
		}

		bits += width_bytes;

	}

}

static	void lcd_draw_bitmap_ARGB8888(const SURFACE *pSurf,int x,int y,int w,int h,int width_bytes,const U8 *bits)
{
	U8 r,g,b,a;
	U8 r0,g0,b0;
	int xx,yy;
	int i;
	////

	for(yy=0;yy<h;yy++)
	{

		__lcd_set_window(x,y+yy,LCD_XMAX,LCD_YMAX);
		__lcd_write_start();

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
						__lcd_set_window(x+xx+1,y+yy,LCD_XMAX,LCD_YMAX);
						__lcd_write_start();
						break;

				case	0xFF:
						__lcd_write_pixel(pSurf->CC->MapRGB(r,g,b));
						break;
						////

				default:
						{

							lcd_get_pixel_RGB(pSurf,x+xx,y+yy,&r0,&g0,&b0);

							r =GUI_AlphaBlendValue(r0,r,a);
							g =GUI_AlphaBlendValue(g0,g,a);
							b =GUI_AlphaBlendValue(b0,b,a);

							__lcd_write_start();
							__lcd_write_pixel(pSurf->CC->MapRGB(r,g,b));
						}
						break;
						////
			}

		}

		bits += width_bytes;

	}

}


static void 	lcd_draw_bitmap_AL1(const SURFACE *pSurf,int x,int y,int w,int h,int width_bytes, const U8 *bits,int bit_offset,COLORREF color)
{

	int xx,yy;
	U8 *p,dat;
	////

	for(yy=0;yy<h;yy++)
	{
		__lcd_set_window(x,y+yy,LCD_XMAX,LCD_YMAX);
		__lcd_write_start();

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
			if( dat & (1<<7)) { __lcd_write_pixel(color); xx++;} else { xx++; __lcd_set_window(x+xx,y+yy,LCD_XMAX,LCD_YMAX);__lcd_write_start();}

off_1:
			if(xx>=w)	break;
			if( dat & (1<<6)) { __lcd_write_pixel(color); xx++;} else { xx++; __lcd_set_window(x+xx,y+yy,LCD_XMAX,LCD_YMAX);__lcd_write_start();}

off_2:
			if(xx>=w)	break;
			if( dat & (1<<5)) { __lcd_write_pixel(color); xx++;} else { xx++; __lcd_set_window(x+xx,y+yy,LCD_XMAX,LCD_YMAX);__lcd_write_start();}

off_3:
			if(xx>=w)	break;
			if( dat & (1<<4)) { __lcd_write_pixel(color); xx++;} else { xx++; __lcd_set_window(x+xx,y+yy,LCD_XMAX,LCD_YMAX);__lcd_write_start();}

off_4:
			if(xx>=w)	break;
			if( dat & (1<<3)) { __lcd_write_pixel(color); xx++;} else { xx++; __lcd_set_window(x+xx,y+yy,LCD_XMAX,LCD_YMAX);__lcd_write_start();}

off_5:
			if(xx>=w)	break;
			if( dat & (1<<2)) { __lcd_write_pixel(color); xx++;} else { xx++; __lcd_set_window(x+xx,y+yy,LCD_XMAX,LCD_YMAX);__lcd_write_start();}

off_6:
			if(xx>=w)	break;
			if( dat & (1<<1)) { __lcd_write_pixel(color); xx++;} else { xx++; __lcd_set_window(x+xx,y+yy,LCD_XMAX,LCD_YMAX);__lcd_write_start();}

off_7:
			if(xx>=w)	break;
			if( dat & (1<<0)) { __lcd_write_pixel(color); xx++;} else { xx++; __lcd_set_window(x+xx,y+yy,LCD_XMAX,LCD_YMAX);__lcd_write_start();}

		}

		bits += width_bytes;

	}

}

static const U8 AL2_8_Tbl[4]={0x00,0x55,0xAA,0xFF};

static void lcd_draw_bitmap_AL2(const SURFACE *pSurf,int x,int y,int w,int h,int width_bytes,const U8 *bits,int bit_offset,COLORREF color)
{
	int i,xx,yy;
	U8 r,g,b,a,aa;
	U8 r0,g0,b0,a0;
	////

	SURF_GetRGB(pSurf,color,&r,&g,&b);

	for(yy=0;yy<h;yy++)
	{
		__lcd_set_window(x,y+yy,LCD_XMAX,LCD_YMAX);
		__lcd_write_start();

		i=0;
		xx =0;

		if(bit_offset!=0)
		{
			aa = bits[i++];
			if(bit_offset==1) goto off_1;
			if(bit_offset==2) goto off_2;
			if(bit_offset==3) goto off_3;
		}

		while(1)
		{
			aa = bits[i++];
//off_0:
			if(xx >= w) break;

			a = AL2_8_Tbl[(aa>>6)&0x3];
			if(a != 0x00)
			{
				if(a==0xFF)
				{
					__lcd_set_window(x+xx,y+yy,LCD_XMAX,LCD_YMAX);
					__lcd_write_start();
					__lcd_write_pixel(color);
				}
				else
				{
					lcd_get_pixel_RGB(pSurf,x+xx,y+yy,&r0,&g0,&b0);

					r0 =__alpha_blend_value(r0,r,a);
					g0 =__alpha_blend_value(g0,g,a);
					b0 =__alpha_blend_value(b0,b,a);

					__lcd_write_start();
					__lcd_write_pixel(SURF_MapRGB(pSurf,r0,g0,b0));
				}
			}
			xx++;

off_1:
			if(xx >= w) break;
			a = AL2_8_Tbl[(aa>>4)&0x3];
			if(a != 0x00)
			{
				if(a==0xFF)
				{
					__lcd_set_window(x+xx,y+yy,LCD_XMAX,LCD_YMAX);
					__lcd_write_start();
					__lcd_write_pixel(color);
				}
				else
				{
					lcd_get_pixel_RGB(pSurf,x+xx,y+yy,&r0,&g0,&b0);

					r0 =__alpha_blend_value(r0,r,a);
					g0 =__alpha_blend_value(g0,g,a);
					b0 =__alpha_blend_value(b0,b,a);

					__lcd_write_start();
					__lcd_write_pixel(SURF_MapRGB(pSurf,r0,g0,b0));
				}
			}
			xx++;

off_2:
			if(xx >= w) break;
			a = AL2_8_Tbl[(aa>>2)&0x3];
			if(a != 0x00)
			{
				if(a==0xFF)
				{
					__lcd_set_window(x+xx,y+yy,LCD_XMAX,LCD_YMAX);
					__lcd_write_start();
					__lcd_write_pixel(color);
				}
				else
				{
					lcd_get_pixel_RGB(pSurf,x+xx,y+yy,&r0,&g0,&b0);

					r0 =__alpha_blend_value(r0,r,a);
					g0 =__alpha_blend_value(g0,g,a);
					b0 =__alpha_blend_value(b0,b,a);

					__lcd_write_start();
					__lcd_write_pixel(SURF_MapRGB(pSurf,r0,g0,b0));
				}
			}
			xx++;

off_3:
			if(xx >= w) break;
			a = AL2_8_Tbl[(aa>>0)&0x3];
			if(a != 0x00)
			{
				if(a==0xFF)
				{
					__lcd_set_window(x+xx,y+yy,LCD_XMAX,LCD_YMAX);
					__lcd_write_start();
					__lcd_write_pixel(color);
				}
				else
				{
					lcd_get_pixel_RGB(pSurf,x+xx,y+yy,&r0,&g0,&b0);

					r0 =__alpha_blend_value(r0,r,a);
					g0 =__alpha_blend_value(g0,g,a);
					b0 =__alpha_blend_value(b0,b,a);

					__lcd_write_start();
					__lcd_write_pixel(SURF_MapRGB(pSurf,r0,g0,b0));
				}
			}
			xx++;

		}

		bits += width_bytes;

	}
}

////////////////////////////
static	const u8 AL4_8_Tbl[16]={0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF};

static void lcd_draw_bitmap_AL4(const SURFACE *pSurf,int x,int y,int w,int h,int width_bytes,const U8 *bits,int bit_offset,COLORREF color)
{
	int i,xx,yy;
	U8 r,g,b,a,aa;
	U8 r0,g0,b0,a0;
	////

	SURF_GetRGB(pSurf,color,&r,&g,&b);

	for(yy=0;yy<h;yy++)
	{
		int s=0;

		__lcd_set_window(x,y+yy,LCD_XMAX,LCD_YMAX);
		__lcd_write_start();

		i=0;
		xx =0;

		if(bit_offset!=0)
		{
			aa = bits[i++];
			goto off_1;
		}

		while(1)
		{
			aa = bits[i++];
//off_0:
			if(xx >= w) break;

			a =AL4_8_Tbl[aa>>4];
			if(a != 0x00)
			{
				if(a==0xFF)
				{
					if(s==0)
					{
					__lcd_set_window(x+xx,y+yy,LCD_XMAX,LCD_YMAX);
					__lcd_write_start();
					}
					__lcd_write_pixel(color);
					s=1;
				}
				else
				{

					lcd_get_pixel_RGB(pSurf,x+xx,y+yy,&r0,&g0,&b0);

					r0 =__alpha_blend_value(r0,r,a);
					g0 =__alpha_blend_value(g0,g,a);
					b0 =__alpha_blend_value(b0,b,a);

					__lcd_write_start();
					__lcd_write_pixel_rgb(r0,g0,b0);
					//__lcd_write_pixel(SURF_MapRGB(pSurf,r0,g0,b0));
					s=0;
				}
			}
			xx++;

off_1:
			if(xx >= w) break;

			a =AL4_8_Tbl[aa&0x0F];
			if(a != 0x00)
			{
				if(a==0xFF)
				{
					if(s==0)
					{
					__lcd_set_window(x+xx,y+yy,LCD_XMAX,LCD_YMAX);
					__lcd_write_start();
					}
					__lcd_write_pixel(color);
					s=1;
				}
				else
				{
					lcd_get_pixel_RGB(pSurf,x+xx,y+yy,&r0,&g0,&b0);

					r0 =__alpha_blend_value(r0,r,a);
					g0 =__alpha_blend_value(g0,g,a);
					b0 =__alpha_blend_value(b0,b,a);

					__lcd_write_start();
					__lcd_write_pixel_rgb(r0,g0,b0);
					//__lcd_write_pixel(SURF_MapRGB(pSurf,r0,g0,b0));
					s=0;
				}
			}
			xx++;

		}

		bits += width_bytes;

	}
}


static void lcd_draw_bitmap_AL8(const SURFACE *pSurf,int x,int y,int w,int h,int width_bytes,const U8 *bits,COLORREF color)
{
	int xx,yy;
	U8 r,g,b,a;
	U8 r0,g0,b0;
	////

	pSurf->CC->GetRGB(color,&r,&g,&b);

	for(yy=0;yy<h;yy++)
	{
		__lcd_set_window(x,y+yy,LCD_XMAX,LCD_YMAX);
		__lcd_write_start();

		for(xx=0;xx<w;xx++)
		{
			a = bits[xx];
			if(a != 0x00)
			{
				if(a==0xFF)
				{
					__lcd_set_window(x+xx,y+yy,LCD_XMAX,LCD_YMAX);
					__lcd_write_start();
					__lcd_write_pixel(color);
				}
				else
				{
					lcd_get_pixel_RGB(pSurf,x+xx,y+yy,&r0,&g0,&b0);

					r0 =__alpha_blend_value(r0,r,a);
					g0 =__alpha_blend_value(g0,g,a);
					b0 =__alpha_blend_value(b0,b,a);

					__lcd_write_start();
					__lcd_write_pixel(SURF_MapRGB(pSurf,r0,g0,b0));
				}
			}

		}

		bits += width_bytes;

	}

}


BOOL 	lcd_scale_bitmap(const SURFACE *pSurf,int x,int y,int w,int h,const BITMAP *bm)
{
	int	xx,yy,x_inc,y_inc,i;
	U32 src_x,src_y;
	U16 color;
	////

	src_x =0;
	src_y =0;

	//调整x,y缩放系数
	x_inc = (bm->Width<<16)  / w;
	y_inc = (bm->Height<<16) / h;

	src_x = src_x<<16;
	src_y = src_y<<16;

	switch(bm->Format)
	{
		case	BM_RGB565:
		{
			U16 *src_p;

			for(yy=0; yy<h; yy++)
			{
				__lcd_set_window(x,y+yy,LCD_XMAX,LCD_YMAX);
				__lcd_write_start();

				src_p = (U16*)((U8*)bm->Bits + (src_x>>16)*2 + (src_y>>16)*bm->WidthBytes);

				i = src_x;
				for(xx=0; xx<w; xx++)
				{
					color = pSurf->CC->MapRGB565(src_p[i>>16]);
					__lcd_write_pixel(color);

					i += x_inc;
				}

				src_y += y_inc;
			}
		}
		return TRUE;
		////

		case	BM_XRGB8888:
		{
			U32 *src_p;

			for(yy=0; yy<h; yy++)
			{
				__lcd_set_window(x,y+yy,LCD_XMAX,LCD_YMAX);
				__lcd_write_start();

				src_p = (U32*)((U8*)bm->Bits + (src_x>>16)*4 + (src_y>>16)*bm->WidthBytes);

				i = src_x;
				for(xx=0; xx<w; xx++)
				{
					color = pSurf->CC->MapXRGB8888(src_p[i>>16]);
					__lcd_write_pixel(color);

					i += x_inc;
				}

				src_y += y_inc;
			}
		}
		return TRUE;
		////

		default:
			return FALSE;

	}

}

int	lcd_rotate_bitmap(const SURFACE *pSurf,int cx,int cy,const BITMAP *bm,int angle)
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

	cos_a	=(float)cos((2*3.1416*angle)/360)*65536.0;
	sin_a	=(float)sin((2*3.1416*angle)/360)*65536.0;

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
				lcd_draw_bitmap_RGB565(pSurf,xx0,yy0,1,1,bm->WidthBytes,bits);
				break;
			case BM_ARGB4444:
				bits += (xx1<<1);
				lcd_draw_bitmap_ARGB4444(pSurf,xx0,yy0,1,1,bm->WidthBytes,bits);
				break;
			case BM_ARGB8888:
				bits += (xx1<<2);
				lcd_draw_bitmap_ARGB8888(pSurf,xx0,yy0,1,1,bm->WidthBytes,bits);
				break;
			default:
				return FALSE;

			}

		}


	}

	return	TRUE;
}

void GL_MPU_16BPP_Init(GL_OP *gd)
{
	gd->SetPos			=lcd_set_pos;
	gd->SetPixel		=lcd_set_pixel;
	gd->GetPixel		=lcd_get_pixel;
	gd->XorPixel		=lcd_xor_pixel;

	gd->HLine			=lcd_draw_hline;
	gd->VLine			=lcd_draw_vline;
	gd->FillArea		=lcd_fill_rect;
	gd->CopyBits		=lcd_copy_bits;
	gd->DrawBitmap_LUT1	=lcd_draw_bitmap_LUT_1;
	gd->DrawBitmap_LUT2	=lcd_draw_bitmap_LUT_2;
	gd->DrawBitmap_LUT4	=lcd_draw_bitmap_LUT_4;
	gd->DrawBitmap_LUT8	=lcd_draw_bitmap_LUT_8;
	gd->DrawBitmap_RGB332	=lcd_draw_bitmap_RGB332;
	gd->DrawBitmap_RGB565	=lcd_draw_bitmap_RGB565;
	gd->DrawBitmap_XRGB1555	=lcd_draw_bitmap_XRGB1555;
	gd->DrawBitmap_ARGB1555	=lcd_draw_bitmap_ARGB1555;
	gd->DrawBitmap_ARGB4444	=lcd_draw_bitmap_ARGB4444;
	gd->DrawBitmap_RGB888	=lcd_draw_bitmap_RGB888;
	gd->DrawBitmap_XRGB8888	=lcd_draw_bitmap_XRGB8888;
	gd->DrawBitmap_ARGB8888	=lcd_draw_bitmap_ARGB8888;
	gd->DrawBitmap_AL1	=lcd_draw_bitmap_AL1;
	gd->DrawBitmap_AL2	=lcd_draw_bitmap_AL2;
	gd->DrawBitmap_AL4	=lcd_draw_bitmap_AL4;
	gd->DrawBitmap_AL8	=lcd_draw_bitmap_AL8;
	gd->ScaleBitmap			=lcd_scale_bitmap;
	gd->RotateBitmap        =lcd_rotate_bitmap;
}

#endif	/*MPU_LCD_TYPE*/

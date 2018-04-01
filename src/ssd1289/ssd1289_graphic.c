#include <math.h>

#include "ssd1289.h"

/* image functions */
int ssd1289_print_image(const tImage *image, uint16_t x_off, uint16_t y_off) {
	int x, y;
	uint32_t datacnt;
	uint16_t data;

	datacnt = 0;
	for(y = 0; y < image->height; y++) {
		for(x = 0; x < image->width; x++) {
			data = image->data[datacnt];
			ssd1289_setpx(x+x_off, y+y_off, data);
			datacnt++;
		}
	}

	return 0;
}


void ssd1289_fill_screen(uint16_t color) {
	int i;
	
	ssd1289_set_cursor(0,0);
	for(i = 0; i < 76800; i++) LCD_RAM = color;
	
}

uint16_t ssd1289_getpx(uint16_t x, uint16_t y) {
	ssd1289_set_cursor(x, y);

	return LCD_RAM;
}

void ssd1289_setpx(uint16_t x, uint16_t y, uint16_t color) {
	if((x > 239) || (y > 319)) return;

	ssd1289_set_cursor(x,y);
	LCD_RAM = color;

	return;
}

/* fucking slow */
void ssd1289_invert_screen() {
	uint16_t color;
	int x,y;
	
	for(y = 0; y < 319; y++) {
		for(x = 0; x < 239; x++) {
			color = ssd1289_getpx(x,y);
			color ^= 0xffff;
			ssd1289_setpx(x, y, color);
		}
	}
}

void ssd1289_invert_area(int x1, int y1, int x2, int y2) {
	uint16_t color;
	int x,y;
	
	for(y = y1; y < y2; y++) {
		for(x = x1; x < x2; x++) {
			color = ssd1289_getpx(x,y);
			color ^= 0xffff;
			ssd1289_setpx(x, y, color);
		}
	}	
}

void ssd1289_lineh(int x, int y, int len, uint16_t color) {
	int i;
	
	for(i = 0; i < len; i++) {
		ssd1289_setpx(x+i, y, color);
	}
	
	return;
}

void ssd1289_linev(int x, int y, int len, uint16_t color) {
	int i;
	
	for(i = 0; i < len; i++) {
		ssd1289_setpx(x, y+i, color);
	}
	
	return;
}

void ssd1289_rect(int x1, int y1, int x2, int y2, uint16_t color) {
	ssd1289_lineh(x1, y1, x2-x1, color);	// top line
	ssd1289_lineh(x1, y2, x2-x1+1, color);	// bottom line
	ssd1289_linev(x1, y1, y2-y1, color);	// left line
	ssd1289_linev(x2, y1, y2-y1, color);	// right line	
	return;
}

void ssd1289_fill_rect(int x1, int y1, int x2, int y2, uint16_t color) {
	int lines = y2-y1;
	
	while(lines >= 0) {
		ssd1289_lineh(x1, y2-lines, x2-x1, color);
		lines--;
	}
	
	return;	
}

void ssd1289_line(int x1, int y1, int x2, int y2, uint16_t color) {
	int x, y, dx, dy, Dx, Dy, e, i;

	// lineh and linev are faster than this more complicated line function
	if(y1 == y2)	// horizontal
	{
		ssd1289_lineh(x1, y1, x2-x1, color);
		return;
	}
	
	if(x1 == x2)	// vertical
	{
		ssd1289_linev(x1, y1, y2-y1, color);
		return;
	}
	
	Dx = x2-x1;
	Dy = y2-y1;

	dx = fabs(x2-x1);
	dy = fabs(y2-y1);

	x = x1;
	y = y1;

	if(dy > dx) {
		e = -dy;
		for(i = 0; i < dy; i++) {
			ssd1289_setpx(x, y, color);
			if(Dy >= 0) y++;
			else y--;
			e += 2*dx;
			if(e >= 0) {
				if(Dx >= 0) x++;
				else x--;
				e -= 2*dy;
			}
		}
	}
	else
	{
		e = -dx;
		for(i = 0; i < dx; i++) {
			ssd1289_setpx(x, y, color);
			if(Dx >= 0) x++;
			else x--;
			e += 2*dy;
			if(e >= 0) {
				if(Dy >= 0) y++;
				else y--;
				e -= 2*dx;
			}
		}
	}
}


void ssd1289_circle(int x, int y, int radius, uint16_t color) { 
	int xc = 0; 
	int yc;
	int p;
	yc=radius;
	p = 3 - (radius<<1);
	
	while (xc <= yc){ 
		
		ssd1289_setpx(x + xc, y + yc, color); 
		ssd1289_setpx(x + xc, y - yc, color); 
		ssd1289_setpx(x - xc, y + yc, color); 
		ssd1289_setpx(x - xc, y - yc, color); 
		ssd1289_setpx(x + yc, y + xc, color); 
		ssd1289_setpx(x + yc, y - xc, color); 
		ssd1289_setpx(x - yc, y + xc, color); 
		ssd1289_setpx(x - yc, y - xc, color); 
		if (p < 0) {
			p += (xc++ << 2) + 6; 
		} else {
			p += ((xc++ - yc--)<<2) + 10; 
		} 
	}
	
	return;
} 

void lcd_hline(unsigned char x1, unsigned char x2, unsigned char y, uint16_t color) {
//	if (x1>lcd_pixelx) x1=lcd_pixelx;
//	if (x2>lcd_pixelx) x2=lcd_pixelx;
//	if (y<lcd_pixely) {
		for (unsigned char i=x1; i<=x2; i++) {
			//lcd_putpixel(i,y);
			ssd1289_setpx(i, y, color);
		}
//	}
}

/*************************************************************************
 * Draw an ellipse
 * Color & fill mode supported
 *  lcd_color = (1,0,-1)
 *  lcd_fill  = (0/1)
 *
 * see: "glcd.c" for Nokia 6100 by Hagen Reddmann
 *
 * enhanced version by oog:
 *  print lines just once to avoid XOR-draw mode hazard
*/

void lcd_ellipse(unsigned char x, unsigned char y, unsigned char rx, unsigned char ry, uint16_t color) {

	if ((rx == 0) | (ry == 0)) {return;}

	int16_t aa = rx * rx;
	int16_t bb = ry * ry;
	int32_t er, cr, ir;
	int16_t ys,ye,xs,xe;

	// remember last y coordinates
	// avoid double print of lines
	// (no blank lines in XOR mode)
	int16_t ys0=0, ye0=0;
	int16_t ysl0=0, yel0=0;

	cr = bb >> 1;
	cr = cr * (rx + rx -1);
	ir = aa >> 1;
	ir = -ir;
	er = 0;
	xs = x;
	xs = xs - rx;
	xe = x;
	xe = xe + rx;
	ys = y;
	ye = y;
	
	
	while (cr >= ir)
	{
		if (ys!=ys0) lcd_hline(xs, xe, ys, color);
		ys0=ys;
		if (ys != ye)
		{
			if (ye!=ye0) lcd_hline(xs, xe, ye, color);
			ye0=ye;
		}
		ys--;
		ye++;
		ir += aa;
		er += ir;
		if (2 * er > cr)
		{
			er -= cr;
			cr -= bb;
			xs++;
			xe--;
		}
	}
	
		ysl0=ys0;
	yel0=ye0;
	
	cr = aa >> 1;
	cr = cr * (ry + ry -1);
	ir = bb >> 1;
	ir = -ir;
	er = 0;

	xs = x;
	xe = x;
	ys = y;
	ys = ys - ry;
	ye = y;
	ye = ye + ry;
	
	while (ir <= cr)
	{
		if (((ys!=ys0) && (ys!=ysl0))) lcd_hline(xs, xe, ys, color);
		ys0=ys;
		if (ys != ye)
		{
			if (((ye!=ye0) && (ye!=yel0))) lcd_hline(xs, xe, ye, color);
			ye0=ye;
		}
		ir += bb;
		er += ir;
		if (2 * er > cr)
		{
			er -= cr;
			cr -= aa;
			ys++;
			ye--;
		}
		xs--;
		xe++;
		}
}


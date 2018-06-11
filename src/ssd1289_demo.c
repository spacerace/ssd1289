#include "leds.h"
#include "buttons.h"
#include "freertos/include/FreeRTOS.h"
#include "freertos/include/task.h"
#include "random.h"						// small, pseudo random
#include "ssd1289.h"

#include "image_amigawbfloppy.h"		// amiga workbench floppy picture
#include "image_windows101.h"			// windows 1.01 bootsplash
#include "image_windows31.h"			// windows 3.1 bootsplash
#include "image_monkeyisland.h"			// scene from monkey island

void demo_screen0();		// main demo screen
void demo_screen1();		// text console demo
void demo_screen2();		// images demo
void demo_screen3();		// ellipse demo
void demo_blacklight0();	// backlight flash/fade demo

void vT_display_demo(void *p) {	
	demo_screen3();
	for(;;) {
		asm("nop");
	}
}
	 
void demo_screen4() {
		int r0, r1, x, y;
		uint16_t color;
		
		ssd1289_clear();
		ssd1289_set_text_cursor(0,0);
		ssd1289_set_font_color(RGB_COL_YELLOW, RGB_COL_BLACK);
		ssd1289_puts("test");
		
		while(btna_hit()) {
			asm("nop");
		}
		
		ssd1289_puts("text");
	
		for(;;) {
			asm("nop");
		}
}

void demo_screen3() {
	int r0, r1, x, y;
	uint16_t color;
	while(1) {
		x = (random_kiss32()%240);
		y = (random_kiss32()%320);
		r0 = (random_kiss32()%80);
		r1 = (random_kiss32()%80);
		color = (uint16_t)(random_kiss32()&0xfffff);
		ssd1289_ellipse(x, y, r0, r1, color);
	}
	return;
}	

void demo_screen0() {
	int xpos, ypos, rad, x, x2, y, y2, i;
	uint16_t color;
	
	ssd1289_clear();
	ssd1289_set_text_cursor(0, 0);
	ssd1289_set_font_color(RGB_COL_CYAN, RGB_COL_BLACK);
	ssd1289_puts(" >  SSD1289 DRIVER DEMO  <");
	
	ssd1289_rect(5, 20, 239-5, 319-5, RGB_COL_CYAN);				// big frame
	ssd1289_rect(10, 25, 60, 75, RGB_COL_CYAN);						// random pixel frame
	ssd1289_rect(65, 25, 239-10, 75, RGB_COL_WHITE);				// random lines frame
	ssd1289_rect(10, 80, 90, 130, RGB_COL_RED);						// fill rect frame
	ssd1289_rect(95, 80, 239-10, 130, RGB_COL_YELLOW);				// rect frame
	
	ssd1289_rect(120, 190, 239-10, 319-10, RGB_COL_GREEN);			// image frame
	ssd1289_fill_rect(10, 190, 129-10-5, 319-20-60, RGB_COL_RED);	// text "press btn a" frame
	ssd1289_rect(10, 320-80+5, 114, 309, RGB_COL_YELLOW);			// circle frame
	
	ssd1289_rect(10, 135, 239-10, 185, RGB_COL_MAGENTA);			// text demo
	
	ssd1289_print_image(&amiga_wb_floppy, 130, 200);
	
	// put 1000 random pixels
	//xDelay = 4 / portTICK_RATE_MS;
	for(i = 0; i < 700; i++) {
			x = (random_xorshift32()%49)+11;
	 		y = (random_xorshift32()%49)+26;
			color = random_xorshift32();
			ssd1289_setpx(x, y, color);
			//vTaskDelay(xDelay);
	}
	
	//xDelay = 5 / portTICK_RATE_MS;
	for(i = 0; i < 80; i++) {
		x = (random_xorshift32()%165)+66;
		y = (random_xorshift32()%49)+26;
		x2 = (random_xorshift32()%165)+66;
		y2 = (random_xorshift32()%49)+26;
		color = random_xorshift32();
		
		ssd1289_line(x, y, x2, y2, color);
		//vTaskDelay(xDelay);
	}
	
	for(i = 0; i < 400; i++) {
		x = (random_xorshift32()%80)+11;
		y = (random_xorshift32()%49)+81;
		x2 = (random_xorshift32()%80)+11;
		y2 = (random_xorshift32()%49)+81;
		color = random_xorshift32();
		
		ssd1289_fill_rect(x, y, x2, y2, color);
		//vTaskDelay(xDelay);
	}
	
	for(i = 0; i < 400; i++) {
		x = (random_xorshift32()%130)+96;
		y = (random_xorshift32()%49)+81;
		x2 = (random_xorshift32()%130)+96;
		y2 = (random_xorshift32()%49)+81;
		color = random_xorshift32();
		
		ssd1289_rect(x, y, x2, y2, color);
		//vTaskDelay(xDelay);
	}
	
	// text demo
	
	ssd1289_set_font(FONT_LINUX_8x16);
	ssd1289_set_font_color(RGB_COL_WHITE, RGB_COL_BLACK);
	ssd1289_puts_at(15, 139, "Linux-Fonts: Std 8x16");

	ssd1289_set_font(FONT_LINUX_8x8);
	ssd1289_set_font_color(RGB_COL_WHITE, RGB_COL_BLACK);
	ssd1289_puts_at(15, 156, "Small 8x8 Font");


	ssd1289_set_font(FONT_LINUX_ACORN_8x8);
	ssd1289_set_font_color(RGB_COL_WHITE, RGB_COL_BLACK);
	ssd1289_puts_at(15, 165, "Small 8x8 Font \"Acorn\"");

	
	ssd1289_set_font(FONT_LINUX_PEARL_8x8);
	ssd1289_set_font_color(RGB_COL_WHITE, RGB_COL_BLACK);
	ssd1289_puts_at(15, 174, "Small 8x8 Font \"Pearl\"");
	
	ssd1289_set_font(FONT_LINUX_ACORN_8x8);
	ssd1289_set_font_color(RGB_COL_WHITE, RGB_COL_RED);
	ssd1289_puts_at(15, 200, "Press btn A!");
	ssd1289_puts_at(15, 210, "Press btn A!");
	ssd1289_puts_at(15, 220, "Press btn A!");
	
	xpos = 42;
	ypos = 278;
	for(rad = 1; rad <= 30; rad++) {
		ssd1289_circle(xpos, ypos, rad, random_kiss32());
		ssd1289_circle(xpos+41, ypos, rad, random_kiss32());
	}
	
}

void demo_screen1() {
	int l;
	int fg, bg;
	
	ssd1289_fill_screen(RGB_COL_BLUE);
	ssd1289_set_font(FONT_LINUX_ACORN_8x8);
	ssd1289_set_font_color(RGB_COL_WHITE, RGB_COL_BLUE);
	ssd1289_set_cursor(0, 0);
	ssd1289_set_cursor(0,0);
	ssd1289_puts("\rSimple text-console with automatic line-wrap.\n\nCharset:\n");
	
	ssd1289_set_cursor(0, 5);
	for(l = 0x20; l < 0xff; l++) {
		//fg = random_kiss32();
		//bg = random_kiss32();
		//ssd1289_set_font_color(fg, bg);
		ssd1289_putc(l);
	}
	
	ssd1289_inc_cursor_y();
	ssd1289_inc_cursor_y();
	
	ssd1289_puts("30x35 chars on 240x320");

	ssd1289_inc_cursor_y();
	ssd1289_inc_cursor_y();
	
	for(l = 0x20; l < 0xff; l++) {
		fg = random_kiss32();
		bg = random_kiss32();
		ssd1289_set_font_color(fg, bg);
		ssd1289_putc(l);
		ssd1289_putc(l);
	}
	for(l = 0x20; l < 0xff; l++) {
		fg = random_kiss32();
		bg = random_kiss32();
		ssd1289_set_font_color(fg, bg);
		ssd1289_putc(l);
	}
	return;
}

void demo_blacklight0() {
	int bl;
	int loops;
	for(loops = 0; loops < 4; loops++) {
		for(bl = 0; bl <= 100; bl++) {
			ssd1289_bl_set(bl);
			vTaskDelay(3);
		}
		for(bl = 100; bl > 0; bl--) {
			ssd1289_bl_set(bl);
			vTaskDelay(3);
		}
	}

	for(loops = 0; loops < 20; loops++) {
		ssd1289_bl_set(100);
		vTaskDelay(50);
		ssd1289_bl_set(0);
		vTaskDelay(50);
	}
	ssd1289_bl_set(100);
	
	return;
}

void demo_screen2() {
	//ssd1289_print_image(&win31, 0, 0);
	//ssd1289_print_image(&mi, 0, 0);
	return;
}	



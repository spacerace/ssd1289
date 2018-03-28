/*
 * SSD1289 driver example
 * Nils Stec <stecdose@gmail.com>
 * 2017-07-01
 * 
 * driver was written to be used with or without an os.
 * this is the freertos example on how to use it.
 * 
 * the whole thing was written using a board called
 * "HY-MiniSTM32V" with an LCD called "HY32D".
 * it has:
 *   - stm32f103vct6, 256k flash, 64k ram
 *   - display connection 8080 bus to fsmc
 * 
 * there are a few tasks running:
 * 		vT_display_demo()	- all you can see on screen when demo is running
 *      vT_usart()			- a simple console running on USART1, interfacing all graphics+demo-functions
 *      vT_led()			- blinks a led and checks buttonX
 * there is also an ISR installed for buttonY it does ____________________
 * 
 *  
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "stm32f10x.h"
#include "usart.h"
#include "leds.h"
#include "buttons.h"
#include "freertos/include/FreeRTOS.h"
#include "freertos/include/task.h"
#include "printf.h"						// very small printf-implementation. see printf.c
#include "random.h"						// small, pseudo random
#include "ssd1289.h"

#include "image_amigawbfloppy.h"		// amiga workbench floppy picture

void vT_display_demo(void *p);
void vT_usart(void *p);
void vT_led(void *p);

void vT_display_demo(void *p) {
    portTickType xDelay = 5 / portTICK_RATE_MS;
	
	int i;
	uint16_t x, x2, y, y2, color;
	
	ssd1289_set_text_cursor(0, 0);
	ssd1289_set_font_color(RGB_COL_CYAN, RGB_COL_BLACK);
	ssd1289_puts(" >  SSD1289 DRIVER DEMO  <");
	
	ssd1289_rect(5, 20, 239-5, 319-5, RGB_COL_CYAN);	// big frame
	ssd1289_rect(10, 25, 60, 75, RGB_COL_CYAN);		// random pixel frame
	ssd1289_rect(65, 25, 239-10, 75, RGB_COL_WHITE);		// random lines frame
	ssd1289_rect(10, 80, 90, 130, RGB_COL_RED);		// fill rect frame
	ssd1289_rect(95, 80, 239-10, 130, RGB_COL_YELLOW);	// rect frame
	
	ssd1289_rect(120, 190, 239-10, 319-10, RGB_COL_GREEN);	// image frame
	
	ssd1289_rect(10, 135, 239-10, 185, RGB_COL_MAGENTA);	// text demo
	
	ssd1289_print_image(&amiga_wb_floppy, 130, 200);
	
	// put 1000 random pixels
	xDelay = 4 / portTICK_RATE_MS;
	for(i = 0; i < 700; i++) {
			x = (random_xorshift32()%49)+11;
			y = (random_xorshift32()%49)+26;
			color = random_xorshift32();
			ssd1289_setpx(x, y, color);
		//	vTaskDelay(xDelay);
	}
	
	xDelay = 5 / portTICK_RATE_MS;
	for(i = 0; i < 80; i++) {
		x = (random_xorshift32()%165)+66;
		y = (random_xorshift32()%49)+26;
		x2 = (random_xorshift32()%165)+66;
		y2 = (random_xorshift32()%49)+26;
		color = random_xorshift32();
		
		ssd1289_line(x, y, x2, y2, color);
	//	vTaskDelay(xDelay);
	}
	
	for(i = 0; i < 400; i++) {
		x = (random_xorshift32()%80)+11;
		y = (random_xorshift32()%49)+81;
		x2 = (random_xorshift32()%80)+11;
		y2 = (random_xorshift32()%49)+81;
		color = random_xorshift32();
		
		ssd1289_fill_rect(x, y, x2, y2, color);
	//	vTaskDelay(xDelay);
	}
	
	for(i = 0; i < 400; i++) {
		x = (random_xorshift32()%130)+96;
		y = (random_xorshift32()%49)+81;
		x2 = (random_xorshift32()%130)+96;
		y2 = (random_xorshift32()%49)+81;
		color = random_xorshift32();
		
		ssd1289_rect(x, y, x2, y2, color);
	//	vTaskDelay(xDelay);
	}
	
	// text demo
	
	ssd1289_set_font(FONT_LINUX_SUN_8x16);
	ssd1289_set_font_color(RGB_COL_WHITE, RGB_COL_BLACK);
	ssd1289_puts_at(15, 140, "Linux' Sun 8x16 Font");

	ssd1289_set_font(FONT_LINUX_8x16);
	ssd1289_set_font_color(RGB_COL_WHITE, RGB_COL_BLACK);
	ssd1289_puts_at(15, 156, "Linux' Std 8x16 Font");

	ssd1289_set_font(FONT_LINUX_ACORN_8x8);
	ssd1289_set_font_color(RGB_COL_WHITE, RGB_COL_BLACK);
	ssd1289_puts_at(15, 174, "Linux' Acorn 8x8 Font");

	ssd1289_set_font(FONT_LINUX_8x8);
	ssd1289_set_font_color(RGB_COL_WHITE, RGB_COL_BLACK);
	ssd1289_puts_at(12, 195, "LnxStd 8x8Fnt");
	
	ssd1289_set_font(FONT_LINUX_PEARL_8x8);
	ssd1289_set_font_color(RGB_COL_WHITE, RGB_COL_BLACK);
	ssd1289_puts_at(12, 205, "Pearl 8x8Font");
	
	ssd1289_set_font(FONT_LINUX_MAC_6x11);
	ssd1289_set_font_color(RGB_COL_WHITE, RGB_COL_BLACK);
	ssd1289_puts_at(20, 225, "Mac 6x11 Font");
	
	
	
	
	
	for(;;) {
			
	}
}

/* run a simple console on USART1 */
void vT_usart(void *p) {
	usart1_puts("<console> $ ");
	for(;;) {
		usart1_putc(usart1_getc());	
	}
}

/* blinks onboard led */
void vT_led(void *p) {
	const portTickType xDelay_long = 400 / portTICK_RATE_MS;
	const portTickType xDelay_short = 30 / portTICK_RATE_MS;
	
	LED1_OFF();
	LED2_OFF();
	for(;;) {
		LED1_ON();
		vTaskDelay(xDelay_short);
		LED1_OFF();
		vTaskDelay(xDelay_long);
		vTaskDelay(xDelay_long);
		vTaskDelay(xDelay_long);
	}
}

int main(void){
	SystemInit();
	usart1_init();
	init_leds();
	ssd1289_init();
	ssd1289_fill(RGB_COL_BLACK);
	ssd1289_textcon_init();
	ssd1289_set_font_color(RGB_COL_YELLOW, RGB_COL_BLACK);
	ssd1289_set_font(FONT_LINUX_SUN_8x16);

	init_buttons();
    
	xTaskCreate(vT_usart,   	 (const char*) "USART Task", 128, NULL, 1, NULL);
	xTaskCreate(vT_led,     	 (const char*) "LED Task", 128, NULL, 1, NULL);
	xTaskCreate(vT_display_demo, (const char*) "SSD1289_DEMO", 128, NULL, 1, NULL);

	// Start RTOS scheduler

	vTaskStartScheduler();


	for(;;) {
		
	}
		
	return 0;
}

void vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName) {
// 	printf("ERROR: vApplicationStackOverflowHook(): Task \"%s\" overflowed its stack\n", pcTaskName);
// 	fflush(stdout);
// 	assert(false);
}


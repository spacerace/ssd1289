/*
 * SSD1289 driver example
 * Nils Stec <stecdose@gmail.com>
 * 2018-03-29
 * 
 * This driver was written to be used with or without an OS.
 * This is the FreeRTOS example on how to use it.
 * 
 * The whole thing was written using a board called
 * "HY-MiniSTM32V" with an LCD called "HY32D".
 * It's specs:
 *   - STM32F103VCT6, 256k flash, 64k RAM
 *   - display connection 8080 bus to FSMC
 * 
 * There are a three tasks running:
 * 		vT_display_demo()	- all you can see on screen when demo is running
 *      vT_shell()			- a simple console running on USART1, 
 * 							  interfacing all graphics+demo-functions
 *      vT_led()			- blinks a led and checks buttonX
 * 
 * There is also an ISR installed for a button connected to PC13.
 * It's purpose is to cycle through the demo screens.
 *  
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "stm32f10x.h"
#include "usart.h"
#include "leds.h"
#include "buttons.h"
#include "encoder.h"
#include "freertos/include/FreeRTOS.h"
#include "freertos/include/task.h"
#include "printf.h"						// very small printf-implementation. see printf.c
#include "random.h"						// small, pseudo random
#include "ssd1289.h"
#include "shell.h"

extern void vT_display_demo(void *p);	// see ssd1289_demo.c
extern void vT_led(void *p);			// see leds.c
void vT_shell(void *p);					// calls console_main() from shell.c
void vT_encoder(void *p);

/* run a simple console on USART1 */
void vT_shell(void *p) {
	init_console();
	for(;;) {
		console_main();		
	}
}

void vT_encoder(void *p) {
	ssd1289_set_font(FONT_LINUX_8x16);
	ssd1289_set_font_color(RGB_COL_YELLOW, RGB_COL_BLACK);
	ssd1289_set_text_cursor(0,0);
	
	for(;;) {
		
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
	ssd1289_set_font(FONT_LINUX_8x16);

	init_buttons();
	encoder_init();	
	init_printf(PRINTF_OUTPUT_SSD1289 | PRINTF_OUTPUT_USART1);
	    
	xTaskCreate(vT_shell,   	 (const char*) "Shell Task", 256, NULL, 1, NULL);
	xTaskCreate(vT_led,     	 (const char*) "LED Task", 48, NULL, 1, NULL);
	xTaskCreate(vT_display_demo, (const char*) "SSD1289_DEMO", 256, NULL, 1, NULL);
	xTaskCreate(vT_encoder,		 (const char*) "Encoder Task", 32, NULL, 1, NULL);
	// Start RTOS scheduler

	vTaskStartScheduler();


	for(;;) {
		
	}
		
	return 0;
}

void vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName) {
	usart1_puts("ERROR: vApplicationStackOverflowHook(): Task ");
	usart1_puts(pcTaskName);
	usart1_puts(" overflowed its stack.\r\n");
	//assert(false);
}


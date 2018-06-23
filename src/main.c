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
#include "random.h"						// small, pseudo random
#include "ssd1289.h"
#include "shell.h"
#include "ssd1289_dotmatrix.h"
#include "ssd1289_7seg.h"
#include "timer.h"
#include "core_cm3.h"
#include "image_monkeyisland.h"

extern void vT_display_demo(void *p);	// see ssd1289_demo.c
extern void vT_led(void *p);			// see leds.c
void vT_shell(void *p);					// calls console_main() from shell.c
void vT_encoder(void *p);
int segment_test(void);
void systick_delay(int ticks);
uint32_t millis();
volatile uint32_t systick_counter;
void dwt_enable();
void dwt_enable_cycle_cnt();
void dwt_disable_cycle_cnt();
uint32_t dwt_get_cycle_cnt();
void dwt_reset_cycle_cnt();

volatile uint32_t *DWT_CONTROL = (uint32_t *)0xE0001000;
volatile uint32_t *DWT_CYCCNT = (uint32_t *)0xE0001004;
volatile uint32_t *DEMCR = (uint32_t *)0xE000EDFC;

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
    char str[32];
    uint32_t start, end, ms;
    FRESULT r;

 	SystemInit();
	usart1_init();
	init_leds();
	ssd1289_init();
	ssd1289_fill(RGB_COL_BLACK);
	ssd1289_textcon_init();
	ssd1289_set_font_color(RGB_COL_YELLOW, RGB_COL_BLACK);
	ssd1289_set_font(FONT_LINUX_ACORN_8x8);

	init_buttons();
	encoder_init();

    LED1_OFF();
    LED2_OFF();

	systick_counter = 0;

    dwt_enable();
	/* 1000 interrupts per second = 1khz */
 	SysTick_Config(SystemCoreClock/1000);


// 	int i,color;
// 	color = LCD_COLOR(0, 0, 255);
// 	for(i = 0; i < 5; i++) {
// 		ssd1289_dotmatrix_digit(i*42, 10, 13, LCD_COLOR(0x1a, 0x2a, 0x3a));
// 		ssd1289_dotmatrix_digit(i*42, 10, i, color<<=3);
// 	}

    //ssd1289_print_image(&mi, 0, 0);

    dwt_reset_cycle_cnt();
    dwt_enable_cycle_cnt();
    start = dwt_get_cycle_cnt();

    printf("Hello World\r\n");

    end = dwt_get_cycle_cnt();
    ms = end-start;

    uint32_t time_used = ms*78;
    time_used /= 10;

    sprintf(str, "%lunS", time_used);

// 	xTaskCreate(vT_shell,   	 (const char*) "Shell Task", 256, NULL, 1, NULL);
// 	xTaskCreate(vT_led,     	 (const char*) "LED Task", 48, NULL, 1, NULL);
// 	xTaskCreate(vT_display_demo, (const char*) "SSD1289_DEMO", 256, NULL, 1, NULL);
// 	xTaskCreate(vT_encoder,		 (const char*) "Encoder Task", 32, NULL, 1, NULL);
	// Start RTOS scheduler

// 	vTaskStartScheduler();


	for(;;) {

	}

	return 0;
}


void dwt_enable() {
    *DEMCR = *DEMCR | 0x01000000;
}

void dwt_enable_cycle_cnt() {
	*DWT_CONTROL = 0x40000001;
}

void dwt_disable_cycle_cnt() {
    *DWT_CONTROL = 0x40000000;
}

uint32_t dwt_get_cycle_cnt() {
    return *DWT_CYCCNT;
}

void dwt_reset_cycle_cnt() {
    *DWT_CYCCNT = 0;
}


int segment_test(void) {
	int x, y, i;
	y = 80;
	i = 0x08;
	for(x = 10; x < 215; x+=27) {
		draw_7segment(x, y, 0xff, LCD_COLOR(0x10, 0, 0));
		segment_putn(x, y, i, LCD_COLOR(0xff, 0, 0));
		i++;
	}


	return 0;
}

void textcon_test(void) {

	ssd1289_puts("abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmopq\n1\n2\n3\n4\n5\n6\n4\n7\n8\n9\na\nb\nc\ndddd9\n4\n5\n6\n4\n7\n8\n9\na\nb\nc\ndddd9");

	ssd1289_set_font_color(RGB_COL_WHITE, RGB_COL_RED);
	ssd1289_set_text_cursor(10, 10);
	ssd1289_puts("39x26 text console");
	ssd1289_set_text_cursor(14, 11);
	ssd1289_puts("8x8 Linux Acorn Font");

	ssd1289_set_font_color(RGB_COL_WHITE, RGB_COL_BLACK);
	ssd1289_set_text_cursor(10, 13);
	ssd1289_puts("39x26 text console");
	ssd1289_set_text_cursor(14, 14);
	ssd1289_puts("8x8 Linux Acorn Font");

	ssd1289_set_font_color(RGB_COL_CYAN, RGB_COL_BLACK);
	ssd1289_set_text_cursor(10, 16);
	ssd1289_puts("39x26 text console");
	ssd1289_set_text_cursor(14, 17);
	ssd1289_puts("8x8 Linux Acorn Font");
}


void SysTick_Handler() {
	systick_counter++;

	static int led1 = 0;

	if((systick_counter % 100)==0) {	// every 100mS
		led1 = !led1;
		if(led1) LED1_ON();
		else	 LED1_OFF();
	}
}

uint32_t millis() {
    return systick_counter;
}

/*
 * parameter says how many ticks to wait. With a 1khz interrupt
 * one tick equals 1mS.
 *
 */
void systick_delay(int ticks) {
	uint32_t ticks_start = systick_counter;

	while(systick_counter < (ticks_start + ticks));

	return;
}


void vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName) {
	usart1_puts("ERROR: vApplicationStackOverflowHook(): Task ");
 	usart1_puts((char *)pcTaskName);
	usart1_puts(" overflowed its stack.\r\n");
	//assert(false);
}


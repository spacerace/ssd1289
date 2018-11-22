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

void systick_delay(int ticks);
uint32_t millis();
volatile uint32_t systick_counter;

void dos_colors();

int main(void){
    SystemInit();
    usart1_init();
    init_leds();
    ssd1289_init();

	ssd1289_fill(RGB_COL_BLACK);
	ssd1289_textcon_init();
	ssd1289_set_font_color(RGB_COL_YELLOW, RGB_COL_BLACK);
	ssd1289_set_font(FONT_XGA_8x14);
    init_ads7843();

    LED1_OFF();
    LED2_OFF();

	systick_counter = 0;
	/* 1000 interrupts per second = 1khz */
	SysTick_Config(SystemCoreClock/1000);

    ssd1289_set_transparency(FALSE);
    ssd1289_set_font(FONT_XGA_8x14);
    ssd1289_set_font_color(RGB_COL_YELLOW, RGB_COL_BLACK);

    ads7843_calibration();
    dos_colors();

    for(;;) {
//         ssd1289_set_text_cursor(0, 0);
//         sprintf(temp, "%08x %08x %d", ads7843_read_x(), ads7843_read_y(), tsirqs);
//         ssd1289_puts(temp);
//         systick_delay(100);
	}

	return 0;
}

void puts_colored(char *str);

void dos_colors() {
    int i;

    ssd1289_clear();

    ssd1289_set_font(FONT_XGA_8x14);

    ssd1289_set_font_color(DOS_COLOR_5, RGB_COL_BLACK);
    ssd1289_set_transparency(FALSE);

    ssd1289_set_text_cursor(0, 0);
    ssd1289_putc(0xC9);     // top left corner
    for(i = 0; i < 38; i++)
        ssd1289_putc(0xCD); // top border
    ssd1289_putc(0xBB);     // top right corner

    ssd1289_set_text_cursor(0, 16);
    ssd1289_putc(0xC8);     // bottom left corner
    for(i = 0; i < 38; i++)
        ssd1289_putc(0xCD); // bottom border
    ssd1289_putc(0xBC); // bottom right corner

    ssd1289_set_text_cursor(7, 0);
    ssd1289_set_font_color(RGB_COL_CYAN, RGB_COL_BLACK);
    ssd1289_puts(" DOS//IBM VGA 8x14 charset ");

    ssd1289_set_font_color(RGB_COL_YELLOW, RGB_COL_BLACK);
    ssd1289_set_text_cursor(6, 0);
    ssd1289_putc('[');
    ssd1289_set_text_cursor(34, 0);
    ssd1289_putc(']');

    ssd1289_set_font_color(DOS_COLOR_5, DOS_COLOR_0);
    for(i = 1; i < 16; i++) {
        ssd1289_set_text_cursor(0, i);
        ssd1289_putc(0xBA);
        ssd1289_set_text_cursor(39, i);
        ssd1289_putc(0xBA);
    }

    ssd1289_set_font_color(DOS_COLOR_7, DOS_COLOR_0);
    ssd1289_set_text_cursor(3, 2);

    ssd1289_ignore_control_chars(1);

    int x, y;
    i = 0;
    for(y = 0; y < 8; y++) {
        ssd1289_set_text_cursor(4, 2+y);
        for(x = 0; x < 32; x++) {
            ssd1289_putc(i);
            i++;
        }
    }

    ssd1289_ignore_control_chars(0);

    uint16_t colors[16] = { DOS_COLOR_0, DOS_COLOR_1, DOS_COLOR_2, DOS_COLOR_3, DOS_COLOR_4,
        DOS_COLOR_5, DOS_COLOR_6, DOS_COLOR_7, DOS_COLOR_8, DOS_COLOR_9,
        DOS_COLOR_10, DOS_COLOR_11, DOS_COLOR_12, DOS_COLOR_13,
        DOS_COLOR_14, DOS_COLOR_15 };

    for(i = 0; i < 8; i++) {
        x = 45+(i*30);
        y = 152;
        ssd1289_rect     (x-1, y-1, x+20, y+26, RGB_COL_YELLOW);
        ssd1289_fill_rect(x, y, x+20, y+25, colors[i]);
    }

    for(i = 0; i < 8; i++) {
        x = 45+(i*30);
        y = 188;
        ssd1289_rect     (x-1, y-1, x+20, y+26, RGB_COL_YELLOW);
        ssd1289_fill_rect(x, y, x+20, y+25, colors[i+7]);
    }

    return;
}

void puts_colored(char *str) {
    uint16_t colors[17] = { DOS_COLOR_0, DOS_COLOR_1, DOS_COLOR_2, DOS_COLOR_3, DOS_COLOR_4,
                            DOS_COLOR_5, DOS_COLOR_6, DOS_COLOR_7, DOS_COLOR_8, DOS_COLOR_9,
                            DOS_COLOR_10, DOS_COLOR_11, DOS_COLOR_12, DOS_COLOR_13,
                            DOS_COLOR_14, DOS_COLOR_15, DOS_COLOR_4 };
    int i = 0;


    while(*str) {
        ssd1289_set_font_color(colors[i], 0);
        ssd1289_putc(*str);
        str++;
        i++;
    }
}

void SysTick_Handler() {
	systick_counter++;

	static int led1 = 0;

	if((systick_counter % 100)==0) {	// every 100mS
		led1 = !led1;
// 		if(led1) LED1_ON();
// 		else	 LED1_OFF();
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


/*
 * SSD1289 driver example
 * Nils Stec <stecdose@gmail.com>
 * 2019-03-19
 *
 * This driver was written to be used with or without an OS.
 * It works fine with FreeRTOS.
 *
 * The whole thing was written using a board called
 * "HY-MiniSTM32V" with an LCD called "HY32D".
 * It's specs:
 *   - STM32F103VCT6, 256k flash, 64k RAM
 *   - display connection 8080 bus to FSMC
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "stm32f10x.h"

#include "systick.h"    // delay and time measurement functions
#include "random.h"		// very small, but pseudorandom
#include "leds.h"
#include "buttons.h"

#include "ssd1289.h"

int main(void){
    SystemInit();
    init_leds();

    /* inititalize systick timer */
    init_systick();
    
    /* initialize display and text functions */
    ssd1289_init();
	ssd1289_fill(RGB_COL_BLACK);
	ssd1289_textcon_init();
	ssd1289_set_font_color(RGB_COL_YELLOW, RGB_COL_BLACK);
	ssd1289_set_font(FONT_XGA_8x14);
    
    /* initialize touchscreen */
    init_ads7843();
    
    /* calibrate touchscreen */
    ads7843_calibration();
    
    for(;;) {
	}

	return 0;
}




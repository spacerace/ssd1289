#include <stdarg.h>
#include "stm32f10x.h"
#include "usart.h"

#include "printf.h"
#include "ssd1289.h"

static int printf_output_mask;

void init_printf(int output_mask) {
	printf_output_mask = output_mask;
}

void printf_putc(char c) {
	if(printf_output_mask & PRINTF_OUTPUT_SSD1289) {
		ssd1289_putc(c);
	}
	if(printf_output_mask & PRINTF_OUTPUT_USART1) {
		usart1_putc(c);
	}

}

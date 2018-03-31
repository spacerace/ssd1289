/** \file printf.c
 * Simplified printf() and sprintf() implementation - prints formatted string to
 * USART (or whereever). Most common % specifiers are supported. It costs you about
 * 3k FLASH memory - without floating point support it uses only 1k ROM!
 * \author Freddie Chopin, Martin Thomas, Marten Petschke and many others
 * \date 16.2.2012
 */

/******************************************************************************
* chip: STM32F10x
* compiler: arm-none-eabi-gcc 4.6.0
*
* functions:
* 	int printf_(const char *format, ...)
* 	int sprintf_(char *buffer, const char *format, ...)
* 	int putc_strg(int character, printf_file_t *stream)
* 	int vfprintf_(printf_file_t *stream, const char *format, va_list arg)
* 	void long_itoa (long val, int radix, int len, vfprintf_stream *stream)
*
******************************************************************************/

#include <stdarg.h>
#include "stm32f10x.h"
#include "usart.h"

#include "printf.h"
#include "ssd1289.h"

//#define INCLUDE_FLOAT  // this enables float in printf() and costs you about 2kByte ROM

void putc_UART1 (char);        // blocking put char; used by printf_()
void putc_strg(char);          // the put() function for sprintf()
char *SPRINTF_buffer;          

static int vfprintf_(void (*) (char), const char *format, va_list arg); //generic print
void long_itoa (long, int, int, void (*) (char)); //heavily used by printf_()

static int printf_output_mask;

void init_printf(int output_mask) {
	printf_output_mask = output_mask;
}

void putc_UART1 (char c)
{
	if(printf_output_mask & PRINTF_OUTPUT_SSD1289) {
		ssd1289_putc(c);
	}
	if(printf_output_mask & PRINTF_OUTPUT_USART1) {
		usart1_putc(c);
	}

}

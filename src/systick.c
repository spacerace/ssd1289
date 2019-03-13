#include "stm32f10x.h"
#include "systick.h"
#include "leds.h"

volatile uint32_t systick_counter;

void init_systick(void) {
    
	/* 1000 interrupts per second = 1khz */
    systick_counter = 0;
    SysTick_Config(SystemCoreClock/1000);
    
    return;
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


#ifndef __SYSTICK_H__
#define __SYSTICK_H__

void init_systick(void);
void systick_delay(int ticks);
uint32_t millis();


#endif // __SYSTICK_H__

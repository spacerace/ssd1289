#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
//#include "stm32f10x_usart.h"
//#include <stdio.h>

static int encoder_value;
static int encoder_direction;

int encoder_init() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
 
    GPIO_InitTypeDef gpio_cfg;
    gpio_cfg.GPIO_Mode = GPIO_Mode_IPU;
    gpio_cfg.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    gpio_cfg.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &gpio_cfg);
 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
 
    TIM_TimeBaseInitTypeDef TIMER_InitStructure;
    TIM_TimeBaseStructInit(&TIMER_InitStructure);
    TIMER_InitStructure.TIM_Period = 100;
    TIMER_InitStructure.TIM_CounterMode = TIM_CounterMode_Up | TIM_CounterMode_Down;
    TIM_TimeBaseInit(TIM5, &TIMER_InitStructure);
 
    TIM_EncoderInterfaceConfig(TIM5, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
 
    TIM_Cmd(TIM5, ENABLE);
	return 0;
}

int encoder_read() {
	return 0;
}

int encoder_reset() {
	return 0;
}






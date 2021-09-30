#include "timer.h"

/**
 * Инициализация прерываний таймера - получаем частоту 1000 Гц. 
 * freq = 84000000/ ((prescal + 1)*(period + 1)*(autoreload + 1));
 */
void timer_init(void)
{
	TIM_TimeBaseInitTypeDef timer;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_TimeBaseStructInit(&timer);
	timer.TIM_Prescaler = 839;
	timer.TIM_Period = 999;
	TIM_TimeBaseInit(TIM2, &timer);

	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
	NVIC_EnableIRQ(TIM2_IRQn);
}


/* Функцыя таймера - ЧАСТОТА 100 гц */
void timer_call_back_func(void)
{
	static int i = 0;

	if (i++ % 10 == 0) {
          led_toggle(LED4);
	}
}

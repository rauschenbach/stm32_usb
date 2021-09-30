#include "led.h"


/* Инициализация LED  */
void led_init(void)
{
    STM_EVAL_LEDInit(LED5);
    STM_EVAL_LEDInit(LED6);
    STM_EVAL_LEDInit(LED3);
    STM_EVAL_LEDInit(LED4);
}


int led_on(Led_TypeDef led)
{
    STM_EVAL_LEDOn(led);
    return 0;
}


int led_off(Led_TypeDef led)
{
    STM_EVAL_LEDOff(led);
    return 0;
}

int led_toggle(Led_TypeDef led)
{
    STM_EVAL_LEDToggle(led);
    return 0;
}

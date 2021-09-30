#ifndef _LED_H
#define _LED_H 

#include "main.h"

void led_init(void);
int  led_on(Led_TypeDef);
int  led_off(Led_TypeDef);
int  led_toggle(Led_TypeDef);




#endif /* led.h */

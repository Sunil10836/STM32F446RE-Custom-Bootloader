#ifndef LED_TEST_H_
#define LED_TEST_H_

#include "stm32f446xx.h"

#define LED_PIN     GPIO_PIN_5
#define LED_PORT    GPIOA

//void LED_Init();
void LED_Test();

void Delay1(void);

#endif

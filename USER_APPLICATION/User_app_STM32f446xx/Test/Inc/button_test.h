#ifndef BUTTON_TEST_H_
#define BUTTON_TEST_H_

#include "stm32f446xx.h"

#define BUTTON_PORT     GPIOC
#define BUTTON_PIN      GPIO_PIN_13

void Button_Test();

void Button_Interrupt_Test();

#endif
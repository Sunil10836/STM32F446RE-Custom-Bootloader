/******************************************************************************
 * @file    led.h
 *
 * @author  Sunil Sutar
 *
 * @project : User Application for STM32F446RE Custom Bootloader Development
 *
 * @date    16 October 2026
 *
 ******************************************************************************
 */

#ifndef LED_TEST_H_
#define LED_TEST_H_

#include "stm32f446xx.h"

#define LED_PIN     GPIO_PIN_5
#define LED_PORT    GPIOA

void LED_Init();
void LED_On();
void LED_Off();
void LED_Toggle();

void Delay1(void);

#endif

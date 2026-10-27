/******************************************************************************
 * @file    button.h
 *
 * @author  Sunil Sutar
 *
 * @project : User Application for STM32F446RE Custom Bootloader Development
 *
 * @date    16 October 2026
 *
 ******************************************************************************
 */

#ifndef BUTTON_TEST_H_
#define BUTTON_TEST_H_

#include "stm32f446xx.h"

#define BUTTON_PORT     GPIOC
#define BUTTON_PIN      GPIO_PIN_13

void Button_Init();

void Button_Interrupt_Init();

#endif
/******************************************************************************
 * @file    led.c
 *
 * @author  Sunil Sutar
 *
 * @project STM32F446RE Custom Bootloader Development
 *
 * @date    15 October 2026
 *
 ******************************************************************************
 */

#include "led.h"

void Delay1(void)
{
    for (volatile uint32_t i = 0; i < 100000; i++);
}

void LED_Init()
{
    GPIO_Handle_t GpioLed;

    GpioLed.pGPIOx = LED_PORT;
    GpioLed.GPIO_PinConfig.GPIO_PinNumber = LED_PIN;
    GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUTPUT;
    GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

    GPIO_Init(&GpioLed);
}


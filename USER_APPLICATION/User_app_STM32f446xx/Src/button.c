
/******************************************************************************
 * @file    button.c
 *
 * @author  Sunil Sutar
 *
 * @project : User Application for STM32F446RE Custom Bootloader Development
 *
 * @date    16 October 2026
 *
 ******************************************************************************
 */

#include "button.h"
#include "led.h"

void Button_Init()
{
    GPIO_Handle_t GpioButton;

    GpioButton.pGPIOx = BUTTON_PORT;
    GpioButton.GPIO_PinConfig.GPIO_PinNumber = BUTTON_PIN;
    GpioButton.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_INPUT;
    GpioButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    GpioButton.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

    GPIO_Init(&GpioButton);

}

void Button_Interrupt_Init()
{
    GPIO_Handle_t GpioButton;

    GpioButton.pGPIOx = BUTTON_PORT;
    GpioButton.GPIO_PinConfig.GPIO_PinNumber = BUTTON_PIN;
    GpioButton.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_FT;
    GpioButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    GpioButton.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

    GPIO_Init(&GpioButton);

    //Interrupt Configuration
    GPIO_IRQPriorityConfig(IRQ_NO_EXTI15_10, 15);
    GPIO_IRQInterruptConfig(IRQ_NO_EXTI15_10, ENABLE);
}

void EXTI15_10_IRQHandler()
{
    GPIO_IRQHandling(BUTTON_PIN);

    GPIO_ToggleOutputPin(LED_PORT, LED_PIN);
}

/******************************************************************************
 * @file    main.c
 *
 * @author  Sunil Sutar
 *
 * @project STM32F446RE Custom Bootloader Development
 *
 * @brief   Main entry point of the user application. Initializes UART,
 *          button interrupt, and LED peripherals, then executes the
 *          application main loop.
 *
 * @date    16 October 2026
 *
 ******************************************************************************
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "led.h"
#include "button.h"
#include "uart.h"

extern USART_Handle_t UART2Handle;

int main(void)
{
    USART2_Init();

    Button_Interrupt_Init();
    
    LED_Init();

    while(1)
    {
        USART_SendString(&UART2Handle, "USART2 : Hello From User Application\r\n");
        
        LED_Toggle();
        
        Delay();
    }
}

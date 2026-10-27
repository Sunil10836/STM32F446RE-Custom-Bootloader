
/******************************************************************************
 * @file    main.c
 *
 * @author  Sunil Sutar
 *
 * @project STM32F446RE Custom Bootloader Development
 *
 * @brief   Contains the main entry point of the STM32F446RE custom bootloader.
 *          Initializes UART interfaces, the button input, and the Flash driver.
 *          Selects between bootloader mode and user application execution
 *          based on the button state.
 *
 * @date    15 October 2026
 *
 ******************************************************************************
 */

#include "main.h"
#include "led.h"
#include "button.h"
#include "uart.h"
#include "bootlaoder.h"
#include "stm32f446xx_flash_driver.h"
#include "crc32.h"

#define BL_DEBUG_MSG_EN

extern USART_Handle_t UART2Handle;
extern USART_Handle_t UART3Handle;

extern FLASH_Handle_t FLASHHandle;

/**
 * @brief  Main entry point of the STM32F446RE custom bootloader.
 *
 * @details
 * Initializes USART2, USART3, the button GPIO, and the Flash driver.
 * Then checks the button state to determine the operating mode:
 *
 * - Button pressed: Enters bootloader mode and waits for UART commands.
 * - Button not pressed: Transfers execution to the user application.
 *
 * Flash and CRC test functions are available as commented-out calls
 * for development and debugging purposes.
 *
 * @param  None
 *
 * @retval None
 */

int main(void)
{
    /* Initialize USART2 for UART communication. */
    USART2_Init();

    /* Initialize USART3 for debug output. */
    USART3_Init();

    /* Initialize the GPIO pin used to select bootloader mode. */
    Button_Init();

    /* Initialize the internal Flash driver and its handle. */
    FLASH_Init(&FLASHHandle);

    /* Lets check whether button is pressed or not, if not pressed jump to user application */
    if(GPIO_ReadFromInputPin(BUTTON_PORT, BUTTON_PIN) == GPIO_PIN_RESET)
    {
        printmsg("BL_DEBUG_MSG : Button is pressed.. going to BL mode\r\n");

        Bootloader_uart_read_data();
    }
    else
    {
        printmsg("BL_DEBUG_MSG : Button is not pressed.. executing user app\r\n");

        Bootloader_JumpToApplication();
    }

    while(1)
    {
        //USART_SendString(&UART2Handle, "USART2 : Hello From Bootloader\r\n");

        //USART_SendString(&UART3Handle, "USART3 : Hello From Bootloader\r\n");

        //Delay();
    }
}



/******************************************************************************
 * @file    uart.c
 *
 * @author  Sunil Sutar
 *
 * @project : User Application for STM32F446RE Custom Bootloader Development
 *
 * @date    16 October 2026
 *
 ******************************************************************************
 */

#include "uart.h"

USART_Handle_t UART2Handle;
USART_Handle_t UART3Handle;

void USART2_PinConfig()
{
    GPIO_Handle_t usart2_gpios;
    
    usart2_gpios.pGPIOx = GPIOA;
    usart2_gpios.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
    usart2_gpios.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    usart2_gpios.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
    usart2_gpios.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    usart2_gpios.GPIO_PinConfig.GPIO_PinAltFunMode = GPIO_AF7;

    //USART2_Tx : PA2
    usart2_gpios.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_2;
    GPIO_Init(&usart2_gpios);

    //USART2_Rx : PA3
    usart2_gpios.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_3;
    GPIO_Init(&usart2_gpios);
}

void USART2_Init()
{
    /* Configure USART2 GPIO */
    USART2_PinConfig();

    /* Configure USART2 */
    UART2Handle.pUSARTx = USART2;
    UART2Handle.USART_Config.UART_BaudRate = USART_STD_BAUD_115200;
    UART2Handle.USART_Config.UART_Mode = USART_MODE_TXRX;
    //UART2Handle.USART_Config.UART_Mode = USART_MODE_ONLY_TX;
    UART2Handle.USART_Config.UART_HWFlowControl = USART_HW_FLOW_CTRL_NONE;
    UART2Handle.USART_Config.UART_WordLength = USART_WORDLEN_8BITS;
    UART2Handle.USART_Config.UART_NoOfStopBits = USART_STOPBITS_1;
    UART2Handle.USART_Config.UART_ParityControl = USART_PARITY_DISABLE;

    USART_Init(&UART2Handle);
}

void USART3_PinConfig()
{
    GPIO_Handle_t usart3_gpios;
    
    usart3_gpios.pGPIOx = GPIOC;
    usart3_gpios.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
    usart3_gpios.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    usart3_gpios.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
    usart3_gpios.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    usart3_gpios.GPIO_PinConfig.GPIO_PinAltFunMode = GPIO_AF7;

    //USART3_Tx : PC10 or PB10 
    usart3_gpios.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_10;
    GPIO_Init(&usart3_gpios);

    //USART3_Rx : PC5 or PB11 
    usart3_gpios.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_11;
    GPIO_Init(&usart3_gpios);
}

void USART3_Init()
{
    /* Configure USART3 GPIO */
    USART3_PinConfig();

    /* Configure USART3 */
    UART3Handle.pUSARTx = USART3;
    UART3Handle.USART_Config.UART_BaudRate = USART_STD_BAUD_115200;
    UART3Handle.USART_Config.UART_Mode = USART_MODE_TXRX;
    //UART2Handle.USART_Config.UART_Mode = USART_MODE_ONLY_TX;
    UART3Handle.USART_Config.UART_HWFlowControl = USART_HW_FLOW_CTRL_NONE;
    UART3Handle.USART_Config.UART_WordLength = USART_WORDLEN_8BITS;
    UART3Handle.USART_Config.UART_NoOfStopBits = USART_STOPBITS_1;
    UART3Handle.USART_Config.UART_ParityControl = USART_PARITY_DISABLE;

    USART_Init(&UART3Handle);
}

void Delay(void)
{
	for(uint32_t i = 0 ; i < 5000000 ; i++);
}

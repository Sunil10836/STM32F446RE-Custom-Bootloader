
/******************************************************************************
 * @file    bootloader.h
 *
 * @author  Sunil Sutar
 *
 * @project STM32F446RE Custom Bootloader Development
 *
 * @brief   Declares the core bootloader functionality for the STM32F446RE.
 *          Contains bootloader-related function prototypes, macros,
 *          constants, and declarations required for command processing,
 *          Flash operations, memory access, and application handover,
 *          as applicable to this implementation.
 *
 * @date    15 October 2026
 *
 ******************************************************************************
 */

#ifndef INC_BOOTLOADER_H_
#define INC_BOOTLOADER_H_

#include "stm32f446xx.h"
#include <stdint.h>
#include "uart.h"

extern USART_Handle_t UART2Handle;
extern USART_Handle_t UART3Handle;

#define APPLICATION_START_ADDRESS    0x08008000U
#define APPLICATION_END_ADDRESS      0x08080000U

#define DEBUG_UART      &UART3Handle
#define CMD_UART        &UART2Handle

void Bootloader_JumpToApplication(void);

void Bootloader_uart_read_data(void);


#endif /* INC_BOOTLOADER_H_ */
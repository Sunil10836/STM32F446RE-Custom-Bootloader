/******************************************************************************
 * @file    uart.h
 *
 * @author  Sunil Sutar
 *
 * @project STM32F446RE Custom Bootloader Development
 *
 * @date    15 October 2026
 *
 ******************************************************************************
 */

#ifndef UART_TEST_H_
#define UART_TEST_H_

#include "stm32f446xx.h"
#include "string.h"


void USART2_PinConfig();
void USART3_PinConfig();

void USART2_Init();
void USART3_Init();

void Delay(void);

void printmsg(char *format, ...);

#endif

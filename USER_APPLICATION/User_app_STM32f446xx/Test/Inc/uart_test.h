#ifndef UART_TEST_H_
#define UART_TEST_H_

#include "stm32f446xx.h"
#include "string.h"

// void USART2_PinConfig();
// void USART3_PinConfig();

// void USART2_Init();
// void USART3_Init();

void Delay(void);

void USART_Transmit_Test();
void USART_Receive_Test();

#endif

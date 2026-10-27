#include "uart_test.h"

USART_Handle_t UART2Handle;
USART_Handle_t UART3Handle;

//char msg[100] = "UART Tx Testing...\r\n";

void USART_Transmit_Test()
{
    USART2_Init();

    while(1)
    {
       USART_SendData(&UART2Handle, (uint8_t *)msg, strlen(msg));

       USART_SendString(&UART2Handle, "STM32 UART Test..\r\n");

       //USART_DeInit(USART2);  //added for tseting purpose

       Delay();
    }
}

void USART_Receive_Test()
{
    USART2_Init();

    uint8_t ch = 0;

    while(1)
    {
        USART_ReceiveData(&UART2Handle, &ch, 1);

        USART_SendData(&UART2Handle, (uint8_t *)&ch, 1);

        Delay();
    }
}

void Delay(void)
{
	for(uint32_t i = 0 ; i < 500000/2 ; i ++);
}

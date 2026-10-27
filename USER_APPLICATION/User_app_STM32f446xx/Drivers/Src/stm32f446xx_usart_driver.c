#include "stm32f446xx_usart_driver.h"

static void USART_SetBaudRate(USART_TypeDef_t *pUSARTx, uint32_t BaudRate);


void USART_PeriClockControl(USART_TypeDef_t *pUSARTx, uint8_t Enable)
{
    if(Enable == ENABLE)
    {
        if(pUSARTx == USART1)
        {
            USART1_PCLK_EN();        
        }
        else if(pUSARTx == USART2)
        {
            USART2_PCLK_EN();
        }
        else if(pUSARTx == USART3)
        {
            USART3_PCLK_EN();
        }
        else if(pUSARTx == UART4)
        {
            UART4_PCLK_EN();
        }
        else if(pUSARTx == UART5)
        {
            UART5_PCLK_EN();
        }
        else if(pUSARTx == USART6)
        {
            USART6_PCLK_EN();
        }
    }
    else
    {
        if(pUSARTx == USART1)
        {
            USART1_PCLK_DI();        
        }
        else if(pUSARTx == USART2)
        {
            USART2_PCLK_DI();
        }
        else if(pUSARTx == USART3)
        {
            USART3_PCLK_DI();
        }
        else if(pUSARTx == UART4)
        {
            UART4_PCLK_DI();
        }
        else if(pUSARTx == UART5)
        {
            UART5_PCLK_DI();
        }
        else if(pUSARTx == USART6)
        {
            USART6_PCLK_DI();
        }
    }
}

void USART_Init(USART_Handle_t *pUARTHandle)
{
    uint32_t tempreg = 0;

    /* 1. Enable Peripheral Clock */
    USART_PeriClockControl(pUARTHandle->pUSARTx, ENABLE);

    /**************** Configuration of CR1 ***********************/
    /* 2. Configure UART Mode */
    switch(pUARTHandle->USART_Config.UART_Mode)
    {
        case USART_MODE_ONLY_TX:
            tempreg |= (1 << USART_CR1_TE);
            break;
        
        case USART_MODE_ONLY_RX:
            tempreg |= (1 << USART_CR1_RE);
            break;

        case USART_MODE_TXRX:
            tempreg |= (1 << USART_CR1_TE);
            tempreg |= (1 << USART_CR1_RE);
            break;
    }

    /* 3. Configuration of Word Length */
    tempreg |= (pUARTHandle->USART_Config.UART_WordLength << USART_CR1_M);


    /* 4. Configuration of parity control  */
    if(pUARTHandle->USART_Config.UART_ParityControl == USART_PARITY_DISABLE)
    {
        //Disable Parity
        tempreg &= ~(1 << USART_CR1_PCE);
    }
    else if(pUARTHandle->USART_Config.UART_ParityControl == USART_PARITY_EVEN)
    {
        //Enable Parity
        tempreg |= (1 << USART_CR1_PCE);

        //Even Parity Selection
        tempreg &= ~(1 << USART_CR1_PS);
    }
    else if(pUARTHandle->USART_Config.UART_ParityControl == USART_PARITY_ODD)
    {
        //Enable Parity
        tempreg |= (1 << USART_CR1_PCE);

        //Odd Parity Selection
        tempreg |= (1 << USART_CR1_PS);  
    }


    /* program CR1 register */
    pUARTHandle->pUSARTx->CR1 = tempreg;

    /**************** Configuration of CR2 ***********************/
    tempreg = 0;

    /* Configure number of stop bits */
    tempreg |= pUARTHandle->USART_Config.UART_NoOfStopBits << USART_CR2_STOP;

    /* program CR2 register */
    pUARTHandle->pUSARTx->CR2 = tempreg;

    /*************** Configuration of CR3 ************************/
    tempreg = 0;

    /* Configure HwFlow Control */
    if(pUARTHandle->USART_Config.UART_HWFlowControl == USART_HW_FLOW_CTRL_CTS)
    {
        tempreg |= (1 << USART_CR3_CTSE);
    }
    else if(pUARTHandle->USART_Config.UART_HWFlowControl == USART_HW_FLOW_CTRL_RTS)
    {
        tempreg |= (1 << USART_CR3_RTSE);
    }
    else if(pUARTHandle->USART_Config.UART_HWFlowControl == USART_HW_FLOW_CTRL_CTS_RTS)
    {
        tempreg |= ((1 << USART_CR3_CTSE) | (1 << USART_CR3_RTSE));
    }    

    /* program CR3 register */
    pUARTHandle->pUSARTx->CR3 = tempreg;

    /*  Configuration of BRR(Baudrate register) */
    USART_SetBaudRate(pUARTHandle->pUSARTx, pUARTHandle->USART_Config.UART_BaudRate);

    /* Enable USART */
    USART_PeripheralControl(pUARTHandle->pUSARTx, ENABLE);

}

void USART_DeInit(USART_TypeDef_t *pUSARTx)
{
    if(pUSARTx == USART1)
    {
        USART1_REG_RESET();
    }
    else if(pUSARTx == USART2)
    {
        USART2_REG_RESET();
    }
    else if(pUSARTx == USART2)
    {
        USART2_REG_RESET();
    }
    else if(pUSARTx == USART3)
    {
        USART3_REG_RESET();
    }
    else if(pUSARTx == UART4)
    {
        UART4_REG_RESET();
    }
    else if(pUSARTx == UART5)
    {
        UART5_REG_RESET();
    }
    else if(pUSARTx == USART6)
    {
        USART6_REG_RESET();
    }
}

void USART_SendData(USART_Handle_t *pUARTHandle, uint8_t *pTxBuffer, uint32_t Len)
{
    uint16_t *pData;

    for(uint32_t i = 0; i < Len; i++)
    {
        /* wait until TXE flag is set */
        while(! USART_GetFlagStatus(pUARTHandle->pUSARTx, USART_FLAG_TXE));

        /* Check the USART_WordLength item for 9BIT or 8BIT in a frame */
        if(pUARTHandle->USART_Config.UART_WordLength == USART_WORDLEN_9BITS)
        {
            //9-bit Word Length
            pData = (uint16_t *) pTxBuffer;
            pUARTHandle->pUSARTx->DR = *pData & (uint16_t)0x01FFU;

            /* check for USART_ParityControl */
            if(pUARTHandle->USART_Config.UART_ParityControl == USART_PARITY_DISABLE)
            {
                //No Parity : 9-bits Data
                //Increment pTxBuffer twice
                pTxBuffer++;
                pTxBuffer++;
            }
            else
            {
                //1-bit: Parity and 8-bit: Data
                //Increment pTxBuffer
                pTxBuffer++;
            }
        }
        else
        {
            //8-bit Word Length
            pUARTHandle->pUSARTx->DR = (*pTxBuffer & (uint8_t)0xFFU);

            //Increment pTxBuffer
            pTxBuffer++;

        }
    }

    /* wait till TC flag is set */
    while(! USART_GetFlagStatus(pUARTHandle->pUSARTx, USART_FLAG_TC));
}

void USART_ReceiveData(USART_Handle_t *pUARTHandle, uint8_t *pRxBuffer, uint32_t Len)
{
    for(uint32_t i = 0; i < Len; i++)
    {
        /* Wait until RXNE flag is set */
        while(!  USART_GetFlagStatus(pUARTHandle->pUSARTx, USART_FLAG_RXNE));

        /* Check the USART_WordLength : 8-bit or 9-bit */
        if(pUARTHandle->USART_Config.UART_WordLength == USART_WORDLEN_9BITS)
        {
            //9-bits
            //Check Parity
            if(pUARTHandle->USART_Config.UART_ParityControl == USART_PARITY_DISABLE)
            {
                //No Parity -> 9-bit Data
                *((uint16_t *)pRxBuffer) = pUARTHandle->pUSARTx->DR & 0x1FFU;

                //Increment pRxBuffer
                pRxBuffer++;
                pRxBuffer++;
            }
            else
            {   
                //Parity -> 1-bit Parity + 8-bit Data
                *pRxBuffer = pUARTHandle->pUSARTx->DR & 0xFF;

                //Increment pRxBuffer
                pRxBuffer++;
            }
        }
        else
        {
            //8-bits
            //Check Parity
            if(pUARTHandle->USART_Config.UART_ParityControl == USART_PARITY_DISABLE)
            {
                //No Parity -> 8-bit Data
                *pRxBuffer = pUARTHandle->pUSARTx->DR & 0xFF;
            }
            else
            {
                //Parity -> 1-bit Parity + 7-bit Data
                *pRxBuffer = (uint8_t) pUARTHandle->pUSARTx->DR & 0x7F;
            }

            //Increment pRxBuffer
            pRxBuffer++;
        }
    }
}

void USART_SendString(USART_Handle_t *pUSARTHandle, const char *pStr)
{
   //USART_SendData(pUSARTHandle, (uint8_t *)pStr, strlen(pStr)); 

   while(*pStr != '\0')
   {
        /* Wait until transmit data register is empty */
        while(!USART_GetFlagStatus(pUSARTHandle->pUSARTx, USART_FLAG_TXE));

        pUSARTHandle->pUSARTx->DR = (uint8_t)*pStr;

        pStr++;
   }

    /* Wait for Transmission Complete */
    while(!USART_GetFlagStatus(pUSARTHandle->pUSARTx, USART_FLAG_TC));

}

void USART_PeripheralControl(USART_TypeDef_t *pUSARTx, uint8_t Enable)
{
    if(Enable == ENABLE)
    {
        pUSARTx->CR1 |= (1 << USART_CR1_UE);
    }
    else
    {
        pUSARTx->CR1 &= ~(1 << USART_CR1_UE);
    }
}

uint8_t USART_GetFlagStatus(USART_TypeDef_t *pUSARTx, uint32_t FlagName)
{
    if(pUSARTx->SR & FlagName)
    {
        return FLAG_SET;
    }
    return FLAG_RESET;
}

static void USART_SetBaudRate(USART_TypeDef_t *pUSARTx, uint32_t BaudRate)
{
    uint32_t Fpclk;
    uint32_t usartdiv;
    uint32_t mantissa, fraction;

    uint32_t tempreg = 0;

    if(pUSARTx == USART1 || pUSARTx == USART6)
    {
       Fpclk = RCC_GetPCLK2Value();
    }
    else
    {
       Fpclk = RCC_GetPCLK1Value();
    }

    /* Check for OVER8 configuration bit */
    if(pUSARTx->CR1 & (1 << USART_CR1_OVER8))
    {
        //Oversampling by 8
        usartdiv = ((25U * Fpclk) / (2U * BaudRate));
    }
    else
    {
        //Oversampling by 16
        usartdiv = ((25U * Fpclk) / (4U * BaudRate));
    }

    /* Calculate the Mantissa part */
    mantissa = usartdiv / 100U;

    tempreg |= mantissa << 4;

    /* Extract the fraction part */
    fraction = (usartdiv - (mantissa * 100U));

    /* Calculation of the final fractional */
    if(pUSARTx->CR1 & (1 << USART_CR1_OVER8))
    {
        //Oversampling by 8
        fraction = ((fraction * 8U) + 50U)/ 100U;
    }
    else
    {
        //Oversampling by 16
        fraction = ((fraction * 16U) + 50U)/ 100U;
    }

    tempreg |= fraction & 0x0F;

    /* copy the value of tempreg in to BRR register */
    pUSARTx->BRR = tempreg;
}

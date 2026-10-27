#include "stm32f446xx_gpio_driver.h"

static uint8_t GPIO_GetPortCode(GPIO_TypeDef_t *pGPIOx);

void GPIO_PeriClockControl(GPIO_TypeDef_t *pGPIOx, uint8_t Enable)
{
    if(Enable == ENABLE)
    {
        if(pGPIOx == GPIOA)
        {
            GPIOA_PCLK_EN();
        }
        else if(pGPIOx == GPIOB)
        {
            GPIOB_PCLK_EN();
        }
        else if(pGPIOx == GPIOC)
        {
            GPIOC_PCLK_EN();
        }
        else if(pGPIOx == GPIOD)
        {
            GPIOD_PCLK_EN();
        }
        else if(pGPIOx == GPIOE)
        {
            GPIOE_PCLK_EN();
        }
        else if(pGPIOx == GPIOF)
        {
            GPIOF_PCLK_EN();
        }
        else if(pGPIOx == GPIOG)
        {
            GPIOG_PCLK_EN();
        }
        else if(pGPIOx == GPIOH)
        {
            GPIOH_PCLK_EN();
        }
    }
    else
    {
        if(pGPIOx == GPIOA)
        {
            GPIOA_PCLK_DI();
        }
        else if(pGPIOx == GPIOB)
        {
            GPIOB_PCLK_DI();
        }
        else if(pGPIOx == GPIOC)
        {
            GPIOC_PCLK_DI();
        }
        else if(pGPIOx == GPIOD)
        {
            GPIOD_PCLK_DI();
        }
        else if(pGPIOx == GPIOE)
        {
            GPIOE_PCLK_DI();
        }
        else if(pGPIOx == GPIOF)
        {
            GPIOF_PCLK_DI();
        }
        else if(pGPIOx == GPIOG)
        {
            GPIOG_PCLK_DI();
        }
        else if(pGPIOx == GPIOH)
        {
            GPIOH_PCLK_DI();
        }
    }
}

void GPIO_Init(GPIO_Handle_t *pGPIOHandle)
{
    uint32_t tempreg = 0;

    /* Enable GPIO Peripheral Clock */
    GPIO_PeriClockControl(pGPIOHandle->pGPIOx, ENABLE);

    /* 1. Configure GPIO Mode */
    if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode <= GPIO_MODE_ANALOG)
    {
        //Non-Interrupt Mode
        tempreg = (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
        pGPIOHandle->pGPIOx->MODER &= ~(0x3U << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
        pGPIOHandle->pGPIOx->MODER |= tempreg;
    }
    else
    {
        //Interrupt Mode
        //1. Configure Trigger Selection
        if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_FT)
        {
            //Falling Edge
            EXTI->FTSR |= (1U << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
            EXTI->RTSR &= ~(1U << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
        }
        else if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RT)
        {
            //Rising Edge
            EXTI->RTSR |= (1U << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
            EXTI->FTSR &= ~(1U << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);   
        }
        else if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RFT)
        {
            //Rising Edge + Falling Edge
            EXTI->RTSR |= (1U << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
            EXTI->FTSR |= (1U << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);   
        }

        //2. Enable Clock for SYSCFG
        SYSCFG_PCLK_EN();

        //3. Configure the GPIO port selection in SYSCFG_EXTICR Register
        uint8_t temp1 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber / 4;
        uint8_t temp2 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber % 4;
        uint8_t portcode = GPIO_GetPortCode(pGPIOHandle->pGPIOx);
        SYSCFG->EXTICR[temp1] |= portcode << (temp2 * 4);

        //4. Enable the EXTI Interrupt delivery using IMR
        EXTI->IMR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
    }

    /* 2. Configure the GPIO Output Speed */
    tempreg = 0;
    tempreg = (pGPIOHandle->GPIO_PinConfig.GPIO_PinSpeed << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
    pGPIOHandle->pGPIOx->OSPEEDR &= ~(0x3U << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
    pGPIOHandle->pGPIOx->OSPEEDR |= tempreg;

    /* 3. Configure the GPIO PuPd Settings */
    tempreg = 0;
    tempreg = (pGPIOHandle->GPIO_PinConfig.GPIO_PinPuPdControl << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
    pGPIOHandle->pGPIOx->PUPDR &= ~(0x3U << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
    pGPIOHandle->pGPIOx->PUPDR |= tempreg;

    /* 4. Configure the GPIO Output Type */
    tempreg = 0;
    tempreg = (pGPIOHandle->GPIO_PinConfig.GPIO_PinOPType << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
    pGPIOHandle->pGPIOx->OTYPER &= ~(0x1U << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
    pGPIOHandle->pGPIOx->OTYPER |= tempreg;

    /* 5. Configure the GPIO Alt Functionality */
    if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_ALTFN)
    {
        uint8_t regIndex;
        uint8_t bitPosition;

        regIndex = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber / 8U;
        bitPosition = ((pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber % 8U) * 4);

        pGPIOHandle->pGPIOx->AFR[regIndex] &= ~(0xFU << bitPosition);   //clearing
        pGPIOHandle->pGPIOx->AFR[regIndex] |= ((pGPIOHandle->GPIO_PinConfig.GPIO_PinAltFunMode & 0xFU) << bitPosition);
    }
}

void GPIO_DeInit(GPIO_TypeDef_t *pGPIOx)
{
    if(pGPIOx == GPIOA)
    {
        GPIOA_REG_RESET();
    }
    else if(pGPIOx == GPIOB)
    {
        GPIOB_REG_RESET();
    }
    else if(pGPIOx == GPIOC)
    {
        GPIOC_REG_RESET();
    }
    else if(pGPIOx == GPIOD)
    {
        GPIOD_REG_RESET();
    }
    else if(pGPIOx == GPIOE)
    {
        GPIOE_REG_RESET();
    }
    else if(pGPIOx == GPIOF)
    {
        GPIOF_REG_RESET();
    }
    else if(pGPIOx == GPIOG)
    {
        GPIOG_REG_RESET();
    }
    else if(pGPIOx == GPIOH)
    {
        GPIOH_REG_RESET();
    }
}

uint8_t GPIO_ReadFromInputPin(GPIO_TypeDef_t *pGPIOx, uint8_t PinNumber)
{
    uint8_t value;

    value = (uint8_t)((pGPIOx->IDR >> PinNumber) & 0x01);

    return value;
}

uint16_t GPIO_ReadFromInputPort(GPIO_TypeDef_t *pGPIOx)
{
    return (uint16_t)(pGPIOx->IDR);
}

void GPIO_WriteToOutputPin(GPIO_TypeDef_t *pGPIOx, uint8_t PinNumber, uint8_t Value)
{
    if(Value == GPIO_PIN_SET)
    {
        pGPIOx->BSRR = (1U << PinNumber);
    }
    else
    {
        pGPIOx->BSRR = (1U << (PinNumber + 16U));
    }
}

void GPIO_WriteToOutputPort(GPIO_TypeDef_t *pGPIOx, uint16_t Value)
{
    pGPIOx->ODR = Value;
}

void GPIO_ToggleOutputPin(GPIO_TypeDef_t *pGPIOx, uint8_t PinNumber)
{
    pGPIOx->ODR ^= (1U << PinNumber);
}

void GPIO_IRQInterruptConfig(uint8_t IRQNumber, uint8_t Enable)
{
    if(Enable == ENABLE)
    {
        if(IRQNumber <= 31)
        {
            *NVIC_ISER0 |= (1 << IRQNumber);
        }
        else if(IRQNumber > 31 && IRQNumber < 64)
        {
            *NVIC_ISER1 |= (1 << IRQNumber % 32);
        }
        else if(IRQNumber >= 64 && IRQNumber < 96)
        {
            *NVIC_ISER2 |= (1 << IRQNumber % 64);
        }
    }
    else
    {
        if(IRQNumber <= 31)
        {
            *NVIC_ICER0 |= (1 << IRQNumber);
        }
        else if(IRQNumber > 31 && IRQNumber < 64)
        {
            *NVIC_ICER1 |= (1 << IRQNumber % 32);
        }
        else if(IRQNumber >= 64 && IRQNumber < 96)
        {
            *NVIC_ICER1 |= (1 << IRQNumber % 64);
        }
    }
}

void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
    //1. First Finding out Priority Register and section in priority register
    uint8_t iprx = IRQNumber / 4;
    uint8_t iprx_section = IRQNumber % 4;

    //Lower 4-bits are not Implement in each IPR section
    uint8_t shift_ammount = (8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED);

    //Program NVIC_PR Register
    *(NVIC_PR_BASE_ADDR + iprx) &= ~(0xFU << shift_ammount);
    *(NVIC_PR_BASE_ADDR + iprx) |= (IRQPriority << shift_ammount);
}

void GPIO_IRQHandling(uint8_t PinNumber)
{
    /* Check whether the interrupt is pending */
    if(EXTI->PR & (1 << PinNumber))
    {
        /* Clear the pending interrupt by writing 1 */
        EXTI->PR |= (1 << PinNumber);
    }
}

static uint8_t GPIO_GetPortCode(GPIO_TypeDef_t *pGPIOx)
{
    if (pGPIOx == GPIOA) return GPIOA_PORT_CODE;
    else if (pGPIOx == GPIOB) return GPIOB_PORT_CODE;
    else if (pGPIOx == GPIOC) return GPIOC_PORT_CODE;
    else if (pGPIOx == GPIOD) return GPIOD_PORT_CODE;
    else if (pGPIOx == GPIOE) return GPIOE_PORT_CODE;
    else if (pGPIOx == GPIOF) return GPIOF_PORT_CODE;
    else if (pGPIOx == GPIOG) return GPIOG_PORT_CODE;
    else if (pGPIOx == GPIOH) return GPIOH_PORT_CODE;
    else 
        return 0;
}
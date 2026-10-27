
/**********************************************************************
 * @file    stm32f466xx_gpio_driver.h
 * @brief   GPIO Driver Header for STM32F446RE
 * @author  Sunil Sutar
 * @version 1.0
 * @date    10 July 2026
 **********************************************************************/

#ifndef STM32F446XX_GPIO_DRIVER_H_
#define STM32F446XX_GPIO_DRIVER_H_

#include "stm32f446xx.h"

/**********************************************************************
 * GPIO Pin Configuration Structure
 *********************************************************************/
typedef struct
{
	uint8_t GPIO_PinNumber;			/*!< possible values from @GPIO_PIN_NUMBER >*/
	uint8_t GPIO_PinMode;			/*!< possible values from @GPIO_PIN_MODES  >*/
	uint8_t GPIO_PinSpeed;			/*!< possible values from @GPIO_PIN_SPEED  >*/
	uint8_t GPIO_PinPuPdControl;	/*!< possible values from @GPIO_PIN_PUPD   >*/
	uint8_t GPIO_PinOPType;			/*!< possible values from @GPIO_PIN_OTYPE  >*/
	uint8_t GPIO_PinAltFunMode;		/*!< possible values from @GPIO_PIN_ALTFN  >*/

} GPIO_PinConfig_t;

/**********************************************************************
 * GPIO Handle Structure
 *********************************************************************/
typedef struct
{
    GPIO_TypeDef_t *pGPIOx;
    GPIO_PinConfig_t GPIO_PinConfig;

} GPIO_Handle_t;


/**********************************************************************
 * GPIO PIN NUMBERS -> @GPIO_PIN_NUMBER 
 *********************************************************************/
#define GPIO_PIN_0                 0U
#define GPIO_PIN_1                 1U
#define GPIO_PIN_2                 2U
#define GPIO_PIN_3                 3U
#define GPIO_PIN_4                 4U
#define GPIO_PIN_5                 5U
#define GPIO_PIN_6                 6U
#define GPIO_PIN_7                 7U
#define GPIO_PIN_8                 8U
#define GPIO_PIN_9                 9U
#define GPIO_PIN_10                10U
#define GPIO_PIN_11                11U
#define GPIO_PIN_12                12U
#define GPIO_PIN_13                13U
#define GPIO_PIN_14                14U
#define GPIO_PIN_15                15U

/**********************************************************************
 * GPIO PIN MODES -> @GPIO_PIN_MODES
 *********************************************************************/
#define GPIO_MODE_INPUT            0U
#define GPIO_MODE_OUTPUT           1U
#define GPIO_MODE_ALTFN            2U
#define GPIO_MODE_ANALOG           3U

/* Interrupt Modes */
#define GPIO_MODE_IT_FT            4U
#define GPIO_MODE_IT_RT            5U
#define GPIO_MODE_IT_RFT           6U

/**********************************************************************
 * GPIO OUTPUT TYPES -> @GPIO_PIN_OTYPE
 *********************************************************************/
#define GPIO_OP_TYPE_PP            0U
#define GPIO_OP_TYPE_OD            1U


/**********************************************************************
 * GPIO OUTPUT SPEED -> @GPIO_PIN_SPEED
 *********************************************************************/
#define GPIO_SPEED_LOW             0U
#define GPIO_SPEED_MEDIUM          1U
#define GPIO_SPEED_FAST            2U
#define GPIO_SPEED_HIGH            3U

/**********************************************************************
 * GPIO PULL-UP / PULL-DOWN -> @GPIO_PIN_PUPD
 *********************************************************************/
#define GPIO_NO_PUPD               0U
#define GPIO_PIN_PU                1U
#define GPIO_PIN_PD                2U

/**********************************************************************
 * GPIO PIN STATE
 *********************************************************************/
#define GPIO_PIN_RESET             0U
#define GPIO_PIN_SET               1U

/**********************************************************************
 * ALTERNATE FUNCTION NUMBERS -> @GPIO_PIN_ALTFN
 *********************************************************************/
#define GPIO_AF0                   0U
#define GPIO_AF1                   1U
#define GPIO_AF2                   2U
#define GPIO_AF3                   3U
#define GPIO_AF4                   4U
#define GPIO_AF5                   5U
#define GPIO_AF6                   6U
#define GPIO_AF7                   7U
#define GPIO_AF8                   8U
#define GPIO_AF9                   9U
#define GPIO_AF10                  10U
#define GPIO_AF11                  11U
#define GPIO_AF12                  12U
#define GPIO_AF13                  13U
#define GPIO_AF14                  14U
#define GPIO_AF15                  15U

/**********************************************************************
 * GPIO PORT CODES
 *********************************************************************/

#define GPIOA_PORT_CODE		      0U
#define GPIOB_PORT_CODE			  1U
#define GPIOC_PORT_CODE      	  2U
#define GPIOD_PORT_CODE           3U
#define GPIOE_PORT_CODE           4U
#define GPIOF_PORT_CODE           5U
#define GPIOG_PORT_CODE           6U
#define GPIOH_PORT_CODE           7U

/**********************************************************************
 * GPIO DRIVER APIS
 *********************************************************************/

 /* Peripheral Clock */
void GPIO_PeriClockControl(GPIO_TypeDef_t *pGPIOx, uint8_t Enable);

/* Initialization */
void GPIO_Init(GPIO_Handle_t *pGPIOHandle);
void GPIO_DeInit(GPIO_TypeDef_t *pGPIOx);

/* Data Read */
uint8_t GPIO_ReadFromInputPin(GPIO_TypeDef_t *pGPIOx, uint8_t PinNumber);
uint16_t GPIO_ReadFromInputPort(GPIO_TypeDef_t *pGPIOx);

/* Data Write */
void GPIO_WriteToOutputPin(GPIO_TypeDef_t *pGPIOx, uint8_t PinNumber, uint8_t Value);
void GPIO_WriteToOutputPort(GPIO_TypeDef_t *pGPIOx, uint16_t Value);
void GPIO_ToggleOutputPin(GPIO_TypeDef_t *pGPIOx, uint8_t PinNumber);

/* Interrupt Configuration */
void GPIO_IRQInterruptConfig(uint8_t IRQNumber, uint8_t Enable);
void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void GPIO_IRQHandling(uint8_t PinNumber);

#endif  /*STM32F446XX_GPIO_DRIVER_H_*/
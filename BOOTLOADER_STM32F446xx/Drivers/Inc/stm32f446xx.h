/*
 * stm32f446xx.h
 *
 *  Created on: July 10, 2026
 *      Author: Sunil Sutar
 */

#ifndef INC_STM32F446XX_H_
#define INC_STM32F446XX_H_

#include <stdint.h>

#define __vo    volatile

/************************ START: Processor Specific Details *********************/

/********************************************************************
 * ARM Cortex Mx Processor NVIC ISERx register addresses
 *******************************************************************/

#define NVIC_ISER0			 ((__vo uint32_t *) 0xE000E100 )
#define NVIC_ISER1			 ((__vo uint32_t *) 0xE000E104 )
#define NVIC_ISER2			 ((__vo uint32_t *) 0xE000E108 )
#define NVIC_ISER3			 ((__vo uint32_t *) 0xE000E10C )

/********************************************************************
 * ARM Cortex Mx Processor NVIC ISERx register addresses
 *******************************************************************/

#define NVIC_ICER0			 ((__vo uint32_t *) 0XE000E180 )
#define NVIC_ICER1			 ((__vo uint32_t *) 0XE000E184 )
#define NVIC_ICER2			 ((__vo uint32_t *) 0XE000E188 )
#define NVIC_ICER3			 ((__vo uint32_t *) 0XE000E18C )

#define NVIC_PR_BASE_ADDR      ((__vo uint32_t*)0xE000E400U)

#define NO_PR_BITS_IMPLEMENTED    4U

/* ============================================================
 * Cortex-M4 System Control Block (SCB)
 * ============================================================ */

#define SCB_BASE_ADDRESS    (0xE000ED00UL)

typedef struct
{
    volatile uint32_t CPUID;       /* 0x000: CPUID Base Register */
    volatile uint32_t ICSR;        /* 0x004: Interrupt Control and State Register */
    volatile uint32_t VTOR;        /* 0x008: Vector Table Offset Register */
    volatile uint32_t AIRCR;       /* 0x00C: Application Interrupt and Reset Control Register */
    volatile uint32_t SCR;         /* 0x010: System Control Register */
    volatile uint32_t CCR;         /* 0x014: Configuration and Control Register */

    volatile uint8_t  SHPR[12];    /* 0x018: System Handler Priority Registers */

    volatile uint32_t SHCSR;       /* 0x024: System Handler Control and State Register */
    volatile uint32_t CFSR;        /* 0x028: Configurable Fault Status Register */
    volatile uint32_t HFSR;        /* 0x02C: HardFault Status Register */
    volatile uint32_t DFSR;        /* 0x030: Debug Fault Status Register */
    volatile uint32_t MMFAR;       /* 0x034: MemManage Fault Address Register */
    volatile uint32_t BFAR;        /* 0x038: BusFault Address Register */
    volatile uint32_t AFSR;        /* 0x03C: Auxiliary Fault Status Register */

    volatile uint32_t PFR[2];      /* 0x040: Processor Feature Registers */
    volatile uint32_t DFR;         /* 0x048: Debug Feature Register */
    volatile uint32_t ADR;         /* 0x04C: Auxiliary Feature Register */
    volatile uint32_t MMFR[4];     /* 0x050: Memory Model Feature Registers */
    volatile uint32_t ISAR[5];     /* 0x060: Instruction Set Attribute Registers */

    uint32_t RESERVED0[5];         /* 0x074 - 0x084 */

    volatile uint32_t CPACR;       /* 0x088: Coprocessor Access Control Register */

} SCB_RegDef_t;

#define SCB                   ((SCB_RegDef_t *)SCB_BASE_ADDRESS)

/* ============================================================
 * Cortex-M4 Core Instructions
 * ============================================================ */

static inline void __disable_irq(void)
{
    __asm volatile ("cpsid i" ::: "memory");
}

static inline void __enable_irq(void)
{
    __asm volatile ("cpsie i" ::: "memory");
}

static inline void __set_MSP(uint32_t value)
{
    __asm volatile ("msr msp, %0"
                    :
                    : "r" (value)
                    : "memory");
}

static inline uint32_t __get_MSP(void)
{
    uint32_t result;

    __asm volatile ("mrs %0, msp"
                    : "=r" (result));

    return result;
}

static inline void __DSB(void)
{
    __asm volatile ("dsb 0xF" ::: "memory");
}

static inline void __ISB(void)
{
    __asm volatile ("isb 0xF" ::: "memory");
}

/************************ END: Processor Specific Details *********************/


/************************ START: MCU Specific Details ************************/

/********************************************************************
 * Base addresses of Flash and SRAM memories
 *******************************************************************/

#define FLASH_BASEADDR				0x08000000U				/*!< This is Base Address of FLASH memory  */
#define SRAM1_BASEADDR				0x20000000U				/*!< This is Base Address of SRAM1 memory  */
#define SRAM2_BASEADDR				0x2001C000U				/*!< This is Base Address of SRAM2 memory  */
#define ROM_BASEADDR				0x1FFF0000U				/*!< This is Base Address of ROM memory  */
#define SRAM 						SRAM1_BASEADDR

/********************************************************************
 * AHBx and APBx Bus Peripheral base addresses
 *******************************************************************/

#define PERIPH_BASEADDR		   		0x40000000U
#define APB1PERIPH_BASEADDR	   		PERIPH_BASEADDR
#define APB2PERIPH_BASEADDR			0x40010000U
#define AHB1PERIPH_BASEADDR			0x40020000U
#define AHB2PERIPH_BASEADDR			0x50000000U


/********************************************************************
 * Base addresses of peripherals which are hanging on AHB1 bus
 *******************************************************************/

#define GPIOA_BASEADDR				(AHB1PERIPH_BASEADDR + 0x0000)
#define GPIOB_BASEADDR				(AHB1PERIPH_BASEADDR + 0x0400)
#define GPIOC_BASEADDR				(AHB1PERIPH_BASEADDR + 0x0800)
#define GPIOD_BASEADDR				(AHB1PERIPH_BASEADDR + 0x0C00)
#define GPIOE_BASEADDR				(AHB1PERIPH_BASEADDR + 0x1000)
#define GPIOF_BASEADDR				(AHB1PERIPH_BASEADDR + 0x1400)
#define GPIOG_BASEADDR				(AHB1PERIPH_BASEADDR + 0x1800)
#define GPIOH_BASEADDR				(AHB1PERIPH_BASEADDR + 0x1C00)

#define RCC_BASEADDR				(AHB1PERIPH_BASEADDR + 0x3800)

/********************************************************************
 * Base addresses of peripherals which are hanging on APB1 bus
 *******************************************************************/

#define I2C1_BASEADDR				(APB1PERIPH_BASEADDR + 0x5400)
#define I2C2_BASEADDR				(APB1PERIPH_BASEADDR + 0x5800)
#define I2C3_BASEADDR				(APB1PERIPH_BASEADDR + 0x5C00)

#define SPI2_BASEADDR				(APB1PERIPH_BASEADDR + 0x3800)
#define SPI3_BASEADDR				(APB1PERIPH_BASEADDR + 0x3C00)

#define USART2_BASEADDR				(APB1PERIPH_BASEADDR + 0x4400)
#define USART3_BASEADDR				(APB1PERIPH_BASEADDR + 0x4800)
#define UART4_BASEADDR				(APB1PERIPH_BASEADDR + 0x4C00)
#define UART5_BASEADDR				(APB1PERIPH_BASEADDR + 0x5000)

/********************************************************************
 * Base addresses of peripherals which are hanging on APB2 bus
 *******************************************************************/

#define SPI1_BASEADDR				(APB2PERIPH_BASEADDR + 0x3000)
#define SPI4_BASEADDR				(APB2PERIPH_BASEADDR + 0x3400)

#define USART1_BASEADDR				(APB2PERIPH_BASEADDR + 0x1000)
#define USART6_BASEADDR				(APB2PERIPH_BASEADDR + 0x1400)

#define SYSCFG_BASEADDR				(APB2PERIPH_BASEADDR + 0x3800)
#define EXTI_BASEADDR				(APB2PERIPH_BASEADDR + 0x3C00)

//used by BL_GET_CID 
#define DBGMCU_BASE_ADDRESS         0xE0042000UL

#define DBGMCU_IDCODE_DEV_ID_MASK   0x0FFFU

//Flash Base Address
#define FLASH_REG_BASE_ADDRESS      0x40023C00UL

/********************************************************************
 * Peripheral register definition structure for GPIO
 *******************************************************************/

typedef struct
{
	__vo uint32_t MODER;             	/*!< GPIO port mode register, 		     Address offset: 0x00 */
	__vo uint32_t OTYPER;              /*!< GPIO port output type register,  		Address offset: 0x04 */
	__vo uint32_t OSPEEDR;			/*!< GPIO port output speed register, 		Address offset: 0x08 */
	__vo uint32_t PUPDR;			/*!< GPIO port pull-up/pull-down register,   Address offset: 0x0C */
	__vo uint32_t IDR;				/*!< GPIO port input data register, 		Address offset: 0x10 */
	__vo uint32_t ODR;				/*!< GPIO port output data register, 		Address offset: 0x14 */
	__vo uint32_t BSRR;				/*!< GPIO port bit set/reset register, 		Address offset: 0x18 */
	__vo uint32_t LCKR;				/*!< GPIO port configuration lock register,  Address offset: 0x1C */
	__vo uint32_t AFR[2];			/*!< AFR[0] : GPIO alternate function low register, AF[1] : GPIO alternate function high register Address offset: 0x20-0x24 */

}GPIO_RegDef_t;

/********************************************************************
 * Peripheral register definition structure for RCC
 *******************************************************************/

typedef struct
{
  __vo uint32_t CR;            /*!< TODO,     										Address offset: 0x00 */
  __vo uint32_t PLLCFGR;       /*!< TODO,     										Address offset: 0x04 */
  __vo uint32_t CFGR;          /*!< TODO,     										Address offset: 0x08 */
  __vo uint32_t CIR;           /*!< TODO,     										Address offset: 0x0C */
  __vo uint32_t AHB1RSTR;      /*!< TODO,     										Address offset: 0x10 */
  __vo uint32_t AHB2RSTR;      /*!< TODO,     										Address offset: 0x14 */
  __vo uint32_t AHB3RSTR;      /*!< TODO,     										Address offset: 0x18 */
       uint32_t RESERVED0;     /*!< Reserved, 0x1C                                                       */
  __vo uint32_t APB1RSTR;      /*!< TODO,     										Address offset: 0x20 */
  __vo uint32_t APB2RSTR;      /*!< TODO,     										Address offset: 0x24 */
       uint32_t RESERVED1[2];  /*!< Reserved, 0x28-0x2C                                                  */
  __vo uint32_t AHB1ENR;       /*!< TODO,     										Address offset: 0x30 */
  __vo uint32_t AHB2ENR;       /*!< TODO,     										Address offset: 0x34 */
  __vo uint32_t AHB3ENR;       /*!< TODO,     										Address offset: 0x38 */
       uint32_t RESERVED2;     /*!< Reserved, 0x3C                                                       */
  __vo uint32_t APB1ENR;       /*!< TODO,     										Address offset: 0x40 */
  __vo uint32_t APB2ENR;       /*!< TODO,     										Address offset: 0x44 */
       uint32_t RESERVED3[2];  /*!< Reserved, 0x48-0x4C                                                  */
  __vo uint32_t AHB1LPENR;     /*!< TODO,     										Address offset: 0x50 */
  __vo uint32_t AHB2LPENR;     /*!< TODO,     										Address offset: 0x54 */
  __vo uint32_t AHB3LPENR;     /*!< TODO,     										Address offset: 0x58 */
       uint32_t RESERVED4;     /*!< Reserved, 0x5C                                                       */
  __vo uint32_t APB1LPENR;     /*!< TODO,     										Address offset: 0x60 */
  __vo uint32_t APB2LPENR;     /*!< RTODO,     										Address offset: 0x64 */
       uint32_t RESERVED5[2];  /*!< Reserved, 0x68-0x6C                                                  */
  __vo uint32_t BDCR;          /*!< TODO,     										Address offset: 0x70 */
  __vo uint32_t CSR;           /*!< TODO,     										Address offset: 0x74 */
       uint32_t RESERVED6[2];  /*!< Reserved, 0x78-0x7C                                                  */
  __vo uint32_t SSCGR;         /*!< TODO,     										Address offset: 0x80 */
  __vo uint32_t PLLI2SCFGR;    /*!< TODO,     										Address offset: 0x84 */
  __vo uint32_t PLLSAICFGR;    /*!< TODO,     										Address offset: 0x88 */
  __vo uint32_t DCKCFGR;       /*!< TODO,     										Address offset: 0x8C */
  __vo uint32_t CKGATENR;      /*!< TODO,     										Address offset: 0x90 */
  __vo uint32_t DCKCFGR2;      /*!< TODO,     										Address offset: 0x94 */

} RCC_RegDef_t;

/********************************************************************
 * Peripheral register definition structure for EXTI
 *******************************************************************/

typedef struct
{
	__vo uint32_t IMR;		/*!< Interrupt mask register (EXTI_IMR), 		   Address Offset = 0x00 */
	__vo uint32_t EMR;		/*!< Event mask register (EXTI_EMR), 		        Address Offset = 0x04 */
	__vo uint32_t RTSR;		/*!< Rising trigger selection register (EXTI_RTSR),  Address Offset = 0x08 */
	__vo uint32_t FTSR;		/*!< Falling trigger selection register (EXTI_FTSR), Address Offset = 0x0C */
	__vo uint32_t SWIER;	/*!< Software interrupt event register (EXTI_SWIER), Address Offset = 0x10 */
	__vo uint32_t PR;		/*!< Pending register (EXTI_PR),                     Address Offset = 0x14 */

}EXTI_RegDef_t;

/********************************************************************
 * Peripheral register definition structure for SYSCFG
 *******************************************************************/

typedef struct
{
    __vo uint32_t MEMRMP;        /*!< Offset: 0x00 */
    __vo uint32_t PMC;           /*!< Offset: 0x04 */
    __vo uint32_t EXTICR[4];     /*!< Offset: 0x08 - 0x14 */
    __vo uint32_t CMPCR;         /*!< Offset: 0x20 */
    uint32_t RESERVED2[2];       /*!< RESERVED	Address Offset = 0x24 to 0x28 */
    __vo uint32_t CFGR;          /*!< Offset: 0x2C */
    
}SYSCFG_RegDef_t;

/********************************************************************
 * Peripheral Register Definition structure for USART
 *******************************************************************/

typedef struct
{
	__vo uint32_t SR;				/*!< USART Status register ,   Address Offset = 0x00 */
	__vo uint32_t DR;				/*!< USART Data register,      Address Offset = 0x04 */
	__vo uint32_t BRR;				/*!< USART Baud rate register, Address Offset = 0x08 */
	__vo uint32_t CR1;				/*!< USART Control register 1, Address Offset = 0x0C */
	__vo uint32_t CR2;				/*!< USART Control register 2, Address Offset = 0x10 */
	__vo uint32_t CR3;				/*!< USART Control register 3, Address Offset = 0x14 */
	__vo uint32_t GTPR;				/*!< USART Guard time and prescaler register, Address Offset = 0x18 */
}USART_RegDef_t;

/********************************************************************
 * Peripheral Register Definition structure for DBGMCU : used by BL_GET_CID
 *******************************************************************/

typedef struct
{
    volatile uint32_t IDCODE;      /* Offset 0x00 */
    volatile uint32_t CR;          /* Offset 0x04 */
    volatile uint32_t APB1FZ;      /* Offset 0x08 */
    volatile uint32_t APB2FZ;      /* Offset 0x0C */
} DBGMCU_RegDef_t;

/********************************************************************
 * Peripheral Register Definition structure for FLASH
 *******************************************************************/

typedef struct
{
    volatile uint32_t ACR;        /* 0x00 */
    volatile uint32_t KEYR;       /* 0x04 */
    volatile uint32_t OPTKEYR;    /* 0x08 */
    volatile uint32_t SR;         /* 0x0C */
    volatile uint32_t CR;         /* 0x10 */
    volatile uint32_t OPTCR;      /* 0x14 */
} FLASH_RegDef_t;

/********************************************************************
 * peripheral definitions ( Peripheral base addresses typecasted to xxx_RegDef_t)
 *******************************************************************/

#define GPIOA  				        ((GPIO_RegDef_t*)GPIOA_BASEADDR)
#define GPIOB  				        ((GPIO_RegDef_t*)GPIOB_BASEADDR)
#define GPIOC  				        ((GPIO_RegDef_t*)GPIOC_BASEADDR)
#define GPIOD  				        ((GPIO_RegDef_t*)GPIOD_BASEADDR)
#define GPIOE  				        ((GPIO_RegDef_t*)GPIOE_BASEADDR)
#define GPIOF  				        ((GPIO_RegDef_t*)GPIOF_BASEADDR)
#define GPIOG  				        ((GPIO_RegDef_t*)GPIOG_BASEADDR)
#define GPIOH  				        ((GPIO_RegDef_t*)GPIOH_BASEADDR)

#define RCC 				        ((RCC_RegDef_t*)RCC_BASEADDR)

#define EXTI                        ((EXTI_RegDef_t*)EXTI_BASEADDR)
#define SYSCFG                      ((SYSCFG_RegDef_t*)SYSCFG_BASEADDR)  

#define USART1		                ( (USART_RegDef_t*) USART1_BASEADDR )
#define USART2		                ( (USART_RegDef_t*) USART2_BASEADDR )
#define USART3		                ( (USART_RegDef_t*) USART3_BASEADDR )
#define UART4		                ( (USART_RegDef_t*) UART4_BASEADDR  )
#define UART5		                ( (USART_RegDef_t*) UART5_BASEADDR  )
#define USART6		                ( (USART_RegDef_t*) USART6_BASEADDR )

//used by BL_GET_CID
#define DBGMCU                      ((DBGMCU_RegDef_t *)DBGMCU_BASE_ADDRESS)

#define FLASH                       ((FLASH_RegDef_t *)FLASH_REG_BASE_ADDRESS)

/********************************************************************
 * Clock Enable Macros for GPIOx peripherals
 ********************************************************************/

#define GPIOA_PCLK_EN()		( RCC->AHB1ENR |= (1 << 0) )
#define GPIOB_PCLK_EN()		( RCC->AHB1ENR |= (1 << 1) )
#define GPIOC_PCLK_EN()		( RCC->AHB1ENR |= (1 << 2) )
#define GPIOD_PCLK_EN()		( RCC->AHB1ENR |= (1 << 3) )
#define GPIOE_PCLK_EN()		( RCC->AHB1ENR |= (1 << 4) )
#define GPIOF_PCLK_EN()		( RCC->AHB1ENR |= (1 << 5) )
#define GPIOG_PCLK_EN()		( RCC->AHB1ENR |= (1 << 6) )
#define GPIOH_PCLK_EN()		( RCC->AHB1ENR |= (1 << 7) )

/********************************************************************
 * Clock Enable Macros for SYSCFG Peripherals
********************************************************************/

#define SYSCFG_PCLK_EN()		(RCC->APB2ENR |= (1 << 14))

/********************************************************************
 * Clock Enable Macros for USARTx peripherals
 ********************************************************************/

#define USART1_PCLK_EN()		(RCC->APB2ENR |= (1 << 4))
#define USART2_PCLK_EN()		(RCC->APB1ENR |= (1 << 17))
#define USART3_PCLK_EN()		(RCC->APB1ENR |= (1 << 18))
#define UART4_PCLK_EN()		    (RCC->APB1ENR |= (1 << 19))
#define UART5_PCLK_EN()		    (RCC->APB1ENR |= (1 << 20))
#define USART6_PCLK_EN()		(RCC->APB2ENR |= (1 << 5))


/********************************************************************
 * Clock Disable Macros for GPIOx peripherals
 ********************************************************************/

#define GPIOA_PCLK_DI()		( RCC->AHB1ENR &= ~(1 << 0) )
#define GPIOB_PCLK_DI()		( RCC->AHB1ENR &= ~(1 << 1) )
#define GPIOC_PCLK_DI()		( RCC->AHB1ENR &= ~(1 << 2) )
#define GPIOD_PCLK_DI()		( RCC->AHB1ENR &= ~(1 << 3) )
#define GPIOE_PCLK_DI()		( RCC->AHB1ENR &= ~(1 << 4) )
#define GPIOF_PCLK_DI()		( RCC->AHB1ENR &= ~(1 << 5) )
#define GPIOG_PCLK_DI()		( RCC->AHB1ENR &= ~(1 << 6) )
#define GPIOH_PCLK_DI()		( RCC->AHB1ENR &= ~(1 << 7) )

/********************************************************************
 * Clock Disable Macros for SYSCFG Peripherals
********************************************************************/

#define SYSCFG_PCLK_DI()		(RCC->APB2ENR &= ~(1 << 14))

/********************************************************************
 * Clock Disable Macros for USARTx Peripherals
 ********************************************************************/

#define USART1_PCLK_DI()		(RCC->APB2ENR &= ~(1 << 4))
#define USART2_PCLK_DI()		(RCC->APB1ENR &= ~(1 << 17))
#define USART3_PCLK_DI()		(RCC->APB1ENR &= ~(1 << 18))
#define UART4_PCLK_DI()		(RCC->APB1ENR &= ~(1 << 19))
#define UART5_PCLK_DI()		(RCC->APB1ENR &= ~(1 << 20))
#define USART6_PCLK_DI()		(RCC->APB2ENR &= ~(1 << 5))

/********************************************************************
 * Macros to reset GPIOx peripherals
 ********************************************************************/
#define GPIOA_REG_RESET()			do{ (RCC->AHB1RSTR |= (1 << 0)); (RCC->AHB1RSTR &= ~(1 << 0)); }while(0)
#define GPIOB_REG_RESET()               do{ (RCC->AHB1RSTR |= (1 << 1)); (RCC->AHB1RSTR &= ~(1 << 1)); }while(0)
#define GPIOC_REG_RESET()               do{ (RCC->AHB1RSTR |= (1 << 2)); (RCC->AHB1RSTR &= ~(1 << 2)); }while(0)
#define GPIOD_REG_RESET()               do{ (RCC->AHB1RSTR |= (1 << 3)); (RCC->AHB1RSTR &= ~(1 << 3)); }while(0)
#define GPIOE_REG_RESET()               do{ (RCC->AHB1RSTR |= (1 << 4)); (RCC->AHB1RSTR &= ~(1 << 4)); }while(0)
#define GPIOF_REG_RESET()               do{ (RCC->AHB1RSTR |= (1 << 5)); (RCC->AHB1RSTR &= ~(1 << 5)); }while(0)
#define GPIOG_REG_RESET()               do{ (RCC->AHB1RSTR |= (1 << 6)); (RCC->AHB1RSTR &= ~(1 << 6)); }while(0)
#define GPIOH_REG_RESET()               do{ (RCC->AHB1RSTR |= (1 << 7)); (RCC->AHB1RSTR &= ~(1 << 7)); }while(0)

/********************************************************************
 * Macros to reset USARTx peripherals
 ********************************************************************/
#define USART1_REG_RESET()              do{ (RCC->APB2RSTR |= (1 << 4));  (RCC->APB2RSTR &= ~(1 << 4)); }while(0)
#define USART2_REG_RESET()              do{ (RCC->APB1RSTR |= (1 << 17)); (RCC->APB1RSTR &= ~(1 << 17)); }while(0)
#define USART3_REG_RESET()              do{ (RCC->APB1RSTR |= (1 << 18)); (RCC->APB1RSTR &= ~(1 << 18)); }while(0)
#define UART4_REG_RESET()               do{ (RCC->APB1RSTR |= (1 << 19)); (RCC->APB1RSTR &= ~(1 << 19)); }while(0)
#define UART5_REG_RESET()               do{ (RCC->APB1RSTR |= (1 << 20)); (RCC->APB1RSTR &= ~(1 << 20)); }while(0)
#define USART6_REG_RESET()              do{ (RCC->APB2RSTR |= (1 << 5));  (RCC->APB2RSTR &= ~(1 << 5)); }while(0)

/********************************************************************
 * USART CR1 Bit Definitions
 ********************************************************************/
#define USART_CR1_SBK       0U
#define USART_CR1_RWU       1U
#define USART_CR1_RE        2U
#define USART_CR1_TE        3U
#define USART_CR1_IDLEIE    4U
#define USART_CR1_RXNEIE    5U
#define USART_CR1_TCIE      6U
#define USART_CR1_TXEIE     7U
#define USART_CR1_PEIE      8U
#define USART_CR1_PS        9U
#define USART_CR1_PCE       10U
#define USART_CR1_WAKE      11U
#define USART_CR1_M         12U
#define USART_CR1_UE        13U
#define USART_CR1_OVER8     15U

/********************************************************************
 * USART CR2 Bit Definitions
 ********************************************************************/
#define USART_CR2_STOP      12U

/********************************************************************
 * USART CR3 Bit Definitions
 ********************************************************************/
#define USART_CR3_RTSE      8U
#define USART_CR3_CTSE      9U
#define USART_CR3_DMAT      7U
#define USART_CR3_DMAR      6U

/********************************************************************
 * USART SR Bit Definitions
 ********************************************************************/
#define USART_SR_PE         0U
#define USART_SR_FE         1U
#define USART_SR_NF         2U
#define USART_SR_ORE        3U
#define USART_SR_IDLE       4U
#define USART_SR_RXNE       5U
#define USART_SR_TC         6U
#define USART_SR_TXE        7U
#define USART_SR_LBD        8U
#define USART_SR_CTS        9U

/********************************************************************
 * FLASH CR Bit Definitions
 ********************************************************************/
#define FLASH_CR_PG         0U
#define FLASH_CR_SER        1U
#define FLASH_CR_MER        2U
#define FLASH_CR_SNB        3U
#define FLASH_CR_PSIZE      8U
#define FLASH_CR_STRT       16U
#define FLASH_CR_LOCK       31U

/********************************************************************
 * FLASH SR Bit Definitions
 ********************************************************************/
#define FLASH_SR_EOP        0U
#define FLASH_SR_OPERR      1U
#define FLASH_SR_WRPERR     4U
#define FLASH_SR_PGAERR     5U
#define FLASH_SR_PGPERR     6U
#define FLASH_SR_PGSERR     7U
#define FLASH_SR_RDERR      8U
#define FLASH_SR_BSY        16U

/********************************************************************
 * FLASH OPTCR Bit Definitions
 ********************************************************************/
#define FLASH_OPTCR_RDP     8U

//FLASH RDP Level Values
#define RDP_LEVEL_0_VALUE         0xAAU
#define RDP_LEVEL_2_VALUE         0xCCU

/********************************************************************
 * Macros for GPIO IRQ Numbers
 ********************************************************************/
#define IRQ_NO_EXTI0         6U
#define IRQ_NO_EXTI1         7U
#define IRQ_NO_EXTI2         8U
#define IRQ_NO_EXTI3         9U
#define IRQ_NO_EXTI4         10U
#define IRQ_NO_EXTI9_5       23U
#define IRQ_NO_EXTI15_10     40U

//other macros
#define ENABLE                1U
#define DISABLE               0U

#define FLAG_SET              ENABLE
#define FLAG_RESET            DISABLE

#include "stm32f446xx_gpio_driver.h"
#include "stm32f446xx_usart_driver.h"
#include "stm32f446xx_rcc_driver.h"
#include "stm32f446xx_flash_driver.h"

#endif
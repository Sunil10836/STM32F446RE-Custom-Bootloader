
/**********************************************************************
 * @file    stm32f466xx_flash_driver.h
 * @brief   FLASH Driver Header for STM32F446RE
 * @author  Sunil Sutar
 * @version 1.0
 * @date    28 Sep 2026
 **********************************************************************/

#ifndef STM32F446XX_FLASH_DRIVER_H
#define STM32F446XX_FLASH_DRIVER_H

#include "stm32f446xx.h"

typedef enum
{
    FLASH_PROGRAM_BYTE        = 0x00U,  /* PSIZE = 00: x8  */
    FLASH_PROGRAM_HALF_WORD   = 0x01U,  /* PSIZE = 01: x16 */
    FLASH_PROGRAM_WORD        = 0x02U,  /* PSIZE = 10: x32 */
    FLASH_PROGRAM_DOUBLE_WORD = 0x03U   /* PSIZE = 11: x64 */

} FLASH_ProgramSize_t;

typedef enum
{
    FLASH_VOLTAGE_RANGE_1 = 0,
    FLASH_VOLTAGE_RANGE_2,
    FLASH_VOLTAGE_RANGE_3,
    FLASH_VOLTAGE_RANGE_4

} FLASH_VoltageRange_t;

typedef enum
{
    FLASH_STATE_RESET = 0,
    FLASH_STATE_READY,
    FLASH_STATE_BUSY,
    FLASH_STATE_ERROR

} FLASH_State_t;

typedef enum
{
    FLASH_SECTOR_0 = 0,
    FLASH_SECTOR_1,
    FLASH_SECTOR_2,
    FLASH_SECTOR_3,
    FLASH_SECTOR_4,
    FLASH_SECTOR_5,
    FLASH_SECTOR_6,
    FLASH_SECTOR_7
    
} FLASH_Sector_t;

typedef enum
{
    FLASH_STATUS_OK = 0,
    FLASH_STATUS_BUSY,
    FLASH_STATUS_ERROR,
    FLASH_STATUS_LOCKED,
    FLASH_STATUS_INVALID_ADDRESS,
    FLASH_STATUS_INVALID_SECTOR,
    FLASH_STATUS_ALIGNMENT_ERROR,
    FLASH_STATUS_TIMEOUT,
    FLASH_STATUS_PROTECTED
    
} FLASH_Status_t;

/**********************************************************************
 * FLASH Configuration Structure
 *********************************************************************/
typedef struct
{
    FLASH_ProgramSize_t    ProgramSize;
    FLASH_VoltageRange_t   VoltageRange;

} FLASH_Config_t;

/**********************************************************************
 * FLASH Handle Structure
 *********************************************************************/
typedef struct
{
    FLASH_RegDef_t *pFLASH;

    FLASH_Config_t Config;

    FLASH_State_t State;

    uint32_t ErrorCode;

} FLASH_Handle_t;

/*
 * ============================================================
 * FLASH Memory Address
 * ============================================================
 */

#define FLASH_START_ADDRESS                0x08000000UL
#define FLASH_END_ADDRESS                  0x0807FFFFUL

/* Flash keys */
#define FLASH_KEY1              0x45670123U
#define FLASH_KEY2              0xCDEF89ABU

#define FLASH_TIMEOUT_VALUE     1000000U

/* Application : Sector 2 to Sector 7 */
#define FLASH_APPLICATION_START_ADDRESS    0x08008000UL
#define FLASH_APPLICATION_END_ADDRESS      0x0807FFFFUL

#define FLASH_FIRST_APPLICATION_SECTOR    2U
#define FLASH_LAST_APPLICATION_SECTOR     7U


/* FLASH Error Flags */
#define FLASH_ERROR_NONE         0x00000000U
#define FLASH_ERROR_OPERR        (1U << FLASH_SR_OPERR)
#define FLASH_ERROR_WRPERR       (1U << FLASH_SR_WRPERR)
#define FLASH_ERROR_PGAERR       (1U << FLASH_SR_PGAERR)
#define FLASH_ERROR_PGPERR       (1U << FLASH_SR_PGPERR)
#define FLASH_ERROR_PGSERR       (1U << FLASH_SR_PGSERR)
#define FLASH_ERROR_RDERR        (1U << FLASH_SR_RDERR)

#define FLASH_ERROR_MASK        (FLASH_ERROR_OPERR  | \
                                 FLASH_ERROR_WRPERR | \
                                 FLASH_ERROR_PGAERR | \
                                 FLASH_ERROR_PGPERR | \
                                 FLASH_ERROR_PGSERR | \
                                 FLASH_ERROR_RDERR    \
                                 )


#define FLASH_CR_ERASE_MASK ((1U << FLASH_CR_PG)  | \
                             (1U << FLASH_CR_SER) | \
                             (1U << FLASH_CR_MER) | \
                             (0xFU << FLASH_CR_SNB))

/* Function prototypes */

void FLASH_Init(FLASH_Handle_t *pFLASHHandle);

void FLASH_DeInit(FLASH_Handle_t *pFLASHHandle);

void FLASH_Unlock(FLASH_Handle_t *pFLASHHandle);

void FLASH_Lock(FLASH_Handle_t *pFLASHHandle);

uint8_t FLASH_IsLocked(FLASH_Handle_t *pFLASHHandle);

uint8_t FLASH_GetBusyStatus(FLASH_Handle_t *pFLASHHandle);

FLASH_Status_t FLASH_GetStatus(FLASH_Handle_t *pFLASHHandle);

void FLASH_ClearFlags(FLASH_Handle_t *pFLASHHandle);

uint32_t FLASH_GetError(FLASH_Handle_t *pFLASHHandle);

FLASH_Status_t FLASH_WaitForOperation(FLASH_Handle_t *pFLASHHandle);

FLASH_Status_t FLASH_EraseSector(FLASH_Handle_t *pFLASHHandle, FLASH_Sector_t Sector);

FLASH_Status_t FLASH_ProgramWord(FLASH_Handle_t *pFLASHHandle, uint32_t Address, uint32_t Data);

uint32_t FLASH_ReadWord(FLASH_Handle_t *pFLASHHandle, uint32_t Address);

FLASH_Status_t FLASH_ProgramBuffer(FLASH_Handle_t *pFLASHHandle, uint32_t Address, const uint32_t *pData, uint32_t Length);

FLASH_Status_t FLASH_VerifyBuffer(FLASH_Handle_t *pFLASHHandle, uint32_t Address, const uint32_t *pData, uint32_t Length);

FLASH_Status_t FLASH_ProgramBytes(FLASH_Handle_t *pFLASHHandle, uint32_t Address, const uint8_t *pData, uint32_t Length);

FLASH_Status_t FLASH_VerifyBytes(FLASH_Handle_t *pFLASHHandle, uint32_t Address, const uint8_t *pData, uint32_t Length);

FLASH_Status_t FLASH_ProgramAndVerifyBytes(FLASH_Handle_t *pFLASHHandle, uint32_t Address, const uint8_t *pData, uint32_t Length);

#endif  /* STM32F446XX_FLASH_DRIVER_H */
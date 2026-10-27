
#include "stm32f446xx_flash_driver.h"
#include "uart.h"

static FLASH_Status_t FLASH_ValidateApplicationAddress(uint32_t Address);
static FLASH_Status_t FLASH_ValidateApplicationRange(uint32_t Address, uint32_t Length);

void FLASH_Init(FLASH_Handle_t *pFLASHHandle)
{
    if(pFLASHHandle == NULL)
    {
        return;
    }

    pFLASHHandle->pFLASH = (FLASH_RegDef_t *)FLASH_REG_BASE_ADDRESS;

    pFLASHHandle->Config.ProgramSize = FLASH_PROGRAM_WORD;

    pFLASHHandle->Config.VoltageRange = FLASH_VOLTAGE_RANGE_3;

    pFLASHHandle->State = FLASH_STATE_READY;

    pFLASHHandle->ErrorCode = FLASH_ERROR_NONE;

}

void FLASH_DeInit(FLASH_Handle_t *pFLASHHandle)
{

}

void FLASH_Unlock(FLASH_Handle_t *pFLASHHandle)
{
    if(pFLASHHandle == NULL)
    {
        return;
    }

    if(pFLASHHandle->pFLASH->CR & (1U << FLASH_CR_LOCK))
    {
        pFLASHHandle->pFLASH->KEYR = FLASH_KEY1;
        pFLASHHandle->pFLASH->KEYR = FLASH_KEY2;
    }

    /* Verify unlock */
    if (pFLASHHandle->pFLASH->CR & (1U << FLASH_CR_LOCK))
    {
        /* Unlock failed */
        pFLASHHandle->ErrorCode = FLASH_ERROR_OPERR;
    }
}

void FLASH_Lock(FLASH_Handle_t *pFLASHHandle)
{
    if(pFLASHHandle == NULL)
    {
        return;
    }

    pFLASHHandle->pFLASH->CR |= (1U << FLASH_CR_LOCK);
}

uint8_t FLASH_IsLocked(FLASH_Handle_t *pFLASHHandle)
{
    if(pFLASHHandle == NULL)
    {
        return 1U;
    }

    if(pFLASHHandle->pFLASH->CR & (1U << FLASH_CR_LOCK))
    {
        return 1U;
    }

    return 0U;
}

FLASH_Status_t FLASH_GetStatus(FLASH_Handle_t *pFLASHHandle)
{
    uint32_t status;

    if(pFLASHHandle == NULL)
    {
        return FLASH_STATUS_ERROR;
    }

    status = pFLASHHandle->pFLASH->SR;

    if(status & FLASH_SR_BSY)
    {
        return FLASH_STATUS_BUSY;
    }

    if(status & FLASH_ERROR_MASK)
    {
        return FLASH_STATUS_ERROR;
    }

    return FLASH_STATUS_OK;
}

void FLASH_ClearFlags(FLASH_Handle_t *pFLASHHandle)
{
    if(pFLASHHandle == NULL)
    {
        return;
    }

    pFLASHHandle->pFLASH->SR = ((1U << FLASH_SR_EOP) | FLASH_ERROR_MASK);
}

uint8_t FLASH_GetBusyStatus(FLASH_Handle_t *pFLASHHandle)
{
    if(pFLASHHandle == NULL)
    {
        return 1U;
    }

    if(pFLASHHandle->pFLASH->SR & FLASH_SR_BSY)
    {
        return 1U;
    }

    return 0U;
}

uint32_t FLASH_GetError(FLASH_Handle_t *pFLASHHandle)
{
    if(pFLASHHandle == NULL)
    {
        return FLASH_ERROR_OPERR;
    }

    return (pFLASHHandle->pFLASH->SR & FLASH_ERROR_MASK);
}

FLASH_Status_t FLASH_WaitForOperation(FLASH_Handle_t *pFLASHHandle)
{
    uint32_t timeout = FLASH_TIMEOUT_VALUE;

    if(pFLASHHandle == NULL)
    {
        return FLASH_STATUS_ERROR;
    }

    pFLASHHandle->State = FLASH_STATE_BUSY;

    /*
     * Wait until BSY becomes 0
     */
    while (pFLASHHandle->pFLASH->SR & (1U << FLASH_SR_BSY))
    {
        if (timeout == 0U)
        {
            pFLASHHandle->State = FLASH_STATE_ERROR;

            return FLASH_STATUS_TIMEOUT;
        }

        timeout--;
    }

    /*
     * Check Flash errors
     */
    if (pFLASHHandle->pFLASH->SR & FLASH_ERROR_MASK)
    {
        pFLASHHandle->ErrorCode = pFLASHHandle->pFLASH->SR & FLASH_ERROR_MASK;

        pFLASHHandle->State = FLASH_STATE_ERROR;

        return FLASH_STATUS_ERROR;
    }

    /*
     * Operation successful
     */
    pFLASHHandle->State = FLASH_STATE_READY;

    return FLASH_STATUS_OK;
}

FLASH_Status_t FLASH_GetSector( uint32_t Address, FLASH_Sector_t *pSector)
{
    if (pSector == NULL)
    {
        return FLASH_STATUS_ERROR;
    }

    /*
     * Check Flash address range
     */
    if ((Address < FLASH_START_ADDRESS) || (Address > FLASH_END_ADDRESS))
    {
        return FLASH_STATUS_INVALID_ADDRESS;
    }

    /*
     * Sector 0
     * 0x08000000 - 0x08003FFF
     */
    if (Address < 0x08004000UL)
    {
        *pSector = FLASH_SECTOR_0;
    }


    /*
     * Sector 1
     * 0x08004000 - 0x08007FFF
     */
    else if (Address < 0x08008000UL)
    {
        *pSector = FLASH_SECTOR_1;
    }


    /*
     * Sector 2
     * 0x08008000 - 0x0800BFFF
     */
    else if (Address < 0x0800C000UL)
    {
        *pSector = FLASH_SECTOR_2;
    }


    /*
     * Sector 3
     * 0x0800C000 - 0x0800FFFF
     */
    else if (Address < 0x08010000UL)
    {
        *pSector = FLASH_SECTOR_3;
    }


    /*
     * Sector 4
     * 0x08010000 - 0x0801FFFF
     */
    else if (Address < 0x08020000UL)
    {
        *pSector = FLASH_SECTOR_4;
    }


    /*
     * Sector 5
     * 0x08020000 - 0x0803FFFF
     */
    else if (Address < 0x08040000UL)
    {
        *pSector = FLASH_SECTOR_5;
    }


    /*
     * Sector 6
     * 0x08040000 - 0x0805FFFF
     */
    else if (Address < 0x08060000UL)
    {
        *pSector = FLASH_SECTOR_6;
    }


    /*
     * Sector 7
     * 0x08060000 - 0x0807FFFF
     */
    else
    {
        *pSector = FLASH_SECTOR_7;
    }

    return FLASH_STATUS_OK;
}

static void FLASH_SetProgramSize(FLASH_Handle_t *pFLASHHandle)
{
    uint32_t tempreg = 0;

    tempreg = pFLASHHandle->pFLASH->CR;

    /* Clear existing PSIZE bits */
    tempreg &= ~(0x3U << FLASH_CR_PSIZE);

    /* Set configured PSIZE */
    tempreg |= ((uint32_t)pFLASHHandle->Config.ProgramSize << FLASH_CR_PSIZE);

    pFLASHHandle->pFLASH->CR = tempreg;
}

FLASH_Status_t FLASH_EraseSector(FLASH_Handle_t *pFLASHHandle, FLASH_Sector_t Sector)
{
    uint32_t tempreg = 0;

    /* Validate handle */
    if (pFLASHHandle == NULL)
    {
        return FLASH_STATUS_ERROR;
    }

    /* Validate sector */
    if (Sector > FLASH_SECTOR_7)
    {
        return FLASH_STATUS_INVALID_SECTOR;
    }

    /* Protect bootloader sectors */
    if (Sector < FLASH_FIRST_APPLICATION_SECTOR)
    {
        return FLASH_STATUS_PROTECTED;
    }

    /* Flash must be unlocked */
    if (FLASH_IsLocked(pFLASHHandle))
    {
        return FLASH_STATUS_LOCKED;
    }

    /* Flash must not be busy */
    if (pFLASHHandle->pFLASH->SR & (1U << FLASH_SR_BSY))
    {
        return FLASH_STATUS_BUSY;
    }

    /* Clear old status/error flags */
    FLASH_ClearFlags(pFLASHHandle);

    /* Configure programming parallelism */
    FLASH_SetProgramSize(pFLASHHandle);

    /* Read current CR */
    tempreg = pFLASHHandle->pFLASH->CR;

    /*
     * Clear previous erase configuration
     *
     * PG
     * SER
     * MER
     * SNB
     */
    tempreg &= ~FLASH_CR_ERASE_MASK;

    /* Select sector */
    tempreg |= ((uint32_t)Sector << FLASH_CR_SNB);

    /* Enable sector erase */
    tempreg |= (1U << FLASH_CR_SER);

    /* Write configuration */
    pFLASHHandle->pFLASH->CR = tempreg;

    /* Start erase operation */
    pFLASHHandle->pFLASH->CR |= (1U << FLASH_CR_STRT);

    /* Wait for completion */
    if (FLASH_WaitForOperation(pFLASHHandle) != FLASH_STATUS_OK)
    {
        /* Disable sector erase mode */
        pFLASHHandle->pFLASH->CR &= ~(1U << FLASH_CR_SER);

        return FLASH_STATUS_ERROR;
    }

    /* Disable sector erase mode */
    pFLASHHandle->pFLASH->CR &= ~(1U << FLASH_CR_SER);

    /* Clear EOP and any remaining flags */
    FLASH_ClearFlags(pFLASHHandle);

    return FLASH_STATUS_OK;
}

FLASH_Status_t FLASH_ProgramWord(FLASH_Handle_t *pFLASHHandle, uint32_t Address, uint32_t Data)
{
    /* Validate handle */
    if(pFLASHHandle == NULL)
    {
        return FLASH_STATUS_ERROR;
    }

    /* Flash must be unlocked */
    if(FLASH_IsLocked(pFLASHHandle))
    {
        return FLASH_STATUS_LOCKED;
    }

    /* Validate Flash address */
    if(Address < FLASH_START_ADDRESS || Address > FLASH_END_ADDRESS)
    {
        return FLASH_STATUS_INVALID_ADDRESS;
    }

    /* Address must be 4-byte aligned */
    if((Address % 4U) != 0)
    {
        return FLASH_STATUS_ALIGNMENT_ERROR;
    }

    /* Flash must not be busy */
    if(pFLASHHandle->pFLASH->SR & (1U << FLASH_SR_BSY))
    {
        return FLASH_STATUS_BUSY;
    }

    /* Clear old status/error flags */
    FLASH_ClearFlags(pFLASHHandle);

    /* Configure programming parallelism */
    FLASH_SetProgramSize(pFLASHHandle);

    /* Enable Flash programming */
    pFLASHHandle->pFLASH->CR |= (1U << FLASH_CR_PG);

    /* Program 32-bit word */
    *(volatile uint32_t *)Address = Data;

    /* Wait for programming operation to complete */
    if(FLASH_WaitForOperation(pFLASHHandle) != FLASH_STATUS_OK)
    {
        /* Disable programming */
        pFLASHHandle->pFLASH->CR &= ~(1U << FLASH_CR_PG);

        return FLASH_STATUS_ERROR;
    }

    /* Check Flash error status */
    pFLASHHandle->ErrorCode = FLASH_GetError(pFLASHHandle);

    if(pFLASHHandle->ErrorCode != FLASH_ERROR_NONE)
    {
        /* Disable programming */
        pFLASHHandle->pFLASH->CR &= ~(1U << FLASH_CR_PG);

        return FLASH_STATUS_ERROR;
    }

    /* Disable Flash programming */
    pFLASHHandle->pFLASH->CR &= ~(1U << FLASH_CR_PG);

    /* Clear EOP and error flags */
    FLASH_ClearFlags(pFLASHHandle);

    return FLASH_STATUS_OK;
}

uint32_t FLASH_ReadWord(FLASH_Handle_t *pFLASHHandle, uint32_t Address)
{
    /* Validate handle */
    if(pFLASHHandle == NULL)
    {
        return 0xFFFFFFFFU;
    }

    /* Validate Flash address */
    if(Address < FLASH_START_ADDRESS || Address > FLASH_END_ADDRESS)
    {
        return 0xFFFFFFFFU;
    }

    /* Address must be 4-byte aligned */
    if((Address % 4U) != 0)
    {
        return 0xFFFFFFFFU;
    }

    /* Read 32-bit word from Flash */
    return *(volatile uint32_t *)Address;

}

FLASH_Status_t FLASH_ProgramBuffer(FLASH_Handle_t *pFLASHHandle, uint32_t Address, 
                                                const uint32_t *pData, uint32_t Length)
{
    uint32_t i;
    FLASH_Status_t status;

    /* Validate handle */
    if(pFLASHHandle == NULL)
    {
        return FLASH_STATUS_ERROR;
    }

    /* Validate data pointer */
    if(pData == NULL)
    {
        return FLASH_STATUS_ERROR;
    }

    status = FLASH_ValidateApplicationRange(Address, Length);

    if (status != FLASH_STATUS_OK)
    {
        return status;
    }

    /* Flash must be unlocked */
    if(FLASH_IsLocked(pFLASHHandle))
    {
        return FLASH_STATUS_LOCKED;
    }

    /*
     * Program each 32-bit word
     */
    for (i = 0U; i < Length; i++)
    {
        status = FLASH_ProgramWord(pFLASHHandle, Address, pData[i]);

        if (status != FLASH_STATUS_OK)
        {
            return status;
        }

        Address += 4U;
    }

    return FLASH_STATUS_OK;
}

FLASH_Status_t FLASH_VerifyBuffer(FLASH_Handle_t *pFLASHHandle, uint32_t Address, 
                                    const uint32_t *pData, uint32_t Length)
{
    uint32_t i;
    uint32_t flash_data;
    FLASH_Status_t status;

    /* Validate handle */
    if (pFLASHHandle == NULL)
    {
        return FLASH_STATUS_ERROR;
    }

    /* Validate data pointer */
    if (pData == NULL)
    {
        return FLASH_STATUS_ERROR;
    }

    /* Validate complete application Flash range */
    status = FLASH_ValidateApplicationRange(Address, Length);

    if (status != FLASH_STATUS_OK)
    {
        return status;
    }

    /* Protect bootloader area */
    if (FLASH_ValidateApplicationAddress(Address) != FLASH_STATUS_OK)
    {
        return FLASH_STATUS_PROTECTED;
    }

    /* Flash must be unlocked */
    if(FLASH_IsLocked(pFLASHHandle))
    {
        return FLASH_STATUS_LOCKED;
    }
    
    /*
     * Verify each 32-bit word
     */
    for (i = 0U; i < Length; i++)
    {
        flash_data = FLASH_ReadWord(pFLASHHandle, Address);

        if (flash_data != pData[i])
        {
            return FLASH_STATUS_ERROR;
        }

        Address += 4U;
    }

    return FLASH_STATUS_OK;
}

FLASH_Status_t FLASH_ProgramBytes(FLASH_Handle_t *pFLASHHandle, uint32_t Address,
                                  const uint8_t *pData, uint32_t Length)
{
    uint32_t i;
    uint32_t WordData;
    uint32_t ByteCount;
    FLASH_Status_t status;

    /* Check handle */
    if (pFLASHHandle == NULL)
    {
        return FLASH_STATUS_ERROR;
    }

    /* Check data buffer */
    if (pData == NULL)
    {
        return FLASH_STATUS_ERROR;
    }

    /* Length must not be zero */
    if (Length == 0U)
    {
        return FLASH_STATUS_ERROR;
    }

    /* Address must be 4-byte aligned */
    if ((Address & 0x3U) != 0U)
    {
        return FLASH_STATUS_ALIGNMENT_ERROR;
    }

    /* Flash must be unlocked */
    if (FLASH_IsLocked(pFLASHHandle))
    {
        return FLASH_STATUS_LOCKED;
    }

    /*
     * Application protection
     *
     * Bootloader:
     *   0x08000000 - 0x08007FFF
     *
     * Application:
     *   0x08008000 - 0x0807FFFF
     */
    if (Address < FLASH_APPLICATION_START_ADDRESS)
    {
        return FLASH_STATUS_PROTECTED;
    }

    if (Address > FLASH_APPLICATION_END_ADDRESS)
    {
        return FLASH_STATUS_INVALID_ADDRESS;
    }

    /*
     * Check that the complete byte buffer
     * fits inside application Flash.
     */
    if ((Length - 1U) >
        (FLASH_APPLICATION_END_ADDRESS - Address))
    {
        return FLASH_STATUS_INVALID_ADDRESS;
    }

    /*
     * Program data 4 bytes at a time.
     */
    i = 0U;

    while (i < Length)
    {
        /*
         * Start with erased Flash value.
         *
         * This is important for the final
         * partial word.
         */
        WordData = 0xFFFFFFFFU;

        /*
         * Calculate remaining bytes.
         */
        ByteCount = Length - i;

        if (ByteCount > 4U)
        {
            ByteCount = 4U;
        }

        /*
         * Copy byte 0
         */
        WordData &= 0xFFFFFF00U;
        WordData |= (uint32_t)pData[i];

        /*
         * Copy byte 1
         */
        if (ByteCount > 1U)
        {
            WordData &= 0xFFFF00FFU;
            WordData |= ((uint32_t)pData[i + 1U] << 8U);
        }

        /*
         * Copy byte 2
         */
        if (ByteCount > 2U)
        {
            WordData &= 0xFF00FFFFU;
            WordData |= ((uint32_t)pData[i + 2U] << 16U);
        }

        /*
         * Copy byte 3
         */
        if (ByteCount > 3U)
        {
            WordData &= 0x00FFFFFFU;
            WordData |= ((uint32_t)pData[i + 3U] << 24U);
        }

        /*
         * Program the complete 32-bit word.
         */
        status = FLASH_ProgramWord(pFLASHHandle, Address, WordData);

        if (status != FLASH_STATUS_OK)
        {
            return status;
        }

        /*
         * Move to next Flash word.
         */
        Address += 4U;

        /*
         * Move to next input bytes.
         */
        i += ByteCount;
    }

    return FLASH_STATUS_OK;

}

FLASH_Status_t FLASH_VerifyBytes(FLASH_Handle_t *pFLASHHandle, uint32_t Address, 
                                    const uint8_t *pData, uint32_t Length)
{
    uint32_t i;
    uint8_t FlashByte;

    if (pFLASHHandle == NULL)
    {
        return FLASH_STATUS_ERROR;
    }

    if (pData == NULL)
    {
        return FLASH_STATUS_ERROR;
    }

    if (Length == 0U)
    {
        return FLASH_STATUS_ERROR;
    }

    /*
     * Address must be 4-byte aligned.
     * FLASH_ProgramBytes() currently requires
     * a 32-bit aligned starting address.
     */
    if ((Address & 0x3U) != 0U)
    {
        return FLASH_STATUS_ALIGNMENT_ERROR;
    }

    /*
     * Protect bootloader region.
     */
    if (Address < FLASH_APPLICATION_START_ADDRESS)
    {
        return FLASH_STATUS_PROTECTED;
    }

    if (Address > FLASH_APPLICATION_END_ADDRESS)
    {
        return FLASH_STATUS_INVALID_ADDRESS;
    }

    /*
     * Make sure the complete buffer is
     * inside application Flash.
     */
    if ((Length - 1U) > (FLASH_APPLICATION_END_ADDRESS - Address))
    {
        return FLASH_STATUS_INVALID_ADDRESS;
    }

    /*
     * Compare byte-by-byte.
     */
    for (i = 0U; i < Length; i++)
    {
        FlashByte = *(volatile uint8_t *)Address;

        if (FlashByte != pData[i])
        {
            pFLASHHandle->ErrorCode = FLASH_ERROR_OPERR;
            return FLASH_STATUS_ERROR;
        }

        Address++;
    }

    return FLASH_STATUS_OK;
}

FLASH_Status_t FLASH_ProgramAndVerifyBytes(FLASH_Handle_t *pFLASHHandle, uint32_t Address, 
                                                        const uint8_t *pData, uint32_t Length)
{
    FLASH_Status_t status;

    if (pFLASHHandle == NULL)
    {
        return FLASH_STATUS_ERROR;
    }

    if (pData == NULL)
    {
        return FLASH_STATUS_ERROR;
    }

    if (Length == 0U)
    {
        return FLASH_STATUS_ERROR;
    }

    /*
     * Program firmware bytes
     */
    status = FLASH_ProgramBytes(pFLASHHandle, Address, pData, Length);

    if (status != FLASH_STATUS_OK)
    {
        return status;
    }

    /*
     * Verify programmed firmware
     */
    status = FLASH_VerifyBytes(pFLASHHandle, Address, pData, Length);

    if (status != FLASH_STATUS_OK)
    {
        return status;
    }

    return FLASH_STATUS_OK;

}

static FLASH_Status_t FLASH_ValidateApplicationAddress(uint32_t Address)
{
    if ((Address < FLASH_APPLICATION_START_ADDRESS) || (Address > FLASH_APPLICATION_END_ADDRESS))
    {
        return FLASH_STATUS_PROTECTED;
    }

    return FLASH_STATUS_OK;
}

static FLASH_Status_t FLASH_ValidateApplicationRange(uint32_t Address, uint32_t Length)
{
    uint32_t WordCount;

    if (Length == 0U)
    {
        return FLASH_STATUS_ERROR;
    }

    if((Address % 4U) != 0)
    {
        return FLASH_STATUS_ALIGNMENT_ERROR;
    }

    if (Address < FLASH_APPLICATION_START_ADDRESS)
    {
        return FLASH_STATUS_PROTECTED;
    }

    if (Address > FLASH_APPLICATION_END_ADDRESS)
    {
        return FLASH_STATUS_INVALID_ADDRESS;
    }

    WordCount = Length - 1U;

    if (WordCount > ((FLASH_APPLICATION_END_ADDRESS - Address) / 4U))
    {
        return FLASH_STATUS_INVALID_ADDRESS;
    }

    return FLASH_STATUS_OK;
}
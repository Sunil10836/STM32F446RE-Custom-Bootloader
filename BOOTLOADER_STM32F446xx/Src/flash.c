/******************************************************************************
 * @file    flash.c
 *
 * @author  Sunil Sutar
 *
 * @project STM32F446RE Custom Bootloader Development
 *
 * @date    15 October 2026
 *
 ******************************************************************************
 */

#include <stdio.h>
#include "flash.h"
#include "uart.h"

FLASH_Handle_t FLASHHandle;

void Flash_Test_1(void)
{
    uint8_t locked_before;
    uint8_t locked_after;

    FLASH_Init(&FLASHHandle);

    locked_before = FLASH_IsLocked(&FLASHHandle);

    FLASH_Unlock(&FLASHHandle);

    locked_after = FLASH_IsLocked(&FLASHHandle);

    FLASH_Lock(&FLASHHandle);

    (void)locked_after;
    (void)locked_before;
    
    while (1)
    {
    }

}

/*
void Flash_Test_2(void)
{
    uint8_t locked_before;
    uint8_t locked_after;
    uint32_t cr_before;
    uint32_t cr_after_unlock;
    uint32_t cr_after_lock;
    uint32_t sr;

    FLASH_Init(&FLASHHandle);

    cr_before = FLASHHandle.pFLASH->CR;
    sr = FLASHHandle.pFLASH->SR;

    locked_before = FLASH_IsLocked(&FLASHHandle);

    FLASH_Unlock(&FLASHHandle);

    cr_after_unlock = FLASHHandle.pFLASH->CR;

    locked_after = FLASH_IsLocked(&FLASHHandle);

    FLASH_Lock(&FLASHHandle);

    cr_after_lock = FLASHHandle.pFLASH->CR;

    while (1)
    {
    }
}
*/

void Flash_Erase_Test(void)
{
    FLASH_Status_t status;

    FLASH_Init(&FLASHHandle);

    FLASH_Unlock(&FLASHHandle);

    FLASH_ClearFlags(&FLASHHandle);

    status = FLASH_EraseSector(&FLASHHandle, FLASH_SECTOR_2);

    FLASH_Lock(&FLASHHandle);

    while (1)
    {
    }
}

void FLASH_ProgramWord_Test()
{
    FLASH_Status_t status;

    FLASH_Init(&FLASHHandle);

    FLASH_Unlock(&FLASHHandle);

    status = FLASH_ProgramWord(&FLASHHandle, 0x08008000, 0x12345678);

    FLASH_Lock(&FLASHHandle);

    while (1)
    {

    }
}

void FLASH_ReadWord_Test(void)
{
    uint32_t data;
    FLASH_Status_t status;

    FLASH_Init(&FLASHHandle);

    /* Unlock Flash */
    FLASH_Unlock(&FLASHHandle);

    status = FLASH_ProgramWord(&FLASHHandle, 0x08008000, 0x12345678);

    if (status != FLASH_STATUS_OK)
    {
        printmsg("Flash Program FAILED\r\n");

        while (1)
        {
        }
    }

    /* Lock Flash after programming */
    FLASH_Lock(&FLASHHandle);

    data = FLASH_ReadWord(&FLASHHandle, 0x08008000);

    printf("Flash Data = 0x%08lX\r\n", data);

    if (data == 0x12345678)
    {
        printmsg("Flash Read Verification: PASS\r\n");
    }
    else
    {
        printmsg("Flash Read Verification: FAIL\r\n");
    }

    while (1)
    {

    }
}

void FLASH_ProgramBuffer_Test(void)
{
    uint32_t data[] =
    {
        0x11111111,
        0x22222222,
        0x33333333,
        0x44444444
    };

    FLASH_Status_t status;

    FLASH_Init(&FLASHHandle);

    FLASH_Unlock(&FLASHHandle);

    status = FLASH_ProgramBuffer(&FLASHHandle, 0x08008000, data, 4);

    if (status != FLASH_STATUS_OK)
    {
        printmsg("Flash Buffer Programming FAILED\r\n");

        while (1)
        {
        }
    }

    printmsg("Flash Buffer Programming PASS\r\n");

    status = FLASH_VerifyBuffer(&FLASHHandle, 0x08008000, data, 4);

    FLASH_Lock(&FLASHHandle);

    if (status == FLASH_STATUS_OK)
    {
        printmsg("Flash Buffer Verification PASS\r\n");
    }
    else
    {
        printmsg("Flash Buffer Verification FAILED\r\n");
    }

    while (1)
    {

    }
}

void FLASH_Protection_Test(void)
{
    FLASH_Status_t status;

    FLASH_Init(&FLASHHandle);

    FLASH_Unlock(&FLASHHandle);

    /* Try to erase bootloader sector */
    status = FLASH_EraseSector(&FLASHHandle, FLASH_SECTOR_0);

    if (status == FLASH_STATUS_PROTECTED)
    {
        printmsg("Sector 0 Protection PASS\r\n");
    }
    else
    {
        printmsg("Sector 0 Protection FAILED\r\n");
    }

    /* Try to erase application sector */
    status = FLASH_EraseSector(&FLASHHandle, FLASH_SECTOR_2);

    if (status == FLASH_STATUS_OK)
    {
        printmsg("Sector 2 Erase PASS\r\n");
    }
    else
    {
        printmsg("Sector 2 Erase FAILED\r\n");
    }

    FLASH_Lock(&FLASHHandle);

    while (1)
    {
    }
}

void FLASH_ProgramBytes_Test(void)
{
    uint8_t test_data[10] =
    {
        0x11,
        0x22,
        0x33,
        0x44,
        0x55,
        0x66,
        0x77,
        0x88,
        0x99,
        0xAA
    };

    FLASH_Status_t status;

    FLASH_Init(&FLASHHandle);

    FLASH_Unlock(&FLASHHandle);

    status = FLASH_ProgramBytes(&FLASHHandle, 0x08008000, test_data, 10);

    FLASH_Lock(&FLASHHandle);
}

void FLASH_VerifyBytes_Test(void)
{
    uint8_t test_data[10] =
    {
        0x11,
        0x22,
        0x33,
        0x44,
        0x55,
        0x66,
        0x77,
        0x88,
        0x99,
        0xAB
    };

    volatile FLASH_Status_t status;

    FLASH_Init(&FLASHHandle);

    FLASH_Unlock(&FLASHHandle);

    status = FLASH_VerifyBytes(&FLASHHandle, 0x08008000, test_data, 10);
    
    FLASH_Lock(&FLASHHandle);
}

void FLASH_ProgramAndVerifyBytes_Test(void)
{
    uint8_t test_data[10] =
    {
        0x11,
        0x22,
        0x33,
        0x44,
        0x55,
        0x66,
        0x77,
        0x88,
        0x99,
        0xAA
    };

    volatile FLASH_Status_t status;

    FLASH_Init(&FLASHHandle);

    /*
     * Erase application sector first.
     */
    FLASH_Unlock(&FLASHHandle);

    status = FLASH_EraseSector(&FLASHHandle, FLASH_SECTOR_2);

    if (status != FLASH_STATUS_OK)
    {
        FLASH_Lock(&FLASHHandle);
        return;
    }

    /*
     * Program + Verify
     */
    status = FLASH_ProgramAndVerifyBytes(&FLASHHandle, 0x08008000, test_data, 10);

    FLASH_Lock(&FLASHHandle);
}
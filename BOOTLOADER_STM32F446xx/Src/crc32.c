/******************************************************************************
 * @file    crc32.c
 *
 * @author  Sunil Sutar
 *
 * @project STM32F446RE Custom Bootloader Development
 *
 * @brief   Implements CRC32 calculation and verification functionality for
 *          the STM32F446RE custom bootloader. CRC32 is used to detect
 *          accidental corruption of command packets during UART
 *          communication between the Python host application and the MCU.
 *
 * @date    15 October 2026
 *
 ******************************************************************************
 */

#include "crc32.h"


uint32_t CRC32_Calculate(const uint8_t *data, uint32_t length)
{
    uint32_t crc = 0xFFFFFFFFU;

    if ((data == 0) && (length != 0U))
    {
        return 0U;
    }

    for (uint32_t i = 0U; i < length; i++)
    {
        crc ^= data[i];

        for (uint32_t bit = 0U; bit < 8U; bit++)
        {
            if ((crc & 1U) != 0U)
            {
                crc = (crc >> 1U) ^ 0xEDB88320U;
            }
            else
            {
                crc >>= 1U;
            }
        }
    }

    return crc ^ 0xFFFFFFFFU;
}

void CRC32_Test(void)
{
    const uint8_t test_data[] = "123456789";
    uint32_t crc = CRC32_Calculate(test_data, 9U);

    printmsg("CRC32 = 0x%08lX\r\n", (unsigned long)crc);
}

void CRC32_Firmware_Test(void)
{
    const uint8_t *app_address = (const uint8_t *)FLASH_APPLICATION_START_ADDRESS;

    uint32_t firmware_size = 2488U;

    uint32_t crc = CRC32_Calculate(app_address, firmware_size);

    printmsg("Application CRC32: 0x%08lX\r\n", (unsigned long)crc);
}
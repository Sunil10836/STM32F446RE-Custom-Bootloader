/******************************************************************************
 * @file    crc32.h
 *
 * @author  Sunil Sutar
 *
 * @project STM32F446RE Custom Bootloader Development
 *
 * @date    15 October 2026
 *
 ******************************************************************************
 */

#ifndef CRC32_H
#define CRC32_H

#include <stdint.h>
#include "uart.h"

uint32_t CRC32_Calculate(const uint8_t *data, uint32_t length);

void CRC32_Test(void);

void CRC32_Firmware_Test(void);

#endif /* CRC32_H */
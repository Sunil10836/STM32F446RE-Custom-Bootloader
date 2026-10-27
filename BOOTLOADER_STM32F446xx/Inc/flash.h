/******************************************************************************
 * @file    flash.h
 *
 * @author  Sunil Sutar
 *
 * @project STM32F446RE Custom Bootloader Development
 *
 * @date    15 October 2026
 *
 ******************************************************************************
 */

#ifndef FLASH_H_
#define FLASH_H_

#include "stm32f446xx_flash_driver.h"

void Flash_Test_1(void);
void Flash_Test_2(void);
void Flash_Erase_Test(void);

void FLASH_ProgramWord_Test();
void FLASH_ReadWord_Test();
void FLASH_ProgramBuffer_Test(void);

void FLASH_Protection_Test(void);
void FLASH_ProgramBytes_Test(void);
void FLASH_VerifyBytes_Test(void);
void FLASH_ProgramAndVerifyBytes_Test(void);

#endif /* FLASH_H_ */
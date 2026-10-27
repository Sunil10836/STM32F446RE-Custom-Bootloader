
/******************************************************************************
 * @file    main.h
 *
 * @author  Sunil Sutar
 *
 * @project STM32F446RE Custom Bootloader Development
 *
 * @date    15 October 2026
 *
 ******************************************************************************
 */

#ifndef MAIN_H
#define MAIN_H

void Bootloader_uart_read_data(void);

void Bootloader_jump_to_user_app(void);

#endif

/******************************************************************************
 * @file    bootloader_protocol.h
 *
 * @author  Sunil Sutar
 *
 * @project STM32F446RE Custom Bootloader Development
 *
 * @brief   Defines the communication protocol interface for the STM32F446RE
 *          custom bootloader. Contains protocol-related macros, command IDs,
 *          packet definitions, response constants, and function declarations
 *          required for communication between the Python host application
 *          and the MCU over UART, as applicable to this implementation.
 *
 * @date    15 October 2026
 *
 ******************************************************************************
 */

#ifndef INC_BOOTLOADER_PROTOCOL_H_
#define INC_BOOTLOADER_PROTOCOL_H_

#include <stdint.h>
#include "crc32.h"

/* Bootloader commands */
#define BL_GET_VER              0x51U
#define BL_GET_HELP             0x52U
#define BL_GET_CID              0x53U
#define BL_GET_RDP_STATUS       0x54U
#define BL_GO_TO_ADDR           0x55U
#define BL_FLASH_ERASE          0x56U
#define BL_MEM_WRITE            0x57U
#define BL_EN_R_W_PROTECT       0x58U
#define BL_MEM_READ             0x59U
#define BL_READ_SECTOR_P_STATUS 0x5AU
#define BL_OTP_READ             0x5BU
#define BL_DIS_R_W_PROTECT      0x5CU
#define BL_VERIFY_APP_CRC       0x5DU

/* Bootloader response */
#define BL_ACK                  0xA5U
#define BL_NACK                 0x7FU

/* Bootloader version */
#define BL_VERSION              0x10U

/* Flash Read Protection Level */
#define RDP_LEVEL_0               0x00U
#define RDP_LEVEL_1               0x01U
#define RDP_LEVEL_2               0x02U

#define BL_GO_TO_ADDR_SUCCESS    0x00U
#define BL_GO_TO_ADDR_FAILED     0x01U

#define BL_MEM_READ_MAX_LENGTH    128U

#define BL_VERIFY_APP_CRC_REQUEST_LENGTH    9U

void bootloader_uart_write_data(uint8_t *pBuffer, uint32_t len);
void BL_SendAck(uint8_t command_length, uint8_t response_length);
void BL_SendNack(uint8_t command_length, uint8_t response_length);

void bootloader_handle_getver_cmd(uint8_t *bl_rx_buffer);
void bootloader_handle_gethelp_cmd(uint8_t *bl_rx_buffer);
void bootloader_handle_getcid_cmd(uint8_t *bl_rx_buffer);
void bootloader_handle_getrdp_status_cmd(uint8_t *bl_rx_buffer);
void bootloader_handle_go_to_addr_cmd(uint8_t *bl_rx_buffer);
void bootloader_handle_flash_erase_cmd(uint8_t *bl_rx_buffer);
void bootloader_handle_mem_write_cmd(uint8_t *bl_rx_buffer);
void bootloader_handle_mem_read_cmd(uint8_t *bl_rx_buffer);
void bootloader_handle_verify_app_crc_cmd(uint8_t *bl_rx_buffer);

uint8_t get_bootloader_version(void);
uint16_t get_mcu_chip_id(void);
uint8_t get_mcu_rdp_level(void);
uint8_t verify_address(uint32_t go_address);

#endif /* INC_BOOTLOADER_PROTOCOL_H_ */
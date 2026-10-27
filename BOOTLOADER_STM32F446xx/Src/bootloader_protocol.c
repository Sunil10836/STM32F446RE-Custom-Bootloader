/******************************************************************************
 * @file    bootloader_protocol.c
 *
 * @author  Sunil Sutar
 *
 * @project STM32F446RE Custom Bootloader Development
 *
 * @brief   Implements the bootloader communication protocol for exchanging
 *          commands and responses between the Python host application and
 *          the STM32F446RE MCU over UART. Handles protocol-related
 *          functionality such as packet processing, command identification,
 *          and response formatting, as implemented in this source file.
 *
 * @date    15 October 2026
 *
 ******************************************************************************
 */

#include "bootloader_protocol.h"
#include "bootlaoder.h"
#include "uart.h"

extern FLASH_Handle_t FLASHHandle;

static uint32_t Bootloader_CalculateCRC32(const uint8_t *data, uint32_t length);
static uint32_t Bootloader_ExtractCRC32(const uint8_t *pData);
static uint8_t Bootloader_VerifyCommandCRC(const uint8_t *packet, uint8_t packet_length);

/**
 * @brief  Transmits data to the host application over UART.
 *
 * @details
 * Sends the specified buffer of data through the UART interface
 * configured by CMD_UART.
 *
 * Uses USART_SendData() to transmit the requested number of bytes.
 * This function provides a common UART transmission interface for
 * sending bootloader responses and other protocol data.
 *
 * @param  pBuffer Pointer to the data buffer to be transmitted.
 * @param  len     Number of bytes to transmit.
 *
 * @retval None
 *
 * @note   CMD_UART specifies the UART peripheral used for
 *         bootloader communication.
 */

void bootloader_uart_write_data(uint8_t *pBuffer, uint32_t len)
{
    USART_SendData(CMD_UART, pBuffer, len);
}

/**
 * @brief  Sends an acknowledgment (ACK) response to the host application.
 *
 * @details
 * Constructs and transmits an ACK response to indicate that the
 * bootloader has accepted or successfully processed a command,
 * according to the response convention used by the project.
 *
 * The response may include a response-length field and subsequent
 * response data, depending on the calling command handler.
 *
 * @param  command_length Length associated with the received command,
 *                        as interpreted by the implementation.
 *
 * @param  response_length Length of the response data or response
 *                         field, as defined by the implementation.
 *
 * @retval None
 *
 * @note   ACK value: 0xA5.
 *
 * @note   Confirm the exact parameter meanings and transmitted byte
 *         sequence against the function implementation.
 */

void BL_SendAck(uint8_t command_length, uint8_t response_length)
{
    uint8_t ack_value = BL_ACK;
    
    (void) command_length;

    bootloader_uart_write_data(&ack_value, 1U);
    bootloader_uart_write_data(&response_length, 1U);
}

/**
 * @brief  Sends a negative acknowledgment (NACK) response to the host.
 *
 * @details
 * Constructs and transmits a NACK response to indicate that a
 * command was rejected or could not be processed successfully.
 *
 * The response communicates failure according to the project's
 * bootloader response protocol and may include a response-length
 * field and error-status information.
 *
 * @param  command_length Length associated with the received command,
 *                        as interpreted by the implementation.
 *
 * @param  response_length Length of the response data or response
 *                         field, as defined by the implementation.
 *
 * @retval None
 *
 * @note   NACK value: 0x7F.
 *
 * @note   Confirm the exact parameter meanings and transmitted byte
 *         sequence against the function implementation.
 */

void BL_SendNack(uint8_t command_length, uint8_t response_length)
{
    uint8_t nack_value = BL_NACK;

    (void) command_length;
    
    bootloader_uart_write_data(&nack_value, 1U);
    bootloader_uart_write_data(&response_length, 1U);
}

/* =============================================================
   BL_GET_VER Command 
   ============================================================= */

/**
 * @brief  Handles the bootloader version retrieval command.
 *
 * @details
 * Processes the BL_GET_VER command (0x51) received from the Python
 * host application.
 *  
 * Packet format:
 * Length | Command | CRC32[4]
 * 
 * Example:
 * 05 51 CE 6E 8E EF
 * 
 * Verifies the command packet CRC32 before processing the request.
 * If the CRC is valid, returns the bootloader version to the host
 * using the bootloader response protocol.
 *
 * If the CRC verification fails, sends a NACK response with the
 * appropriate error status.
 *
 * @note   Command ID: BL_GET_VER (0x51).
 *
 * @note   A successful version response was observed as:
 *         A5 01 10
 *
 * @note   A CRC error response was observed as:
 *         7F 01 02
 *
 * @param  None or command packet pointer, depending on the function
 *         prototype.
 *
 * @retval None, if the function returns void.
 */

void bootloader_handle_getver_cmd(uint8_t *bl_rx_buffer)
{
    uint8_t bl_version;
    uint8_t length;

    printmsg("BL_DEBUG_MSG: bootloader_handle_getver_cmd\r\n");

    /* 1. Validate packet length */
    if (bl_rx_buffer[0] != 5U)
    {
        printmsg("BL_DEBUG_MSG: Invalid GET_VER packet length\r\n");
        BL_SendNack(bl_rx_buffer[0], 1U);
        return;
    }

    /* 2. Verify command CRC */
    if (Bootloader_VerifyCommandCRC(bl_rx_buffer, bl_rx_buffer[0]) != 0U)
    {
        uint8_t error_status = 0x02U;

        printmsg("BL_DEBUG_MSG: GET_VER CRC verification FAILED\r\n");
        BL_SendNack(bl_rx_buffer[0], 1U);
        bootloader_uart_write_data(&error_status, 1U);
        return;        
    }

    printmsg("BL_DEBUG_MSG: GET_VER CRC verification PASSED\r\n");

    /* 3. Exclude the four CRC bytes from the logical command length */
    length = (uint8_t)(bl_rx_buffer[0] - 4U);

    /* 4. Send ACK */
    BL_SendAck(length, 1);

    /* 5. Get bootloader version */
    bl_version = get_bootloader_version();

    bootloader_uart_write_data(&bl_version, 1U);
    
}

uint8_t get_bootloader_version(void)
{
    return (uint8_t)BL_VERSION;
}

/* =============================================================
   BL_GET_HELP Command 
   ============================================================= */
/**
 * @brief  Handles the bootloader help command.
 *
 * @details
 * Processes the BL_GET_HELP command (0x52) received from the Python
 * host application.
 *
 * Verifies the received command packet using CRC32 before processing
 * the request. If the CRC is valid, sends the list of supported
 * bootloader command IDs to the host application.
 *
 * If CRC verification fails, sends a NACK response with the
 * appropriate error status.
 *
 * @param  bl_rx_buffer Pointer to the received bootloader command
 *                      packet, including the length field, command
 *                      ID, payload, and CRC32.
 *
 * @retval None
 *
 * @note   Command ID: BL_GET_HELP (0x52).
 *
 * @note   The project supports 13 command IDs, ranging from
 *         0x51 to 0x5D.
 *
 * @note   A successful response was observed beginning with 0xA5,
 *         followed by response length 0x0D and the supported
 *         command IDs.
 */

void bootloader_handle_gethelp_cmd(uint8_t *bl_rx_buffer)
{
    uint8_t length;
    uint8_t error_status = 0x02U;

    printmsg("BL_DEBUG_MSG: bootloader_handle_gethelp_cmd\r\n");

    uint8_t supported_commands[] = 
    {
        BL_GET_VER,
        BL_GET_HELP,
        BL_GET_CID,
        BL_GET_RDP_STATUS,
        BL_GO_TO_ADDR,
        BL_FLASH_ERASE,
        BL_MEM_WRITE,
        BL_EN_R_W_PROTECT,
        BL_MEM_READ,
        BL_READ_SECTOR_P_STATUS,
        BL_OTP_READ,
        BL_DIS_R_W_PROTECT,
        BL_VERIFY_APP_CRC
    };

    /* 1. Validate packet length */
    if (bl_rx_buffer[0] != 5U)
    {
        printmsg("BL_DEBUG_MSG: Invalid GET_HELP packet length\r\n");
        BL_SendNack(bl_rx_buffer[0], 1U);
        return;
    }

    /* 2. Verify command CRC */
    if(Bootloader_VerifyCommandCRC(bl_rx_buffer, bl_rx_buffer[0]) != 0U)
    {
        printmsg("BL_DEBUG_MSG: GET_HELP CRC verification FAILED\r\n");
        BL_SendNack(bl_rx_buffer[0], 1U);
        bootloader_uart_write_data(&error_status, 1U);
        return;
    }

    printmsg("BL_DEBUG_MSG: GET_HELP CRC verification PASSED\r\n");

    /* 3. Logical command length excludes CRC */
    length = (uint8_t)(bl_rx_buffer[0] - 4U);

    /* Send ACK with response length */
    //BL_SendAck(bl_rx_buffer[0], sizeof(supported_commands));
    BL_SendAck(length, sizeof(supported_commands));

    /* Send supported command list */
    bootloader_uart_write_data(supported_commands, sizeof(supported_commands));

}

/* =============================================================
   BL_GET_CID Command 
   ============================================================= */
/**
 * @brief  Handles the MCU chip ID retrieval command.
 *
 * @details
 * Processes the BL_GET_CID command (0x53) received from the Python
 * host application.
 *
 * Verifies the received command packet using CRC32. If the packet is
 * valid, retrieves the MCU device identifier and sends it to the
 * host application using the bootloader response protocol.
 *
 * If CRC verification fails, sends a NACK response with the
 * appropriate error status.
 *
 * @param  bl_rx_buffer Pointer to the received bootloader command
 *                      packet, including the length field, command
 *                      ID, payload, and CRC32.
 *
 * @retval None
 *
 * @note   Command ID: BL_GET_CID (0x53).
 *
 * @note   The observed chip ID in this project is 0x421.
 */

void bootloader_handle_getcid_cmd(uint8_t *bl_rx_buffer)
{
    uint16_t chip_id;
    uint8_t error_status = 0x02U;

    printmsg("BL_DEBUG_MSG: bootloader_handle_getcid_cmd\n");

    /* 1. Validate packet length */
    if (bl_rx_buffer[0] != 5U)
    {
        printmsg("BL_DEBUG_MSG: Invalid GET_CID packet length\r\n");
        BL_SendNack(bl_rx_buffer[0], 1U);
        bootloader_uart_write_data(&error_status, 1U);
        return;
    }

    /* 2. Verify command CRC */
    if (Bootloader_VerifyCommandCRC(bl_rx_buffer, bl_rx_buffer[0]) != 0U)
    {
        printmsg("BL_DEBUG_MSG: GET_CID CRC verification FAILED\r\n");
        BL_SendNack(bl_rx_buffer[0], 1U);
        bootloader_uart_write_data(&error_status, 1U);
        return;
    }

    printmsg("BL_DEBUG_MSG: GET_CID CRC verification PASSED\r\n");

    /* 3. Read the STM32 device ID */
    chip_id = get_mcu_chip_id();

    printmsg("BL_DEBUG_MSG: Chip ID = 0x%03X\r\n", (unsigned int)chip_id);

    /* 4. Send ACK with two response data bytes */
    BL_SendAck(bl_rx_buffer[0] - 4U, 2U);

    /* 5. Transmit chip ID, most significant byte first */
    bootloader_uart_write_data((uint8_t*)&chip_id, 2);

}

uint16_t get_mcu_chip_id(void)
{
    /*
    The STM32F446xx MCUs integrate an MCU ID code. This ID identifies the ST MCU partnumber
    and the die revision. It is part of the DBG_MCU component and is mapped on the
    external PPB bus (see Section 33.16 on page 1304).
    */
    uint16_t cid;
    
    cid = (uint16_t)(DBGMCU->IDCODE) & DBGMCU_IDCODE_DEV_ID_MASK;   //DBGMCU_IDCODE_DEV_ID_MASK=> 0x0FFF
    
    return cid;
}

/* =============================================================
   BL_GET_RDP_STATUS Command 
   ============================================================= */
/**
 * @brief  Handles the read-out protection status command.
 *
 * @details
 * Processes the BL_GET_RDP_STATUS command (0x54) received from the
 * Python host application.
 *
 * Verifies the received command packet using CRC32. If the packet
 * is valid, retrieves the MCU read-out protection (RDP) status
 * and sends the result to the host application.
 *
 * If CRC verification fails, sends a NACK response with the
 * appropriate error status.
 *
 * @param  bl_rx_buffer Pointer to the received bootloader command
 *                      packet, including the length field, command
 *                      ID, payload, and CRC32.
 *
 * @retval None
 *
 * @note   Command ID: BL_GET_RDP_STATUS (0x54).
 *
 * @note   The observed RDP level during testing was level 0.
 */

void bootloader_handle_getrdp_status_cmd(uint8_t *bl_rx_buffer)
{
    uint8_t rdp_level;
    uint8_t error_status = 0x02U;

    printmsg("BL_DEBUG_MSG: bootloader_handle_getrdp_cmd\r\n");

    /* 1. Validate packet length */
    if (bl_rx_buffer[0] != 5U)
    {
        printmsg("BL_DEBUG_MSG: Invalid GET_RDP_STATUS packet length\r\n");
        BL_SendNack(bl_rx_buffer[0], 1U);
        bootloader_uart_write_data(&error_status, 1U);
        return;
    }

    /* 2. Verify command CRC */
    if (Bootloader_VerifyCommandCRC(bl_rx_buffer, bl_rx_buffer[0]) != 0U)
    {
        printmsg("BL_DEBUG_MSG: GET_RDP_STATUS CRC verification FAILED\r\n");
        BL_SendNack(bl_rx_buffer[0], 1U);
        bootloader_uart_write_data(&error_status, 1U);
        return;
    }
    
    printmsg("BL_DEBUG_MSG: GET_RDP_STATUS CRC verification PASSED\r\n");

    /* 3. Read the RDP level from FLASH_OPTCR */
    rdp_level = get_mcu_rdp_level();

    printmsg("BL_DEBUG_MSG: RDP Status = 0x%02X\r\n", (unsigned int)rdp_level);

    /* 4. Send ACK with one response data byte */
    BL_SendAck(bl_rx_buffer[0] - 4U, 1U);           //Exclude CRC Bytes 

    /* Send RDP level */
    bootloader_uart_write_data(&rdp_level, 1U);
}

uint8_t get_mcu_rdp_level(void)
{
    uint8_t rdp_value;

    rdp_value = (uint8_t)((FLASH->OPTCR >> FLASH_OPTCR_RDP) & 0xFFU);

    if(rdp_value == RDP_LEVEL_0_VALUE)
    {
        return RDP_LEVEL_0;
    }
    else if(rdp_value == RDP_LEVEL_2_VALUE)
    {
        return RDP_LEVEL_2;
    }
    else
    {
        return RDP_LEVEL_1;
    }
}

/* =============================================================
   BL_GO_TO_ADDR Command 
   ============================================================= */
/**
 * @brief  Handles the command to jump to a specified memory address.
 *
 * @details
 * Processes the BL_GO_TO_ADDR command (0x55) received from the
 * Python host application.
 *
 * Verifies the received command packet using CRC32, extracts the
 * target address, and validates whether the address is permitted
 * for application handover.
 *
 * If the request is valid, transfers execution to the requested
 * address using the project's application-jump implementation.
 * Otherwise, sends a NACK response with the appropriate error status.
 *
 * @param  bl_rx_buffer Pointer to the received bootloader command
 *                      packet, including the length field, command
 *                      ID, target address, and CRC32.
 *
 * @retval None
 *
 * @note   Command ID: BL_GO_TO_ADDR (0x55).
 *
 * @note   The application Flash region begins at 0x08008000.
 *
 * @warning Validate the target address and application vector table
 *          before transferring execution.
 */

void bootloader_handle_go_to_addr_cmd(uint8_t *bl_rx_buffer)
{
    uint32_t jump_address;
    uint8_t addr_valid;
    uint8_t status;
    uint8_t error_status = 0x02U;
    uint8_t length;

    printmsg("BL_DEBUG_MSG: bootloader_handle_go_to_addr_cmd\r\n");

    /* 1. Validate packet length:
       Command (1) + Address (4) + CRC (4) = 9 
    */
    if (bl_rx_buffer[0] != 9U)
    {
        printmsg("BL_DEBUG_MSG: Invalid GO_TO_ADDR packet length\r\n");
        BL_SendNack(bl_rx_buffer[0], 1U);
        bootloader_uart_write_data(&error_status, 1U);
        return;
    }

    /* 2. Verify CRC before processing the address */
    if (Bootloader_VerifyCommandCRC(bl_rx_buffer, bl_rx_buffer[0]) != 0U)
    {
        printmsg("BL_DEBUG_MSG: GO_TO_ADDR CRC verification FAILED\r\n");
        BL_SendNack(bl_rx_buffer[0], 1U);
        bootloader_uart_write_data(&error_status, 1U);
        return;
    }

    printmsg("BL_DEBUG_MSG: GO_TO_ADDR CRC verification PASSED\r\n");

    /* 3. Extract address in big-endian order */
    jump_address =
        ((uint32_t)bl_rx_buffer[2] << 24U) |
        ((uint32_t)bl_rx_buffer[3] << 16U) |
        ((uint32_t)bl_rx_buffer[4] << 8U)  |
        ((uint32_t)bl_rx_buffer[5]);

    printmsg("BL_DEBUG_MSG: Requested address = 0x%08lX\r\n", (unsigned long)jump_address);

    /* Verify address */
    addr_valid = verify_address(jump_address);

    if(addr_valid)
    {
        status = BL_GO_TO_ADDR_SUCCESS;

        length = (uint8_t)(bl_rx_buffer[0] - 4U);   //exclude CRC

        /* Send ACK */
        BL_SendAck(length, 1U);

        /* Send status */
        bootloader_uart_write_data(&status, 1U);

        printmsg("BL_DEBUG_MSG: Valid address\r\n");

        /* Jump to address */
        void (*jump_to_address)(void);

        jump_to_address = (void(*)(void))jump_address;

        jump_to_address();
    }
    else
    {
        status = BL_GO_TO_ADDR_FAILED;

        /* Send NACK */
        BL_SendNack(bl_rx_buffer[0], 1U);

        /* Send status */
        bootloader_uart_write_data(&status, 1U);

        printmsg("BL_DEBUG_MSG: Invalid address\r\n");
    }
}

uint8_t verify_address(uint32_t go_address)
{
    //Checking Thumb bit
    if ((go_address & 1U) == 0U)
    {
        return 0U;
    }

    go_address &= ~1U;

    //Checking Flash Address
    if((go_address >= 0x08000000U) && (go_address <= 0x0807FFFFU))
    {
        return 1U;
    }

    //Checking SRAM Address
    if((go_address >= 0x20000000U) && (go_address <= 0x2001FFFFU))
    {
        return 1U;
    }

    return 0U;
}

/* =============================================================
   BL_FLASH_ERASE Command 
   ============================================================= */
/**
 * @brief  Handles the Flash erase command.
 *
 * @details
 * Processes the BL_FLASH_ERASE command (0x56) received from the
 * Python host application.
 *
 * Verifies the received command packet using CRC32 and validates
 * the erase parameters before performing the requested Flash
 * sector erase operation.
 *
 * Reports the operation result to the host application using
 * the bootloader response protocol.
 *
 * If packet validation or the Flash erase operation fails, sends
 * an appropriate error response.
 *
 * @param  bl_rx_buffer Pointer to the received bootloader command
 *                      packet, including the length field, command
 *                      ID, erase parameters, and CRC32.
 *
 * @retval None
 *
 * @note   Command ID: BL_FLASH_ERASE (0x56).
 *
 * @note   The application occupies Flash sectors 2 through 7.
 *
 * @warning Validate the sector range to prevent accidental erasure
 *          of bootloader sectors 0 and 1.
 */

void bootloader_handle_flash_erase_cmd(uint8_t *bl_rx_buffer)
{
    uint8_t sector;
    FLASH_Status_t status;
    uint8_t error_status;
    uint8_t logical_length;

    printmsg("BL_DEBUG_MSG: bootloader_handle_flash_erase_cmd\r\n");

    /* 1. Validate packet length:
    Command (1) + Sector (1) + CRC32 (4) = 6 */
    if (bl_rx_buffer[0] != 6U)
    {
        printmsg("BL_DEBUG_MSG: Invalid FLASH_ERASE packet length\r\n");
        BL_SendNack(bl_rx_buffer[0], 1U);
        error_status = 0x01U;
        bootloader_uart_write_data(&error_status, 1U);
        return;
    }

    /* 2. Verify CRC before any flash operation */
    if (Bootloader_VerifyCommandCRC(bl_rx_buffer, bl_rx_buffer[0]) != 0U)
    {
        printmsg("BL_DEBUG_MSG: FLASH_ERASE CRC verification FAILED\r\n");
        BL_SendNack(bl_rx_buffer[0], 1U);
        error_status = 0x02U;
        bootloader_uart_write_data(&error_status, 1U);
        return;
    }

    printmsg("BL_DEBUG_MSG: FLASH_ERASE CRC verification PASSED\r\n");

    /* 3. Extract requested sector number */
    sector = bl_rx_buffer[2];

    printmsg("Sector : %u\r\n", (unsigned int)sector);

    /* 4. Protect bootloader sectors 0 and 1 */
    if (sector < FLASH_FIRST_APPLICATION_SECTOR)
    {
        uint8_t response = (uint8_t)FLASH_STATUS_PROTECTED;
        printmsg("BL_DEBUG_MSG: Protected sector\r\n");
        printmsg("BL_DEBUG_MSG: Sending NACK\r\n");
        BL_SendNack(bl_rx_buffer[0], 1U);
        bootloader_uart_write_data(&response, 1U);
        return;
    }

    /* 5. Validate sector range */
    if((sector > FLASH_LAST_APPLICATION_SECTOR))
    {
        uint8_t response = (uint8_t)FLASH_STATUS_INVALID_SECTOR;
        printmsg("BL_DEBUG_MSG: Invalid/Protected sector\r\n");
        BL_SendNack(bl_rx_buffer[0], 1U);
        bootloader_uart_write_data(&response, 1U);
        return;
    }

    /* 6. Unlock flash */
    FLASH_Unlock(&FLASHHandle);

    /* Verify unlock */
    if (FLASH_IsLocked(&FLASHHandle))
    {
        uint8_t response = (uint8_t)FLASH_STATUS_LOCKED;
        printmsg("BL_DEBUG_MSG: Flash unlock FAILED\r\n");
        BL_SendNack(bl_rx_buffer[0], 1U);
        bootloader_uart_write_data(&response, 1U);
        return;
    }

    /* 7. Erase the requested sector */
    status = FLASH_EraseSector(&FLASHHandle, (FLASH_Sector_t)sector);

    /* 8. Always lock Flash after operation */
    FLASH_Lock(&FLASHHandle);

    /* 9. Send result */
    if (status == FLASH_STATUS_OK)
    {
        printmsg("BL_DEBUG_MSG: Sector erase SUCCESS\r\n");
        logical_length = (uint8_t)(bl_rx_buffer[0] - 4U);
        BL_SendAck(logical_length, 1U);
        error_status = 0x00U;
        bootloader_uart_write_data(&error_status, 1U);
        
    }
    else
    {
        printmsg("BL_DEBUG_MSG: Sector erase failed, status = %u\r\n", (unsigned int)status);
        error_status = (uint8_t)status;
        BL_SendNack(bl_rx_buffer[0], 1U);
        bootloader_uart_write_data(&error_status, 1U);
    }
}

/* =============================================================
   BL_MEM_WRITE Command 
   ============================================================= */
/**
 * @brief  Handles the bootloader memory write command.
 *
 * @details
 * Processes the BL_MEM_WRITE command (0x57) received from the Python
 * host application.
 *
 * Verifies the command packet CRC32 and validates the memory address,
 * data length, and packet format before programming Flash memory.
 * The requested data is written to the permitted application Flash
 * region using the Flash driver.
 *
 * The programmed data is verified, and the operation result is
 * reported to the host application.
 *
 * @param  bl_rx_buffer Pointer to the received command packet,
 *                      including the length field, command ID,
 *                      target address, data length, data bytes,
 *                      and CRC32.
 *
 * @retval None
 *
 * @note   Command ID: BL_MEM_WRITE (0x57).
 *
 * @note   The target address is encoded in big-endian format.
 *
 * @note   The supported data length is 1 to 128 bytes.
 *
 * @note   The application Flash region is 0x08008000 to 0x0807FFFF.
 *
 * @warning Validate the complete address range before programming
 *          to prevent writes outside the application region.
 */

void bootloader_handle_mem_write_cmd(uint8_t *bl_rx_buffer)
{
    uint32_t address;
    uint8_t data_length;
    uint8_t *pData;
    FLASH_Status_t flash_status;
    uint8_t error_status;
    uint8_t expected_length;

    printmsg("BL_DEBUG_MSG: bootloader_handle_mem_write_cmd\r\n");

    /*
     * Minimum packet:
     * Length + Command + Address(4) + DataLength(1)
     * + Data(1) + CRC(4)
     *
     * Length field minimum = 11 bytes.
     */
    if (bl_rx_buffer[0] < 11U)
    {
        printmsg("BL_DEBUG_MSG: Invalid MEM_WRITE packet length\r\n");
        error_status = 0x01U;
        BL_SendNack(bl_rx_buffer[0], 1U);
        bootloader_uart_write_data(&error_status, 1U);
        return;
    }

    /* 1. Verify CRC before programming Flash */
    if (Bootloader_VerifyCommandCRC(bl_rx_buffer, bl_rx_buffer[0]) != 0U)
    {
        printmsg("BL_DEBUG_MSG: MEM_WRITE CRC verification FAILED\r\n");
        error_status = 0x02U;
        BL_SendNack(bl_rx_buffer[0], 1U);
        bootloader_uart_write_data(&error_status, 1U);
        return;
    }

    printmsg("BL_DEBUG_MSG: MEM_WRITE CRC verification PASSED\r\n");

    /* Extract Data length */
    data_length = bl_rx_buffer[6];

    /* Data length must be nonzero and within the supported limit */
    if ((data_length == 0U) || (data_length > 128U))
    {
        printmsg("BL_DEBUG_MSG: Invalid MEM_WRITE data length\r\n");
        error_status = 0x01U;
        BL_SendNack(bl_rx_buffer[0], 1U);
        bootloader_uart_write_data(&error_status, 1U);
        return;
    }

    /*
     * Expected length field:
     * Command(1) + Address(4) + DataLength(1)
     * + Data(N) + CRC(4) = 10 + N
     */
    expected_length = (uint8_t)(10U + data_length);

    if(bl_rx_buffer[0] != expected_length)
    {
        printmsg("BL_DEBUG_MSG: MEM_WRITE packet/data length mismatch\r\n");
        error_status = 0x01U;
        BL_SendNack(bl_rx_buffer[0], 1U);
        bootloader_uart_write_data(&error_status, 1U);
        return;
    }

    /* 3. Extract 32-bit destination address (big-endian) */
    address = ((uint32_t) bl_rx_buffer[2] << 24U) | 
              ((uint32_t) bl_rx_buffer[3] << 16U) |
              ((uint32_t) bl_rx_buffer[4] << 8U)  |
              ((uint32_t) bl_rx_buffer[5]);

    /* Data starts at byte 7 */
    pData = &bl_rx_buffer[7];

    printmsg("Address : %#x\r\n", address);
    printmsg("Data Length : %d\r\n", data_length);

    /* ---------------------------------------------
     * 4. Check application Flash address 
     * --------------------------------------------- */
    if ((address < FLASH_APPLICATION_START_ADDRESS) || 
        (address > FLASH_APPLICATION_END_ADDRESS) ||
        (data_length > (FLASH_APPLICATION_END_ADDRESS - address + 1U)))
    {
        uint8_t response = (uint8_t)FLASH_STATUS_INVALID_ADDRESS;
        printmsg("BL_DEBUG_MSG: Invalid Flash address\r\n");
        printmsg("BL_DEBUG_MSG: Address : %#x\r\n", address);
        BL_SendNack(bl_rx_buffer[0], 1U);
        bootloader_uart_write_data(&response, 1U);
        return;
    }

    /* ---------------------------------------------
     * Validate 4-byte alignment
     * --------------------------------------------- */
    if ((address & 0x03U) != 0U)
    {
        uint8_t response = (uint8_t)FLASH_STATUS_ALIGNMENT_ERROR;
        printmsg("BL_DEBUG_MSG: Unaligned address\r\n");
        BL_SendNack(bl_rx_buffer[0], 1U);
        bootloader_uart_write_data(&response, 1U);
        return;
    }

    /* Unlock Flash */
    FLASH_Unlock(&FLASHHandle);

    if (FLASH_IsLocked(&FLASHHandle))
    {
        uint8_t response = (uint8_t)FLASH_STATUS_LOCKED;
        printmsg("BL_DEBUG_MSG: Flash unlock FAILED\r\n");
        BL_SendNack(bl_rx_buffer[0], 1U);
        bootloader_uart_write_data(&response, 1U);
        return;
    }

    printmsg("BL_DEBUG_MSG: Flash unlocked\r\n");

    /* 5. Program + Verify */
    flash_status  = FLASH_ProgramAndVerifyBytes(&FLASHHandle, address, pData, data_length);

    /* Always lock Flash */
    FLASH_Lock(&FLASHHandle);

    printmsg("BL_DEBUG_MSG: Flash locked\r\n");

    /* Check result */
    /* 6. Return operation status */
    if (flash_status == FLASH_STATUS_OK)
    {
        error_status = 0x00U;
        BL_SendAck(bl_rx_buffer[0], 1U);
        bootloader_uart_write_data(&error_status, 1U);
        printmsg("BL_DEBUG_MSG: Memory write SUCCESS\r\n");
    }
    else
    {
        error_status = (uint8_t)flash_status;
        BL_SendNack(bl_rx_buffer[0], 1U);
        bootloader_uart_write_data(&error_status, 1U);
        printmsg("BL_DEBUG_MSG: Memory write FAILED\r\n");
        printmsg("FLASH Status : %d\r\n", flash_status);
    }
}

/* =============================================================
   BL_MEM_READ Command 
   ============================================================= */
/**
 * @brief  Handles the bootloader memory read command.
 *
 * @details
 * Processes the BL_MEM_READ command (0x59) received from the Python
 * host application.
 *
 * Verifies the command packet CRC32 and validates the requested
 * memory address and read length. If the request is valid, reads
 * the requested number of bytes from memory and sends the result
 * to the host application using the bootloader response protocol.
 *
 * The response includes a status byte followed by the requested
 * memory data, according to the current project protocol.
 *
 * @param  bl_rx_buffer Pointer to the received command packet,
 *                      including the length field, command ID,
 *                      memory address, read length, and CRC32.
 *
 * @retval None
 *
 * @note   Command ID: BL_MEM_READ (0x59).
 *
 * @note   The memory address is encoded in big-endian format.
 *
 * @note   The supported read length is 1 to 128 bytes.
 *
 * @note   For an 8-byte read, the tested response length was 9 bytes:
 *          one status byte followed by eight data bytes.
 *
 * @warning Validate the address and requested length before
 *          accessing memory.
 */

void bootloader_handle_mem_read_cmd(uint8_t *bl_rx_buffer)
{
    uint32_t address;
    uint32_t remaining_bytes;
    uint8_t read_length;
    uint8_t response_length;
    uint8_t error_status;
    uint8_t logical_length;
    volatile const uint8_t *flash_ptr;
   
    printmsg("BL_DEBUG_MSG: bootloader_handle_mem_read_cmd\r\n");

    /* 1. Validate request length:
    Command(1) + Address(4) + ReadLength(1) + CRC(4) = 10 */
    if(bl_rx_buffer[0] != 10U)
    {
        printmsg("BL_DEBUG_MSG: Invalid MEM_READ packet length\r\n");
        error_status = 0x01U;
        BL_SendNack(bl_rx_buffer[0], 1U);
        bootloader_uart_write_data(&error_status, 1U);
        return;
    }

    /* 2. Verify CRC before accessing memory */
    if(Bootloader_VerifyCommandCRC(bl_rx_buffer, bl_rx_buffer[0]) != 0U)
    {
        printmsg("BL_DEBUG_MSG: MEM_READ CRC verification FAILED\r\n");
        error_status = 0x02U;
        BL_SendNack(bl_rx_buffer[0], 1U);
        bootloader_uart_write_data(&error_status, 1U);
        return;
    }

    printmsg("BL_DEBUG_MSG: MEM_READ CRC verification PASSED\r\n");

    /* 3. Extract 32-bit address, (big-endian) */
    address = ((uint32_t)bl_rx_buffer[2] << 24U) |
              ((uint32_t)bl_rx_buffer[3] << 16U) |
              ((uint32_t)bl_rx_buffer[4] << 8U)  |
              ((uint32_t)bl_rx_buffer[5]);

    /* 4. Extract requested read length */
    read_length = bl_rx_buffer[6];

    printmsg("MEM_READ Address: 0x%08lX\r\n", (unsigned long)address);
    printmsg("MEM_READ Length: %u\r\n", (unsigned int)read_length);

    /* 5. Validate requested length */
    if ((read_length == 0U) || (read_length > BL_MEM_READ_MAX_LENGTH))
    {
        printmsg("BL_DEBUG_MSG: Invalid MEM_READ data length\r\n");
        error_status  = (uint8_t)FLASH_STATUS_ERROR;
        BL_SendNack(bl_rx_buffer[0], 1U);
        bootloader_uart_write_data(&error_status , 1U);
        return;
    }

    /* 6. Validate entire application Flash read range */
    if((address < FLASH_APPLICATION_START_ADDRESS) ||
        (address > FLASH_APPLICATION_END_ADDRESS)  ||
        ((uint32_t)read_length > (0x08080000UL - address)))
    {
        printmsg("BL_DEBUG_MSG: MEM_READ address out of range\r\n");
        error_status = (uint8_t)FLASH_STATUS_INVALID_ADDRESS;
        BL_SendNack(bl_rx_buffer[0], 1U);
        bootloader_uart_write_data(&error_status, 1U);
        return;
    }

    /* Validate range without overflowing address + length */
    remaining_bytes = FLASH_APPLICATION_END_ADDRESS - address + 1U;

    if ((uint32_t)read_length > remaining_bytes)
    {
        error_status = (uint8_t)FLASH_STATUS_INVALID_ADDRESS;
        BL_SendNack(bl_rx_buffer[0], 1U);
        bootloader_uart_write_data(&error_status, 1U);
        return;
    }

    /* 7. Prepare response */
    /*
     * Flash is memory-mapped.
     * Reading Flash does not require unlocking the Flash controller.
     */
    flash_ptr = (volatile const uint8_t *)address;
    error_status = (uint8_t)FLASH_STATUS_OK;
    response_length = (uint8_t)(read_length + 1U);
    logical_length = (uint8_t)(bl_rx_buffer[0] - 4U);   //exlude CRC

    /* Send ACK header: ACK + response length */
    BL_SendAck(logical_length, response_length);

    /* Response payload starts with status */
    bootloader_uart_write_data(&error_status, 1U);

    /* Send requested Flash bytes */
    bootloader_uart_write_data((uint8_t *)flash_ptr, read_length);

    printmsg("BL_DEBUG_MSG: MEM_READ SUCCESS\r\n");

}

/* =============================================================
   BL_VERIFY_APP_CRC Command 
   ============================================================= */
/**
 * @brief  Handles the application firmware CRC verification command.
 *
 * @details
 * Processes the BL_VERIFY_APP_CRC command (0x5D) received from the
 * Python host application.
 *
 * Validates the command packet CRC32 before processing the request.
 * Performs the application-image CRC verification according to
 * the algorithm, memory range, and expected CRC definition used
 * by the project implementation.
 *
 * Reports the verification result to the host application through
 * the bootloader response protocol.
 *
 * @param  bl_rx_buffer Pointer to the received command packet,
 *                      including the length field, command ID,
 *                      payload, and CRC32.
 *
 * @retval None
 *
 * @note   Command ID: BL_VERIFY_APP_CRC (0x5D).
 *
 * @note   Command packet CRC32 and application-image CRC32 serve
 *         different purposes. The former checks the received
 *         command packet; the latter checks the firmware image.
 *
 * @warning Confirm the image CRC coverage, expected CRC location,
 *          and comparison logic against the function implementation.
 */

void bootloader_handle_verify_app_crc_cmd(uint8_t *bl_rx_buffer)
{
    uint32_t image_length;
    uint32_t expected_crc;
    uint32_t calculated_crc;
    uint32_t available_length;
    const uint8_t *app_flash;

    uint8_t status;

    printmsg("BL_DEBUG_MSG: BL_VERIFY_APP_CRC\r\n");

    /*
     * Packet:
     * [0] Length = 9
     * [1] Command = 0x5D
     * [2..5] Image length, big-endian
     * [6..9] Expected CRC32, big-endian
     */
    if (bl_rx_buffer[0] != BL_VERIFY_APP_CRC_REQUEST_LENGTH)
    {
        status = (uint8_t)FLASH_STATUS_ERROR;

        BL_SendNack(bl_rx_buffer[0], 1U);
        bootloader_uart_write_data(&status, 1U);
        return;
    }

    image_length =
        ((uint32_t)bl_rx_buffer[2] << 24U) |
        ((uint32_t)bl_rx_buffer[3] << 16U) |
        ((uint32_t)bl_rx_buffer[4] << 8U)  |
        ((uint32_t)bl_rx_buffer[5]);

    expected_crc =
        ((uint32_t)bl_rx_buffer[6] << 24U) |
        ((uint32_t)bl_rx_buffer[7] << 16U) |
        ((uint32_t)bl_rx_buffer[8] << 8U)  |
        ((uint32_t)bl_rx_buffer[9]);

    /* Validate application image length */
    available_length = FLASH_APPLICATION_END_ADDRESS - FLASH_APPLICATION_START_ADDRESS + 1U;

    if((image_length == 0U) || (image_length > available_length))
    {
        status = (uint8_t)FLASH_STATUS_INVALID_ADDRESS;

        BL_SendNack(bl_rx_buffer[0], 1U);
        bootloader_uart_write_data(&status, 1U);
        return;
    }

    app_flash = (const uint8_t *)FLASH_APPLICATION_START_ADDRESS;
    
    calculated_crc = CRC32_Calculate(app_flash, image_length);

    printmsg("CRC expected   : 0x%08lX\r\n", (unsigned long)expected_crc);
    printmsg("CRC calculated : 0x%08lX\r\n", (unsigned long)calculated_crc);

    if(calculated_crc == expected_crc)
    {
        status = (uint8_t)FLASH_STATUS_OK;
        BL_SendAck(bl_rx_buffer[0], 1U);
        bootloader_uart_write_data(&status, 1U);
        printmsg("APP CRC VERIFY: SUCCESS\r\n");
    }
    else
    {
        status = (uint8_t)FLASH_STATUS_ERROR;
        BL_SendNack(bl_rx_buffer[0], 1U);
        bootloader_uart_write_data(&status, 1U);
        printmsg("APP CRC VERIFY: FAILED\r\n");
    }
}

/**
 * @brief  Calculates the CRC32 checksum of a data buffer.
 *
 * @details
 * Calculates the CRC32 checksum for the specified input data.
 * The result is used by the bootloader to detect accidental
 * corruption of command packets or other data.
 *
 * The implementation uses the CRC-32/ISO-HDLC algorithm with
 * reflected polynomial 0xEDB88320, initial value 0xFFFFFFFF,
 * and final XOR value 0xFFFFFFFF.
 *
 * @param  data   Pointer to the input data buffer.
 * @param  length Number of bytes to include in the CRC32 calculation.
 *
 * @retval uint32_t Calculated 32-bit CRC checksum.
 *
 * @note   Standard test vector:
 *         ASCII "123456789" produces CRC32 0xCBF43926.
 *
 * @note   This function calculates the checksum only; it does not
 *         compare it with a received CRC value.
 */

static uint32_t Bootloader_CalculateCRC32(const uint8_t *data, uint32_t length)
{
    uint32_t crc = 0xFFFFFFFFU;

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

/**
 * @brief  Extracts a CRC32 value from a byte buffer.
 *
 * @details
 * Extracts four consecutive bytes from the specified data buffer
 * and combines them into a 32-bit CRC value.
 *
 * The function interprets the four bytes in big-endian order,
 * with the first byte representing the most significant byte.
 *
 * @param  pData Pointer to the first of the four CRC bytes.
 *
 * @retval uint32_t Extracted 32-bit CRC value.
 *
 * @note   The caller must ensure that pData points to at least
 *         four valid bytes.
 */

static uint32_t Bootloader_ExtractCRC32(const uint8_t *pData)
{
    //Big Endian
    return ((uint32_t)pData[0] << 24U) |
           ((uint32_t)pData[1] << 16U) |
           ((uint32_t)pData[2] <<  8U) |
           ((uint32_t)pData[3]);
}

/**
 * @brief  Verifies the CRC32 checksum of a bootloader command packet.
 *
 * @details
 * Calculates the CRC32 of the command ID and payload, then compares
 * the calculated checksum with the CRC32 value received in the packet.
 *
 * The packet format is:
 *
 * Length | Command ID | Payload | CRC32
 *
 * The length byte and the CRC field itself are excluded from the
 * CRC calculation. The received CRC32 is encoded in big-endian order.
 *
 * The packet buffer must contain the complete packet, including
 * the length byte and all four CRC bytes.
 *
 * @param  packet Pointer to the complete received command packet.
 * @param  packet_length Value of the packet length field, excluding
 *                       the initial length byte.
 *
 * @retval 0 CRC verification successful.
 * @retval 1 CRC verification failed.
 *
 * @note   The function expects the CRC32 field to occupy the final
 *         four bytes of the complete packet buffer.
 *
 * @note   The caller must ensure that the packet is fully received
 *         and that packet_length is consistent with the buffer size.
 */

static uint8_t Bootloader_VerifyCommandCRC(const uint8_t *packet, uint8_t packet_length)
{
    uint32_t calculated_crc;
    uint32_t received_crc;
    uint8_t crc_data_length;

    /*
     * Packet length includes:
     * Command + Payload + 4 CRC bytes
     */
    if ((packet == NULL) || (packet_length < 5U))
    {
        return 0U;
    }

    crc_data_length = (uint8_t)(packet_length - 4U);

    /* packet[0] is length; CRC starts at packet[1] */
    calculated_crc = Bootloader_CalculateCRC32(&packet[1], crc_data_length);

    /* Last four bytes contain the transmitted CRC */
    received_crc = Bootloader_ExtractCRC32(&packet[packet_length - 3U]);

    printmsg("BL_DEBUG_MSG: Calculated CRC = 0x%08lX\r\n", (unsigned long)calculated_crc);

    printmsg("BL_DEBUG_MSG: Received CRC   = 0x%08lX\r\n", (unsigned long)received_crc);        

    if(calculated_crc == received_crc)
    {
        return 0U;  /* CRC valid */
    }

    return 1U;      /* CRC invalid */
}
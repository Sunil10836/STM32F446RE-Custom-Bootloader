
/******************************************************************************
 * @file    bootloader.c
 *
 * @author  Sunil Sutar
 *
 * @project STM32F446RE Custom Bootloader Development
 *
 * @brief   Implements the core bootloader functionality for the STM32F446RE.
 *          Handles bootloader command processing, including command-specific
 *          operations such as MCU information retrieval, Flash erase,
 *          memory read/write, and application handover, as implemented
 *          in this source file.
 *
 * @date    15 October 2026
 *
 ******************************************************************************
 */
 

#include "bootlaoder.h"
#include "bootloader_protocol.h"
#include "uart.h"

#define BL_RX_LEN       200

uint8_t bl_rx_buffer[BL_RX_LEN];

/**
 * @brief  Transfers program execution from the bootloader to the user application.
 *
 * @details
 * Reads the application's initial Main Stack Pointer (MSP) and Reset_Handler
 * address from the application vector table located at 0x08008000.
 *
 * Validates the application vector information and prepares the processor
 * for application execution. This may include disabling interrupts,
 * relocating the vector table, setting the application's stack pointer,
 * and branching to the application's Reset_Handler, as implemented
 * by this function.
 *
 * @note   The user application must be programmed correctly and linked
 *         to the application Flash address 0x08008000.
 *
 * @note   The function is expected not to return if the application
 *         handover succeeds.
 *
 * @param  None
 *
 * @retval None
 */

void Bootloader_JumpToApplication(void)
{
    uint32_t app_msp;
    uint32_t application_reset_handler;
    
    //Function Pointer to hold the address of reset handler of the user app
    void (*app_reset_handler )(void);
    //pFunction application_reset_handler;

    printmsg("BL_DEBUG_MSG:bootloader_jump_to_user_app\n");

    /* --------------------------------------------------------
     * Step 1: Read Application Initial MSP
     * -------------------------------------------------------- */
    app_msp = *(volatile uint32_t *) APPLICATION_START_ADDRESS;

    printmsg("BL_DEBUG_MSG: MSP Value : %#x\n", app_msp);

    /* --------------------------------------------------------
     * Step 2: Read Application Reset Handler
     * -------------------------------------------------------- */
    application_reset_handler = *(volatile uint32_t *)(APPLICATION_START_ADDRESS + 4U);

    /* --------------------------------------------------------
     * Step 3: Validate MSP
     * -------------------------------------------------------- */
    if((app_msp < 0x20000000U) || (app_msp > 0x20020000U))
    {
        return;
    }

    /* --------------------------------------------------------
     * Step 4: Validate Reset Handler
     * -------------------------------------------------------- */
    if((application_reset_handler < APPLICATION_START_ADDRESS) || (application_reset_handler >= APPLICATION_END_ADDRESS))
    {
        return;
    }

    /* --------------------------------------------------------
     * Step 5: Convert address to function pointer
     * -------------------------------------------------------- */
    app_reset_handler = (void (*)(void))application_reset_handler;

    printmsg("BL_DEBUG_MSG: app reset handler addr : %#x\n",app_reset_handler);

    /* --------------------------------------------------------
     * Step 6: Disable interrupts
     * -------------------------------------------------------- */
    __disable_irq();

    /* --------------------------------------------------------
     * Step 7: Relocate Vector Table
     * -------------------------------------------------------- */
    SCB->VTOR = APPLICATION_START_ADDRESS;

    __DSB();        // Data Synchronization Barrier: Ensures that all previous memory/register operations are completed before continuing. Complete VTOR update
    __ISB();        // Instruction Synchronization Barrier: Synchronize CPU instruction execution

    /* --------------------------------------------------------
     * Step 8: Load Application Stack Pointer
     * -------------------------------------------------------- */
    __set_MSP(app_msp);

    /* --------------------------------------------------------
     * Step 9: Jump to Application Reset Handler
     * -------------------------------------------------------- */
    app_reset_handler();

    /*
     * Should never return
     */
    while (1)
    {
    }
}

/**
 * @brief  Receives and processes bootloader commands over UART.
 *
 * @details
 * Waits for command packets from the Python host application through
 * the UART interface. Receives the packet according to the protocol's
 * length field and passes the complete packet to the appropriate
 * bootloader command handler for processing.
 *
 * The command handlers perform command-specific validation, including
 * CRC32 verification, before executing the requested operation.
 *
 * Supported operations include retrieving MCU information, erasing
 * Flash sectors, reading and writing memory, verifying firmware,
 * and other bootloader commands implemented in the project.
 *
 * @note   The received packet must include the length field, command ID,
 *         payload, and CRC32 so that the command handler can validate it.
 *
 * @note   This function is called when the boot-mode button is pressed.
 *
 * @param  None
 *
 * @retval None
 */

void Bootloader_uart_read_data(void)
{
    uint8_t rcv_len = 0;

    while(1)
    {
        /* Clear receive buffer */
        memset(bl_rx_buffer, 0, 200);

        /* ------------------------------------------
         * Step 1: Receive packet length
         * ------------------------------------------ */
        USART_ReceiveData(CMD_UART, &rcv_len, 1U);
        
        /* Validate packet length */
        if((rcv_len < 5U) || (rcv_len > (sizeof(bl_rx_buffer) - 1U)))
        {
            printmsg("BL_DEBUG_MSG: Invalid packet length = %u\r\n", (unsigned int)rcv_len);
            continue;
        }

        /* ------------------------------------------
         * Step 2: Receive command, payload and CRC
         * ------------------------------------------ */
        bl_rx_buffer[0] = rcv_len;

        /* Receive Command */
        USART_ReceiveData(CMD_UART, &bl_rx_buffer[1], rcv_len);

        printmsg("BL_DEBUG_MSG: RX packet length = %u\r\n", (unsigned int)rcv_len);
        printmsg("BL_DEBUG_MSG: RX command = 0x%02X\r\n", (unsigned int)bl_rx_buffer[1]);

        /* ------------------------------------------
         * Step 5: Dispatch complete packet to the handler
         * ------------------------------------------ */
        switch (bl_rx_buffer[1])
        {
            case BL_GET_VER:
                bootloader_handle_getver_cmd(bl_rx_buffer);
                break;
            
            case BL_GET_HELP:
                bootloader_handle_gethelp_cmd(bl_rx_buffer);
                break;

            case BL_GET_CID:
                bootloader_handle_getcid_cmd(bl_rx_buffer);
                break;

            case BL_GET_RDP_STATUS:
                bootloader_handle_getrdp_status_cmd(bl_rx_buffer);
                break;

            case BL_GO_TO_ADDR:
                bootloader_handle_go_to_addr_cmd(bl_rx_buffer);
                break;

            case BL_FLASH_ERASE:
                bootloader_handle_flash_erase_cmd(bl_rx_buffer);
                break;

            case BL_MEM_WRITE:
                bootloader_handle_mem_write_cmd(bl_rx_buffer);
                break;

            case BL_MEM_READ:
                bootloader_handle_mem_read_cmd(bl_rx_buffer);
                break;
            
            case BL_VERIFY_APP_CRC:
                bootloader_handle_verify_app_crc_cmd(bl_rx_buffer);
                break;
            
            default:
                printmsg("BL_DEBUG_MSG: Unknown command = 0x%02X\r\n",(unsigned int)bl_rx_buffer[1]);
                BL_SendNack(bl_rx_buffer[0], 1U);
                break;
        }
    }
}

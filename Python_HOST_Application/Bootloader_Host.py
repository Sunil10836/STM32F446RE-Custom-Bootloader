"""
===============================================================================
File        : Bootloder_Host.py
Author      : Sunil Sutar
Project     : STM32F446RE Custom Bootloader Development
Description : Python-based host application for communicating with the
              STM32F446RE custom bootloader over UART.

Features:
    - UART communication with the STM32 bootloader
    - Bootloader command packet construction
    - CRC32 calculation and command integrity checking
    - Bootloader information retrieval
    - Flash sector erase and memory read/write operations
    - Application firmware (.bin) programming
    - Application firmware verification
    - Application CRC32 verification
    - Interactive command-line menu

Communication:
    Interface   : UART
    Baud Rate   : 115200 bps (default)
    Data Format : 8 data bits, no parity, 1 stop bit

Dependencies:
    - pyserial
    - time
    - sys

===============================================================================
"""

import serial       # Provides serial-port communication using the pySerial library.
import time         # Provides time-related functions, including command-processing delays.
import sys          # Provides access to system-specific functions, including program exit.


# =============================================================================
# UART Configuration
# =============================================================================
# DEFAULT_PORT : Default serial port used to connect to the STM32 bootloader.
# BAUDRATE     : UART communication speed in bits per second.
# TIMEOUT      : Maximum time in seconds that a serial read waits for data.
#
# The user can select a different COM port when starting the application.
# =============================================================================

DEFAULT_PORT = "COM11"
BAUDRATE = 115200
TIMEOUT = 2


# ============================================================
# Bootloader Commands
# ============================================================
BL_GET_VER              = 0x51
BL_GET_HELP             = 0x52
BL_GET_CID              = 0x53
BL_GET_RDP_STATUS       = 0x54
BL_GO_TO_ADDR           = 0x55
BL_FLASH_ERASE          = 0x56
BL_MEM_WRITE            = 0x57
BL_EN_R_W_PROTECT       = 0x58
BL_MEM_READ             = 0x59
BL_READ_SECTOR_P_STATUS = 0x5A
BL_OTP_READ             = 0x5B
BL_DIS_R_W_PROTECT      = 0x5C
BL_VERIFY_APP_CRC       = 0x5D

# ============================================================
# APPLICATION Start and End Address
# ============================================================
APPLICATION_START_ADDRESS = 0x08008000
APPLICATION_END_ADDRESS   = 0x08080000   # exclusive

MAX_APPLICATION_SIZE = (APPLICATION_END_ADDRESS - APPLICATION_START_ADDRESS)

BIN_WRITE_CHUNK_SIZE = 128

APPLICATION_SECTORS = [
    (2, 0x08008000, 0x0800BFFF),
    (3, 0x0800C000, 0x0800FFFF),
    (4, 0x08010000, 0x0801FFFF),
    (5, 0x08020000, 0x0803FFFF),
    (6, 0x08040000, 0x0805FFFF),
    (7, 0x08060000, 0x0807FFFF),
]

# ============================================================
# Bootloader Responses
# ============================================================
BL_ACK  = 0xA5
BL_NACK = 0x7F

# ============================================================
# RDP Levels
# ============================================================
RDP_LEVEL_0 = 0x00
RDP_LEVEL_1 = 0x01
RDP_LEVEL_2 = 0x02

# ============================================================
# Bootloader Host Class
# ============================================================

class STM32Bootloader:

    def __init__(self, port, baudrate=115200, timeout=2):

        self.port = port
        self.baudrate = baudrate
        self.timeout = timeout
        self.ser = None


    # --------------------------------------------------------
    # Open Serial Port
    # --------------------------------------------------------

    def connect(self):

        try:

            print("\nConnecting to STM32 bootloader...")
            print(f"Port     : {self.port}")
            print(f"Baudrate : {self.baudrate}")

            self.ser = serial.Serial(
                port=self.port,
                baudrate=self.baudrate,
                bytesize=serial.EIGHTBITS,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                timeout=self.timeout
            )

            print("Serial port opened successfully.")

            # Clear old data
            self.ser.reset_input_buffer()
            self.ser.reset_output_buffer()

            return True

        except serial.SerialException as e:

            print("\nERROR: Could not open serial port.")
            print(e)

            return False


    # --------------------------------------------------------
    # Close Serial Port
    # --------------------------------------------------------

    def disconnect(self):

        if self.ser is not None and self.ser.is_open:

            self.ser.close()

            print("\nSerial port closed.")


    # --------------------------------------------------------
    # Send Command
    # --------------------------------------------------------


    # def send_command(self, command):

    #     try:

    #         packet  = bytes([1, command])
    #         self.ser.write(packet )
    #         print("TX:", packet .hex(" ").upper())
    #         self.ser.flush()
    #         return True

    #     except serial.SerialException as e:

    #         print("UART transmit error:")
    #         print(e)
    #         return False

    def build_command_packet(self, command, payload=b""):

        # 1. Combine command and payload
        command_payload = bytes([command]) + payload

        # 2. Calculate CRC over command + payload
        crc = self.crc32_calculate(command_payload)

        # 3. Convert CRC to 4 big-endian bytes
        crc_bytes = crc.to_bytes(4, byteorder="big")

        # 4. Calculate packet length
        length = len(command_payload) + len(crc_bytes)

        # 5. Construct complete packet
        packet = bytes([length]) + command_payload + crc_bytes

        return packet

    def send_command(self, command, payload=b""):

        try:
            # Check that UART is connected
            if self.ser is None or not self.ser.is_open:
                print("ERROR: Serial port is not connected.")
                return False
        
            # Build packet with CRC32
            packet = self.build_command_packet(command, payload)

            # added for Negative CRC Test
            #if command == BL_GET_VER:
            #   packet = packet[:-1] + bytes([packet[-1] ^ 0x01])
            
            # Transmit packet over UART
            self.ser.write(packet)

            # Display transmitted bytes
            print("TX:", packet.hex(" ").upper())

            # Wait until queued data is transmitted
            self.ser.flush()

            return True

        except (serial.SerialException, ValueError) as e:
            print("UART transmit error:")
            print(e)
            return False
        
    # --------------------------------------------------------
    # GET VERSION
    # --------------------------------------------------------

    def get_version(self):

        print("\n----------------------------------------")
        print("BL_GET_VER  - CRC Protected")
        print("----------------------------------------")

        # Send Length + BL_GET_VER command
        if not self.send_command(BL_GET_VER):
            return False

        # Give STM32 time to process command
        time.sleep(0.1)

        # ----------------------------------------------------
        # Read ACK + length + version
        # ----------------------------------------------------
        response  = self.ser.read(3)

        print("RAW RX:", response.hex(" ").upper())

        if len(response) != 3:
            print("ERROR: Invalid response length")
            return False

        ack  = response[0]
        length = response[1]
        version = response[2]

        print(f"ACK     : 0x{ack:02X}")
        print(f"Length  : {length}")
        print(f"Version : 0x{version:02X}")

        # ----------------------------------------------------
        # Check ACK
        # ----------------------------------------------------
        if ack  == BL_NACK:
            print("Bootloader returned NACK.")
            return False
        
        if ack != BL_ACK:
            print(f"ERROR: Invalid ACK: 0x{ack:02X}")
            return False
        
        if length != 1:
            print("ERROR: Unexpected version response length")
            return False
        
        print("\nBL_GET_VER: SUCCESS")

        return True

    # --------------------------------------------------------
    # GET HELP
    # --------------------------------------------------------
    def get_bootloader_help(self):

        print("\n----------------------------------------")
        print("BL_GET_HELP - CRC Protected")
        print("----------------------------------------")

        # Send Length + Command
        if not self.send_command(BL_GET_HELP):
            return False

        time.sleep(0.05)

        # First receive ACK + response length
        ack_data = self.ser.read(2)

        print("RX ACK:", ack_data.hex(" ").upper())

        if len(ack_data) != 2:
            print("ERROR: Invalid ACK response length")
            return False

        ack = ack_data[0]
        length = ack_data[1]

        print(f"ACK     : 0x{ack:02X}")
        print(f"Length  : {length}")

        if ack == BL_NACK:
            print("ERROR: Bootloader returned NACK")
            return False

        if ack != BL_ACK:
            print(f"ERROR: Invalid ACK: 0x{ack:02X}")
            return False

        # Receive supported command list
        command_data = self.ser.read(length)

        print("Commands:", command_data.hex(" ").upper())

        if len(command_data) != length:
            print("ERROR: Invalid command data length")
            return False

        # Command name mapping
        command_names  = {
            0x51: "BL_GET_VER",
            0x52: "BL_GET_HELP",
            0x53: "BL_GET_CID",
            0x54: "BL_GET_RDP_STATUS",
            0x55: "BL_GO_TO_ADDR",
            0x56: "BL_FLASH_ERASE",
            0x57: "BL_MEM_WRITE",
            0x58: "BL_EN_R_W_PROTECT",
            0x59: "BL_MEM_READ",
            0x5A: "BL_READ_SECTOR_P_STATUS",
            0x5B: "BL_OTP_READ",
            0x5C: "BL_DIS_R_W_PROTECT",
            0x5D: "BL_VERIFY_APP_CRC",
        }

        print("\nSupported Bootloader Commands:")

        for command in command_data:
            name = command_names.get(command, "Unknown")
            print(f"  0x{command:02X} -> {name}")

        print("\nBL_GET_HELP: SUCCESS")

        return True
    
    # --------------------------------------------------------
    # GET CID
    # --------------------------------------------------------
    def get_chip_id(self):

        print("\n----------------------------------------")
        print("BL_GET_CID - CRC Protected")
        print("----------------------------------------")

        # Send Length + Command
        if not self.send_command(BL_GET_CID):
            return False

        time.sleep(0.05)

        # Receive ACK + response length
        ack_data = self.ser.read(2)

        print("RX ACK:", ack_data.hex(" ").upper())

        if len(ack_data) != 2:
            print("ERROR: Invalid ACK response length")
            return False

        ack = ack_data[0]
        length = ack_data[1]

        print(f"ACK     : 0x{ack:02X}")
        print(f"Length  : {length}")

        if ack == BL_NACK:
            print("ERROR: Bootloader returned NACK")
            return False

        if ack != BL_ACK:
            print(f"ERROR: Invalid ACK: 0x{ack:02X}")
            return False

        if length != 2:
            print(f"ERROR: Unexpected CID length: {length}")
            return False

        # Receive CID
        cid_data = self.ser.read(length)

        print("CID Data:", cid_data.hex(" ").upper())

        if len(cid_data) != length:
            print("ERROR: Invalid CID data length")
            return False
        # STM32 sends little-endian CID

        chip_id = cid_data[0] | (cid_data[1] << 8)

        print(f"\nChip ID: 0x{chip_id:03X}")

        print("\nBL_GET_CID: SUCCESS")

        return True

    # --------------------------------------------------------
    # GET RDP Level
    # --------------------------------------------------------
    def get_rdp_status(self):

        print("\n----------------------------------------")
        print("BL_GET_RDP_STATUS - CRC Protected")
        print("----------------------------------------")
        
        # Send Length + Command
        if not self.send_command(BL_GET_RDP_STATUS):
            return False

        time.sleep(0.05)

        # Receive ACK + response length
        ack_data = self.ser.read(2)

        print("RX ACK:", ack_data.hex(" ").upper())

        if len(ack_data) != 2:
            print("ERROR: Invalid ACK response length")
            return False

        ack = ack_data[0]
        length = ack_data[1]

        print(f"ACK     : 0x{ack:02X}")
        print(f"Length  : {length}")

        if ack == BL_NACK:
            print("ERROR: Bootloader returned NACK")
            return False

        if ack != BL_ACK:
            print(f"ERROR: Invalid ACK: 0x{ack:02X}")
            return False

        if length != 1:
            print(f"ERROR: Unexpected RDP response length: {length}")
            return False

        # Receive RDP level
        rdp_data = self.ser.read(1)

        if len(rdp_data) != 1:
            print("ERROR: RDP level not received")
            return False

        rdp_level = rdp_data[0]

        print(f"RDP Level: {rdp_level}")

        if rdp_level == RDP_LEVEL_0:
            print("RDP Status: Level 0 - No Read Protection")

        elif rdp_level == RDP_LEVEL_1:
            print("RDP Status: Level 1 - Read Protection Enabled")

        elif rdp_level == RDP_LEVEL_2:
            print("RDP Status: Level 2 - Chip Protection")

        else:
            print(f"RDP Status: Unknown (0x{rdp_level:02X})")
            return False

        print("\nBL_GET_RDP_STATUS: SUCCESS")

        return True

    # --------------------------------------------------------
    # BL_GO_TO_ADDR
    # --------------------------------------------------------
    def go_to_address(self):

        print("\n----------------------------------------")
        print("BL_GO_TO_ADDR - CRC Protected")
        print("----------------------------------------")

        address_str = input("Enter address (hex): ")

        try:
            address = int(address_str, 16)
        except ValueError:
            print("ERROR: Invalid address")
            return False

        address_bytes = address.to_bytes(4, byteorder="big")

        #command_packet = bytes([0x05, BL_GO_TO_ADDR]) + address_bytes

        #print("TX:", command_packet.hex(" ").upper())

        #self.ser.write(command_packet)

        # send_command() adds the command ID and CRC32
        if not self.send_command(BL_GO_TO_ADDR, address_bytes):
            return False
        
        time.sleep(0.05)

        # Read ACK/NACK
        ack_data = self.ser.read(1)

        print("RX ACK:", ack_data.hex(" ").upper())

        if len(ack_data) != 1:
            print("ERROR: Invalid ACK response length")
            return False

        ack = ack_data[0]
        print(f"ACK     : 0x{ack:02X}")

        # Read response length separately         
        length_data = self.ser.read(1)   

        if len(length_data) != 1:
            print("ERROR: Missing response length")
            return False

        length = length_data[0]
        print(f"Length  : {length}")

        # Read response payload
        response = self.ser.read(length)
        print(f"Response   : {response.hex(' ').upper()}")

        if len(response) != length:
            print("ERROR: Invalid response length")
            return False

        status = response[0]
        print(f"Status  : 0x{status:02X}")

        # Handle NACK
        if ack == BL_NACK:
            print("ERROR: Bootloader returned NACK")
            return False

        if ack != BL_ACK:
            print(f"ERROR: Invalid ACK: 0x{ack:02X}")
            return False

        # Handle successful response
        if length == 1 and status == 0x00:
            print("BL_GO_TO_ADDR: SUCCESS")
            print(f"Jump accepted for address 0x{address:08X}")
            return True

        print("BL_GO_TO_ADDR: FAILED")
        return False

    # ========================================================
    # BL_FLASH_ERASE
    # ========================================================
    def flash_erase(self):

        print("\n----------------------------------------")
        print("BL_FLASH_ERASE - CRC Protected")
        print("----------------------------------------")

        # ----------------------------------------------------
        # Sector input
        #
        # Sector 0 -> Bootloader / Protected
        # Sector 1 -> Bootloader / Protected
        # Sector 2 -> Application
        # Sector 3 -> Application
        # Sector 4 -> Application
        # Sector 5 -> Application
        # Sector 6 -> Application
        # Sector 7 -> Application
        # Sector 8 -> Invalid
        # ----------------------------------------------------
        
        try:
            sector = int(input("Enter sector number (0-7): ").strip())

        except ValueError:
            print("ERROR: Invalid sector number")
            return False

        if sector < 2 or sector > 7:
            print("ERROR: Invalid sector number")
            return False

        # Protect bootloader sectors
        if sector in (0, 1):
            print("ERROR: Sectors 0 and 1 contain the bootloader")
            print("These sectors must not be erased.")
            return False
        
        # ----------------------------------------------------
        # Build packet
        #
        # Length  = 0x02
        # Command = 0x56
        # Sector  = sector number
        #
        # Example:
        # 02 56 02
        # ----------------------------------------------------

        # Payload: one-byte sector number
        payload = bytes([sector])

        # Send command + payload + CRC32
        if not self.send_command(BL_FLASH_ERASE, payload):
            return False

        print("Waiting for STM32 Flash erase...")
        old_timeout = self.ser.timeout
        self.ser.timeout = 2.0

        # ----------------------------------------------------
        # Read ACK/NACK + Response Length
        #
        # ACK  = A5
        # NACK = 7F
        #
        # Example success:
        # A5 01 00
        # ----------------------------------------------------

        # Read ACK/NACK
        ack_data = self.ser.read(1)

        #self.ser.timeout = old_timeout
        #print("RX HEADER:", ack_data.hex(" ").upper())

        if len(ack_data) != 1:
            print("ERROR: No ACK/NACK received")
            return False

        ack = ack_data[0]
        print(f"ACK/NACK : 0x{ack:02X}")
        
        # Read response length
        length_data = self.ser.read(1)

        if len(length_data) != 1:
            print("ERROR: Missing response length")
            return False

        response_length = length_data[0]
        print(f"Length   : {response_length}")

        # Read response data
        response = self.ser.read(response_length)

        if len(response) != response_length:
            print("ERROR: Incomplete response")
            return False

        print(f"ACK/NACK      : 0x{ack:02X}")
        print(f"Response length: {response_length}")
        print(f"Response data : {response.hex(' ').upper()}")
        
        # ----------------------------------------------------
        # Handle NACK
        # ----------------------------------------------------
        if ack == BL_NACK:
            print("RX: NACK")

            if response:
                status = response[0]
                print(f"Error status: 0x{status:02X}")

                if status == 0x02:
                    print("Reason: CRC verification failed")
                elif status == 0x05:
                    print("Reason: Bootloader sector protected")
                elif status == 0x06:
                    print("Reason: Invalid sector")
                else:
                    print("Reason: Command rejected")

            print("BL_FLASH_ERASE: FAILED")
            return False
            
        if ack != BL_ACK:
            print(f"ERROR: Invalid ACK/NACK: 0x{ack:02X}")
            return False

        # ----------------------------------------------------
        # ACK received
        # ----------------------------------------------------
        print("RX: ACK")

        if response_length != 1:
            print(f"ERROR: Unexpected response length: "f"{response_length}")
            return False

        # ----------------------------------------------------
        # Read response/status byte
        #
        # Success:
        # A5 01 00
        #       ^^
        #       status
        # ----------------------------------------------------
        status = response[0]
        print(f"Status   : 0x{status:02X}")

        # ----------------------------------------------------
        # Check erase result
        # ----------------------------------------------------
        if status == 0x00:
            print(f"\nSector {sector} erase SUCCESS")
            print("BL_FLASH_ERASE: SUCCESS")
            return True

        print(f"\nSector {sector} erase FAILED")

        if status == 0x01:
            print("Reason: Flash busy")
        elif status == 0x02:
            print("Reason: Flash error")
        elif status == 0x03:
            print("Reason: Flash locked")
        elif status == 0x05:
            print("Reason: Invalid sector")
        elif status == 0x06:
            print("Reason: Alignment error")
        elif status == 0x07:
            print("Reason: Flash timeout")
        elif status == 0x08:
            print("Reason: Sector protected")
        else:
            print("Reason: Unknown Flash status")

        print("BL_FLASH_ERASE: FAILED")
        return False

    # ========================================================
    # BL_MEM_WRITE
    # ========================================================
    def mem_write(self):

        print("\n----------------------------------------")
        print("BL_MEM_WRITE - CRC Protected")
        print("----------------------------------------")

        # 1. Read destination address
        address_str = input("Enter Flash address (hex): ").strip()

        try:
            address = int(address_str, 16)
        except ValueError:
            print("ERROR: Invalid address")
            return False

        # 2. Read data bytes
        data_input = input("Enter data bytes (example: 00 00 02 20): ")

        try:
            data = bytes.fromhex(data_input)
        except ValueError:
            print("ERROR: Invalid data format")
            return False

        if len(data) == 0:
            print("ERROR: Data cannot be empty")
            return False

        if len(data) > 128:
            print("ERROR: Maximum data length is 128 bytes")
            return False

        # 3. Validate application Flash range
        app_start = 0x08008000
        app_end = 0x08080000  # Exclusive

        if address < app_start or address >= app_end:
            print("ERROR: Address outside application Flash")
            return False

        if len(data) > (app_end - address):
            print("ERROR: Write exceeds application Flash range")
            return False

        print(f"Address    : 0x{address:08X}")
        print(f"Data length: {len(data)} bytes")
        print(f"Data       : {data.hex(' ').upper()}")
   
        # ----------------------------------------------------
        # Build packet
        #
        # Length  = Command + Address[4] + DataLength[1] + Data[N]
        #         = 1 + 4 + 1 + N
        #         = 6 + N
        # ----------------------------------------------------
        # 4. Build payload:
        # Address[4] + DataLength[1] + Data[N]
        payload = (
            address.to_bytes(4, byteorder="big")
            + bytes([len(data)])
            + data
        )

        # 5. Send command + payload + CRC32
        if not self.send_command(BL_MEM_WRITE, payload):
            return False

        # 6. Read ACK/NACK
        ack_data = self.ser.read(1)

        if len(ack_data) != 1:
            print("ERROR: No ACK/NACK received")
            return False

        ack = ack_data[0]
        print(f"ACK/NACK   : 0x{ack:02X}")

        # 7. Read response length
        length_data = self.ser.read(1)

        if len(length_data) != 1:
            print("ERROR: Missing response length")
            return False

        response_length = length_data[0]
        print(f"Length     : {response_length}")

        # 8. Read response data exactly once
        response = self.ser.read(response_length)

        if len(response) != response_length:
            print("ERROR: Incomplete response")
            print(f"Expected   : {response_length}")
            print(f"Received   : {len(response)}")
            return False

        print(f"Response   : {response.hex(' ').upper()}")

        # 9. Handle NACK
        if ack == BL_NACK:
            if response:
                status = response[0]
                print(f"Error status: 0x{status:02X}")

                if status == 0x02:
                    print("Reason: CRC verification failed")
                elif status == 0x04:
                    print("Reason: Invalid Flash address")
                else:
                    print("Reason: Flash write rejected")

            print("BL_MEM_WRITE: FAILED")
            return False

        if ack != BL_ACK:
            print(f"ERROR: Unexpected response: 0x{ack:02X}")
            return False

        # 10. Check write status
        if response_length != 1:
            print("ERROR: Unexpected response length")
            return False

        status = response[0]
        print(f"Status     : 0x{status:02X}")

        if status == 0x00:
            print("BL_MEM_WRITE: SUCCESS")
            return True

        print("BL_MEM_WRITE: FAILED")
        print(f"Flash status: 0x{status:02X}")
        return False


    # ========================================================
    # PROGRAM APPLICATION .BIN : added for temporary test
    # ========================================================
    def erase_sector(self, sector):
        print(f"\nErasing Sector {sector}...")

        command_packet = bytes([0x02, BL_FLASH_ERASE,sector])

        print("TX:", command_packet.hex(" ").upper())

        self.ser.write(command_packet)

        old_timeout = self.ser.timeout
        self.ser.timeout = 2.0

        ack_data = self.ser.read(2)

        self.ser.timeout = old_timeout

        print("RX HEADER:", ack_data.hex(" ").upper())

        if len(ack_data) != 2:
            print("ERROR: Invalid ACK/NACK header length")
            return False

        ack = ack_data[0]
        response_length = ack_data[1]

        response = self.ser.read(response_length)

        if len(response) != response_length:
            print("ERROR: Invalid response length")
            return False

        status = response[0] if response_length > 0 else 0xFF

        print(f"ACK/NACK : 0x{ack:02X}")
        print(f"Status   : 0x{status:02X}")

        if ack == BL_ACK and status == 0x00:
            print(f"Sector {sector} ERASE SUCCESS")
            return True

        print(f"Sector {sector} ERASE FAILED")
        return False

    # ========================================================
    # PROGRAM APPLICATION .BIN : added for temporary test
    # ========================================================

    def mem_write_data(self, address, data):

        if len(data) == 0:
            print("ERROR: Empty data")
            return False

        if len(data) > BIN_WRITE_CHUNK_SIZE:
            print("ERROR: Data chunk too large")
            return False

        if address < APPLICATION_START_ADDRESS:
            print("ERROR: Address below application area")
            return False

        if address >= APPLICATION_END_ADDRESS:
            print("ERROR: Address outside application area")
            return False

        if address & 0x03:
            print("ERROR: Address is not 4-byte aligned")
            return False

        program_length = (len(data) + 3) & ~3

        if address + program_length > APPLICATION_END_ADDRESS:
            print("ERROR: Program data exceeds application area")
            return False

        length = 6 + len(data)

        command_packet = bytes([
            length,
            BL_MEM_WRITE,
            (address >> 24) & 0xFF,
            (address >> 16) & 0xFF,
            (address >> 8) & 0xFF,
            address & 0xFF,
            len(data)
        ]) + data

        print(f"Writing {len(data)} bytes to 0x{address:08X}")

        # Clear stale input before sending a new command.
        # Use this only while debugging if response synchronization is uncertain.
        # self.ser.reset_input_buffer()

        old_timeout = self.ser.timeout

        try:
            self.ser.timeout = 2.0

            bytes_sent = self.ser.write(command_packet)
            self.ser.flush()

            if bytes_sent != len(command_packet):
                print("ERROR: Incomplete command transmission")
                return False

            # Read ACK/NACK header: [ACK/NACK, response_length]
            ack_data = self.ser.read(2)

            print("RX HEADER RAW:", ack_data.hex(" ").upper())
            print("RX HEADER LEN:", len(ack_data))

            if len(ack_data) != 2:
                print("ERROR: Incomplete ACK/NACK header")
                return False

            ack = ack_data[0]
            response_length = ack_data[1]

            # Read response payload using the same timeout.
            response = self.ser.read(response_length)

            print("RX STATUS RAW:", response.hex(" ").upper())
            print("RX STATUS LEN:", len(response))

            if len(response) != response_length:
                print("ERROR: Incomplete response payload")
                return False

            if ack == BL_NACK:
                status = response[0] if response else 0xFF
                print(f"MEM_WRITE NACK at 0x{address:08X}, status=0x{status:02X}")
                return False

            if ack != BL_ACK:
                print(f"ERROR: Invalid ACK/NACK: 0x{ack:02X}")
                return False

            if response_length < 1:
                print("ERROR: Missing status byte")
                return False

            status = response[0]

            print(f"RX: {ack:02X} Status: {status:02X}")

            if status != 0x00:
                print(f"ERROR: Flash write failed: 0x{status:02X}")
                return False

            return True

        finally:
            self.ser.timeout = old_timeout

    # ========================================================
    # PROGRAM APPLICATION .BIN
    # ========================================================

    def program_bin(self):

        print("\n========================================")
        print("        PROGRAM APPLICATION .BIN")
        print("========================================")

        # ----------------------------------------
        # 1. Read BIN file
        # ----------------------------------------
        bin_path = input("Enter application .bin file path: ").strip()

        if not bin_path:
            print("ERROR: File path cannot be empty")
            return False

        try:
            with open(bin_path, "rb") as file:
                firmware = file.read()

        except OSError as e:
            print(f"ERROR: Cannot open .bin file: {e}")
            return False

        firmware_size = len(firmware)

        print("\n----------------------------------------")
        print("Firmware Information")
        print("----------------------------------------")

        print(f"File          : {bin_path}")
        print(f"Size          : {firmware_size} bytes")
        print(f"Size          : {firmware_size / 1024:.2f} KB")

        # Check empty file
        if firmware_size == 0:
            print("ERROR: .bin file is empty")
            return False

        # Check maximum application size
        if firmware_size > MAX_APPLICATION_SIZE:
            print("ERROR: .bin file is too large")
            print(
                f"Maximum allowed size: "
                f"{MAX_APPLICATION_SIZE} bytes"
            )
            return False

        # Need at least vector table
        if firmware_size < 8:
            print("ERROR: .bin file is too small")
            return False

        # ----------------------------------------
        # 2. Read  and Validate Application Vector Table
        # ----------------------------------------

        initial_msp = int.from_bytes(firmware[0:4], byteorder="little")
        reset_handler = int.from_bytes(firmware[4:8], byteorder="little")

        print("\nApplication Vector Table:")
        print(f"Initial MSP   : 0x{initial_msp:08X}")
        print(f"Reset Handler : 0x{reset_handler:08X}")

        if not (0x20000000 < initial_msp <= 0x20020000):
            print("ERROR: Initial MSP is outside SRAM")
            return False

        if not (
            APPLICATION_START_ADDRESS <= (reset_handler & ~1)
            < APPLICATION_END_ADDRESS
            and (reset_handler & 1)
        ):
            print("ERROR: Reset Handler is invalid for application Flash")
            return False
        
        # ----------------------------------------
        # 3. Calculate Application Address Range
        # ----------------------------------------

        application_start = APPLICATION_START_ADDRESS
        application_end = (application_start + firmware_size)

        # The final Flash write is rounded up to a 4-byte boundary.
        padded_end = application_start + ((firmware_size + 3) & ~3)
        
        print("\nApplication Address Range:")
        print(f"Start : 0x{application_start:08X}")
        print(f"End   : 0x{application_end - 1:08X}")

        if padded_end > APPLICATION_END_ADDRESS:
            print("ERROR: Padded Flash write exceeds application area")
            return False
        
        # ----------------------------------------
        # 4. Determine Required Sectors
        # ----------------------------------------

        required_sectors = []

        for sector, start, end in APPLICATION_SECTORS:
            if application_start <= end and application_end > start:
                required_sectors.append(sector)
        
        if not required_sectors:
            print("ERROR: No application sectors selected")
            return False
    
        print("\nRequired Flash Sectors:")
        for sector in required_sectors:
            print(f"  Sector {sector}")

        # ----------------------------------------
        # 5. Confirm operation
        # ----------------------------------------

        print("\n----------------------------------------")
        print("Programming Summary")
        print("----------------------------------------")

        print(f"Firmware Size : " f"{firmware_size} bytes")
        print(f"Start Address : " f"0x{application_start:08X}")
        print(f"End Address   : " f"0x{application_end - 1:08X}")
        print(f"Sectors       : " f"{required_sectors}")

        confirm = input(
            "\nStart programming? (y/n): "
        ).strip().lower()

        if confirm != "y":
            print("Programming cancelled")
            return False

        # ----------------------------------------
        # 6. Erase sectors
        # ----------------------------------------

        print("\n========================================")
        print("STEP 1: FLASH ERASE")
        print("========================================")

        for sector in required_sectors:

            if not self.erase_sector(sector):
                print(
                    f"ERROR: Failed to erase "
                    f"Sector {sector}"
                )
                return False

        print("\nAll required sectors erased.")

        # ----------------------------------------
        # 7. Write firmware in chunks
        # ----------------------------------------

        print("\n========================================")
        print("STEP 2: FIRMWARE WRITE")
        print("========================================")

        total_written = 0

        while total_written < firmware_size:

            chunk = firmware[
                total_written:
                total_written + BIN_WRITE_CHUNK_SIZE
            ]

            address = (APPLICATION_START_ADDRESS + total_written)

            print(f"\nChunk Address : 0x{address:08X}")
            print(f"Chunk Size    : {len(chunk)} bytes")

            if not self.mem_write_data(address, chunk):
                print("\nERROR: Firmware write failed")
                print(f"Failed Address : " f"0x{address:08X}")
                return False
            
            total_written += len(chunk)

            percentage = (total_written * 100) // firmware_size

            print(
                f"Progress      : "
                f"{percentage}% "
                f"({total_written}/{firmware_size})"
            )

        # ----------------------------------------
        # 8. Complete
        # ----------------------------------------

        print("\n========================================")
        print("   FIRMWARE PROGRAMMING SUCCESS")
        print("========================================")

        print(f"Total programmed : " f"{total_written} bytes")

        print(
            f"Address range    : "
            f"0x{application_start:08X} - "
            f"0x{application_end - 1:08X}"
        )

        return True

    # ========================================================
    # BL_MEM_READ
    # ========================================================

    def mem_read_data(self, address, read_length):

        print("\n----------------------------------------")
        print("BL_MEM_READ - CRC Protected")
        print("----------------------------------------")

        # Validate address and read length
        app_start = 0x08008000
        app_end = 0x08080000  # Exclusive

        if not isinstance(address, int) or not (app_start <= address < app_end):
            print("ERROR: Address outside application Flash")
            return None

        if not isinstance(read_length, int) or not (1 <= read_length <= 128):
            print("ERROR: Read length must be between 1 and 128 bytes")
            return None

        if read_length > (app_end - address):
            print("ERROR: Read exceeds application Flash range")
            return None

        print(f"Address     : 0x{address:08X}")
        print(f"Read length : {read_length} bytes")

        # Payload: Address[4] + ReadLength[1]
        payload = (
            address.to_bytes(4, byteorder="big")
            + bytes([read_length])
        )

        # Send command + payload + CRC32
        if not self.send_command(BL_MEM_READ, payload):
            return None

        old_timeout = self.ser.timeout


        try:
            self.ser.timeout = 3.0

            # Read ACK/NACK
            ack_data = self.ser.read(1)

            if len(ack_data) != 1:
                print("ERROR: No ACK/NACK received")
                return None

            ack = ack_data[0]
            print(f"ACK/NACK    : 0x{ack:02X}")

            # Read response length
            length_data = self.ser.read(1)

            if len(length_data) != 1:
                print("ERROR: Missing response length")
                return None

            response_length = length_data[0]
            print(f"Response length: {response_length}")

            # Read response payload exactly once
            response = self.ser.read(response_length)

            if len(response) != response_length:
                print("ERROR: Incomplete response")
                print(f"Expected    : {response_length}")
                print(f"Received    : {len(response)}")
                return None

        finally:
            self.ser.timeout = old_timeout

        print(f"Response    : {response.hex(' ').upper()}")

        # Handle NACK
        if ack == BL_NACK:
            if response:
                print(f"Error status: 0x{response[0]:02X}")
            print("BL_MEM_READ: FAILED")
            return None

        if ack != BL_ACK:
            print(f"ERROR: Unexpected response: 0x{ack:02X}")
            return None

        # Response format: Status[1] + Data[N]
        if response_length != (read_length + 1):
            print("ERROR: Unexpected MEM_READ response length")
            return None

        status = response[0]
        print(f"Status      : 0x{status:02X}")

        if status != 0x00:
            print("BL_MEM_READ: FAILED")
            return None

        data = response[1:]

        print(f"Read data   : {data.hex(' ').upper()}")
        print("BL_MEM_READ: SUCCESS")

        return data

    # ========================================================
    # BL_MEM_READ Test Function
    # ========================================================
    def test_mem_read(self):

        address = APPLICATION_START_ADDRESS
        data = self.mem_read_data(address, 8)

        if data is None:
            print("MEM_READ TEST FAILED")
            return False

        print(f"Address : 0x{address:08X}")
        print("Data    :", data.hex(" ").upper())

        expected_msp = int.from_bytes(data[0:4], "little")
        expected_reset = int.from_bytes(data[4:8], "little")

        print(f"Initial MSP   : 0x{expected_msp:08X}")
        print(f"Reset Handler : 0x{expected_reset:08X}")

        return True
    
    # --------------------------------------------------------
    # Verify Application Bin
    # --------------------------------------------------------

    def verify_application_bin(self, bin_path):

        import os

        try:
            with open(bin_path, "rb") as f:
                expected = f.read()
        except OSError as exc:
            print(f"ERROR: Cannot read BIN file: {exc}")
            return False

        if not expected:
            print("ERROR: BIN file is empty")
            return False

        start = APPLICATION_START_ADDRESS
        end = start + len(expected) - 1

        if end > APPLICATION_END_ADDRESS:
            print("ERROR: BIN file exceeds application Flash")
            return False

        print("\n========================================")
        print("     APPLICATION FLASH VERIFICATION")
        print("========================================")
        print(f"BIN size     : {len(expected)} bytes")
        print(f"Start address: 0x{start:08X}")
        print(f"End address  : 0x{end:08X}")

        chunk_size = 128
        verified = 0

        while verified < len(expected):
            chunk = expected[verified:verified + chunk_size]
            address = start + verified

            actual = self.mem_read_data(address, len(chunk))

            if actual is None:
                print(f"ERROR: Read failed at 0x{address:08X}")
                return False

            if actual != chunk:
                for i, (exp_byte, got_byte) in enumerate(
                    zip(chunk, actual)
                ):

                    if exp_byte != got_byte:
                        fail_addr = address + i
                        print("\nVERIFICATION FAILED")
                        print(f"Address  : 0x{fail_addr:08X}")
                        print(f"Expected : 0x{exp_byte:02X}")
                        print(f"Actual   : 0x{got_byte:02X}")
                        return False

                print("ERROR: Data length mismatch")
                return False

            verified += len(chunk)
            percent = (verified * 100) // len(expected)

            print(
                f"Verified: {verified}/{len(expected)} bytes "
                f"({percent}%)"
            )

        print("\n========================================")
        print("  APPLICATION VERIFICATION PASSED")
        print("========================================")
        return True


    # --------------------------------------------------------
    # CRC Calculation
    # --------------------------------------------------------

    def crc32_calculate(self, data: bytes) -> int:
        crc = 0xFFFFFFFF

        for byte in data:
            crc ^= byte

            for _ in range(8):
                if crc & 1:
                    crc = (crc >> 1) ^ 0xEDB88320
                else:
                    crc >>= 1

        return crc ^ 0xFFFFFFFF

    # --------------------------------------------------------
    # Verify Application CRC
    # --------------------------------------------------------

    def verify_application_crc(self, bin_path):
        """Verify application Flash contents using the STM32 CRC32 command."""

        BL_VERIFY_APP_CRC = 0x5D
        BL_ACK = 0xA5
        BL_NACK = 0x7F

        try:
            # Step 1: Read firmware binary
            with open(bin_path, "rb") as file:
                firmware = file.read()

            image_length = len(firmware)

            if image_length == 0:
                print("ERROR: Firmware file is empty.")
                return False

            if image_length > 0xFFFFFFFF:
                print("ERROR: Firmware size exceeds protocol limit.")
                return False

            # Step 2: Calculate expected CRC
            expected_crc = self.crc32_calculate(firmware)

            # NEGATIVE TEST ONLY: deliberately corrupt expected CRC
            #expected_crc = 0x00000001

            print("\n========== APPLICATION CRC VERIFICATION ==========")
            print(f"Firmware size : {image_length} bytes")
            print(f"Expected CRC  : 0x{expected_crc:08X}")

            # Step 3: Construct request packet
            # Length | Command | ImageLength[4] | ExpectedCRC[4]
            packet = bytearray()
            packet.append(0x09)
            packet.append(BL_VERIFY_APP_CRC)
            packet.extend(image_length.to_bytes(4, byteorder="big"))
            packet.extend(expected_crc.to_bytes(4, byteorder="big"))

            print("TX:", " ".join(f"{byte:02X}" for byte in packet))

            # Step 4: Check serial connection
            if self.ser is None or not self.ser.is_open:
                print("ERROR: Serial port is not connected.")
                return False

            # Step 5: Send request
            self.ser.write(packet)
            self.ser.flush()

            # Step 6: Receive response
            old_timeout = self.ser.timeout

            try:
                self.ser.timeout = 2

                response_header = self.ser.read(2)

                if len(response_header) != 2:
                    print("ERROR: Timeout waiting for STM32 response.")
                    return False

                response_type = response_header[0]
                response_length = response_header[1]

                if response_length < 1:
                    print("ERROR: Invalid response length.")
                    return False

                response_data = self.ser.read(response_length)

                if len(response_data) != response_length:
                    print("ERROR: Incomplete STM32 response.")
                    return False

            finally:
                self.ser.timeout = old_timeout

            # Step 7: Display response
            complete_response = response_header + response_data
            print("RX:", " ".join(f"{byte:02X}" for byte in complete_response))

            status = response_data[0]

            # Step 8: Validate response
            if response_type == BL_ACK and status == 0x00:
                print("APPLICATION CRC VERIFY: SUCCESS")
                print("STM32 Flash contents match the host firmware CRC.")
                print("==================================================\n")
                return True

            if response_type == BL_NACK:
                print(f"APPLICATION CRC VERIFY: FAILED (status 0x{status:02X})")
                print("==================================================\n")
                return False

            print(
                f"ERROR: Unexpected response type=0x{response_type:02X}, "
                f"status=0x{status:02X}"
            )
            print("==================================================\n")
            return False

        except FileNotFoundError:
            print(f"ERROR: Firmware file not found: {bin_path}")
            return False

        except (OSError, ValueError) as error:
            print(f"ERROR: CRC verification failed: {error}")
            return False

# ========================================================
# test_crc32
# ========================================================

def test_crc32():
    test_data = b"123456789"
    crc = STM32Bootloader.crc32_calculate(test_data)

    print(f"CRC32 = 0x{crc:08X}")
    assert crc == 0xCBF43926, "CRC32 test failed!"
    print("CRC32 test PASSED")

def test_crc32_firmware():
    bin_path = input("Enter application .bin path: ").strip()

    with open(bin_path, "rb") as f:
        firmware = f.read()

    crc = STM32Bootloader.crc32_calculate(firmware)

    print(f"Firmware size : {len(firmware)} bytes")
    print(f"Firmware CRC32: 0x{crc:08X}")


# ============================================================
# print Menu
# ============================================================

def print_menu():

    print("\n")
    print("============================================")
    print("       STM32 CUSTOM BOOTLOADER HOST")
    print("============================================")
    print("1. Get Bootloader Version")
    print("2. Get Bootloader Help")
    print("3. Get Chip ID")
    print("4. Get RDP Status")
    print("5. Go To Address")
    print("6. Flash Erase")
    print("7. MEM Write")
    print("8. Programming .bin file")
    print("9. MEM Read")
    print("10. Verify Application BIN")
    print("11. CRC Test")
    print("12. Verify Application CRC")
    print("13. Exit")
    print("============================================")


# ============================================================
# Main
# ============================================================

def main():

    # --------------------------------------------------------
    # Port selection
    # --------------------------------------------------------

    port = input(f"Enter COM port [{DEFAULT_PORT}]: ").strip()

    if port == "":
        port = DEFAULT_PORT

    # --------------------------------------------------------
    # Create bootloader object
    # --------------------------------------------------------

    bootloader = STM32Bootloader(
        port=port,
        baudrate=BAUDRATE,
        timeout=TIMEOUT
    )

    # --------------------------------------------------------
    # Connect
    # --------------------------------------------------------

    if not bootloader.connect():

        sys.exit(1)

    # --------------------------------------------------------
    # Menu loop
    # --------------------------------------------------------

    try:

        while True:

            print_menu()

            choice = input("Enter choice: ").strip()

            if choice == "1":
                bootloader.get_version()
                
            elif choice == "2":
                bootloader.get_bootloader_help()

            elif choice == "3":
                bootloader.get_chip_id()

            elif choice == "4":
                bootloader.get_rdp_status()

            elif choice == "5":
                bootloader.go_to_address()

            elif choice == "6":
                bootloader.flash_erase()
            
            elif choice == "7":
                bootloader.mem_write()

            elif choice == "8":
                bootloader.program_bin()

            elif choice == "9":
                bootloader.test_mem_read()

            elif choice == "10":
                bin_path = input("Enter application .bin file path: ").strip()
                bootloader.verify_application_bin(bin_path)

            elif choice == "11":
                # test_crc32();
                test_crc32_firmware()

            elif choice == "12":
                bootloader.verify_application_crc(r"D:\05_Custom_Bootloader_Development\Bootloader_V2\Workspace\USER_APPLICATION\User_app_STM32f446xx\build\stm32f446_app.bin")
            
            elif choice == "13":
                print("\nExiting...")
                break

            else:
                print("\nInvalid choice.")

    except KeyboardInterrupt:
        print("\n\nProgram interrupted by user.")

    finally:
        bootloader.disconnect()

# ============================================================
# Program Entry
# ============================================================

if __name__ == "__main__":

    main()
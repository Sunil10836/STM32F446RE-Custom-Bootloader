# STM32F446RE Custom Bootloader

A custom bootloader for the STM32F446RE microcontroller, developed using bare-metal Embedded C and register-level programming. The project includes a separate user application and a Python-based host application for communicating with the bootloader over UART.

## 1. Project Overview

This project demonstrates the development of a custom bootloader for the STM32F446RE microcontroller.

The bootloader communicates with a host PC over UART and supports bootloader commands for retrieving device information, managing Flash memory, writing application data, and verifying firmware integrity.

A separate user application is linked to execute from a dedicated Flash memory region. The Python host application provides a command-line interface for interacting with the bootloader and performing firmware-related operations.

### Key Objectives

- Develop a custom STM32 bootloader using bare-metal Embedded C.
- Configure separate Flash memory regions for the bootloader and user application.
- Implement UART-based host-to-bootloader communication.
- Develop command packet construction and CRC32 calculation.
- Implement Flash erase and memory read/write operations.
- Develop a Python host application for bootloader communication.
- Explore firmware programming and application verification.

## 2. Key Features

- **Bare-Metal Development:** Register-level peripheral programming without relying on STM32 HAL for the bootloader drivers.
- **UART Communication:** Serial communication between the host PC and STM32 bootloader.
- **Command-Based Protocol:** Structured packets containing a length field, command, payload, and CRC32.
- **Device Information:** Commands for retrieving bootloader version, supported commands, chip ID, and read-protection status.
- **Flash Management:** Flash sector erase and memory write operations.
- **Application Management:** Separate bootloader and user application projects.
- **Firmware Programming:** Python host application support for programming a binary firmware image.
- **CRC32 Verification:** CRC32 calculation and firmware verification functionality.
- **Python Host Application:** Interactive command-line interface for communicating with the bootloader.

## 3. Hardware Requirements

- STM32 NUCLEO-F446RE development board
- USB connection for board programming and serial communication
- UART connection between the host PC and STM32
- Windows PC

The UART interface must be connected to a serial port accessible from the host application.

## 4. Software and Development Tools

| Tool / Technology | Purpose |
|---|---|
| Embedded C | Bootloader and user application development |
| STM32F446RE | Target microcontroller |
| ARM GNU Toolchain | Compilation and linking |
| GNU Make | Build automation |
| Linker Script | Flash memory layout configuration |
| Python | Host-side application |
| pySerial | UART communication from Python |
| Git | Source code version control |
| GitHub | Project hosting and documentation |

## 5. System Architecture

The project consists of three main components.

### 5.1 Bootloader

The bootloader resides at the beginning of internal Flash memory.

Its responsibilities include:
- Initializing the required peripherals.
- Receiving commands from the host PC over UART.
- Processing supported bootloader commands.
- Managing Flash erase and memory write operations.
- Providing device information and operation responses.
- Supporting application-related operations.

### 5.2 User Application

The user application is built as a separate firmware project and linked to start at the application Flash address.

The application uses the configured UART and GPIO drivers to transmit a UART message and toggle an LED.

The user application must be linked to its assigned Flash region rather than the default Flash origin used by the bootloader.

### 5.3 Python Host Application

The Python host application, `Bootloder_Host.py`, communicates with the STM32 bootloader over UART.

Its functionality includes:
- Establishing and closing the serial connection.
- Constructing and transmitting command packets.
- Retrieving bootloader and device information.
- Requesting Flash erase and memory operations.
- Programming a binary firmware image.
- Calculating CRC32 and performing firmware verification.

## 6. Flash Memory Layout

The internal Flash memory is divided into two logical regions.

| Component | Flash Region | Start Address |
|---|---|---|
| Bootloader | Sectors 0–1 | `0x08000000` |
| User Application | Sectors 2–7 | `0x08008000` |

### Memory Map

```text
STM32F446RE Internal Flash
+----------------------------------+ 0x08000000
|                                  |
|           Bootloader             |
|           Sectors 0–1            |
|                                  |
+----------------------------------+ 0x08008000
|                                  |
|         User Application         |
|           Sectors 2–7            |
|                                  |
+----------------------------------+ 0x08080000
```

The application start address is configured as:

```c
#define APPLICATION_START_ADDRESS    0x08008000
```

The Python host application uses the following application address range:

```python
APPLICATION_START_ADDRESS = 0x08008000
APPLICATION_END_ADDRESS   = 0x08080000
```

The application end address is exclusive.

**Important:** The bootloader and application linker scripts must agree with this memory layout. Flash erase operations must also respect the STM32F446RE Flash sector boundaries.

## 7. UART Communication Protocol

The Python host application communicates with the bootloader using UART.

### Default Configuration

| Parameter | Value |
|---|---|
| Interface | UART |
| Baud Rate | 115200 bps |
| Data Bits | 8 |
| Parity | None |
| Stop Bits | 1 |
| Default Host Port | COM11 |
| Serial Timeout | 2 seconds |

The host application allows the serial port to be selected when starting the program.

### Command Packet Format

The host application constructs command packets using the following format:

```text
+--------+---------+---------+---------+
| Length | Command | Payload |  CRC32  |
+--------+---------+---------+---------+
```

The length field includes the command, payload, and CRC32 bytes, but excludes the initial length byte.

The CRC32 is calculated over the command byte followed by the payload. The resulting CRC is transmitted as four big-endian bytes.

### Bootloader Commands

The host application defines command identifiers for operations including:

- Get bootloader version
- Get supported bootloader commands
- Get chip identification
- Get read-protection status
- Jump to an address
- Erase Flash memory
- Write memory
- Read memory
- Calculate or verify application CRC32, where supported by the implementation

The exact command behavior depends on the corresponding bootloader implementation.

## 8. Python Host Application

**Source file:** `Bootloder_Host.py`

The host application uses Python and the `pySerial` library to communicate with the target microcontroller.

### Install the Dependency

Install Python 3 if it is not already available, then run:

```bash
python -m pip install pyserial
```

### Run the Host Application

Navigate to the directory containing `Bootloder_Host.py`:

```bash
cd Python_Host_Application
```

Run:

```bash
python Bootloder_Host.py
```

Ensure that:
- The STM32 board is connected.
- The correct serial port is selected.
- The bootloader is running.
- The UART configuration matches on both sides.

The default serial port in the current host application is `COM11`. Update `DEFAULT_PORT` if your system uses another port.

## 9. Build Instructions

### Bootloader

1. Open the bootloader project directory.
2. Verify the ARM GNU toolchain installation.
3. Review the Makefile and linker script.
4. Build the project using GNU Make.

Example:

```bash
make
```

The exact command depends on the Makefile configuration.

### User Application

1. Open the user application project directory.
2. Verify that the linker script uses the application Flash start address.
3. Build the application using its Makefile.

Example:

```bash
make
```

The bootloader and application must be built with their respective linker configurations.

## 10. Programming and Testing Workflow

The intended workflow is:

1. Build the bootloader firmware.
2. Program the bootloader into the reserved Flash region.
3. Connect the host PC to the STM32 UART interface.
4. Start the Python host application.
5. Connect to the configured serial port.
6. Retrieve bootloader or device information.
7. Perform supported Flash erase and memory operations.
8. Program the user application binary using the host application.
9. Run the available firmware verification procedures.
10. Test the user application on the STM32 board.

**Safety note:** Flash erase and memory write operations can permanently change the contents of the target memory. Confirm the selected address range before performing these operations.

## 11. Firmware Verification

The Python host application includes CRC32 calculation and application firmware verification functionality.

CRC32 can help detect data corruption during firmware transfer or comparison.

However, CRC32 alone does not provide cryptographic authentication or guarantee that firmware is safe to execute.

Verification results depend on the CRC implementation and the corresponding behavior implemented by the bootloader.

## 12. Repository Structure

The repository is organized around the bootloader, user application, Python host application, and documentation.

```text
STM32F446RE-Custom-Bootloader/
├── Bootloader/
├── User_Application/
├── Python_Host_Application/
│   └── Bootloder_Host.py
├── Documentation/
├── README.md
└── .gitignore
```

The directory names above are illustrative. Use the actual directory structure of the project when committing it to GitHub.

## 13. Future Improvements

Potential enhancements include:

- Add firmware image integrity checks before programming.
- Add more comprehensive error handling and recovery.
- Improve UART communication timeout and response validation.
- Add automated Python-based firmware programming tests.
- Add structured test reports for firmware operations.
- Add a graphical interface for the host application.
- Extend the bootloader with a defined firmware update and recovery strategy.
- Add cryptographic firmware authentication if required by the target application.

These are potential future improvements and are not necessarily implemented in the current version.

## 14. Author

**Sunil Sutar**

Embedded Software Developer

---

*This project is developed for embedded systems learning, firmware development practice, and technical demonstration.*
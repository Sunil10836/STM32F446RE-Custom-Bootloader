TARGET  = stm32f446_app
PREFIX  = arm-none-eabi-

CC      = $(PREFIX)gcc
AS      = $(PREFIX)gcc
OBJCOPY = $(PREFIX)objcopy
SIZE    = $(PREFIX)size

BUILD_DIR = build


# ============================================================
# C Sources
# ============================================================

C_SOURCES = \
$(wildcard Src/*.c)

C_SOURCES += \
$(wildcard Drivers/Src/*.c)


# ============================================================
# Assembly Sources
# ============================================================

ASM_SOURCES = \
$(wildcard Startup/*.s)


# ============================================================
# Include Paths
# ============================================================

C_INCLUDES = \
-IInc \
-IDrivers/Inc


# ============================================================
# MCU
# ============================================================

MCU = cortex-m4


# ============================================================
# Compiler Flags
# ============================================================

CFLAGS =

CFLAGS += -mcpu=$(MCU)
CFLAGS += -mthumb
CFLAGS += -std=c17

CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -Wpedantic

CFLAGS += -Os

CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections

CFLAGS += -MMD
CFLAGS += -MP

CFLAGS += -g3


# ============================================================
# Assembly Flags
# ============================================================

ASFLAGS =

ASFLAGS += -mcpu=$(MCU)
ASFLAGS += -mthumb
ASFLAGS += -x assembler-with-cpp


# ============================================================
# Linker Script
# ============================================================

LDSCRIPT = STM32F446RETX_FLASH.ld


# ============================================================
# Linker Flags
# ============================================================

LDFLAGS =

LDFLAGS += -mcpu=$(MCU)
LDFLAGS += -mthumb

LDFLAGS += -T$(LDSCRIPT)

LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,-Map=$(BUILD_DIR)/$(TARGET).map
LDFLAGS += -Wl,--print-memory-usage

LDFLAGS += --specs=nano.specs


# ============================================================
# Object Files
# ============================================================

OBJECTS = \
$(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o))) \
$(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))


# ============================================================
# Dependency Files
# ============================================================

DEPS = $(OBJECTS:.o=.d)


# ============================================================
# Default Target
# ============================================================

all: $(BUILD_DIR) $(BUILD_DIR)/$(TARGET).elf


# ============================================================
# Create Build Directory
# ============================================================

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)


# ============================================================
# Compile Src/*.c
# ============================================================

$(BUILD_DIR)/%.o: Src/%.c
	$(CC) $(CFLAGS) $(C_INCLUDES) -c $< -o $@


# ============================================================
# Compile Drivers/Src/*.c
# ============================================================

$(BUILD_DIR)/%.o: Drivers/Src/%.c
	$(CC) $(CFLAGS) $(C_INCLUDES) -c $< -o $@


# ============================================================
# Assemble Startup/*.s
# ============================================================

$(BUILD_DIR)/%.o: Startup/%.s
	$(AS) $(ASFLAGS) $(C_INCLUDES) -c $< -o $@


# ============================================================
# Link Application
# ============================================================

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS)

	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

	$(SIZE) $@

	$(OBJCOPY) -O ihex \
	$@ \
	$(BUILD_DIR)/$(TARGET).hex

	$(OBJCOPY) -O binary \
	$@ \
	$(BUILD_DIR)/$(TARGET).bin


# ============================================================
# Dependency Files
# ============================================================

-include $(DEPS)


# ============================================================
# Clean
# ============================================================

clean:
	rm -rf $(BUILD_DIR)


# ============================================================
# Flash Application
# ============================================================

flash:
	STM32_Programmer_CLI \
	-c port=SWD \
	-w $(BUILD_DIR)/$(TARGET).hex \
	-v \
	-rst


# ============================================================
# Flash Binary at Application Address
# ============================================================

flash-bin:
	STM32_Programmer_CLI \
	-c port=SWD \
	-w $(BUILD_DIR)/$(TARGET).bin 0x08008000 \
	-v \
	-rst


# ============================================================
# OpenOCD
# ============================================================

flash-openocd:
	openocd \
	-f interface/stlink.cfg \
	-f target/stm32f4x.cfg \
	-c "program $(BUILD_DIR)/$(TARGET).elf verify reset exit"


# ============================================================
# GDB
# ============================================================

debug:
	arm-none-eabi-gdb $(BUILD_DIR)/$(TARGET).elf


# ============================================================
# Phony Targets
# ============================================================

.PHONY: all clean flash flash-bin flash-openocd debug
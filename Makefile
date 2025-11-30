# NanoSys Makefile
# Build system for the NanoSys educational operating system

# Toolchain
ASM = nasm
CC = i686-elf-gcc
LD = i686-elf-ld

# Directories
BOOT_DIR = boot
KERNEL_DIR = kernel
BUILD_DIR = build

# Flags
ASM_FLAGS = -f bin
KERNEL_ASM_FLAGS = -f elf32
CC_FLAGS = -ffreestanding -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -O2
LD_FLAGS = -T linker.ld

# Output files
BOOTLOADER = $(BUILD_DIR)/boot.bin
KERNEL_BIN = $(BUILD_DIR)/kernel.bin
OS_IMAGE = $(BUILD_DIR)/nanosys.img

# Source files
BOOT_SRC = $(BOOT_DIR)/boot.asm
KERNEL_ENTRY_SRC = $(KERNEL_DIR)/kernel_entry.asm
KERNEL_C_SRC = $(KERNEL_DIR)/kernel.c

# Object files
KERNEL_ENTRY_OBJ = $(BUILD_DIR)/kernel_entry.o
KERNEL_C_OBJ = $(BUILD_DIR)/kernel.o

# Default target
all: $(OS_IMAGE)

# Create build directory
$(BUILD_DIR):
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)

# Build bootloader
$(BOOTLOADER): $(BOOT_SRC) | $(BUILD_DIR)
	@echo Building bootloader...
	$(ASM) $(ASM_FLAGS) $(BOOT_SRC) -o $(BOOTLOADER)

# Build kernel entry (assembly)
$(KERNEL_ENTRY_OBJ): $(KERNEL_ENTRY_SRC) | $(BUILD_DIR)
	@echo Building kernel entry...
	$(ASM) $(KERNEL_ASM_FLAGS) $(KERNEL_ENTRY_SRC) -o $(KERNEL_ENTRY_OBJ)

# Build kernel (C)
$(KERNEL_C_OBJ): $(KERNEL_C_SRC) | $(BUILD_DIR)
	@echo Building kernel...
	$(CC) $(CC_FLAGS) -c $(KERNEL_C_SRC) -o $(KERNEL_C_OBJ)

# Link kernel
$(KERNEL_BIN): $(KERNEL_ENTRY_OBJ) $(KERNEL_C_OBJ)
	@echo Linking kernel...
	$(LD) $(LD_FLAGS) -o $(KERNEL_BIN) $(KERNEL_ENTRY_OBJ) $(KERNEL_C_OBJ)

# Create OS image
$(OS_IMAGE): $(BOOTLOADER) $(KERNEL_BIN)
	@echo Creating OS image...
	@copy /b $(BOOTLOADER) + $(KERNEL_BIN) $(OS_IMAGE) > nul
	@echo NanoSys image created successfully!

# Run in QEMU
run: $(OS_IMAGE)
	@echo Starting NanoSys in QEMU...
	qemu-system-i386 -fda $(OS_IMAGE) -m 32M

# Run in QEMU with debug output
debug: $(OS_IMAGE)
	@echo Starting NanoSys in QEMU (debug mode)...
	qemu-system-i386 -fda $(OS_IMAGE) -m 32M -d cpu_reset -no-reboot

# Clean build files
clean:
	@echo Cleaning build files...
	@if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)
	@echo Clean complete!

# Phony targets
.PHONY: all run debug clean

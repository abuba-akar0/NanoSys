# NanoSys - Project Overview

## Introduction
**NanoSys** is a 32-bit operating system kernel written from scratch for the x86 architecture. It is designed for educational purposes to demonstrate the fundamental concepts of OS development, including booting, protected mode, hardware drivers, and kernel space applications.

## System Architecture

The system consists of three main layers:

1.  **The Bootloader (`boot/`)**:
    *   The first code to run.
    *   Loads the kernel from disk to memory.
    *   Switches the CPU from 16-bit Real Mode to 32-bit Protected Mode.

2.  **The Kernel (`kernel/`)**:
    *   The core of the OS.
    *   Manages hardware resources (Screen, Keyboard, Timer).
    *   Handles Interrupts (IDT).

3.  **User Space (Shell)**:
    *   The interface for the user.
    *   Runs commands like `calc`, `time`, `help`.

## Directory Structure

```
TestOS/
├── boot/               # Bootloader source code
│   └── boot.asm        # The 512-byte boot sector
├── kernel/             # Kernel source code
│   ├── kernel_entry.asm# Assembly entry point
│   ├── kernel.c        # Main C kernel
│   ├── idt.c           # Interrupt Descriptor Table
│   ├── keyboard.c      # Keyboard driver
│   ├── timer.c         # Timer driver
│   ├── utils.c         # Helper functions (atoi, strcmp)
│   └── *.h             # Header files
├── build/              # Compiled binaries
├── Docs/               # Detailed documentation (You are here)
├── build.bat           # Windows build script
├── Makefile            # Linux/Make build script
└── linker.ld           # Linker configuration
```

## Key Features
*   **Custom Bootloader:** No GRUB needed. We write our own boot sector.
*   **32-bit Protected Mode:** Access to 4GB memory space.
*   **VGA Text Mode:** Direct memory access to video RAM (`0xB8000`).
*   **Interrupt Handling:** Custom IDT and ISRs for hardware events.
*   **Drivers:** PS/2 Keyboard and Programmable Interval Timer (PIT).
*   **Shell:** A command-line interface with a built-in calculator.

## Memory Map

| Address Range | Usage |
| :--- | :--- |
| `0x0000 - 0x04FF` | BIOS Interrupt Vector Table (IVT) |
| `0x0500 - 0x7BFF` | Free Memory |
| `0x7C00 - 0x7DFF` | **Bootloader** (Loaded by BIOS) |
| `0x7E00 - 0x9FFFF`| Free Memory / Stack |
| `0xA0000 - 0xBFFFF`| Video Memory (VGA) |
| `0x10000 - ...`   | **Kernel Code** (Loaded by Bootloader) |

---
*Next: Read `02_Bootloader.md` to understand how the system starts.*

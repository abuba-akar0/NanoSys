# NanoSys - Kernel Core Documentation

## Overview
The Kernel is the heart of the operating system. It takes control after the bootloader finishes.

**Files:**
*   `kernel/kernel_entry.asm`: The entry point.
*   `kernel/kernel.c`: The main C code.
*   `kernel/ports.h`: I/O port helpers.

## Kernel Entry (`kernel_entry.asm`)
Since our C compiler (`gcc`) produces code that expects to be called in a certain way, we use a small assembly wrapper to call `kernel_main`.

```nasm
[BITS 32]
[EXTERN kernel_main] ; Tell assembler this function exists elsewhere

global _start
_start:
    call kernel_main ; Call the C function
    jmp $            ; Infinite loop if it returns
```

## VGA Text Mode Driver
In Protected Mode, we can't use `int 0x10` to print to the screen. Instead, we write directly to **Video Memory**.

*   **Address:** `0xB8000`
*   **Format:** Each character on screen takes 2 bytes:
    1.  **ASCII Code** (Byte 0)
    2.  **Attribute/Color** (Byte 1)

**Color Byte Format:**
*   Bits 0-3: Foreground Color
*   Bits 4-6: Background Color
*   Bit 7: Blink

**Example Code (`kernel.c`):**
```c
volatile unsigned short* vga_buffer = (unsigned short*)0xB8000;

void putchar(char c) {
    // Calculate position: y * 80 + x
    int index = cursor_y * 80 + cursor_x;
    
    // Write character + color (white on black = 0x0F)
    vga_buffer[index] = (0x0F << 8) | c;
}
```

## Hardware Cursor
To move the blinking underscore, we communicate with the VGA Controller ports (`0x3D4` and `0x3D5`).

```c
void update_cursor(int x, int y) {
    uint16_t pos = y * 80 + x;
    outb(0x3D4, 0x0F);          // Tell VGA we are sending low byte
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);          // Tell VGA we are sending high byte
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}
```

## Port I/O (`ports.h`)
The CPU communicates with hardware devices (Keyboard, Timer, VGA) using **I/O Ports**. We use the `in` and `out` assembly instructions.

*   `outb(port, data)`: Send a byte to a port.
*   `inb(port)`: Read a byte from a port.

---
*Next: Read `04_Interrupts_and_Drivers.md` to understand how we handle hardware events.*

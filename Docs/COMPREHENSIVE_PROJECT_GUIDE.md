# NanoSys Project: The Comprehensive Guide

This document provides a complete, deep-dive explanation of the **NanoSys** operating system project. It is designed to explain "every inch" of the code, assuming no prior knowledge of OS development.

## 1. Project Overview

**NanoSys** is a 32-bit x86 operating system built from scratch. It does not use `Windows`, `Linux`, or `macOS` kernels; it **is** the kernel.

### The Boot Flow
1.  **BIOS** turns on and loads the first 512 bytes of the disk ([boot.asm](file:///c:/Users/abuba/CLionProjects/NanoSys/boot/boot.asm)) into memory at `0x7C00`.
2.  **Bootloader** ([boot.asm](file:///c:/Users/abuba/CLionProjects/NanoSys/boot/boot.asm)) runs in 16-bit Real Mode. It loads the rest of the OS (the Kernel) from the disk.
3.  **Bootloader** switches the CPU to 32-bit Protected Mode (enabling access to >1MB RAM).
4.  **Bootloader** jumps to the Kernel ([kernel_entry.asm](file:///c:/Users/abuba/CLionProjects/NanoSys/kernel/kernel_entry.asm) -> [kernel.c](file:///c:/Users/abuba/CLionProjects/NanoSys/kernel/kernel.c)).
5.  **Kernel** takes over, sets up drivers (Keyboard, Screen), and runs the command loop.

---

## 2. Component Analysis

### Part 1: The Bootloader ([boot/boot.asm](file:///c:/Users/abuba/CLionProjects/NanoSys/boot/boot.asm))

The bootloader is the first code to run. It has to fit in exactly 512 bytes.

#### Section A: Initialization
```nasm
[BITS 16]           ; We start in 16-bit Real Mode (BIOS default)
[ORG 0x7C00]        ; BIOS always loads us at this memory address

start:
    cli             ; Disable interrupts (don't want distractions yet)
    xor ax, ax      ; Set AX = 0
    mov ds, ax      ; Set Data Segment = 0
    mov es, ax      ; Set Extra Segment = 0
    mov ss, ax      ; Set Stack Segment = 0
    mov sp, 0x7C00  ; Stack grows DOWN from 0x7C00 (safety)
    sti             ; Re-enable interrupts
```

#### Section B: Loading the Kernel
We use **BIOS Interrupt 0x13** to read from the disk. This is the only way to access the disk before writing a complex disk driver.
```nasm
    mov ah, 0x02    ; Function: Read Sectors
    mov al, 32      ; Read 32 sectors (approx 16KB of kernel)
    mov cl, 2       ; Start from Sector 2 (Sector 1 is the bootloader itself)
    mov bx, 0x1000  ; Destination Segment
    mov es, bx
    xor bx, bx      ; Destination Offset = 0x0000 
    ; Result: Kernel loads at 0x1000:0x0000 physically -> 0x10000
    int 0x13        ; Call BIOS
```

#### Section C: Switching to Protected Mode
Real Mode is limited (1MB RAM, 16-bit registers). We need 32-bit Protected Mode.
1.  **Disable Interrupts**: BIOS interrupts (like 0x13) won't work in 32-bit mode, so we turn them off.
2.  **Load GDT**: The Global Descriptor Table tells the CPU about memory segments.
3.  **Enable A20**: A legacy hack to access memory above 1MB.
4.  **Set CR0 Bit**: The magic switch.
```nasm
    mov eax, cr0
    or eax, 1       ; Set bit 0 (Protection Enable)
    mov cr0, eax    ; CPU is now in 32-bit mode!
```
5.  **Far Jump**: We must jump to flush the CPU pipeline.
```nasm
    jmp 0x08:protected_mode
```

---

### Part 2: The Kernel ([kernel/kernel.c](file:///c:/Users/abuba/CLionProjects/NanoSys/kernel/kernel.c))

This is the brain of the OS. Written in C, but compiled with `-ffreestanding` (no standard library).

#### VGA Text Mode Driver
How do we print text without `printf`? We write directly to video memory at `0xB8000`.
Each character on screen takes 2 bytes:
1.  **Byte 1**: The character (ASCII).
2.  **Byte 2**: The color/attribute.

```c
// Pointer to video memory
static volatile unsigned short* vga_buffer = (unsigned short*)0xB8000;

void putchar(char c) {
    // Calculate position: y * 80 + x
    // Write (Color << 8) | Character
    vga_buffer[cursor_y * 80 + cursor_x] = (color << 8) | c;
    cursor_x++;
}
```
*Note: The actual implementation handles newlines `\n`, backspaces `\b`, and scrolling.*

#### The Command Loop
The [kernel_main](file:///c:/Users/abuba/CLionProjects/NanoSys/kernel/kernel.c#311-356) function ends with an infinite loop. However, the interactivity comes from the **Keyboard Interrupt**.
When you press a key:
1.  Keyboard fires interrupt 33.
2.  CPU pauses the loop.
3.  Runs [isr_handler](file:///c:/Users/abuba/CLionProjects/NanoSys/kernel/keyboard.c#82-124) in [keyboard.c](file:///c:/Users/abuba/CLionProjects/NanoSys/kernel/keyboard.c).
4.  Calls [keyboard_input](file:///c:/Users/abuba/CLionProjects/NanoSys/kernel/kernel.c#167-246) in [kernel.c](file:///c:/Users/abuba/CLionProjects/NanoSys/kernel/kernel.c).
5.  Key is added to `cmd_buffer`.
6.  If `ENTER` is pressed, `strcmp` checks the command.

---

### Part 3: Drivers ([kernel/keyboard.c](file:///c:/Users/abuba/CLionProjects/NanoSys/kernel/keyboard.c) & [timer.c](file:///c:/Users/abuba/CLionProjects/NanoSys/kernel/timer.c))

#### Keyboard Driver
The keyboard sends **scancodes**, not ASCII characters.
-   Press 'A' -> Scancode `0x1E`
-   Release 'A' -> Scancode `0x9E` (Bit 7 set)

The driver uses a lookup table (`kbdus`) to translate:
```c
unsigned char kbdus[128] = {
    ..., 'q', 'w', 'e', 'r', 't', 'y', ...
};
```
It looks up `kbdus[0x1E]` to get `'a'`. It also tracks the **Shift** key state to decide whether to use `kbdus` (lowercase) or `kbdus_shift` (uppercase).

#### Programmable Interval Timer (PIT)
The PIT (Pulse generator) is programmed to fire 100 times a second (100Hz).
```c
void timer_init(uint32_t freq) {
    uint32_t divisor = 1193180 / freq; // 1193180 is the hardware clock speed
    outb(0x43, 0x36); // Command byte
    outb(0x40, divisor & 0xFF); // Low byte
    outb(0x40, divisor >> 8);   // High byte
}
```
This allows the OS to count time ([show_uptime](file:///c:/Users/abuba/CLionProjects/NanoSys/kernel/kernel.c#293-310)) even when the user isn't doing anything.

---

### Part 4: Interrupt Descriptor Table ([kernel/idt.c](file:///c:/Users/abuba/CLionProjects/NanoSys/kernel/idt.c))

The IDT is a list of 256 addresses. It tells the CPU "If interrupt X happens, jump to code at Y".

-   **Exceptions (0-31)**: CPU errors (Divide by zero, Page fault).
-   **Hardware Interrupts (32-47)**: Remapped from PIC (Keyboard, Timer).
-   **Software Interrupts (48+)**: System calls (not implemented yet).

In [idt.c](file:///c:/Users/abuba/CLionProjects/NanoSys/kernel/idt.c), we manually populate this table:
```c
idt_set_gate(33, (uint32_t)isr_handler_stub, 0x08, 0x8E);
// 33: Interrupt number
// isr_handler_stub: The assembly code that saves registers
// 0x08: Kernel Code Segment
// 0x8E: Flags (Present, Ring 0, Interrupt Gate)
```

---

## 3. How It All Connects

1.  **Power On**: BIOS loads [boot.asm](file:///c:/Users/abuba/CLionProjects/NanoSys/boot/boot.asm).
2.  **Boot**: [boot.asm](file:///c:/Users/abuba/CLionProjects/NanoSys/boot/boot.asm) loads `kernel.bin` to `0x10000`.
3.  **Kernel Start**: [kernel_entry.asm](file:///c:/Users/abuba/CLionProjects/NanoSys/kernel/kernel_entry.asm) jumps to [kernel_main()](file:///c:/Users/abuba/CLionProjects/NanoSys/kernel/kernel.c#311-356).
4.  **Init**: [kernel_main](file:///c:/Users/abuba/CLionProjects/NanoSys/kernel/kernel.c#311-356) calls [idt_init()](file:///c:/Users/abuba/CLionProjects/NanoSys/kernel/idt.c#28-49), [keyboard_init()](file:///c:/Users/abuba/CLionProjects/NanoSys/kernel/keyboard.c#125-166), [timer_init()](file:///c:/Users/abuba/CLionProjects/NanoSys/kernel/timer.c#18-45).
5.  **Output**: [display_banner()](file:///c:/Users/abuba/CLionProjects/NanoSys/kernel/kernel.c#113-124) writes to `0xB8000` to show the welcome message.
6.  **Input**: User presses keys -> Interrupt fires -> [keyboard_input](file:///c:/Users/abuba/CLionProjects/NanoSys/kernel/kernel.c#167-246) -> Buffer -> Command Parser.

## 4. Summary of Files

| File | Purpose | Key Concept |
| :--- | :--- | :--- |
| [boot.asm](file:///c:/Users/abuba/CLionProjects/NanoSys/boot/boot.asm) | First 512 bytes, loads OS | Real vs Protected Mode |
| [kernel.c](file:///c:/Users/abuba/CLionProjects/NanoSys/kernel/kernel.c) | Main logic, Shell, VGA | Direct Memory Access |
| [idt.c](file:///c:/Users/abuba/CLionProjects/NanoSys/kernel/idt.c) | Interrupt Table setup | Hardware Event Handling |
| [keyboard.c](file:///c:/Users/abuba/CLionProjects/NanoSys/kernel/keyboard.c) | PS/2 Driver | Scancode Mapping |
| [ports.h](file:///c:/Users/abuba/CLionProjects/NanoSys/kernel/ports.h) | `inb`/`outb` assembly wrappers | Communicating with Hardware |

This project demonstrates the absolute fundamentals of how software interacts with hardware, bridging the gap between electricity and screen pixels.

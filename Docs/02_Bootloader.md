# NanoSys - Bootloader Documentation

## Overview
The bootloader is the most critical part of the startup process. It is a small program (max 512 bytes) located in the first sector of the disk (Master Boot Record).

**File:** `boot/boot.asm`

## The Boot Process

### 1. BIOS Handoff
When the PC turns on, the BIOS performs a POST (Power-On Self-Test). It then looks for a bootable device. It identifies our disk as bootable because of the **Boot Signature** `0xAA55` at the very end of the sector.

The BIOS loads our 512 bytes to memory address `0x7C00` and jumps to it.

### 2. Real Mode (16-bit)
The CPU starts in **Real Mode**. This means:
*   We can only access 1MB of RAM.
*   We use 16-bit registers (`ax`, `bx`, `cx`, `dx`).
*   We can use BIOS interrupts (like `int 0x10` for screen, `int 0x13` for disk).

**Code Example:**
```nasm
[BITS 16]
[ORG 0x7C00]    ; Tell assembler where we are loaded

start:
    ; Setup stack
    mov bp, 0x7C00
    mov sp, bp
```

### 3. Loading the Kernel
We need to load the rest of our OS (the kernel) from the disk into memory. We use BIOS interrupt `0x13`.

**Code Explanation:**
```nasm
mov ah, 0x02        ; BIOS Read Sector function
mov al, 32          ; Read 32 sectors (enough for our kernel)
mov ch, 0           ; Cylinder 0
mov cl, 2           ; Start from Sector 2 (Sector 1 is bootloader)
mov dh, 0           ; Head 0
mov bx, 0x1000      ; Segment 0x1000
mov es, bx          ; ES = 0x1000
xor bx, bx          ; BX = 0. Target = ES:BX = 0x1000:0000 = 0x10000
int 0x13            ; Trigger BIOS interrupt
```
*Result:* The kernel is now copied from the disk to RAM address `0x10000`.

### 4. The GDT (Global Descriptor Table)
To switch to 32-bit Protected Mode, we must define a GDT. The GDT tells the CPU about memory segments. In Protected Mode, we don't use segments like `CS:IP` in Real Mode; we use **Selectors** and **Descriptors**.

We define two segments:
1.  **Code Segment:** Read/Execute, covers entire 4GB.
2.  **Data Segment:** Read/Write, covers entire 4GB.

This is called a "Flat Memory Model".

### 5. Switching to Protected Mode
This is the "Point of No Return". Once we switch, we cannot use BIOS interrupts anymore!

1.  **Disable Interrupts:** `cli` (We don't want BIOS interrupts messing us up).
2.  **Load GDT:** `lgdt [gdt_descriptor]`.
3.  **Enable A20 Line:** Allows access to memory above 1MB.
4.  **Set PE Bit:** Set bit 0 of `CR0` register to 1.
5.  **Far Jump:** `jmp 0x08:protected_mode`. This flushes the CPU pipeline.

### 6. 32-bit Protected Mode
Now we are in 32-bit mode!
*   Registers are now 32-bit (`eax`, `ebx`, `esp`...).
*   We set up the segment registers (`ds`, `ss`, `es`...) to point to our Data Segment (`0x10`).
*   We update the stack pointer to `0x90000` (a safe free area).

Finally, we jump to the kernel:
```nasm
jmp 0x08:0x10000    ; Jump to where we loaded the kernel
```

---
*Next: Read `03_Kernel_Core.md` to see what happens next.*

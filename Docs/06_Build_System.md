# NanoSys - Build System Documentation

## Overview
Building an operating system is complex because we mix Assembly and C, and we need to produce a raw binary image, not a standard Windows `.exe`.

**Tools Used:**
*   **NASM:** Assembler for `.asm` files.
*   **GCC (Cross-Compiler):** Compiler for `.c` files (`i686-elf-gcc`).
*   **LD (Linker):** Combines object files (`i686-elf-ld`).

## The Build Script (`build.bat`)

### Step 1: Assemble Bootloader
```bat
nasm -f bin boot\boot.asm -o build\boot.bin
```
*   `-f bin`: Output raw binary (no headers).
*   This produces exactly 512 bytes.

### Step 2: Compile Kernel
```bat
i686-elf-gcc -ffreestanding -c kernel\kernel.c -o build\kernel.o
```
*   `-ffreestanding`: Tells GCC we don't have a standard library (no `printf`, `malloc`).
*   `-c`: Compile only, don't link yet.

### Step 3: Link Kernel
```bat
i686-elf-ld -T linker.ld -o build\kernel.bin ...
```
*   `-T linker.ld`: Use our custom memory layout.
*   **Output:** `kernel.bin` (The raw machine code of our kernel).

### Step 4: Create Disk Image
```bat
copy /b build\boot.bin + build\kernel.bin build\nanosys.img
```
*   We simply glue the bootloader and the kernel together into one file.
*   `boot.bin` is the first 512 bytes.
*   `kernel.bin` follows immediately after.

## The Linker Script (`linker.ld`)
This file tells the linker where to put code in memory.

```ld
ENTRY(_start)

SECTIONS
{
    . = 0x10000;    /* Start at 1MB address */

    .text : { *(.text) }    /* Code */
    .data : { *(.data) }    /* Global variables */
    .bss  : { *(.bss)  }    /* Uninitialized variables */
}
```
*   **Why 0x10000?** Because our bootloader copies the kernel to that physical address in RAM. If this doesn't match, all our pointers will be wrong!

## Running with QEMU
```bat
qemu-system-i386 -fda build\nanosys.img
```
*   `-fda`: Treat the file as a Floppy Disk A.
*   QEMU boots from this "virtual floppy" just like a real PC would.

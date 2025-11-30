# NanoSys - Interrupts & Drivers Documentation

## Overview
An operating system must respond to hardware events (like a key press or a timer tick). This is done using **Interrupts**.

**Files:**
*   `kernel/idt.c`, `kernel/idt.h`: Interrupt Descriptor Table.
*   `kernel/interrupts.asm`: Assembly wrappers.
*   `kernel/keyboard.c`: Keyboard driver.
*   `kernel/timer.c`: Timer driver.

## The IDT (Interrupt Descriptor Table)
The IDT is a table that tells the CPU: *"When Interrupt X happens, run Function Y."*

Each entry in the table is 8 bytes long and contains:
*   **Base Address:** Where the handler function is in memory.
*   **Selector:** The Code Segment (0x08).
*   **Flags:** Type of interrupt (32-bit, Ring 0).

**Initialization:**
1.  We create an array of `idt_entry_t`.
2.  We populate it with our handler addresses.
3.  We load it using the `lidt` instruction.

## Interrupt Service Routines (ISRs)
When an interrupt fires, the CPU stops what it's doing and jumps to the ISR. We need assembly wrappers (`interrupts.asm`) to save the CPU state before running C code.

**Flow:**
1.  **Interrupt Fires** (e.g., Key Press -> IRQ1).
2.  **CPU Jumps** to `isr_handler_stub` (in assembly).
3.  **Assembly Stub:**
    *   `pusha`: Saves all registers (`eax`, `ebx`, etc.) to the stack.
    *   Calls the C function (`isr_handler`).
    *   `popa`: Restores all registers.
    *   `iret`: Returns from interrupt (resumes previous task).

## The PIC (Programmable Interrupt Controller)
The PIC is a chip that aggregates hardware interrupts and sends them to the CPU.
*   **Master PIC:** Ports `0x20`, `0x21`. Handles IRQ 0-7.
*   **Slave PIC:** Ports `0xA0`, `0xA1`. Handles IRQ 8-15.

We must **Remap** the PIC because by default it uses interrupts 0-7, which conflict with CPU exceptions (like "Division by Zero"). We remap them to 32-47.

## Drivers

### 1. Keyboard Driver (`keyboard.c`)
*   **IRQ:** 1 (Mapped to Interrupt 33).
*   **Port:** `0x60` (Data Port).
*   **Logic:**
    1.  Read byte from port `0x60`.
    2.  Check if it's a "Make Code" (Press) or "Break Code" (Release).
    3.  Look up the code in a table (`kbdus`) to get the ASCII character.
    4.  Pass the character to the Shell.

### 2. Timer Driver (`timer.c`)
*   **IRQ:** 0 (Mapped to Interrupt 32).
*   **Chip:** 8253/8254 PIT (Programmable Interval Timer).
*   **Logic:**
    1.  We configure the PIT to fire 100 times per second (100 Hz).
    2.  Every time it fires, `timer_handler` runs.
    3.  We increment a global `tick` counter.
    4.  We use this counter to calculate Uptime (`ticks / 100 = seconds`).

---
*Next: Read `05_Shell_and_Apps.md` to see how we use these drivers.*

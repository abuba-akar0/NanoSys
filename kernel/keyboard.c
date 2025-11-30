#include "keyboard.h"
#include "kernel.h"
#include "idt.h"
#include "types.h"
#include "ports.h"

// Scancode to ASCII lookup table (US QWERTY)
// This maps the "make codes" (key presses) to characters.
unsigned char kbdus[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',   /* Backspace */
  '\t',                 /* Tab */
  'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* Enter key */
    0,                  /* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',   0,        /* Left shift */
  '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0,              /* Right shift */
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
  '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
  '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};

// The generic ISR handler called from assembly
// In a real OS, this would be in a separate 'isr.c' file
// and would dispatch based on interrupt number.
// For now, we assume any interrupt is the keyboard (IRQ 1 = INT 33)
void isr_handler(void) {
    // Read status register
    unsigned char status = inb(0x64);

    // Check if output buffer is full (data available)
    if (status & 0x1) {
        // Read scancode from data port
        unsigned char scancode = inb(0x60);

        // If the top bit is set, it's a "break code" (key release)
        // We only care about "make codes" (key press) for now
        if (!(scancode & 0x80)) {
            // Convert to ASCII
            char c = kbdus[scancode];
            
            // Send to kernel input handler
            if (c != 0) {
                keyboard_input(c);
            }
        }
    }

    // Send End of Interrupt (EOI) to PIC (Programmable Interrupt Controller)
    // If we don't do this, the PIC won't send any more interrupts!
    outb(0x20, 0x20);
}

// Initialize the keyboard
void keyboard_init(void) {
    // 1. Remap the PIC (Programmable Interrupt Controller)
    // The PIC maps IRQs 0-7 to interrupts 8-15 by default.
    // This conflicts with CPU exceptions. We need to remap them to 32-39.
    
    // ICW1: Start initialization
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    // ICW2: Vector offsets (32 for master, 40 for slave)
    outb(0x21, 0x20);
    outb(0xA1, 0x28);

    // ICW3: Cascading
    outb(0x21, 0x04);
    outb(0xA1, 0x02);

    // ICW4: Environment (8086 mode)
    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    // Mask all interrupts except IRQ0 (Timer) and IRQ1 (Keyboard)
    // 0xFC = 1111 1100 (bits 0 and 1 are 0, meaning enabled)
    outb(0x21, 0xFC);
    outb(0xA1, 0xFF);

    // 2. Install the interrupt handler in the IDT
    // IRQ 1 maps to Interrupt 33 (32 + 1)
    extern void isr_handler_stub(void);
    idt_set_gate(33, (uint32_t)isr_handler_stub, 0x08, 0x8E);
    
    // IRQ 0 maps to Interrupt 32
    extern void timer_stub(void);
    idt_set_gate(32, (uint32_t)timer_stub, 0x08, 0x8E);
    
    // Enable interrupts (STI)
    __asm__ volatile("sti");
    
    println("[INFO] Keyboard driver initialized");
}

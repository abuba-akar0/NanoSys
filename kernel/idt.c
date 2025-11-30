// NanoSys IDT Implementation
// Sets up the Interrupt Descriptor Table

#include "idt.h"
#include "kernel.h" // For memset/memcpy if we had them, or custom utils

// Declare the IDT array with 256 entries (0-255)
idt_entry_t idt_entries[256];
idt_ptr_t   idt_ptr;

// External assembly function to load the IDT
// Defined in interrupts.asm (we will create this later)
extern void idt_load();

// Set an entry in the IDT
// num:   Interrupt number (0-255)
// base:  Address of the handler function (ISR)
// sel:   Code segment selector (0x08)
// flags: Access rights (0x8E = Present, Ring 0, 32-bit Interrupt Gate)
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt_entries[num].base_low = base & 0xFFFF;       // Lower 16 bits of address
    idt_entries[num].base_high = (base >> 16) & 0xFFFF; // Upper 16 bits
    idt_entries[num].selector = sel;
    idt_entries[num].always0 = 0;
    idt_entries[num].flags = flags;
}

// Initialize the IDT
void idt_init(void) {
    // Set up the IDT pointer
    idt_ptr.limit = sizeof(idt_entry_t) * 256 - 1;
    idt_ptr.base  = (uint32_t)&idt_entries;

    // Clear the IDT memory (simple loop since we don't have memset yet)
    unsigned char *ptr = (unsigned char *)&idt_entries;
    for(int i = 0; i < sizeof(idt_entry_t) * 256; i++) {
        ptr[i] = 0;
    }

    // Load the IDT into the CPU register
    // We use inline assembly here to call the external 'lidt' instruction wrapper
    // Actually, let's just do the inline assembly for lidt right here for simplicity
    __asm__ volatile ("lidt (%0)" : : "r" (&idt_ptr));
    
    // Note: We haven't added any actual handlers yet! 
    // The CPU will triple fault if an interrupt occurs now.
    // We will add them in the next steps.
}

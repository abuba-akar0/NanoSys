// NanoSys IDT Header
// Defines the structure of the Interrupt Descriptor Table
// The IDT tells the CPU where to jump when an interrupt occurs.

#ifndef IDT_H
#define IDT_H

#include "types.h" // For standard integer types

// A struct defining an IDT entry (Gate Descriptor)
// This structure must match exactly what the x86 CPU expects.
// Total size: 8 bytes
struct idt_entry_struct {
    uint16_t base_low;      // The lower 16 bits of the address to jump to when this interrupt fires
    uint16_t selector;      // Kernel segment selector (usually 0x08 for our code segment)
    uint8_t  always0;       // This must always be 0
    uint8_t  flags;         // Flags: Present? Ring level? Gate type?
    uint16_t base_high;     // The upper 16 bits of the address to jump to
} __attribute__((packed));  // "packed" tells GCC not to add any padding bytes between fields

typedef struct idt_entry_struct idt_entry_t;

// A struct describing a pointer to an array of interrupt handlers.
// This is in a format suitable for the 'lidt' assembly instruction.
struct idt_ptr_struct {
    uint16_t limit;         // The size of the table in bytes - 1
    uint32_t base;          // The address of the first element in our idt_entry_t array
} __attribute__((packed));

typedef struct idt_ptr_struct idt_ptr_t;

// Function prototypes
void idt_init(void);
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);

#endif

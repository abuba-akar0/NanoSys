// NanoSys PIT (Programmable Interval Timer) Driver
// Handles system timer interrupts (IRQ 0)

#include "timer.h"
#include "kernel.h"
#include "ports.h"
#include "idt.h"

static uint32_t tick = 0;

void timer_handler(void) {
    tick++;
    
    // Send EOI to PIC
    outb(0x20, 0x20);
}

void timer_init(uint32_t freq) {
    // The value we send to the PIT is the value to divide it's input clock
    // (1193180 Hz) by, to get our required frequency.
    uint32_t divisor = 1193180 / freq;

    // Send the command byte.
    // 0x36 = 0011 0110
    // 00  - Channel 0
    // 11  - Access mode: lobyte/hibyte
    // 011 - Mode 3 (square wave generator)
    // 0   - 16-bit binary
    outb(0x43, 0x36);

    // Split the divisor into bytes
    uint8_t l = (uint8_t)(divisor & 0xFF);
    uint8_t h = (uint8_t)((divisor >> 8) & 0xFF);

    // Send the frequency divisor
    outb(0x40, l);
    outb(0x40, h);
    
    // Register the interrupt handler (IRQ 0 = Interrupt 32)
    // Note: We'll do this in idt.c or here. Let's rely on idt.c calling the assembly stub
    // which calls our timer_handler.
    
    println("[INFO] PIT initialized");
}

uint32_t get_tick_count(void) {
    return tick;
}

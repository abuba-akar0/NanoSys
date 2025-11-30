// NanoSys Keyboard Driver Header

#ifndef KEYBOARD_H
#define KEYBOARD_H

// Initialize the keyboard driver
void keyboard_init(void);

// Main interrupt handler for keyboard (IRQ 1)
void keyboard_handler(void);

#endif

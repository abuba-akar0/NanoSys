#ifndef TIMER_H
#define TIMER_H

#include "types.h"

// Initialize the PIT
void timer_init(uint32_t freq);

// Timer interrupt handler
void timer_handler(void);

// Get current tick count
uint32_t get_tick_count(void);

#endif

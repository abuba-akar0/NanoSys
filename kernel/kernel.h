// NanoSys Kernel Header
// Main kernel header file with definitions and function prototypes

#ifndef KERNEL_H
#define KERNEL_H

// Basic definitions
#define NULL ((void*)0)

// VGA text mode constants
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

// VGA color codes
#define COLOR_BLACK 0
#define COLOR_BLUE 1
#define COLOR_GREEN 2
#define COLOR_CYAN 3
#define COLOR_RED 4
#define COLOR_MAGENTA 5
#define COLOR_BROWN 6
#define COLOR_LIGHT_GREY 7
#define COLOR_DARK_GREY 8
#define COLOR_LIGHT_BLUE 9
#define COLOR_LIGHT_GREEN 10
#define COLOR_LIGHT_CYAN 11
#define COLOR_LIGHT_RED 12
#define COLOR_LIGHT_MAGENTA 13
#define COLOR_YELLOW 14
#define COLOR_WHITE 15

// Function prototypes
void clear_screen(void);
void putchar(char c);
void print(const char* str);
void println(const char* str);
void set_color(unsigned char fg, unsigned char bg);
void print_hex(unsigned int num);
void print_dec(unsigned int num);
void delay(unsigned int count);
void delay(unsigned int count);
void keyboard_input(char c);
void show_uptime(void);

#endif // KERNEL_H

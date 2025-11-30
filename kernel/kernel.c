// NanoSys Kernel - Main kernel code
// Simple educational OS kernel inspired by early Linux

#include "kernel.h"
#include "idt.h"
#include "keyboard.h"
#include "timer.h"

// VGA text mode buffer
static volatile unsigned short* vga_buffer = (unsigned short*)0xB8000;
static unsigned int cursor_x = 0;
static unsigned int cursor_y = 0;
static unsigned char color = 0x0F; // White on black

// Kernel version
#define NANOSYS_VERSION "0.1.0"

// Clear screen
void clear_screen(void) {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = (color << 8) | ' ';
    }
    cursor_x = 0;
    cursor_y = 0;
}

#include "ports.h"

// Update hardware cursor
void update_cursor(int x, int y) {
    uint16_t pos = y * VGA_WIDTH + x;
    
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

// Print a character
void putchar(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else if (c == '\t') {
        cursor_x = (cursor_x + 4) & ~(4 - 1);
    } else if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
            vga_buffer[cursor_y * VGA_WIDTH + cursor_x] = (color << 8) | ' ';
        }
    } else {
        vga_buffer[cursor_y * VGA_WIDTH + cursor_x] = (color << 8) | c;
        cursor_x++;
    }

    // Handle line wrap
    if (cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }

    // Handle screen scrolling
    if (cursor_y >= VGA_HEIGHT) {
        // Scroll up
        for (int i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++) {
            vga_buffer[i] = vga_buffer[i + VGA_WIDTH];
        }
        // Clear last line
        for (int i = (VGA_HEIGHT - 1) * VGA_WIDTH; i < VGA_HEIGHT * VGA_WIDTH; i++) {
            vga_buffer[i] = (color << 8) | ' ';
        }
        cursor_y = VGA_HEIGHT - 1;
    }
    
    // Update hardware cursor
    update_cursor(cursor_x, cursor_y);
}

// Print a string
void print(const char* str) {
    while (*str) {
        putchar(*str++);
    }
}

// Print a string with newline
void println(const char* str) {
    print(str);
    putchar('\n');
}

// Set text color
void set_color(unsigned char fg, unsigned char bg) {
    color = (bg << 4) | (fg & 0x0F);
}

// Print a number in hexadecimal
void print_hex(unsigned int num) {
    char hex_chars[] = "0123456789ABCDEF";
    print("0x");
    for (int i = 28; i >= 0; i -= 4) {
        putchar(hex_chars[(num >> i) & 0xF]);
    }
}

// Simple delay function
void delay(unsigned int count) {
    for (volatile unsigned int i = 0; i < count * 10000; i++);
}

// Display welcome banner
void display_banner(void) {
    set_color(COLOR_LIGHT_CYAN, COLOR_BLACK);
    println("=====================================");
    print("     NanoSys v");
    print(NANOSYS_VERSION);
    println(" - Educational OS");
    println("=====================================");
    set_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    println("");
}

// Display system information
void display_system_info(void) {
    set_color(COLOR_LIGHT_GREEN, COLOR_BLACK);
    println("[INFO] System initialized successfully");
    set_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    println("[INFO] CPU: Protected Mode (32-bit)");
    println("[INFO] Memory: Basic memory management");
    println("[INFO] Display: VGA Text Mode (80x25)");
    println("");
}

// Simulate some boot processes
void boot_sequence(void) {
    const char* services[] = {
        "Initializing IDT...",
        "Setting up memory management...",
        "Detecting hardware...",
        "Loading system modules...",
        "Starting kernel services...",
        NULL
    };

    set_color(COLOR_YELLOW, COLOR_BLACK);
    for (int i = 0; services[i] != NULL; i++) {
        print("[ ");
        set_color(COLOR_LIGHT_GREEN, COLOR_BLACK);
        print("OK");
        set_color(COLOR_YELLOW, COLOR_BLACK);
        print(" ] ");
        set_color(COLOR_LIGHT_GREY, COLOR_BLACK);
        println(services[i]);
        delay(50);
    }
    println("");
}

#include "utils.h"

#define CMD_BUF_SIZE 128
static char cmd_buffer[CMD_BUF_SIZE];
static int cmd_len = 0;

void keyboard_input(char c) {
    if (c == '\n') {
        putchar('\n');
        cmd_buffer[cmd_len] = 0; // Null terminate
        
        // Process command
        if (cmd_len > 0) {
            if (strcmp(cmd_buffer, "help") == 0) {
                println("Available commands:");
                println("  help    - Display this help message");
                println("  info    - Display system information");
                println("  clear   - Clear the screen");
                println("  time    - Display uptime");
                println("  calc    - Calculator (e.g., calc 10 + 5)");
            } else if (strcmp(cmd_buffer, "info") == 0) {
                display_system_info();
            } else if (strcmp(cmd_buffer, "clear") == 0) {
                clear_screen();
            } else if (strcmp(cmd_buffer, "time") == 0) {
                show_uptime();
            } else if (cmd_buffer[0] == 'c' && cmd_buffer[1] == 'a' && cmd_buffer[2] == 'l' && cmd_buffer[3] == 'c' && cmd_buffer[4] == ' ') {
                // Simple parser for "calc A + B"
                // Format expected: "calc 10 + 20"
                char* p = cmd_buffer + 5;
                
                // Parse first number
                int num1 = atoi(p);
                
                // Find operator
                while (*p && isdigit(*p)) p++;
                while (*p && *p == ' ') p++;
                char op = *p;
                p++;
                
                // Parse second number
                while (*p && *p == ' ') p++;
                int num2 = atoi(p);
                
                int result = 0;
                if (op == '+') result = num1 + num2;
                else if (op == '-') result = num1 - num2;
                else if (op == '*') result = num1 * num2;
                else if (op == '/') {
                    if (num2 != 0) result = num1 / num2;
                    else {
                        println("Error: Division by zero");
                        goto cmd_done;
                    }
                } else {
                    println("Error: Invalid operator. Use +, -, *, /");
                    goto cmd_done;
                }
                
                print("Result: ");
                print_dec(result);
                println("");
                
                cmd_done:;
            } else {
                print("Unknown command: ");
                println(cmd_buffer);
            }
        }
        
        // Reset buffer
        cmd_len = 0;
        print("> "); // Prompt
    } else if (c == '\b') {
        if (cmd_len > 0) {
            putchar('\b');
            cmd_len--;
        }
    } else {
        if (cmd_len < CMD_BUF_SIZE - 1) {
            putchar(c);
            cmd_buffer[cmd_len++] = c;
        }
    }
}

// Simple command prompt
void command_prompt(void) {
    set_color(COLOR_LIGHT_CYAN, COLOR_BLACK);
    println("=====================================");
    println("      NanoSys Shell v0.1");
    println("=====================================");
    set_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    println("");
    println("Type 'help' for commands.");
    print("> ");
}

// Color test
void color_test(void) {
    println("");
    println("Color Test:");
    for (int i = 0; i < 16; i++) {
        set_color(i, COLOR_BLACK);
        print("Color ");
        print_hex(i);
        println("");
    }
    set_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    println("");
}

// Print a number in decimal
void print_dec(unsigned int num) {
    if (num == 0) {
        putchar('0');
        return;
    }

    char buffer[12]; // Enough for 32-bit int
    int i = 0;
    
    while (num > 0) {
        buffer[i++] = (num % 10) + '0';
        num /= 10;
    }
    
    while (--i >= 0) {
        putchar(buffer[i]);
    }
}

// Simple uptime counter
void show_uptime(void) {
    uint32_t ticks = get_tick_count();
    uint32_t seconds = ticks / 100; // Assuming 100Hz
    uint32_t minutes = seconds / 60;
    seconds = seconds % 60;
    
    println("");
    set_color(COLOR_LIGHT_GREEN, COLOR_BLACK);
    print("System uptime: ");
    print_dec(minutes);
    print("m ");
    print_dec(seconds);
    println("s");
    set_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    println("");
}

// Main kernel function
void kernel_main(void) {
    // Clear screen
    clear_screen();

    // Display boot banner
    display_banner();

    // Initialize IDT
    idt_init();

    // Initialize Keyboard
    keyboard_init();

    // Initialize Timer (100 Hz)
    timer_init(100);

    // Boot sequence
    boot_sequence();

    // Display system info
    display_system_info();

    // Display command prompt
    command_prompt();

    // Demonstrate various features
    // color_test(); // Commented out to keep screen clean
    show_uptime();
    
    // Success message
    set_color(COLOR_LIGHT_GREEN, COLOR_BLACK);
    println("=====================================");
    println("   NanoSys kernel running!");
    println("   System is operational.");
    println("=====================================");
    set_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    println("");
    println("Type something! (Keyboard is now active)");

    // Idle loop
    while (1) {
        __asm__ volatile("hlt");
    }
}

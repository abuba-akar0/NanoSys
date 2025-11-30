# NanoSys - Shell & Applications Documentation

## Overview
The **Shell** is the user interface of NanoSys. It allows the user to type commands and receive output. Since we don't have a standard library (libc), we implement everything from scratch.

**Files:**
*   `kernel/kernel.c`: Contains the shell loop and command parsing.
*   `kernel/utils.c`: Helper functions.

## The Command Buffer
We cannot just process one character at a time. We need to store what the user types until they press `ENTER`.

```c
#define CMD_BUF_SIZE 128
static char cmd_buffer[CMD_BUF_SIZE];
static int cmd_len = 0;
```

**Input Logic (`keyboard_input`):**
1.  **Character Typed:** Add to `cmd_buffer` and print to screen.
2.  **Backspace:** Remove last char from buffer and update screen.
3.  **Enter:** Null-terminate the string and pass it to the command parser.

## Command Parsing
We compare the buffer string against known commands using `strcmp`.

```c
if (strcmp(cmd_buffer, "help") == 0) {
    // Show help
} else if (strcmp(cmd_buffer, "clear") == 0) {
    clear_screen();
}
```

## The Calculator (`calc`)
The calculator demonstrates parsing arguments from a command string.

**Example:** `calc 10 + 20`

**Logic:**
1.  **Skip "calc ":** Start pointer at index 5.
2.  **Parse First Number:** Use `atoi()` (ASCII to Integer) to read "10".
3.  **Find Operator:** Skip digits and spaces until we find `+`.
4.  **Parse Second Number:** Skip operator and spaces, then `atoi()` to read "20".
5.  **Compute:** Perform the math (`10 + 20 = 30`).
6.  **Print:** Use `print_dec()` to show "30".

## Utility Functions (`utils.c`)
Since we are "freestanding", we implemented:
*   `strcmp(s1, s2)`: Returns 0 if strings match.
*   `atoi(str)`: Converts "123" to integer `123`.
*   `isdigit(c)`: Returns true if char is '0'-'9'.
*   `print_dec(num)`: Converts integer to string and prints it (using modulo/division).

---
*Next: Read `06_Build_System.md` to learn how to compile it all.*

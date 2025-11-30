# 🖥️ NanoSys - Educational x86 Operating System

![Version](https://img.shields.io/badge/version-0.1.0-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)
![Build](https://img.shields.io/badge/build-passing-brightgreen.svg)

**NanoSys** is a lightweight, 32-bit operating system kernel built from scratch. It demonstrates the magic of OS development—from the first byte of the bootloader to a working interactive shell.

> *"Talk is cheap. Show me the code." - Linus Torvalds*

---

## ✨ Features

### 🚀 Core System
-   **Custom Bootloader:** 512-byte assembly magic that switches to 32-bit Protected Mode.
-   **Kernel:** Written in C, running at `0x10000`.
-   **Drivers:**
    -   🎹 **Keyboard:** PS/2 driver with interrupt handling.
    -   ⏱️ **Timer:** Programmable Interval Timer (PIT) for real-time tracking.
    -   🖥️ **VGA:** Direct memory access text mode driver (80x25).

### 🛠️ User Experience
-   **Interactive Shell:** Type commands, clear screen, and explore.
-   **Calculator:** Built-in `calc` command (e.g., `calc 10 + 20`).
-   **System Stats:** Real-time uptime counter.

---

## 📚 Documentation

We believe in understanding **how** things work. Check out our detailed guides in the `Docs/` folder:

-   [📖 01_Overview.md](Docs/01_Overview.md) - The Big Picture.
-   [💾 02_Bootloader.md](Docs/02_Bootloader.md) - How it starts.
-   [🧠 03_Kernel_Core.md](Docs/03_Kernel_Core.md) - The brain of the OS.
-   [⚡ 04_Interrupts_and_Drivers.md](Docs/04_Interrupts_and_Drivers.md) - Handling hardware.
-   [🐚 05_Shell_and_Apps.md](Docs/05_Shell_and_Apps.md) - The user interface.
-   [🔨 06_Build_System.md](Docs/06_Build_System.md) - How to compile it.

---

## 🚀 Quick Start

### Prerequisites
-   **QEMU** (x86 Emulator)
-   **NASM** (Assembler)
-   **GCC Cross-Compiler** (`i686-elf-gcc`)
-   **Make** (or use provided `build.bat` on Windows)

### Running on Windows
Simply run the batch script:
```bash
.\build.bat run
```

### Running on Linux/Mac
```bash
make run
```

---

## 🎮 Commands

Once the OS is running, try these commands:

| Command | Description | Example |
| :--- | :--- | :--- |
| `help` | Show available commands | `help` |
| `clear` | Clear the screen | `clear` |
| `time` | Show system uptime | `time` |
| `calc` | Perform arithmetic | `calc 42 * 10` |
| `info` | Show system info | `info` |

---

## 🏗️ Project Structure

```
TestOS/
├── boot/           # Assembly bootloader
├── kernel/         # C Kernel & Drivers
├── build/          # Compiled binaries
├── Docs/           # Documentation
├── build.bat       # Build script
└── linker.ld       # Memory layout
```

## 🤝 Contributing
This is an educational project. Feel free to fork, break, and fix it!
1.  Fork the repo.
2.  Create a feature branch.
3.  Submit a Pull Request.

## 📄 License
MIT License. Use it to learn, teach, or build the next Linux!

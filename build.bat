@echo off
REM NanoSys Build Script for Windows
REM Simple batch file to build the OS

echo =====================================
echo    Building NanoSys v0.1.0
echo =====================================
echo.

REM Check if build directory exists
if not exist build mkdir build

echo [1/6] Assembling bootloader...
nasm -f bin boot\boot.asm -o build\boot.bin
if %errorlevel% neq 0 goto :error

echo [2/6] Assembling kernel entry...
nasm -f elf32 kernel\kernel_entry.asm -o build\kernel_entry.o
if %errorlevel% neq 0 goto :error

echo [3/6] Compiling kernel...
i686-elf-gcc -ffreestanding -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -O2 -c kernel\kernel.c -o build\kernel.o
i686-elf-gcc -ffreestanding -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -O2 -c kernel\idt.c -o build\idt.o
i686-elf-gcc -ffreestanding -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -O2 -c kernel\keyboard.c -o build\keyboard.o
i686-elf-gcc -ffreestanding -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -O2 -c kernel\timer.c -o build\timer.o
i686-elf-gcc -ffreestanding -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -O2 -c kernel\utils.c -o build\utils.o
if %errorlevel% neq 0 goto :error

echo [3.5/6] Assembling interrupts...
nasm -f elf32 kernel\interrupts.asm -o build\interrupts.o
if %errorlevel% neq 0 goto :error

echo [4/6] Linking kernel...
i686-elf-ld -T linker.ld -o build\kernel.bin build\kernel_entry.o build\interrupts.o build\kernel.o build\idt.o build\keyboard.o build\timer.o build\utils.o
if %errorlevel% neq 0 goto :error

echo [5/6] Creating OS image...
copy /b build\boot.bin + build\kernel.bin build\nanosys.img > nul
if %errorlevel% neq 0 goto :error

echo [6/6] Build complete!
echo.
echo =====================================
echo    NanoSys image created!
echo    Location: build\nanosys.img
echo =====================================
echo.
echo To run: qemu-system-i386 -fda build\nanosys.img
echo Or use: build.bat run
goto :end

:error
echo.
echo =====================================
echo    Build failed!
echo =====================================
pause
exit /b 1

:end
if "%1"=="run" (
    echo.
    echo Starting NanoSys in QEMU...
    qemu-system-i386 -fda build\nanosys.img -m 32M
)

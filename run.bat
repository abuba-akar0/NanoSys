@echo off
REM NanoSys Run Script
REM Builds and runs NanoSys in QEMU

echo Starting NanoSys...
echo.

REM Check if image exists
if not exist build\nanosys.img (
    echo OS image not found. Building...
    call build.bat
    if %errorlevel% neq 0 exit /b 1
)

echo Launching QEMU...
qemu-system-i386 -fda build\nanosys.img -m 32M

echo.
echo NanoSys terminated.
pause

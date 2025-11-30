; NanoSys Kernel Entry Point
; Assembly entry point for the kernel

[BITS 32]
[EXTERN kernel_main]    ; C kernel main function

section .text
global _start

_start:
    ; Setup stack
    mov esp, 0x90000

    ; Call C kernel
    call kernel_main

    ; Hang if kernel returns
    cli
.hang:
    hlt
    jmp .hang

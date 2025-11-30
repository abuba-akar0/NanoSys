; NanoSys Interrupt Wrappers
; This file contains the low-level assembly code to handle interrupts.
; When an interrupt occurs, the CPU jumps here. We save the state,
; call the C handler, and then restore the state.

[BITS 32]

; Export symbols to C
global isr_handler_stub   ; We'll keep this for keyboard for now to match existing code
global timer_stub         ; New stub for timer

; Import symbols from C
extern isr_handler        ; Keyboard handler (we'll rename this in C later or keep as is)
extern timer_handler      ; Timer handler

section .text

; Timer Interrupt Handler (IRQ 0)
timer_stub:
    pusha
    push ds
    push es
    push fs
    push gs
    
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    call timer_handler
    
    pop gs
    pop fs
    pop es
    pop ds
    popa
    iret

; Keyboard Interrupt Handler (IRQ 1)
isr_handler_stub:
    pusha
    push ds
    push es
    push fs
    push gs

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call isr_handler

    pop gs
    pop fs
    pop es
    pop ds
    popa
    iret

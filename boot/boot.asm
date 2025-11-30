; NanoSys Bootloader - Stage 1
; Simple bootloader inspired by early Linux
; Loads kernel into memory and jumps to it

[BITS 16]
[ORG 0x7C00]

start:
    ; Setup segments
    cli                     ; Disable interrupts
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00          ; Stack grows downward from bootloader
    sti                     ; Enable interrupts

    ; Display boot message
    mov si, boot_msg
    call print_string

    ; Load kernel from disk
    mov si, loading_msg
    call print_string

    ; Save boot drive number (BIOS passes it in DL)
    mov [boot_drive], dl

    ; Reset disk system
    mov ah, 0x00            ; Reset disk function
    mov dl, [boot_drive]    ; Use the boot drive
    int 0x13
    jc disk_error

    ; Read kernel sectors (load 32 sectors starting at sector 2)
    mov ah, 0x02            ; Read sectors function
    mov al, 32              ; Number of sectors to read (increased)
    mov ch, 0               ; Cylinder 0
    mov cl, 2               ; Start at sector 2 (sector 1 is boot sector)
    mov dh, 0               ; Head 0
    mov dl, [boot_drive]    ; Drive number from BIOS
    mov bx, 0x1000          ; Segment to load to
    mov es, bx
    xor bx, bx              ; Offset 0x0000
    int 0x13
    jc disk_error

    ; Verify sectors were read
    cmp al, 32              ; AL should contain number of sectors read
    jne disk_error

    ; Enter protected mode
    mov si, pmode_msg
    call print_string

    cli                     ; Disable interrupts

    ; Load GDT
    lgdt [gdt_descriptor]

    ; Enable A20 line
    in al, 0x92
    or al, 2
    out 0x92, al

    ; Set PE bit in CR0
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; Far jump to flush pipeline and enter 32-bit mode
    jmp 0x08:protected_mode

disk_error:
    mov si, disk_error_msg
    call print_string
    jmp $

; Print string in real mode (SI = string address)
print_string:
    pusha
    mov ah, 0x0E            ; BIOS teletype function
.loop:
    lodsb
    cmp al, 0
    je .done
    int 0x10
    jmp .loop
.done:
    popa
    ret

; Messages
boot_msg        db 'NanoSys Bootloader v1.0', 0x0D, 0x0A, 0
loading_msg     db 'Loading kernel...', 0x0D, 0x0A, 0
disk_error_msg  db 'Disk read error!', 0x0D, 0x0A, 0
pmode_msg       db 'Entering protected mode...', 0x0D, 0x0A, 0

; Variables
boot_drive      db 0

; Global Descriptor Table
gdt_start:
    ; Null descriptor
    dq 0

    ; Code segment descriptor
    dw 0xFFFF       ; Limit (bits 0-15)
    dw 0            ; Base (bits 0-15)
    db 0            ; Base (bits 16-23)
    db 10011010b    ; Access byte (present, ring 0, code segment, executable, readable)
    db 11001111b    ; Flags and limit (4KB granularity, 32-bit, limit bits 16-19)
    db 0            ; Base (bits 24-31)

    ; Data segment descriptor
    dw 0xFFFF       ; Limit (bits 0-15)
    dw 0            ; Base (bits 0-15)
    db 0            ; Base (bits 16-23)
    db 10010010b    ; Access byte (present, ring 0, data segment, writable)
    db 11001111b    ; Flags and limit
    db 0            ; Base (bits 24-31)

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; Size
    dd gdt_start                ; Offset

[BITS 32]
protected_mode:
    ; Setup segments for protected mode
    mov ax, 0x10        ; Data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000    ; New stack

    ; Jump to kernel
    jmp 0x08:0x10000

times 510-($-$$) db 0
dw 0xAA55               ; Boot signature

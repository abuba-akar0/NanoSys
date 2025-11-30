/* NanoSys Linker Script */
/* Defines memory layout for the kernel */

OUTPUT_FORMAT("binary")
ENTRY(_start)

SECTIONS
{
    . = 0x10000;  /* Kernel loads at 1MB */

    .text : ALIGN(4096)
    {
        *(.text)
    }

    .rodata : ALIGN(4096)
    {
        *(.rodata)
    }

    .data : ALIGN(4096)
    {
        *(.data)
    }

    .bss : ALIGN(4096)
    {
        *(COMMON)
        *(.bss)
    }
}

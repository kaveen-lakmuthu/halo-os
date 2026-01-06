ENTRY(start)

SECTIONS {
    /* 1. Boot Section (Physical 1MB) */
    . = 1M;

    .boot :
    {
        /* Define the physical start for the header */
        header_start = .;
        KEEP(*(.multiboot_header))
        *(.boot)
    }

    /* 2. Higher Half Kernel */
    . += 0xFFFFFFFF80000000;

    .text : AT(ADDR(.boot) + SIZEOF(.boot))
    {
        *(.text)
    }

    .rodata : AT(ADDR(.text) + SIZEOF(.text))
    {
        *(.rodata)
    }

    .data : AT(ADDR(.rodata) + SIZEOF(.rodata))
    {
        *(.data)
    }

    .bss : AT(ADDR(.data) + SIZEOF(.data))
    {
        *(COMMON)
        *(.bss)
    }

    /* CALCULATE PHYSICAL END */
    /* Take current Virtual Address (.) and subtract the Offset */
    kernel_physical_end = . - 0xFFFFFFFF80000000;
}
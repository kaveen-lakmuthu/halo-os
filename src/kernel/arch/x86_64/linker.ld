ENTRY(start)

SECTIONS {
    /* 1. Boot Section (Physical 1MB) */
    . = 1M;

    .boot :
    {
        header_start = .;
        KEEP(*(.multiboot_header))
        *(.boot)
    }

    /* 2. Higher Half Kernel Start */
    . += 0xFFFFFFFF80000000;

    .text : AT(ADDR(.boot) + SIZEOF(.boot))
    {
        *(.text)
    }

    .rodata : AT(ADDR(.text) + SIZEOF(.text))
    {
        *(.rodata)
    }

    /* --- THE FIX: DISCARD UNUSED SECTIONS --- */
    /* This prevents .eh_frame from colliding with .data */
    /DISCARD/ :
    {
        *(.eh_frame)
        *(.comment)
    }

    .data : AT(ADDR(.rodata) + SIZEOF(.rodata))
    {
        *(.data)
    }

    /* 3. End of File (For Header.asm) */
    kernel_file_end = . - 0xFFFFFFFF80000000;

    .bss : AT(ADDR(.data) + SIZEOF(.data))
    {
        *(COMMON)
        *(.bss)
    }

    /* 4. End of Memory (For kmain.c PMM safety) */
    kernel_physical_end = . - 0xFFFFFFFF80000000;
}
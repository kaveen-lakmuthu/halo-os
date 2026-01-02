ENTRY(start)

SECTIONS {
    /* GRUB loads at 1MB (1M) */
    . = 1M;

    .boot :
    {
        /* Ensure the Multiboot Header comes first */
        KEEP(*(.multiboot_header))
    }

    .text :
    {
        *(.text)
    }
    
    .bss :
    {
        *(COMMON)
        *(.bss)
    }
}
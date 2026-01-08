section .multiboot_header
align 8

extern kernel_physical_end
extern start

header_start:
    ; 1. Magic
    dd 0xe85250d6
    ; 2. Arch
    dd 0
    ; 3. Length
    dd header_end - header_start
    ; 4. Checksum
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

    ; 5. Address Tag
    dw 2
    dw 0
    dd 24
    dd header_start         ; header_addr
    dd header_start         ; load_addr
    dd kernel_physical_end  ; load_end_addr
    dd kernel_physical_end  ; bss_end_addr (Let GRUB zero the normal BSS)

    ; 6. Entry Tag
    align 8
    dw 3
    dw 0
    dd 12
    dd start

    ; 7. End Tag
    align 8
    dw 0
    dw 0
    dd 8
header_end:
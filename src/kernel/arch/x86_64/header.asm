section .multiboot_header
align 8

; Need these symbols from the Linker and Boot assembly
extern kernel_physical_end
extern start

header_start:
    ; 1. Magic Number
    dd 0xe85250d6

    ; 2. Architecture
    dd 0

    ; 3. Header Length
    dd header_end - header_start

    ; 4. Checksum
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

    ; 5. Address Tag (Type 2)
    ; Forces GRUB to load manually.
    dw 2                    ; Type
    dw 0                    ; Flags
    dd 24                   ; Size
    dd header_start         ; header_addr
    dd header_start         ; load_addr
    dd kernel_physical_end  ; load_end_addr
    dd kernel_physical_end  ; bss_end_addr

    ; 6. Entry Address Tag (Type 3) -- NEW!
    ; Tells GRUB exactly where to jump (the 'start' label)
    align 8
    dw 3                    ; Type
    dw 0                    ; Flags
    dd 12                   ; Size
    dd start                ; Entry Address

    ; 7. End Tag
    align 8
    dw 0
    dw 0
    dd 8
header_end:
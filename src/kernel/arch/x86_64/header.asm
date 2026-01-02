section .multiboot_header
align 8

header_start:
    ; 1. Magic Number
    ; This specific number tells GRUB "I am a Multiboot2 kernel"
    dd 0xe85250d6

    ; 2. Architecture
    ; 0 = i386 (Protected Mode).
    ; Even though we want 64-bit, GRUB starts us in 32-bit mode.
    dd 0

    ; 3. Header Length
    ; Calculates the size of this header automatically
    dd header_end - header_start

    ; 4. Checksum
    ; The Magic + Arch + Length + Checksum must equal 0.
    ; This proves the header isn't corrupted.
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

    ; 5. End Tag
    ; Tells GRUB we are done with our settings.
    dw 0    ; Type
    dw 0    ; Flags
    dd 8    ; Size
header_end:
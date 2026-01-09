global gdt_load

section .text
bits 64

gdt_load:
    ; 1. Load the GDT
    lgdt [rdi]

    ; 2. Reload Data Segments (Offset 0x10)
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; 3. Reload Code Segment (Offset 0x08)
    ; Push the Code Segment and the address, then use retfq
    push 0x08           ; New CS
    lea rax, [rel .reload_cs] 
    push rax            ; Return Address
    retfq               ; Far Return (Loads CS and RIP)

.reload_cs:
    ret
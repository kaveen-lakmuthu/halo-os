global load_cr3

section .text
bits 64

; void load_cr3(uint64_t pml4_addr)
load_cr3:
    mov cr3, rdi  ; Move the argument (PML4 address) into CR3
    ret
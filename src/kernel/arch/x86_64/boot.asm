global start
extern kmain

section .text
bits 32     ; We are still in 32-bit Protected Mode!

start:
    ; 1. Set up the Stack
    ; C needs a stack to push variables onto.
    mov esp, stack_top

    ; 2. Call the Kernel
    call kmain

    ; 3. Hang
    ; If kmain returns, we halt the CPU.
    hlt
    jmp $

section .bss
align 16
stack_bottom:
    ; Reserve 16KB for the kernel stack
    resb 16384
stack_top:
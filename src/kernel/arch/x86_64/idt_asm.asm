global idt_load

section .text
bits 64

; void idt_load(struct idt_ptr* ptr);
idt_load:
    lidt [rdi]  ; Load IDT from the pointer passed in RDI
    ret
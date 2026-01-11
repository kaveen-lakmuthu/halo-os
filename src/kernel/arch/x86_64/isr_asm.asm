global isr_stub_table ; Export the array so C can see it

; --- MACROS ---
; Macro for exceptions that DO NOT push an error code
; Push a dummy '0' so the stack format is consistent
%macro ISR_NOERRCODE 1
    global isr%1
    isr%1:
        cli             ; Disable interrupts
        push 0          ; Push dummy error code
        push %1         ; Push interrupt number
        jmp isr_common_stub
%endmacro

; Macro for exceptions that DO push an error code automatically
%macro ISR_ERRCODE 1
    global isr%1
    isr%1:
        cli
        ; Error code is already pushed by CPU
        push %1         ; Push interrupt number
        jmp isr_common_stub
%endmacro

section .text
extern isr_handler

; --- THE COMMON STUB ---
; All ISRs jump here to save state and call C
isr_common_stub:
    ; 1. Save CPU state (Registers not saved by CPU)
    push rax
    push rcx
    push rdx
    push rbx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    ; 2. Call the C Handler
    ; System V ABI requires the first argument in RDI.
    ; Pass the Stack Pointer (RSP), which points to the struct.
    mov rdi, rsp
    call isr_handler

    ; 3. Restore CPU state
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    pop rdx
    pop rcx
    pop rax

    ; 4. Clean up stack
    ; Pushed 'int_no' and 'err_code' (8 bytes each = 16 bytes).
    ; Need to remove them before returning, or 'iretq' will read garbage.
    add rsp, 16
    
    ; 5. Return from Interrupt
    iretq

; --- DEFINE THE 32 HANDLERS ---
ISR_NOERRCODE 0  ; Divide by Zero
ISR_NOERRCODE 1  ; Debug
ISR_NOERRCODE 2  ; NMI
ISR_NOERRCODE 3  ; Breakpoint
ISR_NOERRCODE 4  ; Overflow
ISR_NOERRCODE 5  ; Bound Range
ISR_NOERRCODE 6  ; Invalid Opcode
ISR_NOERRCODE 7  ; Device Not Available
ISR_ERRCODE   8  ; Double Fault
ISR_NOERRCODE 9  ; Coprocessor Segment Overrun
ISR_ERRCODE   10 ; Invalid TSS
ISR_ERRCODE   11 ; Segment Not Present
ISR_ERRCODE   12 ; Stack-Segment Fault
ISR_ERRCODE   13 ; General Protection Fault
ISR_ERRCODE   14 ; Page Fault
ISR_NOERRCODE 15 ; Reserved
ISR_NOERRCODE 16 ; x87 Floating-Point Exception
ISR_ERRCODE   17 ; Alignment Check
ISR_NOERRCODE 18 ; Machine Check
ISR_NOERRCODE 19 ; SIMD Floating-Point Exception
ISR_NOERRCODE 20 ; Virtualization Exception
ISR_NOERRCODE 21 ; Reserved
ISR_NOERRCODE 22 ; Reserved
ISR_NOERRCODE 23 ; Reserved
ISR_NOERRCODE 24 ; Reserved
ISR_NOERRCODE 25 ; Reserved
ISR_NOERRCODE 26 ; Reserved
ISR_NOERRCODE 27 ; Reserved
ISR_NOERRCODE 28 ; Reserved
ISR_NOERRCODE 29 ; Reserved
ISR_ERRCODE   30 ; Security Exception
ISR_NOERRCODE 31 ; Reserved

; --- THE TABLE OF POINTERS ---
; This allows C to just loop through 'isr_stub_table'
section .data
align 8
isr_stub_table:
    dq isr0, isr1, isr2, isr3, isr4, isr5, isr6, isr7
    dq isr8, isr9, isr10, isr11, isr12, isr13, isr14, isr15
    dq isr16, isr17, isr18, isr19, isr20, isr21, isr22, isr23
    dq isr24, isr25, isr26, isr27, isr28, isr29, isr30, isr31
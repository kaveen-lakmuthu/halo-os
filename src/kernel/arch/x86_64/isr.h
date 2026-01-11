#ifndef ISR_H
#define ISR_H

#include <stdint.h>

// This structure represents the state of the CPU stack when an interrupt happens.
// The Assembly stub will push these values, and will read them in C.
struct interrupt_frame {
    // 1. Pushed manually by our Assembly Stub (isr_asm.asm)
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rdi, rsi, rbp, rbx, rdx, rcx, rax;
    
    // 2. Pushed manually by Assembly Stub to identify the interrupt
    uint64_t int_no;      // Interrupt Number (e.g., 0 = Divide by Zero)
    uint64_t err_code;    // Error Code (some exceptions push this automatically, else push 0)

    // 3. Pushed automatically by the CPU
    uint64_t rip;         // Return Instruction Pointer (where to jump back)
    uint64_t cs;          // Code Segment Selector
    uint64_t rflags;      // CPU Flags
    uint64_t rsp;         // Stack Pointer
    uint64_t ss;          // Stack Segment Selector
} __attribute__((packed));

// The "Main" handler
void isr_handler(struct interrupt_frame* frame);

// Initializer to register gates
void isr_init(void);

#endif
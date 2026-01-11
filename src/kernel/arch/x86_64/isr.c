#include "isr.h"
#include "idt.h"
#include "../../drivers/vga.h"

// Import the array of pointers from assembly
extern void* isr_stub_table[];

// Messages for the first 32 exceptions (Intel Defined)
const char* exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void isr_handler(struct interrupt_frame* frame) {
    terminal_setcolor(VGA_COLOR_LIGHT_RED);
    terminal_writestring("\n\n=== INTERRUPT EXCEPTION ===\n");
    
    terminal_writestring("Type: ");
    if (frame->int_no < 32) {
        terminal_writestring(exception_messages[frame->int_no]);
    } else {
        terminal_writestring("Unknown");
    }
    
    terminal_writestring("\nException Number: ");
    
    terminal_writestring("\n\nSYSTEM HALTED.");
    
    // Hang forever
    while(1) {
        __asm__("hlt");
    }
}

void isr_init(void) {
    // Loop through the first 32 entries (Exceptions)
    for (int i = 0; i < 32; i++) {
        // 0x8E = 10001110b
        // Present (1), Ring 0 (00), Interrupt Gate (1110)
        idt_set_gate(i, (uint64_t)isr_stub_table[i], 0x08, 0x8E);
    }
}
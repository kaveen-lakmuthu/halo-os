#include "isr.h"
#include "idt.h"
#include "../../drivers/vga.h"
#include "../../drivers/pic.h"
#include "../../drivers/keyboard.h"

// Import the array of pointers from assembly
extern void* isr_stub_table[];

/**
 * @brief Print an unsigned 64-bit integer to the terminal.
 *
 * @param[in] num The number to print
 */
static void terminal_putint(uint64_t num) {
    if (num == 0) {
        terminal_putchar('0');
        return;
    }

    char buffer[20];
    int idx = 0;
    uint64_t temp = num;

    // Extract digits in reverse
    while (temp > 0) {
        buffer[idx++] = '0' + (temp % 10);
        temp /= 10;
    }

    // Print in correct order
    while (idx > 0) {
        terminal_putchar(buffer[--idx]);
    }
}

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
    "Control Protection Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Security Exception",
    "Reserved",
    "Reserved"
};

// Simple handler for IRQs
void irq_handler(struct interrupt_frame* frame) {
    // IRQ 1 = Keyboard (Vector 33)
    if (frame->int_no == 33) {
        keyboard_handler();
    }

    // IMPORTANT: Tell PIC the process is done, or it will never send another interrupt.
    // IRQ number = Interrupt Number - 32
    pic_send_eoi(frame->int_no - 32);
}

/**
 * @brief Handle CPU exceptions and display error information.
 *
 * @param[in] frame Pointer to the interrupt frame with exception details
 */
void isr_handler(struct interrupt_frame* frame) {
    // 1. Exceptions (0-31)
    if (frame->int_no < 32) {
        terminal_setcolor(VGA_COLOR_LIGHT_RED);
        terminal_writestring("\n=== INTERRUPT EXCEPTION ===\n");
        
        terminal_writestring("Type: ");
        terminal_writestring(exception_messages[frame->int_no]);
        
        terminal_writestring("\nNumber: ");
        terminal_writehex(frame->int_no);
        
        terminal_writestring("\nError:  ");
        terminal_writehex(frame->err_code);
        
        terminal_writestring("\nRIP:    ");
        terminal_writehex(frame->rip);
        
        terminal_writestring("\n\nSYSTEM HALTED.");
        while(1) { __asm__("hlt"); }
    } 
    // 2. Hardware Interrupts
    else if (frame->int_no >= 32 && frame->int_no < 48) {
        irq_handler(frame);
    }
}

/**
 * @brief Initialize interrupt service routines for exceptions.
 *
 * Sets up IDT gates for the first 32 CPU exceptions (Intel defined).
 */
void isr_init(void) {
    // Change loop limit from 32 to 48!
    for (int i = 0; i < 48; i++) {
        idt_set_gate(i, (uint64_t)isr_stub_table[i], 0x08, 0x8E);
    }
}
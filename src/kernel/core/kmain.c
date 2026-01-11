#include "../drivers/vga.h"
#include "../memory/pmm.h"
#include "../arch/x86_64/gdt.h"
#include "../arch/x86_64/idt.h"
#include "../arch/x86_64/isr.h"

void kmain(uint64_t multiboot_addr) {
    // Silence compiler warning
    (void)multiboot_addr;

    terminal_initialize();
    terminal_writestring("Halo OS Kernel Initializing...\n");

    // GDT
    gdt_init();
    terminal_writestring("[GDT] Loaded.\n");

    // IDT
    idt_init();
    terminal_writestring("[IDT] Loaded.\n");

    // ISR (Connect the gates)
    isr_init();
    terminal_writestring("[ISR] Handlers installed.\n");

    // // Enable Interrupts
    // // "sti" stands for Set Interrupt Flag. The CPU will now listen to interrupts.
    // __asm__ volatile ("sti"); 
    // terminal_writestring("[CPU] Interrupts Enabled.\n");

    // THE TEST: Sabotage!
    terminal_writestring("Testing Divide By Zero...\n");
    volatile int a = 5;
    volatile int b = 0;
    volatile int c = a / b; 

    (void)c;

    // Should never reach this line
    terminal_writestring("If you see this, the test FAILED.\n");

    while(1) { __asm__("hlt"); }
}
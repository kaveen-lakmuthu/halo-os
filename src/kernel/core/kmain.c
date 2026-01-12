#include "../drivers/vga.h"
#include "../memory/pmm.h"
#include "../memory/vmm.h"
#include "../arch/x86_64/gdt.h"
#include "../arch/x86_64/idt.h"
#include "../arch/x86_64/isr.h"

void kmain(uint64_t multiboot_addr) {
    (void)multiboot_addr; // Silence warning

    terminal_initialize();
    terminal_writestring("Halo OS Kernel Initializing...\n");

    gdt_init();
    terminal_writestring("[GDT] Loaded.\n");

    idt_init();
    terminal_writestring("[IDT] Loaded.\n");

    isr_init();
    terminal_writestring("[ISR] Handlers installed.\n");
    
    extern uint64_t kernel_physical_end;
    pmm_init(multiboot_addr, (uint64_t)&kernel_physical_end);

    // --- NEW: VMM TEST ---
    vmm_init();

    terminal_writestring("Kernel initialized successfully.\n");
    terminal_writestring("Waiting for interrupts...\n");

    while(1) { __asm__("hlt"); }
}
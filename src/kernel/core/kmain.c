#include <stdint.h>
#include "../drivers/vga.h"
#include "../drivers/pic.h"
#include "../drivers/keyboard.h"
#include "shell.h"
#include "../arch/x86_64/io.h"
#include "../arch/x86_64/gdt.h"
#include "../arch/x86_64/idt.h"
#include "../arch/x86_64/isr.h"
#include "../memory/pmm.h"
#include "../memory/vmm.h"

void kmain(uint64_t multiboot_addr) {
    // Silence compiler warning about unused parameter
    (void)multiboot_addr;

    terminal_setcolor(VGA_COLOR_LIGHT_GREY | (VGA_COLOR_BLACK << 4));
    terminal_initialize();
    terminal_writestring("Halo OS Kernel Initializing...\n");

    // 1. Setup CPU Structures
    gdt_init();
    terminal_writestring("[GDT] Loaded.\n");

    idt_init();
    terminal_writestring("[IDT] Loaded.\n");

    isr_init();
    terminal_writestring("[ISR] Handlers installed.\n");

    pic_remap();
    terminal_writestring("[PIC] Remapped to 32-47.\n");

    // 2. Setup Memory (Critical to do this before Enabling Interrupts)
    extern uint64_t kernel_physical_end;
    pmm_init(multiboot_addr, (uint64_t)&kernel_physical_end);
    vmm_init();

    // 3. Enable Interrupts now that the environment is stable
    // Unmask Keyboard (IRQ1)
    outb(0x21, 0xFD); 
    
    __asm__ volatile ("sti");
    terminal_writestring("[CPU] Interrupts Enabled. Press any key!\n");

    // Initialize the keyboard buffer
    keyboard_init();

    terminal_writestring("Welcome to Halo OS.\n");
    terminal_writestring("Type 'help' for commands.\n\n");
    terminal_writestring("> "); // The first prompt

    // THE MAIN KERNEL LOOP
    while(1) {
        if (command_ready) {
            shell_execute();
        }
        __asm__ volatile("hlt"); // Wait for next interrupt (power save)
    }
}
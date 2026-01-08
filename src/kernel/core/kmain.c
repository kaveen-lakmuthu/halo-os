#include "../drivers/vga.h"
#include "../memory/pmm.h"

// Need the symbol from the linker script to know where the kernel ends
extern uint64_t kernel_physical_end;

// Helper to print hex numbers (naive implementation for now)
void print_hex(uint64_t value) {
    terminal_writestring("0x");
    char hex_chars[] = "0123456789ABCDEF";
    for (int i = 60; i >= 0; i -= 4) {
        int nibble = (value >> i) & 0xF;
        terminal_putchar(hex_chars[nibble]);
    }
}

void kmain(uint64_t multiboot_addr) {
    terminal_initialize();
    terminal_writestring("Halo OS Kernel Initializing...\n");

    // 1. Initialize PMM
    // Pass the Multiboot pointer (to find RAM) and the kernel end (to protect ourselves)
    // Note: kernel_physical_end is an address, need its value.
    pmm_init(multiboot_addr, (uint64_t)&kernel_physical_end);

    // 2. Test Allocation
    void* page1 = pmm_alloc_frame();
    void* page2 = pmm_alloc_frame();
    void* page3 = pmm_alloc_frame();

    terminal_writestring("Allocated Frame 3: ");
    print_hex((uint64_t)page3);
    terminal_writestring("\n");

    terminal_writestring("Allocated Frame 1: ");
    print_hex((uint64_t)page1); 
    terminal_writestring("\n");

    terminal_writestring("Allocated Frame 2: ");
    print_hex((uint64_t)page2);
    terminal_writestring("\n");

    // 3. Test Freeing
    terminal_writestring("Freeing Frame 2...\n");
    pmm_free_frame(page2);

    void* page4 = pmm_alloc_frame();
    terminal_writestring("Allocated Frame 4 (Should match Frame 2): ");
    print_hex((uint64_t)page4);
    terminal_writestring("\n");

    while(1) {
        __asm__("hlt");
    }
}
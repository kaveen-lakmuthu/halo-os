#include "../drivers/vga.h"

void kmain(void) {
    terminal_initialize();
    
    // Create a pointer to the function itself to see its address
    uint64_t current_address = (uint64_t)&kmain;

    terminal_writestring("Halo OS Higher Half Kernel\n");

    // Check if address is big (Higher Half) or small (Lower Half)
    if (current_address > 0xFFFFFFFF00000000) {
        terminal_setcolor(VGA_COLOR_GREEN);
        terminal_writestring("[SUCCESS] Kernel Running in Higher Half!\n");
    } else {
        terminal_setcolor(VGA_COLOR_RED);
        terminal_writestring("[FAIL] Still in Lower Half.\n");
    }

    while(1) {
        __asm__("hlt");
    }
}
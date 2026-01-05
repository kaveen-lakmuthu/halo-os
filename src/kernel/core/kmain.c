#include "../drivers/vga.h"

void kmain(void) {
    terminal_initialize();
    terminal_writestring("Hello, Halo OS!\n");

    // --- The 64-bit Check ---
    if (sizeof(void*) == 8) {
        terminal_setcolor(VGA_COLOR_GREEN);
        terminal_writestring("[SUCCESS] Running in 64-bit Long Mode.\n");
    } else {
        terminal_setcolor(VGA_COLOR_RED);
        terminal_writestring("[WARNING] Still in 32-bit Protected Mode.\n");
    }
    // ------------------------

    terminal_setcolor(VGA_COLOR_LIGHT_GREY);
    terminal_writestring("Waiting for commands...");

    while(1) {
        __asm__("hlt");
    }
}
#include "../drivers/vga.h"

void kmain(void) {
    // 1. Clear the screen and setup variables
    terminal_initialize();

    // 2. Print the first message
    terminal_writestring("Hello, Halo OS!\n");
    
    // 3. Change colour and print status
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[SUCCESS] Kernel initialized successfully.\n");
    
    // 4. Reset colour
    terminal_setcolor(VGA_COLOR_LIGHT_GREY);
    terminal_writestring("Waiting for commands...");

    // Halt loop
    while(1) {
        __asm__("hlt");
    }
}
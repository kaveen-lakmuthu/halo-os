#include "shell.h"
#include "../drivers/vga.h"
#include "../drivers/keyboard.h"
#include "../arch/x86_64/io.h"
#include "../arch/x86_64/cpuid.h"

// Helper: String Compare (returns 0 if equal)
int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

void power_reboot(void) {
    uint8_t good = 0x02;
    while (good & 0x02) {
        good = inb(0x64);
    }
    outb(0x64, 0xFE);
    __asm__ volatile("hlt");
}

// Execute the command in the buffer
void shell_execute(void) {
    terminal_writestring("Command: ");
    terminal_writestring(keyboard_buffer);
    terminal_writestring("\n");

    if (strcmp(keyboard_buffer, "help") == 0) {
        terminal_writestring("--- Halo OS Help ---\n");
        terminal_writestring("  reboot      - Restart the computer\n");
        terminal_writestring("  theme matrix - Green on Black\n");
        terminal_writestring("  theme blue   - White on Blue\n");
        terminal_writestring("  theme error  - Red on Black\n");
        terminal_writestring("  cpu         - Show CPU Vendor\\n");
        terminal_writestring("  clear       - Clear screen\n");
    } 
    // --- REBOOT ---
    else if (strcmp(keyboard_buffer, "reboot") == 0) {
        terminal_writestring("Rebooting...\n");
        power_reboot();
    }
    // --- THEMES ---
    else if (strcmp(keyboard_buffer, "theme matrix") == 0) {
        // Light Green (10) on Black (0)
        terminal_setcolor(VGA_COLOR_LIGHT_GREEN | (VGA_COLOR_BLACK << 4));
        terminal_initialize(); // Clear screen to apply bg color
        terminal_writestring("The Matrix has you...\n");
    }
    else if (strcmp(keyboard_buffer, "theme blue") == 0) {
        // White (15) on Blue (1)
        terminal_setcolor(VGA_COLOR_WHITE | (VGA_COLOR_BLUE << 4));
        terminal_initialize();
        terminal_writestring("Halo OS - Blue Screen Edition\n");
    }
    else if (strcmp(keyboard_buffer, "theme error") == 0) {
        // Light Red (12) on Black (0)
        terminal_setcolor(VGA_COLOR_LIGHT_RED | (VGA_COLOR_BLACK << 4));
        terminal_initialize();
    }
    // --- CPU COMMAND ---
    else if (strcmp(keyboard_buffer, "cpu") == 0) {
        uint32_t eax, ebx, ecx, edx;
        cpuid(0, &eax, &edx, &ecx, &ebx); // Vendor string in ebx, edx, ecx

        char vendor[13];
        ((uint32_t*)vendor)[0] = ebx;
        ((uint32_t*)vendor)[1] = edx;
        ((uint32_t*)vendor)[2] = ecx;
        vendor[12] = '\0';

        terminal_writestring("CPU Vendor: ");
        terminal_writestring(vendor);
        terminal_writestring("\n");
    }
    // --- EXISTING COMMANDS ---
    else if (strcmp(keyboard_buffer, "clear") == 0) {
        terminal_initialize();
    }
    else if (strcmp(keyboard_buffer, "about") == 0) {
        terminal_writestring("Halo OS v0.2\n");
        terminal_writestring("Built by Kaveen.\n");
    }
    else if (strcmp(keyboard_buffer, "") == 0) {
        // Empty command
    }
    else {
        terminal_writestring("Unknown command. Type 'help'.\n");
    }

    // Reset buffer for next command
    keyboard_init();
    terminal_writestring("> "); // Print prompt
}

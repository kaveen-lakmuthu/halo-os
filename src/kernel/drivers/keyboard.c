#include "keyboard.h"
#include "../arch/x86_64/io.h"
#include "vga.h"

// State variables
char keyboard_buffer[MAX_BUFFER_SIZE];
int buffer_index = 0;
bool command_ready = false;

// US Keyboard Layout (Scancode Set 1)
// 0 means "Key not mapped" or "Special Key" (like Shift/Ctrl)
unsigned char kbd_us[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',   
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',      
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',   
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,              
  '*',
    0,  // Alt
  ' ',  // Space
    0,  // CapsLock
    0,  // F1
    0, 0, 0, 0, 0, 0, 0, 0, // F2-F9
    0,  // F10
    0,  // NumLock
    0,  // ScrollLock
    0,  // Home
    0,  // Up
    0,  // PageUp
  '-',
    0,  // Left
    0,
    0,  // Right
  '+',
    0,  // End
    0,  // Down
    0,  // PageDown
    0,  // Insert
    0,  // Delete
    0, 0, 0,
    0,  // F11
    0,  // F12
    0,  // All other keys are undefined
};

void keyboard_init(void) {
    buffer_index = 0;
    command_ready = false;
    for (int i = 0; i < MAX_BUFFER_SIZE; i++) {
        keyboard_buffer[i] = 0;
    }
}

void keyboard_handler(void) {
    uint8_t scancode = inb(0x60);

    // If key released, ignore
    if (scancode & 0x80) return;

    // Decode key
    if (scancode < 128 && kbd_us[scancode] != 0) {
        char c = kbd_us[scancode];

        // 1. Handle Enter
        if (c == '\n') {
            terminal_putchar('\n'); // New line on screen
            keyboard_buffer[buffer_index] = '\0'; // Null-terminate string
            command_ready = true;   // Tell Kernel to execute!
            return;
        }

        // 2. Handle Backspace
        if (c == '\b') {
            if (buffer_index > 0) {
                terminal_putchar('\b'); // Erase on screen
                buffer_index--;         // Erase in memory
                keyboard_buffer[buffer_index] = 0;
            }
            return;
        }

        // 3. Handle Normal Character
        if (buffer_index < MAX_BUFFER_SIZE - 1) {
            terminal_putchar(c);           // Print to screen
            keyboard_buffer[buffer_index] = c; // Store in memory
            buffer_index++;
        }
    }
}

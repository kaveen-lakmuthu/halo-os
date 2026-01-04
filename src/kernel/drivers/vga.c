#include "vga.h"

// VGA Text Mode Buffer Address
static volatile uint16_t* const VGA_MEMORY = (volatile uint16_t*) 0xB8000;
// Screen Dimensions
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

// Current Cursor Position and Colour
static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

// Helper: Combine a character and a colour into a 16-bit VGA entry
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
	return (uint16_t) uc | (uint16_t) color << 8;
}

// 1. Initialize the terminal (Clear screen)
void terminal_initialize(void) {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4; // Grey text on Black
	terminal_buffer = VGA_MEMORY;

	// Fill the screen with "Space" characters (clears garbage)
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

// 2. Set the current text colour
void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}

// 3. Put a single character at a specific X,Y location
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

// 4. Write a character at the current cursor position
void terminal_putchar(char c) {
    // Handle Newline character
    if (c == '\n') {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT)
            terminal_row = 0;
        return;
    }

	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
    
    // Move cursor forward
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT)
			terminal_row = 0;
	}
}

// 5. Write a whole string
void terminal_writestring(const char* data) {
    // Loop through string until null-terminator
	for (size_t i = 0; data[i] != 0; i++)
		terminal_putchar(data[i]);
}
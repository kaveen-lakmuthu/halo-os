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
static volatile uint16_t* terminal_buffer;

/**
 * @brief Combine a character and color into a 16-bit VGA entry.
 *
 * @param[in] uc    The character to encode
 * @param[in] color The color byte
 * @return uint16_t The encoded VGA entry
 */
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t) uc | (uint16_t) color << 8;
}

/**
 * @brief Initialize the VGA terminal (clear screen, reset cursor).
 */
void terminal_initialize(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_buffer = VGA_MEMORY;

    // Fill the screen with "Space" characters (clears garbage)
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
}

/**
 * @brief Set the current text color for subsequent writes.
 *
 * @param[in] color VGA color byte (foreground | (background << 4))
 */
void terminal_setcolor(uint8_t color) {
    terminal_color = color;
}

/**
 * @brief Put a single character at a specific X,Y location.
 *
 * @param[in] c     The character to write
 * @param[in] color VGA color byte
 * @param[in] x     X coordinate (0-79)
 * @param[in] y     Y coordinate (0-24)
 */
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
    // Bounds checking
    if (x >= VGA_WIDTH || y >= VGA_HEIGHT) {
        return;  // Silently ignore out-of-bounds writes
    }
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, color);
}

/**
 * @brief Write a single character at the current cursor position.
 *
 * @param[in] c The character to write
 */
void terminal_putchar(char c) {
    // 1. Handle Newline
    if (c == '\n') {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) {
            // Scroll: shift all lines up by one
            for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
                for (size_t x = 0; x < VGA_WIDTH; x++) {
                    size_t src_idx = (y + 1) * VGA_WIDTH + x;
                    size_t dst_idx = y * VGA_WIDTH + x;
                    terminal_buffer[dst_idx] = terminal_buffer[src_idx];
                }
            }
            // Clear the last line
            for (size_t x = 0; x < VGA_WIDTH; x++) {
                size_t idx = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
                terminal_buffer[idx] = vga_entry(' ', terminal_color);
            }
            terminal_row = VGA_HEIGHT - 1;
        }
        return;
    }
    // 2. Handle Backspace
    else if (c == '\b') {
        if (terminal_column > 0) {
            terminal_column--;  // Move cursor back
            // Overwrite the old character with a space
            terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
        }
        return;
    }
    // 3. Handle Normal Characters
    else {
        // Bounds check before writing
        if (terminal_column >= VGA_WIDTH || terminal_row >= VGA_HEIGHT) {
            return;
        }

        terminal_putentryat(c, terminal_color, terminal_column, terminal_row);

        // Move cursor forward
        if (++terminal_column == VGA_WIDTH) {
            terminal_column = 0;
            if (++terminal_row == VGA_HEIGHT) {
                // Scroll again
                for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
                    for (size_t x = 0; x < VGA_WIDTH; x++) {
                        size_t src_idx = (y + 1) * VGA_WIDTH + x;
                        size_t dst_idx = y * VGA_WIDTH + x;
                        terminal_buffer[dst_idx] = terminal_buffer[src_idx];
                    }
                }
                // Clear the last line
                for (size_t x = 0; x < VGA_WIDTH; x++) {
                    size_t idx = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
                    terminal_buffer[idx] = vga_entry(' ', terminal_color);
                }
                terminal_row = VGA_HEIGHT - 1;
            }
        }
    }
}

/**
 * @brief Write a null-terminated string to the terminal.
 *
 * @param[in] data Pointer to the string to write
 */
void terminal_writestring(const char* data) {
    // Loop through string until null-terminator
    for (size_t i = 0; data[i] != 0; i++)
        terminal_putchar(data[i]);
}

/**
 * @brief Write a 64-bit unsigned integer in hexadecimal format.
 *
 * @param[in] n The number to write
 */
void terminal_writehex(uint64_t n) {
    terminal_writestring("0x");
    char hex[] = "0123456789ABCDEF";
    for (int i = 60; i >= 0; i -= 4) {
        // Shift and mask to get the 4 bits
        int index = (n >> i) & 0xF;
        terminal_putchar(hex[index]);
    }
}
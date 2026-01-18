#ifndef VGA_H
#define VGA_H

#include <stdint.h>
#include <stddef.h>

// Standard VGA Colors
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

/**
 * @brief Initialize the VGA terminal (clear screen, reset cursor).
 *
 * @return void
 */
void terminal_initialize(void);

/**
 * @brief Set the current text color for subsequent writes.
 *
 * @param[in] color VGA color byte (foreground | (background << 4))
 * @return void
 */
void terminal_setcolor(uint8_t color);

/**
 * @brief Put a single character at a specific X,Y location.
 *
 * @param[in] c     The character to write
 * @param[in] color VGA color byte
 * @param[in] x     X coordinate (0-79)
 * @param[in] y     Y coordinate (0-24)
 * @return void
 */
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y);

/**
 * @brief Write a null-terminated string to the terminal.
 *
 * @param[in] data Pointer to the string to write
 * @return void
 */
void terminal_writestring(const char* data);

/**
 * @brief Write a single character at the current cursor position.
 *
 * @param[in] c The character to write
 * @return void
 */
void terminal_putchar(char c);

/**
 * @brief Write a 64-bit unsigned integer in hexadecimal format.
 *
 * @param[in] n The number to write
 * @return void
 */
void terminal_writehex(uint64_t n);

#endif
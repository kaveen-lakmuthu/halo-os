#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>

// The maximum command length (e.g., 256 characters)
#define MAX_BUFFER_SIZE 256

// Public flag: "Did the user just press Enter?"
extern bool command_ready;

// The buffer itself
extern char keyboard_buffer[MAX_BUFFER_SIZE];

void keyboard_handler(void);
void keyboard_init(void); // To clear the buffer initially

#endif

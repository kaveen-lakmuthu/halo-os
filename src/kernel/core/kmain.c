#include <stdint.h>

void kmain() {
    // Pointer to Video Memory (VGA text mode)
    volatile char* video_memory = (volatile char*) 0xb8000;

    // Print 'O' in Green
    video_memory[0] = 'O';
    video_memory[1] = 0x02; // 0x02 = Green on Black

    // Print 'K' in Green
    video_memory[2] = 'K';
    video_memory[3] = 0x02;
    
    // Halt the CPU loop
    while(1) {
        __asm__("hlt");
    }
}
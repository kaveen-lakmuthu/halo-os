#include "../arch/x86_64/io.h"
#include <stdint.h>

// --- PIC Ports ---
#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

// --- Commands ---
#define ICW1_INIT    0x11  // Init command
#define ICW4_8086    0x01  // 8086 mode

#define PIC1_OFFSET  0x20  // Master starts at 32
#define PIC2_OFFSET  0x28  // Slave starts at 40

void pic_send_eoi(uint8_t irq) {
    // End of Interrupt: Tell PIC that OS is done handling the interrupt
    if(irq >= 8)
        outb(PIC2_COMMAND, 0x20); // Send to Slave
    
    outb(PIC1_COMMAND, 0x20);     // Send to Master
}

void pic_remap(void) {
    // 1. Start Initialization (ICW1)
    outb(PIC1_COMMAND, ICW1_INIT); io_wait();
    outb(PIC2_COMMAND, ICW1_INIT); io_wait();

    // 2. Set Offsets (ICW2) - THE IMPORTANT PART
    outb(PIC1_DATA, PIC1_OFFSET); io_wait(); // Master -> 32 (0x20)
    outb(PIC2_DATA, PIC2_OFFSET); io_wait(); // Slave -> 40 (0x28)

    // 3. Chain Master and Slave (ICW3)
    outb(PIC1_DATA, 4); io_wait(); // Tell Master there is a Slave at IRQ2
    outb(PIC2_DATA, 2); io_wait(); // Tell Slave its cascade identity

    // 4. Set Mode (ICW4)
    outb(PIC1_DATA, ICW4_8086); io_wait();
    outb(PIC2_DATA, ICW4_8086); io_wait();

    // 5. Mask all interrupts (Unmask specific IRQs later)
    // TODO: unmask specific IRQs later (like Keyboard).
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}
#include "idt.h"

// The IDT has 256 entries (0-255)
struct idt_entry idt[256];
struct idt_ptr idtr;

// Assembly function to load the IDT
extern void idt_load(struct idt_ptr* ptr);

/**
 * @brief Set an IDT gate (entry) for an interrupt handler.
 *
 * @param[in] n          IDT entry index (0-255)
 * @param[in] handler    Address of the interrupt handler function
 * @param[in] sel        Segment selector for the handler
 * @param[in] type_attr  Type and attributes byte
 */
void idt_set_gate(int n, uint64_t handler, uint16_t sel, uint8_t type_attr) {
    idt[n].offset_low  = (uint16_t)handler;
    idt[n].selector    = sel;
    idt[n].ist         = 0;
    idt[n].type_attr   = type_attr;
    idt[n].offset_mid  = (uint16_t)(handler >> 16);
    idt[n].offset_high = (uint32_t)(handler >> 32);
    idt[n].zero        = 0;
}

/**
 * @brief Initialize the Interrupt Descriptor Table (IDT).
 *
 * Sets up the IDT pointer and loads it into the IDTR register.
 * Clears all IDT entries initially.
 */
void idt_init(void) {
    // 1. Set up the IDT Pointer
    idtr.base = (uint64_t)&idt;
    idtr.limit = (sizeof(struct idt_entry) * 256) - 1;

    // 2. Clear the IDT (initialize everything to zero for now)
    // TODO:et gates here using idt_set_gate()
    for (int i = 0; i < 256; i++) {
        // Just zeroing out for safety
        idt[i].offset_low = 0;
        idt[i].selector = 0;
        idt[i].type_attr = 0;
        // ... ( NOTE: static arrays are zeroed by default in C, but good to be explicit later)
    }

    // 3. Load the IDT
    idt_load(&idtr);
}
#ifndef IDT_H
#define IDT_H

#include <stdint.h>

// IDT Entry (16 bytes for x86_64)
struct idt_entry {
    uint16_t offset_low;       // Lower 16 bits of handler address
    uint16_t selector;         // Kernel Code Segment Selector (0x08)
    uint8_t  ist;              // Interrupt Stack Table (usually 0)
    uint8_t  type_attr;        // Type and Attributes (Present, Ring 0, etc.)
    uint16_t offset_mid;       // Middle 16 bits of handler address
    uint32_t offset_high;      // Upper 32 bits of handler address
    uint32_t zero;             // Reserved (must be 0)
} __attribute__((packed));

// IDT Pointer (Loaded into IDTR register)
struct idt_ptr {
    uint16_t limit;            // Size of IDT - 1
    uint64_t base;             // Address of IDT array
} __attribute__((packed));

/**
 * @brief Initialize the Interrupt Descriptor Table (IDT).
 *
 * Sets up the IDT pointer and loads it into the IDTR register.
 * Clears all IDT entries initially.
 *
 * @return void
 */
void idt_init(void);

/**
 * @brief Set an IDT gate (entry) for an interrupt handler.
 *
 * @param[in] n          IDT entry index (0-255)
 * @param[in] handler    Address of the interrupt handler function
 * @param[in] sel        Segment selector for the handler
 * @param[in] type_attr  Type and attributes byte
 * @return void
 */
void idt_set_gate(int n, uint64_t handler, uint16_t sel, uint8_t type_attr);

#endif
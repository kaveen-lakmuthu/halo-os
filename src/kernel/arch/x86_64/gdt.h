#ifndef GDT_H
#define GDT_H

#include <stdint.h>

// A single GDT entry (Segment Descriptor)
struct gdt_entry {
    uint16_t limit_low;     // Lower 16 bits of the limit
    uint16_t base_low;      // Lower 16 bits of the base
    uint8_t  base_middle;   // Middle 8 bits of the base
    uint8_t  access;        // Access flags (Ring 0 vs Ring 3, Code vs Data)
    uint8_t  granularity;   // Granularity + High 4 bits of limit
    uint8_t  base_high;     // High 8 bits of the base
} __attribute__((packed));

// The GDT Pointer (what load into the GDTR register)
struct gdt_ptr {
    uint16_t limit;         // Size of GDT - 1
    uint64_t base;          // Address of the GDT
} __attribute__((packed));

/**
 * @brief Initialize the Global Descriptor Table (GDT).
 *
 * Sets up the GDT with null, kernel code, and kernel data segments.
 * Loads the GDT pointer into the GDTR register.
 *
 * @return void
 */
void gdt_init(void);

#endif
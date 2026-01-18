#include "gdt.h"

// Need 3 entries: Null, Code, Data
// TODO: Add User Code/Data and TSS here in future iterations.
#define GDT_ENTRIES 3

struct gdt_entry gdt[GDT_ENTRIES];
struct gdt_ptr gdt_pointer;

// External assembly function to actually load the register
extern void gdt_load(struct gdt_ptr* gdt_ptr);

/**
 * @brief Set a GDT entry with the given parameters.
 *
 * @param[in] num    GDT entry index
 * @param[in] base   Base address of the segment
 * @param[in] limit  Size limit of the segment
 * @param[in] access Access flags (Present, Ring, Code/Data)
 * @param[in] gran   Granularity flags
 */
static void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit,
                         uint8_t access, uint8_t gran) {
    gdt[num].base_low    = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high   = (base >> 24) & 0xFF;

    gdt[num].limit_low   = (limit & 0xFFFF);
    gdt[num].granularity = (limit >> 16) & 0x0F;

    gdt[num].granularity |= gran & 0xF0;
    gdt[num].access      = access;
}

void gdt_init(void) {
    // 1. Setup the GDT Pointer
    gdt_pointer.limit = (sizeof(struct gdt_entry) * GDT_ENTRIES) - 1;
    gdt_pointer.base  = (uint64_t)&gdt;

    // 2. Null Descriptor (Index 0) - MUST BE ZERO
    gdt_set_gate(0, 0, 0, 0, 0);

    // 3. Kernel Code Segment (Index 1)
    // Access: 0x9A = 10011010b (Present, Ring 0, Code, Exec/Read)
    // Granularity: 0xA0 = 10100000b (Long Mode, 4KB pages)
    gdt_set_gate(1, 0, 0, 0x9A, 0xA0);

    // 4. Kernel Data Segment (Index 2)
    // Access: 0x92 = 10010010b (Present, Ring 0, Data, Read/Write)
    // Granularity: 0x00
    gdt_set_gate(2, 0, 0, 0x92, 0x00);

    // 5. Load it
    gdt_load(&gdt_pointer);
}
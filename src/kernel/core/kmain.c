#include "../drivers/vga.h"
#include <multiboot.h> // Uses -Isrc/kernel/include from Makefile

// Helper to print hex numbers (naive implementation for now)
void print_hex(uint64_t value) {
    terminal_writestring("0x");
    char hex_chars[] = "0123456789ABCDEF";
    for (int i = 60; i >= 0; i -= 4) {
        int nibble = (value >> i) & 0xF;
        terminal_putchar(hex_chars[nibble]);
    }
}

// Helper to print decimal (simple version)
void print_dec(uint64_t value) {
    if (value == 0) {
        terminal_putchar('0');
        return;
    }
    char buffer[20];
    int i = 0;
    while (value > 0) {
        buffer[i++] = (value % 10) + '0';
        value /= 10;
    }
    while (--i >= 0) {
        terminal_putchar(buffer[i]);
    }
}

void kmain(uint64_t multiboot_addr) {
    terminal_initialize();
    terminal_writestring("Halo OS Memory Probe\n");

    // 1. Cast the pointer
    // Note: multiboot_addr is a physical address (e.g. 0x10000).
    // Because this identity mapped the first 1GB, this can access it directly.
    // struct multiboot_info* taghdr = (struct multiboot_info*) multiboot_addr;

    terminal_writestring("Multiboot Info at: ");
    print_hex(multiboot_addr);
    terminal_writestring("\n");

    // 2. Loop through tags
    // The tags start right after the 'reserved' field
    struct multiboot_tag* tag = (struct multiboot_tag*)(multiboot_addr + 8);

    terminal_writestring("Scanning Memory Map...\n");

    while (tag->type != 0) {
        if (tag->type == MULTIBOOT_TAG_TYPE_MMAP) {
            struct multiboot_tag_mmap* mmap = (struct multiboot_tag_mmap*) tag;
            
            // Calculate how many entries exist
            // (Size of tag - Size of header) / Size of entry
            int num_entries = (mmap->size - sizeof(struct multiboot_tag_mmap)) / mmap->entry_size;

            for (int i = 0; i < num_entries; i++) {
                struct multiboot_mmap_entry* entry = (struct multiboot_mmap_entry*) ((uint64_t)mmap->entries + (i * mmap->entry_size));
                
                // Print only Available memory (Type 1)
                if (entry->type == MULTIBOOT_MEMORY_AVAILABLE) {
                    terminal_setcolor(VGA_COLOR_GREEN);
                    terminal_writestring("[FREE] Base: ");
                    print_hex(entry->addr);
                    terminal_writestring(" Len: ");
                    print_hex(entry->len);
                    terminal_writestring("\n");
                } else {
                    terminal_setcolor(VGA_COLOR_LIGHT_GREY);
                    terminal_writestring("[RESV] Base: ");
                    print_hex(entry->addr);
                    terminal_writestring(" Len: ");
                    print_hex(entry->len);
                    terminal_writestring("\n");
                }
            }
        }
        
        // Move to next tag. Align to 8 bytes.
        tag = (struct multiboot_tag*) ((uint8_t*)tag + ((tag->size + 7) & ~7));
    }
    
    terminal_setcolor(VGA_COLOR_WHITE);
    terminal_writestring("Memory Scan Complete.");

    while(1) {
        __asm__("hlt");
    }
}
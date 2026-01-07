#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <stdint.h>

// The Magic number passed by GRUB in EAX (checked this in asm)
#define MULTIBOOT2_BOOTLOADER_MAGIC 0x36d76289

// Basic Tag Header
struct multiboot_tag {
    uint32_t type;
    uint32_t size;
};

// The Main Info Structure passed in EBX
struct multiboot_info {
    uint32_t total_size;
    uint32_t reserved;
    struct multiboot_tag tags[];
};

// Tag Type 6: Memory Map
#define MULTIBOOT_TAG_TYPE_MMAP 6

struct multiboot_mmap_entry {
    uint64_t addr;
    uint64_t len;
    uint32_t type;
    uint32_t zero;
};

struct multiboot_tag_mmap {
    uint32_t type;
    uint32_t size;
    uint32_t entry_size;
    uint32_t entry_version;
    struct multiboot_mmap_entry entries[];
};

// Memory Region Types
#define MULTIBOOT_MEMORY_AVAILABLE 1
#define MULTIBOOT_MEMORY_RESERVED  2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE 3
#define MULTIBOOT_MEMORY_NVS 4
#define MULTIBOOT_MEMORY_BADRAM 5

#endif
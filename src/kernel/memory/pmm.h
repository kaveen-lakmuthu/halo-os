#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <stddef.h>

// Page Size is 4KB (Standard for x86_64)
#define PAGE_SIZE 4096

// Function Prototypes
void pmm_init(uint64_t multiboot_addr, uint64_t kernel_physical_end);
void* pmm_alloc_frame();
void pmm_free_frame(void* frame_addr);

#endif
#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <stddef.h>

// Page Size is 4KB (Standard for x86_64)
#define PAGE_SIZE 4096

/**
 * @brief Get the number of free frames available.
 *
 * @return uint64_t Number of free physical frames
 */
uint64_t pmm_get_free_frames(void);

/**
 * @brief Initialize the Physical Memory Manager.
 *
 * Parses the multiboot memory map and sets up the frame bitmap.
 * Marks kernel and reserved regions as used.
 *
 * @param[in] multiboot_addr     Address of the multiboot info structure
 * @param[in] kernel_physical_end Physical end address of the kernel
 */
void pmm_init(uint64_t multiboot_addr, uint64_t kernel_physical_end);

/**
 * @brief Allocate a single physical page frame.
 *
 * @return void* Physical address of allocated frame, or NULL if none available
 */
void* pmm_alloc_frame(void);

/**
 * @brief Free a previously allocated physical page frame.
 *
 * @param[in] frame_addr Physical address of the frame to free
 */
void pmm_free_frame(void* frame_addr);

#endif
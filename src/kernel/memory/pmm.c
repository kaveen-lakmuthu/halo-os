#include "pmm.h"
#include <multiboot.h>
#include "../drivers/vga.h"

// 1. Configuration
#define MAX_MEMORY_SIZE 0x40000000  // 1GB (reduced from 4GB for testing)
#define FRAMES_COUNT (MAX_MEMORY_SIZE / PAGE_SIZE)
#define BITMAP_SIZE (FRAMES_COUNT / 8)

// 2. THE FIX: Hardcoded location for the Bitmap (5MB Mark)
// This prevents it from overlapping with the Kernel or Multiboot Info.
// 5MB is safer than 16MB as it's well within the identity-mapped region.
static uint8_t* bitmap = (uint8_t*)0x500000;

// 3. Frame allocation tracking for optimization
static uint64_t free_frames_count = 0;
static uint64_t next_free_frame = 0;

// 4. Helper Functions - Forward declaration
static int bitmap_test(uint64_t frame_index);

static void bitmap_set(uint64_t frame_index) {
    if (!bitmap_test(frame_index)) {
        free_frames_count--;
    }
    bitmap[frame_index / 8] |= (1 << (frame_index % 8));
}

static void bitmap_unset(uint64_t frame_index) {
    if (bitmap_test(frame_index)) {
        free_frames_count++;
    }
    bitmap[frame_index / 8] &= ~(1 << (frame_index % 8));
}

static int bitmap_test(uint64_t frame_index) {
    return (bitmap[frame_index / 8] & (1 << (frame_index % 8))) != 0;
}

static uint64_t get_frame_index(uint64_t physical_address) {
    return physical_address / PAGE_SIZE;
}

// 4. Initialization
/**
 * @brief Initialize the Physical Memory Manager.
 *
 * Parses the multiboot memory map and sets up the frame bitmap.
 * Marks kernel and reserved regions as used.
 *
 * @param[in] multiboot_addr Address of the multiboot info structure
 * @param[in] kernel_end     Physical end address of the kernel
 */
void pmm_init(uint64_t multiboot_addr, uint64_t kernel_end) {
    terminal_writestring("[PMM] Init started.\n");

    // Debug: Print where this putting the bitmap
    terminal_writestring("[PMM] Bitmap stored at: 0x500000 (5MB)\n");

    // A. Mark EVERYTHING as used initially
    free_frames_count = 0;
    next_free_frame = 0;
    
    // Safer initialization: write one byte at a time
    terminal_writestring("[PMM] Initializing bitmap...\n");
    for (uint32_t i = 0; i < BITMAP_SIZE; i++) {
        bitmap[i] = 0xFF; 
    }
    terminal_writestring("[PMM] Bitmap initialized.\n");
    free_frames_count = 0;  // Reset counter after marking all as used
    
    // B. Parse Multiboot
    terminal_writestring("[PMM] Parsing multiboot info...\n");
    struct multiboot_tag* tag = (struct multiboot_tag*)(multiboot_addr + 8);

    while (tag->type != 0) {
        if (tag->type == MULTIBOOT_TAG_TYPE_MMAP) {
            terminal_writestring("[PMM] Found Memory Map!\n");

            struct multiboot_tag_mmap* mmap = (struct multiboot_tag_mmap*) tag;
            int num_entries = (mmap->size - sizeof(struct multiboot_tag_mmap)) /
                              mmap->entry_size;

            terminal_writestring("[PMM] Processing entries...\n");

            for (int i = 0; i < num_entries; i++) {
                struct multiboot_mmap_entry* entry =
                    (struct multiboot_mmap_entry*) ((uint64_t)mmap->entries +
                                                     (i * mmap->entry_size));
                
                // If region is Available (Type 1), free those frames
                if (entry->type == MULTIBOOT_MEMORY_AVAILABLE) {
                    uint64_t start_frame = entry->addr / PAGE_SIZE;
                    uint64_t end_frame = (entry->addr + entry->len) / PAGE_SIZE;

                    if (end_frame > FRAMES_COUNT) end_frame = FRAMES_COUNT;

                    // Do not free the bitmap's own memory (5MB to 5MB + 128KB)
                    uint64_t bitmap_start = 0x500000 / PAGE_SIZE;
                    uint64_t bitmap_end = bitmap_start + (BITMAP_SIZE / PAGE_SIZE) + 1;

                    for (uint64_t f = start_frame; f < end_frame; f++) {
                        // Skip the bitmap area itself!
                        if (f >= bitmap_start && f < bitmap_end) continue;
                        
                        bitmap_unset(f);
                    }
                }
            }
            
            terminal_writestring("[PMM] Entries processed.\n");
        }
        tag = (struct multiboot_tag*) ((uint8_t*)tag + ((tag->size + 7) & ~7));
    }

    // C. Mark Kernel & Low Memory as Used
    terminal_writestring("[PMM] Marking kernel memory...\n");
    uint64_t reserved_frames = (kernel_end + PAGE_SIZE - 1) / PAGE_SIZE;
    // Add extra safety for BIOS/GRUB (first 2MB)
    if (reserved_frames < 512) reserved_frames = 512; 

    for (uint64_t f = 0; f < reserved_frames; f++) {
        bitmap_set(f);
    }

    terminal_writestring("[PMM] Init Complete.\n");
}

// 6. Allocation / Free
/**
 * @brief Allocate a single physical page frame.
 *
 * @return void* Physical address of allocated frame, or NULL if none available
 */
void* pmm_alloc_frame() {
    // Quick check: no free frames
    if (free_frames_count == 0) {
        return NULL;
    }

    // Start from next_free_frame to avoid rescanning
    for (uint64_t i = next_free_frame; i < FRAMES_COUNT; i++) {
        if (!bitmap_test(i)) {
            bitmap_set(i);
            next_free_frame = i + 1;  // Update for next allocation
            return (void*)(i * PAGE_SIZE);
        }
    }

    // Wrap around and search from beginning
    for (uint64_t i = 0; i < next_free_frame; i++) {
        if (!bitmap_test(i)) {
            bitmap_set(i);
            next_free_frame = i + 1;
            return (void*)(i * PAGE_SIZE);
        }
    }

    return NULL;
}

/**
 * @brief Free a previously allocated physical page frame.
 *
 * @param[in] frame_addr Physical address of the frame to free
 */
void pmm_free_frame(void* frame_addr) {
    uint64_t frame = get_frame_index((uint64_t)frame_addr);
    if (frame < FRAMES_COUNT && bitmap_test(frame)) {
        bitmap_unset(frame);
        // Update next_free_frame if freed an earlier frame
        if (frame < next_free_frame) {
            next_free_frame = frame;
        }
    }
}

/**
 * @brief Get the number of free frames available.
 *
 * @return uint64_t Number of free physical frames
 */
uint64_t pmm_get_free_frames(void) {
    return free_frames_count;
}
#include "pmm.h"
#include <multiboot.h>
#include "../drivers/vga.h"

// 1. Configuration
#define MAX_MEMORY_SIZE 0x100000000 // 4GB
#define FRAMES_COUNT (MAX_MEMORY_SIZE / PAGE_SIZE)
#define BITMAP_SIZE (FRAMES_COUNT / 8)

// 2. THE FIX: Hardcoded location for the Bitmap (16MB Mark)
// This prevents it from overlapping with the Kernel or Multiboot Info.
static uint8_t* bitmap = (uint8_t*)0x1000000;

// 3. Helper Functions
static void bitmap_set(uint64_t frame_index) {
    bitmap[frame_index / 8] |= (1 << (frame_index % 8));
}

static void bitmap_unset(uint64_t frame_index) {
    bitmap[frame_index / 8] &= ~(1 << (frame_index % 8));
}

static int bitmap_test(uint64_t frame_index) {
    return (bitmap[frame_index / 8] & (1 << (frame_index % 8))) != 0;
}

static uint64_t get_frame_index(uint64_t physical_address) {
    return physical_address / PAGE_SIZE;
}

// 4. Initialization
void pmm_init(uint64_t multiboot_addr, uint64_t kernel_end) {
    terminal_writestring("[PMM] Init started.\n");

    // Debug: Print where this putting the bitmap
    terminal_writestring("[PMM] Bitmap stored at: 0x1000000\n");

    // A. Mark EVERYTHING as used initially
    for (int i = 0; i < BITMAP_SIZE; i++) {
        bitmap[i] = 0xFF; 
    }
    
    // B. Parse Multiboot
    struct multiboot_tag* tag = (struct multiboot_tag*)(multiboot_addr + 8);

    while (tag->type != 0) {
        if (tag->type == MULTIBOOT_TAG_TYPE_MMAP) {
            terminal_writestring("[PMM] Found Memory Map!\n"); // Visual confirmation
            
            struct multiboot_tag_mmap* mmap = (struct multiboot_tag_mmap*) tag;
            int num_entries = (mmap->size - sizeof(struct multiboot_tag_mmap)) / mmap->entry_size;

            for (int i = 0; i < num_entries; i++) {
                struct multiboot_mmap_entry* entry = (struct multiboot_mmap_entry*) ((uint64_t)mmap->entries + (i * mmap->entry_size));
                
                // If region is Available (Type 1), free those frames
                if (entry->type == MULTIBOOT_MEMORY_AVAILABLE) {
                    uint64_t start_frame = entry->addr / PAGE_SIZE;
                    uint64_t end_frame = (entry->addr + entry->len) / PAGE_SIZE;

                    if (end_frame > FRAMES_COUNT) end_frame = FRAMES_COUNT;

                    // Do not free the bitmap's own memory (16MB to 16MB + 128KB)
                    uint64_t bitmap_start = 0x1000000 / PAGE_SIZE;
                    uint64_t bitmap_end = bitmap_start + (BITMAP_SIZE / PAGE_SIZE) + 1;

                    for (uint64_t f = start_frame; f < end_frame; f++) {
                        // Skip the bitmap area itself!
                        if (f >= bitmap_start && f < bitmap_end) continue;
                        
                        bitmap_unset(f);
                    }
                }
            }
        }
        tag = (struct multiboot_tag*) ((uint8_t*)tag + ((tag->size + 7) & ~7));
    }

    // C. Mark Kernel & Low Memory as Used
    uint64_t reserved_frames = (kernel_end + PAGE_SIZE - 1) / PAGE_SIZE;
    // Add extra safety for BIOS/GRUB (first 2MB)
    if (reserved_frames < 512) reserved_frames = 512; 

    for (uint64_t f = 0; f < reserved_frames; f++) {
        bitmap_set(f);
    }

    terminal_writestring("[PMM] Init Complete.\n");
}

// 5. Allocation / Free
void* pmm_alloc_frame() {
    for (uint64_t i = 0; i < FRAMES_COUNT; i++) {
        if (!bitmap_test(i)) {
            bitmap_set(i);
            return (void*)(i * PAGE_SIZE);
        }
    }
    return NULL;
}

void pmm_free_frame(void* frame_addr) {
    uint64_t frame = get_frame_index((uint64_t)frame_addr);
    bitmap_unset(frame);
}
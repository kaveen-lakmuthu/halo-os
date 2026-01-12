#ifndef VMM_H
#define VMM_H

#include <stdint.h>
#include <stddef.h>

// --- Paging Flags ---
// These bits are set in the Page Table Entries to control access.
#define PTE_PRESENT   1         // Page is present in RAM
#define PTE_WRITE     2         // Page is writable
#define PTE_USER      4         // User Mode can access this page
#define PTE_NX        (1ULL << 63) // No Execute (prevents running code here)

// --- Constants ---
#define PAGE_SIZE 4096

// --- Function Prototypes ---

// Initialize the VMM (Create a new PML4 and switch to it)
void vmm_init(void);

// Map a specific virtual page to a physical frame
// flags: e.g., PTE_PRESENT | PTE_WRITE
void vmm_map_page(uint64_t virtual_addr, uint64_t physical_addr, uint64_t flags);

// Unmap a page (make it inaccessible)
void vmm_unmap_page(uint64_t virtual_addr);

// Switch the CPU to use a specific PML4 table (Context Switch)
void vmm_switch_pml4(uint64_t pml4_physical_addr);

#endif
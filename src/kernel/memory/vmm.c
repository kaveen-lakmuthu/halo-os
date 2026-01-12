#include "vmm.h"
#include "pmm.h"
#include "../drivers/vga.h"

// The Kernel's main Page Map Level 4
// Allocate this in vmm_init
uint64_t* kernel_pml4 = NULL;

// Helper: Get the index for a specific level from a virtual address
// x86_64 addresses are split into 9-bit chunks for each level
#define PML4_INDEX(va) (((va) >> 39) & 0x1FF)
#define PDP_INDEX(va)  (((va) >> 30) & 0x1FF)
#define PD_INDEX(va)   (((va) >> 21) & 0x1FF)
#define PT_INDEX(va)   (((va) >> 12) & 0x1FF)

// Helper: Invalidate the TLB (Translation Lookaside Buffer) for one page
// This forces the CPU to relearn the mapping for this address.
static void vmm_flush_tlb(uint64_t virtual_addr) {
    __asm__ volatile("invlpg (%0)" :: "r" (virtual_addr) : "memory");
}

// The Core Mapping Function
void vmm_map_page(uint64_t virtual_addr, uint64_t physical_addr, uint64_t flags) {
    // A. Calculate Indices
    uint64_t pml4_idx = PML4_INDEX(virtual_addr);
    uint64_t pdp_idx  = PDP_INDEX(virtual_addr);
    uint64_t pd_idx   = PD_INDEX(virtual_addr);
    uint64_t pt_idx   = PT_INDEX(virtual_addr);

    // B. Walk the PML4 -> PDP
    // Check if the PDP entry exists. If not, allocate a new table.
    if (!(kernel_pml4[pml4_idx] & PTE_PRESENT)) {
        uint64_t pdp_alloc = (uint64_t)pmm_alloc_frame();
        // Clear the new page (crucial! garbage means random mappings)
        uint64_t* ptr = (uint64_t*)pdp_alloc; // Note: In a full OS,need to map this first! 
                                              // But right now just identity mapped, so the os can access it directly.
        for(int i=0; i<512; i++) ptr[i] = 0;
        
        // Point PML4 to this new PDP
        kernel_pml4[pml4_idx] = pdp_alloc | PTE_PRESENT | PTE_WRITE;
    }
    
    // Get the PDP address from the PML4 entry (mask out flags)
    uint64_t* pdp = (uint64_t*)(kernel_pml4[pml4_idx] & ~0xFFF);

    // C. Walk the PDP -> PD
    if (!(pdp[pdp_idx] & PTE_PRESENT)) {
        uint64_t pd_alloc = (uint64_t)pmm_alloc_frame();
        uint64_t* ptr = (uint64_t*)pd_alloc;
        for(int i=0; i<512; i++) ptr[i] = 0;

        pdp[pdp_idx] = pd_alloc | PTE_PRESENT | PTE_WRITE;
    }
    uint64_t* pd = (uint64_t*)(pdp[pdp_idx] & ~0xFFF);

    // D. Walk the PD -> PT
    if (!(pd[pd_idx] & PTE_PRESENT)) {
        uint64_t pt_alloc = (uint64_t)pmm_alloc_frame();
        uint64_t* ptr = (uint64_t*)pt_alloc;
        for(int i=0; i<512; i++) ptr[i] = 0;

        pd[pd_idx] = pt_alloc | PTE_PRESENT | PTE_WRITE;
    }
    uint64_t* pt = (uint64_t*)(pd[pd_idx] & ~0xFFF);

    // E. Final Step: Map the Physical Frame
    pt[pt_idx] = physical_addr | flags;
    
    // F. Flush TLB
    vmm_flush_tlb(virtual_addr);
}

// Assembly helper to load CR3 register
extern void load_cr3(uint64_t pml4_addr);

void vmm_init(void) {
    terminal_writestring("[VMM] Initializing Paging...\n");

    // 1. Allocate a new PML4 table
    uint64_t pml4_phys = (uint64_t)pmm_alloc_frame();
    kernel_pml4 = (uint64_t*)pml4_phys;

    // 2. Zero it out
    for(int i=0; i<512; i++) kernel_pml4[i] = 0;

    // 3. THE FIX: Map Both Low and High Memory
    // <ap 128MB. This covers the Kernel, PMM Bitmap, and VGA.
    uint64_t limit = 0x8000000; // 128MB
    uint64_t kernel_offset = 0xFFFFFFFF80000000; // The Offset from Linker Script

    for (uint64_t addr = 0; addr < limit; addr += PAGE_SIZE) {
        // A. Identity Map (Physical 0 -> Virtual 0)
        // Keeps the CPU happy (RIP is currently here).
        // Allows accessing hardware (Video Memory, PMM Bitmap at 16MB) directly.
        vmm_map_page(addr, addr, PTE_PRESENT | PTE_WRITE);

        // B. Higher Half Map (Physical 0 -> Virtual 0xFFFFFFFF80000000)
        // Keeps the C code happy (Global variables and Linker addresses are here).
        vmm_map_page(kernel_offset + addr, addr, PTE_PRESENT | PTE_WRITE);
    }
    
    // 4. Test Mapping: Map a random high address to video memory
    // Virtual 0xDEADBEEF000 -> Physical 0xB8000 (VGA Text Buffer)
    vmm_map_page(0xDEADBEEF000, 0xB8000, PTE_PRESENT | PTE_WRITE);

    // 5. Load the new Page Table (CR3 Register)
    // The moment this executes, Switch to the new mappings.
    // If this hadn't mapped the Higher Half above, crash here!
    load_cr3(pml4_phys);
    
    terminal_writestring("[VMM] Paging Enabled. PML4 loaded.\n");
}

// Unmap Function
void vmm_unmap_page(uint64_t virtual_addr) {
    // (Simplified for now: just mark Present bit as 0 in the PT)
    // In a real OS, this also free the physical frame if needed.
    uint64_t pml4_idx = PML4_INDEX(virtual_addr);
    uint64_t pdp_idx  = PDP_INDEX(virtual_addr);
    uint64_t pd_idx   = PD_INDEX(virtual_addr);
    uint64_t pt_idx   = PT_INDEX(virtual_addr);

    if (kernel_pml4[pml4_idx] & PTE_PRESENT) {
        uint64_t* pdp = (uint64_t*)(kernel_pml4[pml4_idx] & ~0xFFF);
        if (pdp[pdp_idx] & PTE_PRESENT) {
            uint64_t* pd = (uint64_t*)(pdp[pdp_idx] & ~0xFFF);
            if (pd[pd_idx] & PTE_PRESENT) {
                uint64_t* pt = (uint64_t*)(pd[pd_idx] & ~0xFFF);
                pt[pt_idx] = 0; // Clear the entry
                vmm_flush_tlb(virtual_addr);
            }
        }
    }
}
# Halo OS - Architecture Specification

* **Version:** 0.1 (Draft)
* **Target Platform:** x86_64 (Intel/AMD 64-bit)
* **Kernel Type:** Monolithic (Higher Half)

---

## 1. Core Philosophy
Halo OS is a 64-bit UNIX-like operating system designed for deep educational value. It prioritises architectural clarity over commercial viability.
* **Kernel:** Written in C11 (ISO/IEC 9899:2011) for transparency and direct hardware control.
* **Userland:** Written in C++20 for robust abstraction in GUI and applications.
* **License:** MIT / GPL (To be decided).

## 2. Boot Process
We utilise the **Multiboot2** protocol to maintain compatibility with both Legacy BIOS and UEFI hardware via GRUB.

1.  **Stage 1 (Loader):** GRUB loads the kernel ELF binary into physical memory (usually at `0x100000` / 1MB).
2.  **Stage 2 (Trampoline):**
    * CPU state: 32-bit Protected Mode.
    * Kernel Entry: `src/arch/x86_64/boot.asm`.
    * Responsibility: Build initial Identity Mapped Page Tables, enable PAE, set EFER.LME, enable Paging, and perform the *Far Jump* to Long Mode.
3.  **Stage 3 (Higher Half):**
    * CPU state: 64-bit Long Mode.
    * Kernel jumps to `0xFFFFFFFF80000000` (Negative 2GB).
    * Stack is re-aligned to the higher half.
    * C Kernel Main (`kmain()`) is executed.

## 3. Memory Management
The system utilises a **Higher Half** kernel layout to strictly separate kernel and user space.

### 3.1 Virtual Memory Map
| Start Address | End Address | Size | Region | Purpose |
| :--- | :--- | :--- | :--- | :--- |
| `0x0000000000000000` | `0x00007FFFFFFFFFFF` | 128 TB | **User Space** | Applications, Heap, Stack |
| `0x0000800000000000` | `0xFFFF7FFFFFFFFFFF` | ~16 EB | **Non-Canonical** | **Gap (GP Fault if accessed)** |
| `0xFFFF800000000000` | `0xFFFFFFFF7FFFFFFF` | ~128 TB | **Kernel Space** | Direct Physical Map, Drivers |
| `0xFFFFFFFF80000000` | `0xFFFFFFFFFFFFFFFF` | 2 GB | **Kernel Core** | Kernel Code (.text), Data, Heap |

### 3.2 Allocation Strategy
* **PMM (Physical Memory Manager):** Bitmap Allocator (4KB granularity).
* **VMM (Virtual Memory Manager):** 4-Level Paging (PML4, PDPT, PD, PT).
* **Heap:** Slab Allocator (for small objects) and Linked List (for large blocks).

## 4. Process Management
* **Multitasking:** Preemptive.
* **Scheduling Algorithm:** Round Robin (initial implementation).
* **Context Switching:** Software context switch saving standard registers (`rax`, `rbx`, etc.) and SSE/AVX state.
* **Executable Format:** **ELF64** (System V ABI).

## 5. System Call Interface
* **Mechanism:** `syscall` / `sysret` instructions (fast path) replacing legacy `int 0x80`.
* **ABI:** Parameters passed in registers (`rdi`, `rsi`, `rdx`, `r10`, `r8`, `r9`).
* **Compatibility:** Linux-compatible syscall numbering where feasible (to ease porting of tools).

## 6. Storage & Filesystem
* **Root Filesystem:** **Ext2** (Second Extended Filesystem).
    * Support for Inodes, Superblocks, Groups.
    * Forward compatible with Ext3/Ext4 drivers.
* **Initial Boot:** TAR-based Initrd (Initial Ramdisk) for loading essential modules before disk drivers wake up.

## 7. Graphics & GUI
* **Kernel Mode:** Linear Framebuffer support (via Multiboot2). No BIOS interrupts.
* **User Mode:** Custom Window Compositor.
    * Written in C++.
    * Communicates via Shared Memory buffers.
    * Double-buffered rendering to prevent tearing.

---
# Halo OS

A 64-bit Monolithic Operating System built from scratch for educational purposes.

## Current Status (v0.2)
- ✅ Boots to 64-bit long mode via Multiboot2
- ✅ Higher-half kernel mapping
- ✅ Physical and Virtual Memory Managers
- ✅ Interrupt handling (exceptions + hardware IRQs)
- ✅ VGA text mode driver with color themes
- ✅ PS/2 Keyboard driver with input buffering
- ✅ Interactive command shell with 8 commands
- ✅ CPU vendor detection via CPUID

## Architecture
* **Kernel:** C11 (Higher Half, Monolithic)
* **Userland:** C++20 (planned)
* **Boot:** Multiboot2 -> Long Mode
* **Memory:** 4-level paging, bitmap PMM

## Features
### Implemented
- **Memory Management**: Bitmap-based physical allocator, 4-level page tables
- **Interrupts**: Full exception/IRQ handling, PIC remapping
- **Drivers**: VGA (80x25 text), PS/2 Keyboard (scancode translation)
- **Shell Commands**: help, clear, about, reboot, theme, cpu

### In Progress
- Kernel heap allocator (kmalloc/kfree)
- Process management and scheduling

See `docs/` for full architectural specifications.
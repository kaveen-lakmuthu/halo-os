# Halo OS - Project Roadmap

## Epoch 1: The Spark (Kernel Bootstrap)
* [x] Setup Cross-Compiler and Build System (Make).
* [x] Implement Multiboot2 Header.
* [x] Initial 32-bit Trampoline (Entry Point).
* [x] Implement VGA Driver and Text Mode Output.
* [ ] Establish Higher Half Paging.
* [ ] Enter 64-bit Long Mode.
* [ ] **Milestone:** Kernel boots and prints to Framebuffer in 64-bit mode.

## Epoch 2: The Foundation (Memory & Interrupts)
* [ ] Global Descriptor Table (GDT).
* [ ] Interrupt Descriptor Table (IDT).
* [ ] Physical Memory Manager (Bitmap).
* [ ] Virtual Memory Manager (Paging/Mapping).
* [ ] Kernel Heap (kmalloc/kfree).
* [ ] **Milestone:** Kernel can handle Page Faults without crashing.

## Epoch 3: The Hardware (Drivers)
* [ ] ACPI Table Parsing (Finding hardware).
* [ ] APIC Timer (High precision timer).
* [ ] PS/2 Keyboard Driver.
* [ ] Serial Port (Logging).
* [ ] **Milestone:** Typing on the keyboard displays characters on screen.

## Epoch 4: The Multitasking (Processes)
* [ ] Process Control Block (PCB) structure.
* [ ] Context Switching Logic (Assembly).
* [ ] Round Robin Scheduler.
* [ ] The `syscall` interface.
* [ ] **Milestone:** Two threads running "simultaneously" (printing A and B).

## Epoch 5: The Filesystem
* [ ] ATA/AHCI Disk Driver.
* [ ] Ext2 Filesystem Driver (Read-only first).
* [ ] ELF64 Loader.
* [ ] **Milestone:** Loading a simple "Hello World" program from disk.
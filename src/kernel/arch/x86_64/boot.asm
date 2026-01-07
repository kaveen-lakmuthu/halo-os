global start
extern kmain

%define KERNEL_VIRTUAL_BASE 0xFFFFFFFF80000000

; --- BOOT SECTION (Physical 1MB) ---
section .boot
bits 32

start:
    ; 1. Setup Stack
    mov esp, stack_top

    ; 2. SAVE MULTIBOOT POINTER (CRITICAL NEW STEP)
    ; GRUB puts the address of the info structure in EBX.
    ; Must save it now before clobber registers.
    mov [multiboot_info_ptr], ebx

    ; 3. Checks
    call check_multiboot
    call check_cpuid
    call check_long_mode

    ; 4. Paging Setup
    call setup_page_tables
    call enable_paging

    ; 5. Load GDT
    lgdt [gdt64.pointer]

    ; 6. Long Jump
    jmp gdt64.code:long_mode_start

; ... (Keep check_multiboot, check_cpuid, check_long_mode, setup_page_tables, enable_paging exactly as they were) ...
; ... (Can copy-paste the subroutines from the previous session here) ...

; --- PASTE EXISTING SUBROUTINES HERE IF OVERWRITE THE FILE ---
; (For brevity, Assume kept the subroutines check_multiboot through enable_paging)

check_multiboot:
    cmp eax, 0x36d76289
    jne .no_multiboot
    ret
.no_multiboot:
    mov al, "M"
    jmp error

check_cpuid:
    pushfd
    pop eax
    mov ecx, eax
    xor eax, 1 << 21
    push eax
    popfd
    pushfd
    pop eax
    push ecx
    popfd
    cmp eax, ecx
    je .no_cpuid
    ret
.no_cpuid:
    mov al, "C"
    jmp error

check_long_mode:
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb .no_long_mode

    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jz .no_long_mode
    ret
.no_long_mode:
    mov al, "L"
    jmp error

setup_page_tables:
    mov eax, page_table_l3
    or eax, 0b11
    mov [page_table_l4], eax

    mov eax, page_table_l2
    or eax, 0b11
    mov [page_table_l3], eax

    mov eax, page_table_l2
    or eax, 0b11
    mov [page_table_l3 + 510 * 8], eax

    mov ecx, 0
.loop:
    mov eax, 0x200000
    mul ecx
    or eax, 0b10000011
    mov [page_table_l2 + ecx * 8], eax

    inc ecx
    cmp ecx, 512
    jne .loop

    mov eax, page_table_l3
    or eax, 0b11
    mov [page_table_l4 + 511 * 8], eax
    ret

enable_paging:
    mov eax, page_table_l4
    mov cr3, eax

    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax
    ret

error:
    mov dword [0xb8000], 0x4f524f45
    mov dword [0xb8004], 0x4f3a4f52
    mov dword [0xb8008], 0x4f204f20
    mov byte  [0xb800a], al
    hlt

; --- 64-BIT TRAMPOLINE ---
bits 64
long_mode_start:
    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; PREPARE ARGUMENTS FOR KMAIN
    ; In 64-bit System V ABI, the first argument goes into RDI.
    ; Load the saved physical pointer into RDI.
    ; Since Identity Mapped the first 1GB, this physical address is valid.
    mov edi, [multiboot_info_ptr]
    
    mov rax, kmain
    call rax
    hlt

; --- DATA ---
align 4096
page_table_l4:
    times 4096 db 0
page_table_l3:
    times 4096 db 0
page_table_l2:
    times 4096 db 0
stack_bottom:
    times 16384 db 0
stack_top:

; STORAGE FOR MULTIBOOT POINTER
align 4
multiboot_info_ptr:
    dd 0    ; Allocate 4 bytes to store the 32-bit pointer from GRUB

; --- GDT ---
gdt64:
    dq 0
.code: equ $ - gdt64
    dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53)
.pointer:
    dw $ - gdt64 - 1
    dq gdt64
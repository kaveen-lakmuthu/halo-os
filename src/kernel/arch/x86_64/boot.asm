global start
extern kmain

; --- BOOT SECTION (Physical 1MB) ---
section .boot
bits 32

start:
    ; 1. Setup Stack
    mov esp, stack_top

    ; 2. Checks
    call check_multiboot
    call check_cpuid
    call check_long_mode

    ; 3. Paging Setup
    call setup_page_tables
    call enable_paging

    ; 4. Load the 64-bit GDT
    lgdt [gdt64.pointer]

    ; 5. The Long Jump (Trampoline)
    ; Jump to 'long_mode_start', which is NOW inside .boot (Low Memory)
    ; This fits perfectly in a 32-bit jump instruction.
    jmp gdt64.code:long_mode_start

; --- Subroutines ---

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
    ; 1. Map L4[0] -> L3
    mov eax, page_table_l3
    or eax, 0b11
    mov [page_table_l4], eax

    ; 2. Map L3[0] -> L2 (For identity mapping 0-1GB)
    mov eax, page_table_l2
    or eax, 0b11
    mov [page_table_l3], eax

    ; 3. Map L3[510] -> L2 (For Higher Half Kernel -2GB)  <-- ADD THIS BLOCK
    mov eax, page_table_l2
    or eax, 0b11
    mov [page_table_l3 + 510 * 8], eax

    ; 4. Map P2 entries (0-1GB physical)
    mov ecx, 0
.loop:
    mov eax, 0x200000
    mul ecx
    or eax, 0b10000011
    mov [page_table_l2 + ecx * 8], eax

    inc ecx
    cmp ecx, 512
    jne .loop

    ; 5. Map L4[511] -> L3 (Recursive/High Mem)
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

; --- 64-BIT TRAMPOLINE (Still in .boot section) ---
bits 64
long_mode_start:
    ; This is now in 64-bit mode, but still in Low Memory code!
    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Make the "Stratospheric Jump" to High Memory.
    ; kmain is linked at 0xFFFFFFFF80..., so 'mov rax, kmain' loads that huge address.
    mov rax, kmain
    call rax
    hlt

; --- DATA STRUCTURES (In .boot section) ---
; Use 'times ... db 0' instead of 'resb' because this is a PROGBITS section, not BSS.
align 4096
page_table_l4:
    times 4096 db 0
page_table_l3:
    times 4096 db 0
page_table_l2:
    times 4096 db 0
stack_bottom:
    times 16384 db 0 ; 16KB Stack
stack_top:

; --- GDT ---
gdt64:
    dq 0
.code: equ $ - gdt64
    dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53)
.pointer:
    dw $ - gdt64 - 1
    dq gdt64
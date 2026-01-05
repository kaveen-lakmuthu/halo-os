global start
extern kmain

section .text
bits 32     ; Start in 32-bit mode

start:
    ; 1. Setup Stack
    mov esp, stack_top

    ; 2. Checks (Safety First)
    call check_multiboot
    call check_cpuid
    call check_long_mode

    ; 3. Paging Setup
    call setup_page_tables
    call enable_paging

    ; 4. Load the 64-bit GDT
    lgdt [gdt64.pointer]

    ; 5. The Long Jump
    ; Jump to the "long_mode_start" label using the 64-bit code segment (gdt64.code)
    jmp gdt64.code:long_mode_start

; --- Subroutines ---

check_multiboot:
    cmp eax, 0x36d76289    ; Magic number written by GRUB into EAX
    jne .no_multiboot
    ret
.no_multiboot:
    mov al, "M"
    jmp error

check_cpuid:
    ; Attempt to flip the ID bit in EFLAGS to check if CPUID is supported
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
    ; Check if CPU supports "Extended Processor Info"
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb .no_long_mode

    ; Check if Long Mode bit is set
    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jz .no_long_mode
    ret
.no_long_mode:
    mov al, "L"
    jmp error

setup_page_tables:
    ; Map first P4 entry to P3 table
    mov eax, page_table_l3
    or eax, 0b11 ; Present + Writable
    mov [page_table_l4], eax

    ; Map first P3 entry to P2 table
    mov eax, page_table_l2
    or eax, 0b11 ; Present + Writable
    mov [page_table_l3], eax

    ; Map P2 entries to 2MB physical pages (Identity Mapping)
    mov ecx, 0         ; Counter
.loop:
    mov eax, 0x200000  ; 2MB size
    mul ecx
    or eax, 0b10000011 ; Present + Writable + Huge Page
    mov [page_table_l2 + ecx * 8], eax

    inc ecx
    cmp ecx, 512       ; Map 512 entries (1GB total)
    jne .loop
    ret

enable_paging:
    ; 1. Pass P4 table location to CR3
    mov eax, page_table_l4
    mov cr3, eax

    ; 2. Enable PAE (Physical Address Extension)
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; 3. Switch to Long Mode (EFER MSR)
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; 4. Enable Paging (CR0)
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax
    ret

error:
    ; Print "ERR: X" to screen (Video Memory hack for panic)
    mov dword [0xb8000], 0x4f524f45
    mov dword [0xb8004], 0x4f3a4f52
    mov dword [0xb8008], 0x4f204f20
    mov byte  [0xb800a], al
    hlt

section .bss
align 4096
page_table_l4:
    resb 4096
page_table_l3:
    resb 4096
page_table_l2:
    resb 4096
stack_bottom:
    resb 4096 * 4
stack_top:

section .rodata
gdt64:
    dq 0 ; Zero Entry
.code: equ $ - gdt64
    dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53) ; Code Segment
.pointer:
    dw $ - gdt64 - 1
    dq gdt64

; --- 64-BIT LAND ---

section .text
bits 64
long_mode_start:
    ; Clear data segments (they are meaningless in 64-bit, but good practice)
    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Call the C Kernel
    call kmain
    hlt
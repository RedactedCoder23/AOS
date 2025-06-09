BITS 16
ORG 0x7e00

KERNEL_LOAD   equ 0x10000
%ifndef KERNEL_START_SECTOR
%define KERNEL_START_SECTOR 3
%endif

start:

    cli
    xor ax, ax
    mov ss, ax
    mov sp, 0x7c00
    sti
    xor ax, ax
    mov ds, ax

    ; load kernel from disk to KERNEL_LOAD
    mov ah, 0x02            ; read sectors
    mov al, KERNEL_SECTORS
    mov ch, 0
    mov cl, KERNEL_START_SECTOR
    mov dh, 0
    mov bx, 0
    mov ax, KERNEL_LOAD >> 4
    mov es, ax
    int 0x13
    jc disk_error

    ; enable A20
    in al, 0x92
    or al, 2
    out 0x92, al

    ; setup GDT
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp 0x08:protected_entry

disk_error:
    hlt
    jmp disk_error

align 8
gdt_start:
    dq 0
    dq 0x00CF9A000000FFFF
    dq 0x00CF92000000FFFF
gdt_end:
gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

BITS 32
protected_entry:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax
    mov esp, 0x90000
    cld

    ; copy kernel to 1MiB
    mov esi, KERNEL_LOAD
    mov edi, 0x00100000
    mov ecx, KERNEL_SIZE
    rep movsb

    ; zero BSS
    mov edi, BSS_START
    mov ecx, BSS_END - BSS_START
    xor eax, eax
    rep stosb

    mov eax, KERNEL_ENTRY
    call eax

.halt:
    cli
    hlt
    jmp .halt

TIMES 510-($-$$) db 0
DW 0xAA55

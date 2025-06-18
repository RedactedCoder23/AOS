;
; AOS — stage1.asm
; (c) 2025 RedactedCoder23
; Purpose: Bootloader stage1
; Dependencies: stage2.bin
;
BITS 16
ORG 0x7c00

start:
    mov [boot_drive], dl
    mov bx, 0x7e00
    mov ah, 0x02
    mov al, 1
    mov ch, 0
    mov cl, 2
    mov dh, 0
    int 0x13
    jmp 0x0000:0x7e00

boot_drive: db 0

TIMES 510-($-$$) db 0
DW 0xAA55

BITS 16
ORG 0x7c00

mov ah, 0x0e
mov al, 'X'
int 0x10

cli
hlt

times 510-($-$$) db 0
DW 0xAA55

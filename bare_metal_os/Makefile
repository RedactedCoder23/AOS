ASM = nasm
ASMFLAGS = -f bin
CC = gcc
LD = ld
CFLAGS = -ffreestanding -O2 -Wall -Wextra -m32
LDFLAGS = -T kernel.ld -m elf_i386 -z separate-code

.PHONY: all clean

all: bootloader.bin aos.bin

kernel.bin: kernel.c
		$(CC) $(CFLAGS) -c $< -o kernel.o
		$(LD) $(LDFLAGS) kernel.o -o kernel.elf
		objcopy -O binary kernel.elf kernel.bin

bootloader.bin: bootloader.asm kernel.bin
		@size=$$(wc -c < kernel.bin); \
		sectors=$$((($$size + 511)/512)); \
		bstart=$$(nm kernel.elf | awk '$$3=="__bss_start" {print $$1}'); \
		bend=$$(nm kernel.elf | awk '$$3=="__bss_end"   {print $$1}'); \
		$(ASM) $(ASMFLAGS) \
		  -DKERNEL_ENTRY=0x00100000 \
		  -DBSS_START=0x$${bstart} \
		  -DBSS_END=0x$${bend} \
		  -DKERNEL_SIZE=$${size} \
		  -DKERNEL_SECTORS=$${sectors} \
		  $< -o $@

aos.bin: bootloader.bin kernel.bin
	cat bootloader.bin kernel.bin > $@

clean:
	rm -f *.bin *.o *.elf kernel.elf

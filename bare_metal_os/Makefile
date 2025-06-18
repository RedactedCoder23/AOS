ASM = nasm
ASMFLAGS = -f bin
ARCH ?= x86_64
CC = gcc
LD = ld
CFLAGS = -ffreestanding -O2 -Wall -Wextra -m32
LDFLAGS = -T kernel.ld -m elf_i386 -z separate-code
# Path to GRUB stage2. Use grub-install to discover the proper libdir if
# available; fall back to the traditional location.
STAGE2 ?= $(shell grub-install --print=libdir 2>/dev/null || echo /usr/lib/grub)/i386-pc/stage2

.PHONY: all clean stage1.bin stage2.bin bootloader.bin kernel.bin aos.bin

all: bootloader.bin kernel.bin aos.bin
	

kernel.bin: kernel.c memory.c fs.c branch.c idt.c traps.c interpreter/command_interpreter.c serial.c \
	../src/generated/commands.c ../src/generated/command_map.c config_stub.c
	$(CC) $(CFLAGS) -I.. -I../include -I../src/generated -c kernel.c -o kernel.o
	$(CC) $(CFLAGS) -I.. -I../include -I../src/generated -c memory.c -o memory.o
	$(CC) $(CFLAGS) -I.. -I../include -I../src/generated -c fs.c -o fs.o
	$(CC) $(CFLAGS) -I.. -I../include -I../src/generated -c branch.c -o branch.o
	$(CC) $(CFLAGS) -I.. -I../include -I../src/generated -c idt.c -o idt.o
	$(CC) $(CFLAGS) -I.. -I../include -I../src/generated -c traps.c -o traps.o
	$(CC) $(CFLAGS) -I.. -I../include -I../src/generated -c interpreter/command_interpreter.c -o command_interpreter.o
	$(CC) $(CFLAGS) -I.. -I../include -I../src/generated -c serial.c -o serial.o
	$(CC) $(CFLAGS) -I.. -I../include -I../src/generated -c ../src/generated/commands.c -o commands.o
	$(CC) $(CFLAGS) -I.. -I../include -I../src/generated -c ../src/generated/command_map.c -o command_map.o
	$(CC) $(CFLAGS) -I.. -I../include -I../src/generated -c config_stub.c -o config.o
	$(LD) $(LDFLAGS) kernel.o memory.o fs.o branch.o idt.o traps.o command_interpreter.o serial.o commands.o command_map.o config.o -o kernel.elf
	objcopy -O binary kernel.elf kernel.bin

stage1.bin: stage1.asm
	$(ASM) $(ASMFLAGS) $< -o $@

stage2.bin:
	cp $(STAGE2) $@

bootloader.bin: stage1.bin stage2.bin
	cat stage1.bin stage2.bin > $@

aos.bin: bootloader.bin kernel.bin
	cat bootloader.bin kernel.bin > $@

clean:
	rm -f *.bin *.o *.elf kernel.elf

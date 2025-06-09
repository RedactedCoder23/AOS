ASM = nasm
ASMFLAGS = -f bin
ARCH ?= x86_64
CC = gcc
LD = ld
CFLAGS = -ffreestanding -O2 -Wall -Wextra -Werror -m32
LDFLAGS = -T kernel.ld -m elf_i386 -z separate-code

.PHONY: all clean stage1.bin stage2.bin bootloader.bin kernel.bin aos.bin

all: bootloader.bin kernel.bin aos.bin
	
kernel.bin: kernel.c memory.c fs.c branch.c ai_trigger.c interpreter/command_interpreter.c ../commands.c ../command_map.c config_stub.c ../logging.c ../error.c
	$(CC) $(CFLAGS) -I.. -I../include -c kernel.c -o kernel.o
	$(CC) $(CFLAGS) -I.. -I../include -c memory.c -o memory.o
	$(CC) $(CFLAGS) -I.. -I../include -c fs.c -o fs.o
       $(CC) $(CFLAGS) -I.. -I../include -c branch.c -o branch.o
       $(CC) $(CFLAGS) -I.. -I../include -c ai_trigger.c -o ai_trigger.o
	$(CC) $(CFLAGS) -I.. -I../include -c interpreter/command_interpreter.c -o command_interpreter.o
	$(CC) $(CFLAGS) -I.. -I../include -c ../commands.c -o commands.o
	$(CC) $(CFLAGS) -I.. -I../include -c ../command_map.c -o command_map.o
	$(CC) $(CFLAGS) -I.. -I../include -c config_stub.c -o config.o
	$(CC) $(CFLAGS) -I.. -I../include -c ../logging.c -o logging.o
	$(CC) $(CFLAGS) -I.. -I../include -c ../error.c -o error.o
       $(LD) $(LDFLAGS) kernel.o memory.o fs.o branch.o ai_trigger.o command_interpreter.o commands.o command_map.o config.o logging.o error.o -o kernel.elf
	objcopy -O binary kernel.elf kernel.bin


stage1.bin: stage1.asm
	$(ASM) $(ASMFLAGS) $< -o $@
	
	stage2.bin: stage2.asm kernel.bin
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
  -DKERNEL_START_SECTOR=3 \
  $< -o $@

bootloader.bin: stage1.bin stage2.bin
	cat stage1.bin stage2.bin > $@

aos.bin: bootloader.bin kernel.bin
	cat bootloader.bin kernel.bin > $@

clean:
	rm -f *.bin *.o *.elf kernel.elf

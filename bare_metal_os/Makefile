ASM=nasm
ASMFLAGS=-f bin

all: bootloader.bin

bootloader.bin: bootloader.asm
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -f *.bin *.o

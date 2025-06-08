.PHONY: all generate host bare run clean memory fs ai branch
RUN_OPTS ?= -serial stdio

all: host bare
	
# 1. Regenerate C sources
generate:
	python3 generate_aos_mappings.py

# 2. Build host-side harness
host: generate
	@echo "→ Building host_test"
	@mkdir -p build
	gcc -DRUNNING_ON_HOST -Iinclude src/*.c bare_metal_os/memory.c \
	bare_metal_os/filesystem.c command_map.c commands.c -o build/host_test

# 3. Build bare-metal image
bare: generate
	@echo "→ Building bare-metal image"
	@cd bare_metal_os && $(MAKE) clean && $(MAKE) all
	@cp bare_metal_os/aos.bin aos.bin

# 4. Run in QEMU (detect qemu binary)
run: bare
	@echo "→ Running AOS in QEMU"
	@sh -c '\
	for cmd in qemu-system-x86_64 qemu-system-i386 qemu; do \
	which $$cmd >/dev/null 2>&1 && { EMU=$$cmd; break; } \
	done; \
	if [ -z "$$EMU" ]; then \
	echo "Error: install qemu-system-x86_64, qemu-system-i386 or qemu"; exit 1; \
	fi; \
	$$EMU -drive format=raw,file=aos.bin $(RUN_OPTS)'

clean:
	@echo "→ Cleaning"
	@cd bare_metal_os && $(MAKE) clean

memory: host
	        @echo "Running memory allocator test"
	        echo "TESTALLOC\nexit" | ./build/host_test

fs: host
	        @echo "Running filesystem test"
	        echo "CAT\nexit" | ./build/host_test

ai: host
	        @echo "Running AI service test"
	        echo "AI\nexit" | ./build/host_test

branch: host
	        @echo "Running branch manager test"
	        echo "OPENBR\nexit" | ./build/host_test

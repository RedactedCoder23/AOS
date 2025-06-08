.PHONY: all generate host bare clean

all: host bare

# 1. Regenerate C sources from text masters
generate:
	python3 generate_aos_mappings.py

# 2. Build host-side test harness
host: generate
	@echo "→ Building host_test harness"
	@mkdir -p build
	gcc -Iinclude src/*.c command_map.c commands.c -o build/host_test

# 3. Build bare-metal image
bare: generate
	@echo "→ Assembling bare-metal bootloader"
	@cd bare_metal_os && make clean && make all
	@cp bare_metal_os/bootloader.bin aos.bin

clean:
	@echo "→ Cleaning build artifacts"
	rm -rf build aos.bin bare_metal_os/*.o bare_metal_os/*.bin

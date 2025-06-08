.PHONY: all generate host bare run clean

all: host bare

# 1. Regenerate C sources from text masters
generate:
	python3 generate_aos_mappings.py

# 2. Build host-side test harness
host: generate
	@echo "→ Building host_test harness"
	@mkdir -p build
	gcc -Iinclude src/*.c command_map.c commands.c -o build/host_test

test: build/utils_test
	./build/utils_test

build/utils_test: tests/utils_test.c src/utils.c include/utils.h
	@mkdir -p build
	gcc -Iinclude tests/utils_test.c src/utils.c -o build/utils_test

# 3. Build bare-metal image
bare: generate
	@echo "\u2192 Building bare-metal image"
	@cd bare_metal_os && make clean && make all
	@cp bare_metal_os/aos.bin aos.bin

.PHONY: run

# Build and launch in QEMU
run: bare
	@echo "\u2192 Running AOS in QEMU"
	@sh -c '\
  if which qemu-system-x86_64 >/dev/null 2>&1; then EMU=qemu-system-x86_64; \
  elif which qemu-system-i386 >/dev/null 2>&1; then EMU=qemu-system-i386; \
  else echo "Error: please install qemu-system-x86_64 or qemu-system-i386" && exit 1; \
  fi; \
  $$EMU -drive format=raw,file=aos.bin -serial stdio'

clean:
	@echo "→ Cleaning build artifacts"
	rm -rf build aos.bin bare_metal_os/*.o bare_metal_os/*.bin

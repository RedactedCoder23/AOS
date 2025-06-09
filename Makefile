.PHONY: all generate host bare run clean

all: host bare

# 1. Regenerate C sources from text masters
generate:
	python3 generate_aos_mappings.py
	@$(MAKE) -s checklist

# 2. Build host-side test harness
NCURSES_CFLAGS := $(shell pkg-config --cflags ncurses 2>/dev/null)
NCURSES_LIBS := $(shell pkg-config --libs ncurses 2>/dev/null || echo -lncurses)

host: generate
	@echo "→ Building host binaries"
	@mkdir -p build
	gcc -Iinclude $(NCURSES_CFLAGS) src/main.c src/interpreter.c src/branch_manager.c src/ui_graph.c command_map.c commands.c $(NCURSES_LIBS) -lm -o build/host_test
	gcc -Iinclude $(NCURSES_CFLAGS) src/ui_graph.c src/branch_manager.c src/ui_main.c $(NCURSES_LIBS) -lm -o build/ui_graph

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
  $$EMU -nographic -drive format=raw,file=aos.bin $(RUN_OPTS)'

clean:
	@echo "→ Cleaning build artifacts"
	rm -rf build aos.bin
	@$(MAKE) -C bare_metal_os clean

memory:
	@echo "→ Building memory demo"
	@mkdir -p build
	gcc -Isubsystems/memory subsystems/memory/memory.c subsystems/memory/memory_main.c -o build/memory_demo

fs:
	@echo "→ Building fs demo"
	@mkdir -p build
	gcc -Isubsystems/fs subsystems/fs/fs.c subsystems/fs/fs_main.c -o build/fs_demo

ai:
	@echo "→ Building ai demo"
	@mkdir -p build
	gcc -Isubsystems/ai subsystems/ai/ai.c subsystems/ai/ai_main.c -o build/ai_demo

branch:
	@echo "→ Building branch demo"
	@mkdir -p build
	gcc -Isubsystems/branch subsystems/branch/branch.c subsystems/branch/branch_main.c -o build/branch_demo

ui: host
	@echo "UI built via host target"

checklist:
	@if [ -s AOS-CHECKLIST.log ]; then \
	echo "Checklist has entries:"; cat AOS-CHECKLIST.log; exit 1; \
	else \
	echo "Checklist clean"; \
	fi

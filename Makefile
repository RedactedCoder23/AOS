.PHONY: all generate host bare run clean ui ui-check web-ui branch-vm plugins iso efi branch-net desktop-ui ai-service policy net
		
all: host bare

# 1. Regenerate C sources from text masters
generate:
	python3 generate_aos_mappings.py
	@$(MAKE) -s checklist

# 2. Build host-side test harness
NCURSES_CFLAGS := $(shell pkg-config --cflags ncurses 2>/dev/null)
NCURSES_LIBS := $(shell pkg-config --libs ncurses 2>/dev/null || echo -lncurses)

host: generate subsystems
	@echo "→ Building host binaries"
	@mkdir -p build
	gcc -rdynamic -Iinclude -Isubsystems/memory -Isubsystems/fs -Isubsystems/ai -Isubsystems/branch -Isubsystems/net $(NCURSES_CFLAGS) src/main.c src/interpreter.c src/branch_manager.c src/ui_graph.c src/branch_vm.c src/plugin_loader.c src/branch_net.c src/ai_syscall.c src/policy.c src/memory.c src/app_runtime.c src/config.c command_map.c commands.c subsystems/memory/memory.c subsystems/fs/fs.c subsystems/ai/ai.c subsystems/branch/branch.c subsystems/net/net.c $(NCURSES_LIBS) -ldl -lcurl -lm -o build/host_test
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

boot: bare
	@echo "\u2192 Booting AOS"
	@if [ ! -f aos.bin ]; then echo "boot error: missing aos.bin" | tee -a AGENT.md; exit 1; fi
	@sh -c '\
  if which qemu-system-x86_64 >/dev/null 2>&1; then EMU=qemu-system-x86_64; \
  elif which qemu-system-i386 >/dev/null 2>&1; then EMU=qemu-system-i386; \
  else echo "boot error: qemu not installed" | tee -a AGENT.md; exit 1; \
  fi; \
  $$EMU -nographic -drive format=raw,file=aos.bin $(RUN_OPTS)'

bare-smoke: bare
	@out=$$(./examples/bare_repl_smoke.sh 2>&1); \
	echo "$$out"; \
	echo "$$out" | grep -q "bare-smoke: ALL TESTS PASS" && exit 0; \
	echo "$$out" >> AOS-CHECKLIST.log; \
	exit 1

clean:
	@echo "→ Cleaning build artifacts"
	rm -rf build aos.bin
	@$(MAKE) -C bare_metal_os clean

memory:
	@echo "→ Building memory demo"
	@mkdir -p build
	gcc -Isubsystems/memory subsystems/memory/memory.c src/memory.c examples/memory_demo.c -o build/memory_demo

fs:
	@echo "→ Building fs demo"
	@mkdir -p build
	gcc -Isubsystems/fs -Iinclude subsystems/fs/fs.c examples/fs_demo.c -o build/fs_demo

ai:
	@echo "→ Building ai demo"
	@mkdir -p build
	gcc -Isubsystems/ai -Iinclude subsystems/ai/ai.c src/ai_syscall.c examples/ai_demo.c -lcurl -o build/ai_demo

branch:
	@echo "→ Building branch demo"
	@mkdir -p build
	gcc -Isubsystems/branch subsystems/branch/branch.c examples/branch_demo.c -o build/branch_demo

branch-vm:
	@echo "→ Building branch VM demo"
	@mkdir -p build
	gcc -Iinclude src/branch_vm.c subsystems/branch/branch.c examples/branch_vm_demo.c -o build/branch_vm_demo

plugins:
	@echo "→ Building plugins demo"
	@mkdir -p build/plugins
	gcc -fPIC -shared -o build/plugins/sample.so examples/sample_plugin.c
	gcc -Iinclude src/plugin_loader.c examples/plugin_demo.c -ldl -o build/plugin_demo

aos-cli:
	@echo "→ Building aos CLI"
	@mkdir -p build
	gcc -Isubsystems/branch subsystems/branch/branch.c examples/aos_cli.c -o build/aos

branch-net:
	@echo "→ Building branch net demo"
	@mkdir -p build
	@gcc -Iinclude src/branch_net.c src/branch_manager.c examples/branch_fed_demo.c -pthread -lm -o build/branch_fed_demo

ai-service:
	@echo "→ Building ai service demo"
	@mkdir -p build
	gcc -Iinclude -Isubsystems/ai src/ai_syscall.c examples/ai_service_demo.c subsystems/ai/ai.c -o build/ai_service_demo -lcurl

policy:
	@echo "→ Building policy demo"
	@mkdir -p build
	gcc -Iinclude src/policy.c examples/policy_demo.c -o build/policy_demo

net:
	@echo "→ Building net echo demo"
	@mkdir -p build
	gcc -Isubsystems/net subsystems/net/net.c examples/net_echo.c -o build/net_echo
apps:
	@echo "→ Building sample apps"
	@mkdir -p build/apps
	gcc apps_src/fileman.c -o build/apps/fileman
	gcc apps_src/textedit.c -o build/apps/textedit

net-http:
	@echo "→ Building http server demo"
	@mkdir -p build
	gcc -Isubsystems/net subsystems/net/net.c examples/http_server.c -o build/http_server

test: host branch
	@echo "→ Running branch demo"
	@./build/branch_demo >/tmp/branch_demo.out && cat /tmp/branch_demo.out

test-memory: memory
	./examples/memory_demo.sh

test-fs: fs
	./examples/fs_smoke.sh

test-branch: branch
	./examples/branch_smoke.sh

test-plugin: plugins
	./examples/plugin_smoke.sh

test-policy: policy
	./examples/policy_smoke.sh

test-net: net
	./examples/net_echo_test.sh

efi:
	@echo "→ Building EFI stub"
	mkdir -p bare_metal_os/efi
	echo "stub" > bare_metal_os/efi/aos.efi
	
iso: efi
	@echo "→ Creating aos.iso"
	touch aos.iso

subsystems: memory fs ai branch net

ui: host
	@echo "UI built via host target"

ui-check: ui
	@echo "\u2192 Verifying UI binary"
	@./build/ui_graph --help

web-ui:
	@echo "\u2192 Launching web UI at http://localhost:8000"
	python3 scripts/branch_ui.py

desktop-ui:
	@echo "\u2192 Launching desktop UI at http://localhost:8000"
	python3 scripts/desktop_backend.py
checklist:
	@if [ -s AOS-CHECKLIST.log ]; then \
	echo "Checklist has entries:"; cat AOS-CHECKLIST.log; exit 1; \
	else \
	echo "Checklist clean"; \
	fi

.PHONY: all generate host bootloader kernel bare run clean ui ui-check web-ui branch-vm plugins iso efi branch-net desktop-ui ai-service aicell modeld policy net
		
CC_TARGET ?= x86_64

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
	gcc -Wall -Werror -rdynamic -Iinclude -Isubsystems/memory -Isubsystems/fs -Isubsystems/ai -Isubsystems/branch -Isubsystems/net $(NCURSES_CFLAGS) \
	            src/main.c src/repl.c src/interpreter.c src/branch_manager.c src/ui_graph.c \
	            src/branch_vm.c src/plugin_loader.c src/plugin_supervisor.c src/wasm_runtime.c \
	            src/branch_net.c src/ai_syscall.c src/aicell.c src/checkpoint.c src/policy.c \
	            src/memory.c src/app_runtime.c src/config.c src/logging.c src/error.c \
	            command_map.c commands.c \
	            subsystems/memory/memory.c subsystems/fs/fs.c subsystems/ai/ai.c \
	            subsystems/branch/branch.c subsystems/net/net.c \
	            $(NCURSES_LIBS) -ldl -lcurl -lm -o build/host_test
	gcc -Wall -Werror -Iinclude $(NCURSES_CFLAGS) src/ui_graph.c src/branch_manager.c \
            src/logging.c src/error.c src/ui_main.c $(NCURSES_LIBS) -lm -o build/ui_graph

# 3. Build bare-metal components
bootloader: generate
	       @echo "\u2192 Building bootloader"
	       @cd bare_metal_os && make clean stage1.bin stage2.bin bootloader.bin ARCH=$(CC_TARGET)
	       @cp bare_metal_os/bootloader.bin bootloader.bin

	kernel: generate
	       @echo "\u2192 Building kernel"
	       @cd bare_metal_os && make kernel.bin ARCH=$(CC_TARGET)

	bare: bootloader kernel
	       @echo "\u2192 Creating aos.bin"
	       @cd bare_metal_os && cat bootloader.bin kernel.bin > aos.bin
	       @cp bare_metal_os/aos.bin aos.bin

.PHONY: run

# Build and launch in QEMU
run: bare
	        @echo "\u2192 Running AOS in QEMU"
	        @sh -c '\
		  case "$(CC_TARGET)" in \
		    x86_64) EMU=qemu-system-x86_64 ;; \
		    riscv64) EMU=qemu-system-riscv64 ;; \
		    arm) EMU=qemu-system-arm ;; \
		    *) EMU=qemu-system-x86_64 ;; \
		  esac; \
		  if ! which $$EMU >/dev/null 2>&1; then echo "Error: $$EMU not installed" && exit 1; fi; \
	  if [ "$(CC_TARGET)" = "x86_64" ]; then \
		$$EMU -nographic -drive format=raw,file=aos.bin $(RUN_OPTS); \
		else \
		$$EMU -nographic -kernel aos.bin $(RUN_OPTS); \
		fi'
		
		boot: bare
		@echo "\u2192 Booting AOS"
	       @if [ ! -f aos.bin ]; then echo "boot error: missing aos.bin" | tee -a AGENT.md; exit 1; fi
	       @sh -c '\
	  case "$(CC_TARGET)" in \
	    x86_64) EMU=qemu-system-x86_64 ;; \
	    riscv64) EMU=qemu-system-riscv64 ;; \
	    arm) EMU=qemu-system-arm ;; \
	    *) EMU=qemu-system-x86_64 ;; \
	  esac; \
	  if ! which $$EMU >/dev/null 2>&1; then echo "boot error: $$EMU not installed" | tee -a AGENT.md; exit 1; fi; \
	  if [ "$(CC_TARGET)" = "x86_64" ]; then \
	    $$EMU -nographic -drive format=raw,file=aos.bin $(RUN_OPTS); \
	  else \
	    $$EMU -nographic -kernel aos.bin $(RUN_OPTS); \
	  fi'

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
	       gcc -Wall -Werror -Isubsystems/memory -Iinclude subsystems/memory/memory.c src/memory.c examples/memory_demo.c src/logging.c src/error.c -o build/memory_demo

fs:
	@echo "→ Building fs demo"
	@mkdir -p build
	       gcc -Wall -Werror -Isubsystems/fs -Iinclude subsystems/fs/fs.c examples/fs_demo.c -o build/fs_demo

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
	        gcc -Iinclude src/plugin_loader.c src/plugin_supervisor.c src/wasm_runtime.c \
	            src/logging.c src/error.c examples/plugin_demo.c -ldl -o build/plugin_demo

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

aicell:
	@echo "→ Building aicell demo"
	@mkdir -p build
	        gcc -Iinclude src/aicell.c examples/aicell_daemon.c -o build/aicell_daemon -lrt
	        gcc -Iinclude src/aicell.c examples/aicell_client.c -o build/aicell_client -lrt

modeld:
	@echo "→ Building aos-modeld"
	@mkdir -p build
	        gcc -Iinclude src/aicell.c src/modeld.c -o build/aos-modeld -lrt

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

# Aggregate test target used by CI
test: test-unit test-integration test-fuzz
	@echo '→ All tests completed'

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

test-unit:
	@echo "→ Running unit tests"
	@mkdir -p build/tests
	gcc --coverage -Isubsystems/memory -Iinclude tests/unit/test_memory.c \
	subsystems/memory/memory.c src/logging.c src/error.c -o build/tests/test_memory
	@./build/tests/test_memory

test-integration:
	@echo "\u2192 Running integration tests"
	@mkdir -p build/tests
	gcc --coverage -Isubsystems/fs -Isubsystems/memory -Iinclude \
	tests/integration/test_fs_memory.c \
	subsystems/fs/fs.c subsystems/memory/memory.c src/logging.c src/error.c -o build/tests/test_fs
	@./build/tests/test_fs
	gcc --coverage -Isubsystems/fs -Isubsystems/memory -Iinclude \
	tests/integration/test_persistence.c \
	subsystems/fs/fs.c subsystems/memory/memory.c src/logging.c src/error.c -o build/tests/test_persistence
	@./build/tests/test_persistence

test-fuzz:
	@echo "\u2192 Running memory fuzz tests under ASan"
	@mkdir -p build/tests
	gcc -fsanitize=address -g -Isubsystems/memory -Iinclude \
	tests/unit/test_memory_fuzz.c \
	subsystems/memory/memory.c src/logging.c src/error.c -o build/tests/test_memory_fuzz
	@./build/tests/test_memory_fuzz

# Generate simple coverage report
coverage: test
	@echo "\u2192 Generating coverage report"
	@gcov -o build/tests $(shell find build/tests -name '*.gcno') >/tmp/coverage.txt || true
	@cat /tmp/coverage.txt

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

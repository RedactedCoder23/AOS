.PHONY: all clean test install regenerate host bootloader kernel bare run ui ui-check web-ui branch-vm plugins iso efi branch-net desktop-ui ai-service aicell modeld policy net subsystems checklist

MAKEFLAGS += -j$(shell nproc)

CC_TARGET ?= x86_64

SUBSYSTEM_DIRS := subsystems/memory subsystems/fs subsystems/ai subsystems/branch subsystems/net subsystems/dev subsystems/security
HOST_SRCS := \
src/main.c src/repl.c src/interpreter.c src/branch_manager.c src/ui_graph.c \
src/branch_vm.c src/plugin_loader.c src/plugin_supervisor.c src/wasm_runtime.c \
src/branch_net.c src/ai_syscall.c src/aicell.c src/checkpoint.c src/policy.c \
src/memory.c src/app_runtime.c src/config.c src/logging.c src/error.c \
src/generated/command_map.c src/generated/commands.c \
subsystems/memory/memory.c subsystems/fs/fs.c subsystems/ai/ai.c \
subsystems/branch/branch.c subsystems/net/net.c \
subsystems/dev/dev.c subsystems/security/security.c
HOST_OBJS := $(patsubst %.c, build/obj/%.o, $(HOST_SRCS))
CFLAGS := -Wall -Werror -Wno-format-truncation

all: host

# 1. Regenerate C sources from text masters
regenerate:
	python3 scripts/generate_aos_mappings.py --outdir src/generated
	@$(MAKE) -s checklist

# 2. Build host-side test harness
NCURSES_CFLAGS := $(shell pkg-config --cflags ncurses 2>/dev/null)
NCURSES_LIBS := $(shell pkg-config --libs ncurses 2>/dev/null)
CURL_LIBS := $(shell pkg-config --libs libcurl 2>/dev/null)

check_deps:
	@pkg-config --exists ncurses || (echo "Error: ncurses development files missing" && exit 1)
	@pkg-config --exists libcurl || (echo "Error: libcurl development files missing" && exit 1)

build/obj/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -Iinclude -Isrc/generated $(addprefix -I,$(SUBSYSTEM_DIRS)) $(NCURSES_CFLAGS) -c $< -o $@

host: check_deps regenerate subsystems $(HOST_OBJS) build/obj/src/ui_main.o
	@echo "→ Building host binaries"
	$(CC) -rdynamic $(HOST_OBJS) $(NCURSES_LIBS) $(CURL_LIBS) -ldl -lm -o build/host_test
	$(CC) build/obj/src/ui_graph.o build/obj/src/branch_manager.o \
build/obj/src/logging.o build/obj/src/error.o build/obj/src/ui_main.o \
$(NCURSES_LIBS) -lm -o build/ui_graph
	@mkdir -p build
	gcc -Wall -Werror -rdynamic -Iinclude -Isrc/generated -Isubsystems/memory -Isubsystems/fs -Isubsystems/ai -Isubsystems/branch -Isubsystems/net $(NCURSES_CFLAGS) \
	            src/main.c src/repl.c src/interpreter.c src/branch_manager.c src/ui_graph.c \
	            src/branch_vm.c src/plugin_loader.c src/plugin_supervisor.c src/wasm_runtime.c \
	            src/branch_net.c src/ai_syscall.c src/aicell.c src/checkpoint.c src/policy.c \
	            src/memory.c src/app_runtime.c src/config.c src/logging.c src/error.c \
	            src/generated/command_map.c src/generated/commands.c \
	            subsystems/memory/memory.c subsystems/fs/fs.c subsystems/ai/ai.c \
	            subsystems/branch/branch.c subsystems/net/net.c \
	            $(NCURSES_LIBS) -ldl -lcurl -lm -o build/host_test
	gcc -Wall -Werror -Iinclude -Isrc/generated $(NCURSES_CFLAGS) src/ui_graph.c src/branch_manager.c \
	     src/logging.c src/error.c src/ui_main.c $(NCURSES_LIBS) -lm -o build/ui_graph

# 3. Build bare-metal components
bootloader: regenerate
	@echo "→ Building bootloader"
	$(MAKE) -C bare_metal_os bootloader.bin ARCH=$(CC_TARGET)
	cp bare_metal_os/bootloader.bin bootloader.bin

kernel: regenerate
	@echo "→ Building kernel"
	$(MAKE) -C bare_metal_os kernel.bin ARCH=$(CC_TARGET)
	cp bare_metal_os/kernel.bin kernel.bin

bare: bootloader kernel
	@echo "→ Creating aos.bin"
	cat bootloader.bin kernel.bin > aos.bin

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
	@rm -rf build # remove compiled objects and tests
	@rm -f aos.bin bootloader.bin kernel.bin aos.iso # remove OS binaries
	@rm -f AOS-CHECKLIST.log /tmp/coverage.txt # remove logs created by tests
	@rm -f bare_metal_os/*.bin bare_metal_os/*.o bare_metal_os/*.elf bare_metal_os/kernel.elf # bare metal artifacts

memory:
	@echo "→ Building memory demo"
	@mkdir -p build
	       gcc -Wall -Werror -Isubsystems/memory -Iinclude -Isrc/generated subsystems/memory/memory.c src/memory.c examples/memory_demo.c src/logging.c src/error.c -o build/memory_demo

fs:
	@echo "→ Building fs demo"
	@mkdir -p build
	       gcc -Wall -Werror -Isubsystems/fs -Iinclude -Isrc/generated subsystems/fs/fs.c examples/fs_demo.c -o build/fs_demo

ai:
	@echo "→ Building ai demo"
	@mkdir -p build
	gcc -Isubsystems/ai -Iinclude -Isrc/generated subsystems/ai/ai.c src/ai_syscall.c examples/ai_demo.c -lcurl -o build/ai_demo

branch:
	@echo "→ Building branch demo"
	@mkdir -p build
	gcc -Isubsystems/branch subsystems/branch/branch.c examples/branch_demo.c -o build/branch_demo

branch-vm:
	@echo "→ Building branch VM demo"
	@mkdir -p build
	gcc -Iinclude -Isrc/generated src/branch_vm.c subsystems/branch/branch.c examples/branch_vm_demo.c -o build/branch_vm_demo

plugins:
	        @echo "→ Building plugins demo"
	        @mkdir -p build/plugins
	        gcc -fPIC -shared -o build/plugins/sample.so examples/sample_plugin.c
	        gcc -Iinclude -Isrc/generated src/plugin_loader.c src/plugin_supervisor.c src/wasm_runtime.c \
	            src/logging.c src/error.c examples/plugin_demo.c -ldl -o build/plugin_demo

aos-cli:
	@echo "→ Building aos CLI"
	@mkdir -p build
	gcc -Isubsystems/branch subsystems/branch/branch.c examples/aos_cli.c -o build/aos

branch-net:
	@echo "→ Building branch net demo"
	@mkdir -p build
	@gcc -Iinclude -Isrc/generated \
	        src/branch_net.c src/branch_manager.c \
	        src/logging.c src/error.c \
	        examples/branch_fed_demo.c \
	        -pthread -lm -o build/branch_fed_demo

ai-service:
	@echo "→ Building ai service demo"
	@mkdir -p build
	        gcc -Iinclude -Isrc/generated -Isubsystems/ai src/ai_syscall.c examples/ai_service_demo.c subsystems/ai/ai.c -o build/ai_service_demo -lcurl

aicell:
	@echo "→ Building aicell demo"
	@mkdir -p build
	        gcc -Iinclude -Isrc/generated src/aicell.c examples/aicell_daemon.c -o build/aicell_daemon -lrt
	        gcc -Iinclude -Isrc/generated src/aicell.c examples/aicell_client.c -o build/aicell_client -lrt

modeld:
	@echo "→ Building aos-modeld"
	@mkdir -p build
	        gcc -Iinclude -Isrc/generated src/aicell.c src/modeld.c -o build/aos-modeld -lrt

policy:
	@echo "→ Building policy demo"
	@mkdir -p build
	gcc -Iinclude -Isrc/generated src/policy.c examples/policy_demo.c -o build/policy_demo

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
test: test-unit test-integration
	@echo "→ Running full test suite"

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
@mkdir -p build/tests build/plugins
gcc --coverage -Isubsystems/memory -Iinclude \
tests/unit/test_memory.c \
subsystems/memory/memory.c src/logging.c src/error.c \
-o build/tests/test_memory
@./build/tests/test_memory
gcc --coverage -Isubsystems/branch -Iinclude \
tests/c/test_branch.c \
subsystems/branch/branch.c src/logging.c src/error.c \
-o build/tests/test_branch
@./build/tests/test_branch
gcc --coverage -Isubsystems/net -Iinclude \
tests/c/test_net.c \
subsystems/net/net.c src/logging.c src/error.c \
-o build/tests/test_net
@./build/tests/test_net
gcc -fPIC -shared -o build/plugins/sample.so examples/sample_plugin.c
gcc --coverage -Iinclude \
tests/c/test_plugin.c src/plugin_loader.c src/plugin_supervisor.c src/wasm_runtime.c src/logging.c src/error.c -ldl \
-o build/tests/test_plugin
@./build/tests/test_plugin
gcc --coverage -Iinclude \
tests/c/test_policy.c src/policy.c src/logging.c src/error.c \
-o build/tests/test_policy
@./build/tests/test_policy
@python3 -m pytest -q tests/python
	gcc --coverage -Isubsystems/branch -Iinclude \
	tests/c/test_branch.c \
	subsystems/branch/branch.c src/logging.c src/error.c \
	-o build/tests/test_branch
	@./build/tests/test_branch
	gcc --coverage -Isubsystems/net -Iinclude \
	tests/c/test_net.c \
	subsystems/net/net.c src/logging.c src/error.c \
	-o build/tests/test_net
	@./build/tests/test_net
	gcc -fPIC -shared -o build/plugins/sample.so examples/sample_plugin.c
	gcc --coverage -Iinclude \
	tests/c/test_plugin.c src/plugin_loader.c src/plugin_supervisor.c src/wasm_runtime.c src/logging.c src/error.c -ldl \
	-o build/tests/test_plugin
	@./build/tests/test_plugin
	gcc --coverage -Iinclude \
	tests/c/test_policy.c src/policy.c src/logging.c src/error.c \
	-o build/tests/test_policy
	@./build/tests/test_policy
	@python3 -m pytest -q tests/python
		
test-integration:
	@echo "\u2192 Running integration tests"
	@mkdir -p build/tests
		gcc --coverage -Isubsystems/fs -Isubsystems/memory -Iinclude \
		tests/integration/test_fs_memory.c \
		subsystems/fs/fs.c subsystems/memory/memory.c src/logging.c src/error.c \
	-o build/tests/test_fs
	@./build/tests/test_fs
	gcc --coverage -Isubsystems/fs -Isubsystems/memory -Iinclude \
	tests/integration/test_persistence.c \
	subsystems/fs/fs.c subsystems/memory/memory.c src/logging.c src/error.c \
	-o build/tests/test_persistence
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

install: host
	@echo "→ Installing host binary to /usr/local/bin (requires sudo)"
	install -m755 build/host_test /usr/local/bin/aos-host
checklist:
	@if [ -s AOS-CHECKLIST.log ]; then \
	echo "Checklist has entries:"; cat AOS-CHECKLIST.log; exit 1; \
	else \
	echo "Checklist clean"; \
	fi

generate: regenerate

.PHONY: compdb
compdb: regenerate
	compiledb -n make host

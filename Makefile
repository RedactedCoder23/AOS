.PHONY: all clean test install regenerate host bootloader kernel bare run ui ui-check web-ui branch-vm plugins iso efi branch-net desktop-ui ai-service aicell modeld ipc-host branch-dashboard policy net subsystems checklist verify-all

MAKEFLAGS += -j$(shell nproc)
VERSION := 0.3.0

CC_TARGET ?= x86_64

SUBSYSTEM_DIRS := subsystems/memory subsystems/fs subsystems/ai subsystems/net subsystems/dev subsystems/security
HOST_SRCS := \
src/main.c src/repl.c src/interpreter.c src/branch_manager.c src/ui_graph.c \
src/branch_vm.c src/plugin_loader.c src/plugin_supervisor.c src/wasm_runtime.c \
src/lang_vm.c \
src/branch_net.c src/ai_syscall.c src/aicell.c src/checkpoint.c src/policy.c \
src/memory.c src/app_runtime.c src/config.c src/logging.c src/audit.c src/error.c \
src/branch_syscalls.c \
src/generated/command_map.c src/generated/commands.c \
subsystems/memory/memory.c subsystems/fs/fs.c subsystems/ai/ai.c \
subsystems/net/net.c \
subsystems/dev/dev.c subsystems/security/security.c
HOST_OBJS := $(patsubst %.c, build/obj/%.o, $(HOST_SRCS))
CFLAGS := -Wall -Werror -Wno-format-truncation

all: host

build: all

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
# 3. Build bare-metal components
bootloader: regenerate
	@echo "→ Building bootloader"
	$(MAKE) -C bare_metal_os bootloader.bin ARCH=$(CC_TARGET)
	cp bare_metal_os/bootloader.bin bootloader.bin

kernel: regenerate
	@echo "→ Building kernel"
	$(MAKE) -C bare_metal_os stage2.bin kernel.bin ARCH=$(CC_TARGET)
	cp bare_metal_os/kernel.bin kernel.bin
	cp bare_metal_os/stage2.bin stage2.bin

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
	gcc -Wall -Werror -Isubsystems/fs -Isubsystems/memory -Iinclude -Isrc/generated \
	subsystems/fs/fs.c subsystems/memory/memory.c src/memory.c \
	src/logging.c src/error.c examples/fs_demo.c \
-o build/fs_demo

ai:
	@echo "→ Building ai demo"
	@mkdir -p build
	gcc -Isubsystems/ai -Iinclude -Isrc/generated subsystems/ai/ai.c src/ai_syscall.c examples/ai_demo.c -lcurl -o build/ai_demo


branch-vm:
	@echo "→ Building branch VM demo"
	@mkdir -p build
	gcc -Iinclude -Isrc/generated src/branch_vm.c examples/branch_vm_demo.c -o build/branch_vm_demo

plugins:
	        @echo "→ Building plugins demo"
	        @mkdir -p build/plugins
	        gcc -fPIC -shared -o build/plugins/sample.so examples/sample_plugin.c
	        gcc -Iinclude -Isrc/generated src/plugin_loader.c src/plugin_supervisor.c src/wasm_runtime.c \
	            src/logging.c src/error.c examples/plugin_demo.c -ldl -o build/plugin_demo


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

ipc-host:
	@echo "→ Building ipc host daemon"
	@mkdir -p build
	gcc -Iinclude -Isrc/generated src/ipc_host.c src/branch_manager.c \
	src/logging.c src/error.c -o build/ipc_host
branch-dashboard:
	@echo "→ Building branch dashboard"
	@mkdir -p build
	gcc -Iinclude -Isrc/generated ui/branch_dashboard.c src/logging.c src/error.c \
-lncurses -o build/branch_dashboard

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

# 4. Verification and tests
# Aggregate test target used by CI
test: test-unit test-integration
	@echo "→ Running full test suite"

test-memory: memory
	./examples/memory_demo.sh

test-fs: fs
	./examples/fs_smoke.sh

	
test-plugin: plugins
	./examples/plugin_smoke.sh

test-policy: policy
	./examples/policy_smoke.sh

test-net: net
	./examples/net_echo_test.sh
test-merge-ai:
	pytest -q tests/python/test_merge_ai_split.py

test-lifecycle:
	pytest -q scripts/tests/test_branch_lifecycle.py

test-negative:
	pytest -q tests/python/test_negative_paths.py

test-all: test-unit test-integration test-merge-ai test-lifecycle test-negative

# Run full verification script
verify-all:
	./verify_all.sh
test-unit:
	@echo "→ Running unit tests"
	@mkdir -p build/tests build/plugins
	gcc -Isubsystems/memory -Iinclude \
	tests/unit/test_memory.c \
	subsystems/memory/memory.c src/logging.c src/error.c \
	-o build/tests/test_memory
	@./build/tests/test_memory
	gcc -Isubsystems/net -Iinclude \
	tests/c/test_net.c \
	subsystems/net/net.c src/logging.c src/error.c \
	-o build/tests/test_net
	@./build/tests/test_net
	gcc -fPIC -shared -o build/plugins/sample.so examples/sample_plugin.c
	gcc -Iinclude -Isubsystems/security -Isubsystems/dev \
tests/c/test_plugin.c src/plugin_loader.c src/plugin_supervisor.c src/wasm_runtime.c subsystems/security/security.c src/logging.c src/error.c -ldl \
	-o build/tests/test_plugin
	@./build/tests/test_plugin
	gcc -Iinclude \
	tests/c/test_policy.c src/policy.c src/audit.c src/logging.c src/error.c \
	-o build/tests/test_policy
	@./build/tests/test_policy
	gcc -Isubsystems/dev -Iinclude \
	tests/c/test_dev.c subsystems/dev/dev.c src/logging.c src/error.c \
	-o build/tests/test_dev
	@./build/tests/test_dev
	gcc -Isubsystems/security -Iinclude \
	tests/c/test_security.c subsystems/security/security.c src/logging.c src/error.c \
	-o build/tests/test_security
	@./build/tests/test_security
	gcc -Isubsystems/fs -Isubsystems/memory -Iinclude \
tests/c/test_fs.c subsystems/fs/fs.c subsystems/memory/memory.c \
src/memory.c src/logging.c src/error.c \
	-o build/tests/test_fs_unit
	@./build/tests/test_fs_unit
	gcc -Isubsystems/ai -Iinclude \
	tests/c/test_ai.c subsystems/ai/ai.c src/ai_syscall.c src/logging.c src/error.c -lcurl \
	-o build/tests/test_ai
	@./build/tests/test_ai
	gcc -Iinclude -pthread \
                tests/ai_test.c src/syscall.c src/ipc_host.c \
                src/branch_manager.c \
                src/logging.c src/error.c -DIPC_HOST_LIBRARY \
                -o build/tests/ai_test
			@./build/tests/ai_test
	gcc -Iinclude -Isubsystems/security \
	tests/c/test_wasm_runtime.c src/wasm_runtime.c subsystems/security/security.c src/logging.c src/error.c \
	-o build/tests/test_wasm_runtime
	@./build/tests/test_wasm_runtime
	gcc -Isubsystems/memory -Iinclude \
	tests/memory_test.c subsystems/memory/memory.c src/logging.c src/error.c \
	-o build/tests/test_memory_paging
	@./build/tests/test_memory_paging
	gcc -Iinclude \
	tests/c/test_ui.c src/logging.c src/error.c -lncurses \
	-o build/tests/test_ui
	@./build/tests/test_ui
	gcc -Iinclude -Isubsystems/ai  \
	tests/lang_test.c src/lang_vm.c src/branch_manager.c  \
	subsystems/ai/ai.c src/ai_syscall.c src/logging.c src/error.c -lcurl  \
	-o build/tests/test_lang
	@./build/tests/test_lang
	@pip install -r requirements.txt
	@python3 -m pytest --cov=./ -q tests/python
test-integration:
	@echo "\u2192 Running integration tests"
	@pip install -r requirements.txt
	@mkdir -p build/tests
	gcc -Isubsystems/fs -Isubsystems/memory -Iinclude tests/integration/test_fs_memory.c \
	subsystems/fs/fs.c subsystems/memory/memory.c src/memory.c src/logging.c src/error.c -o build/tests/test_fs
	@./build/tests/test_fs
	gcc -Isubsystems/fs -Isubsystems/memory -Iinclude tests/integration/test_persistence.c \
	subsystems/fs/fs.c subsystems/memory/memory.c src/memory.c src/logging.c src/error.c -o build/tests/test_persistence
	@./build/tests/test_persistence
	        @[ -f aos.bin ] && ./scripts/qemu_smoke.sh $(CC_TARGET) || echo "aos.bin missing, skipping qemu"
	
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

demo-test:
	bash demo/demo_test.sh

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

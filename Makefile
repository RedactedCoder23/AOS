# Top-level Makefile

# toolchain & flags
CC      := gcc
CFLAGS  := -Wall -Wextra -O2 -Iinclude `pkg-config --cflags ncurses || echo ""`
LDFLAGS := `pkg-config --libs ncurses || echo "-lncurses"`

# sources
KERNEL_SRCS := $(wildcard src/*.c subsystems/*/*.c)
HOST_SRCS   := $(wildcard host/*.c)
GEN_SRCS    := gen/command_map.c gen/commands.c

# object files
KERNEL_OBJS := $(KERNEL_SRCS:.c=.o)
HOST_OBJS   := $(HOST_SRCS:.c=.o)
GEN_OBJS    := $(GEN_SRCS:.c=.o)

.PHONY: all host kernel bare test install clean

all: host bare

host: $(HOST_OBJS) $(GEN_OBJS)
	$(CC) $(CFLAGS) $^ -o bin/aos-host $(LDFLAGS)

bare: $(KERNEL_OBJS) $(GEN_OBJS)
	$(CC) $(CFLAGS) -ffreestanding -nostdlib $^ -o bin/aos-bare.elf

%/%.o: %/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# auto-generate code
gen/command_map.c gen/commands.c: mappings.json scripts/generate_aos_mappings.py
	python3 scripts/generate_aos_mappings.py mappings.json gen/

test: all
	@echo "→ Running unit tests…"
	@$(MAKE) -C tests/unit run
	@echo "→ Running integration tests…"
	@$(MAKE) -C tests/integration run

install: all
	install -d $(DESTDIR)/usr/local/bin
	install bin/aos-host $(DESTDIR)/usr/local/bin/aos-host
	install bin/aos-bare.elf $(DESTDIR)/usr/local/bin/aos-bare

clean:
	rm -rf bin/*.o src/*.o host/*.o gen/*.c gen/*.o

MAKEFLAGS += -j$(shell nproc)

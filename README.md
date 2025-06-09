# AOS

Minimal experimental OS used for interpreter tests.

## Build

```bash
make host   # build host-side REPL
make bare   # build aos.bin for QEMU
```

AOS uses BIOS INT 13h to load the 1 MiB kernel image.

## Running AOS in QEMU

**Prerequisite**: install QEMU—e.g., on Debian/Ubuntu:

```bash
sudo apt install qemu-system-x86
```

First build both targets:

```bash
make host
make bare
```

Then launch AOS:

```bash
make run
```

This invokes QEMU (preferring `qemu-system-x86_64`) and attaches the serial console to your terminal. If no QEMU binary is found, the command prints an error message.

## Memory Subsystem Demo

Run the memory allocator demo:

```bash
./examples/memory_demo.sh
```

## Filesystem Demo

```bash
./examples/fs_demo.sh
```

## AI Demo

```bash
./examples/ai_demo.sh
```

## Branch Manager Demo

```bash
./examples/branch_demo.sh
```

## Branch Graph UI

Build and run:
```bash
make ui
./build/ui_graph
```

Controls:

* Arrows / h,j,k,l: move selection
* Enter: switch branch
* n: create new branch
* c: connect to another branch
* q: quit

Example script:
```bash
./examples/ui_graph_demo.sh
```

Branch state is saved to `~/.aos/branches.bin`, so any branches you create will
persist between sessions.

## Checklist Log

The build process writes warnings and errors to `AOS-CHECKLIST.log`.
Most `make` targets will fail if this log is non-empty.
Check the file whenever a build fails:

```bash
cat AOS-CHECKLIST.log
```

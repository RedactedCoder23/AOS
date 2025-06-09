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

Run the memory allocator demo which showcases the free-list allocator:

```bash
make memory
./build/memory_demo
```
The demo allocates and frees several blocks and prints their addresses.

## Filesystem Demo

```bash
make fs
./build/fs_demo
```
Demonstrates a tiny in-memory filesystem with open/read/write/close commands.

## AI Demo

```bash
make ai
./build/ai_demo
```
Uses libcurl to fetch a response from a demo service.

## Branch Manager Demo

```bash
make branch
./build/branch_demo
```
Creates a few branches, stops one, then deletes it, showing persistence via `~/.aos/branches.json`.

## Sample REPL Session

```
AOS> MEM_ALLOC 128
Allocated at 0x1f0a010
AOS> FS_LS
(no files)
AOS> AI_PROMPT Hello
Hi, I'm an AI stub...
AOS> BR_LIST
0:main (running)
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

Branch state is saved to `~/.aos/branches.json`, so any branches you create will
persist between sessions.

## Checklist Log

The build process writes warnings and errors to `AOS-CHECKLIST.log`.
Most `make` targets will fail if this log is non-empty.
Check the file whenever a build fails:

```bash
cat AOS-CHECKLIST.log
```

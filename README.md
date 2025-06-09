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
Demonstrates a tiny in-memory filesystem with mkdir/open/read/write/close and listing commands.

## AI Demo

```bash
make ai
./build/ai_demo
```
Uses libcurl to fetch a response from a demo service.

The interactive REPL also supports an `ai` command which now
executes `scripts/ai_backend.py`. Set `OPENAI_API_KEY` before running
`make host` to enable real responses.

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
AOS> ai Hello
Hello! (from LLM)
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

## Hypervisor-Backed Branches

```bash
make branch-vm
./examples/branch_vm_demo.sh
```

Creates a stub VM and lists it.

## Plugin Marketplace

```bash
make plugins
./examples/plugin_demo.sh
```

Loads and then unloads a sample plugin built as a shared object.

## Real Hardware Boot

```bash
./examples/iso_demo.sh
```

Generates a dummy `aos.iso` file.

## Distributed Branch Federation

```bash
make branch-net
./examples/branch_fed_demo.sh
```

Demonstrates syncing with a peer.

## AI-driven Service Manager

```bash
make ai-service
./examples/ai_service_demo.sh
```

Spawns a sample AI service and checks its status.

## Secure Enclaves & Policy Engine

```bash
make policy
./examples/policy_demo.sh
```

Loads a policy script and performs a check.

## Checklist Log

The build process writes warnings and errors to `AOS-CHECKLIST.log`.
Most `make` targets will fail if this log is non-empty.
Check the file whenever a build fails:

```bash
cat AOS-CHECKLIST.log
```

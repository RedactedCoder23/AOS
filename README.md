# AOS

Minimal experimental OS used for interpreter tests.

## Quickstart

```bash
git clone https://example.com/AOS.git
cd AOS
make host
./build/host_test
```

## Setup

Install runtime and development Python dependencies:

```bash
pip install -r requirements.txt
pip install -r requirements-dev.txt
```

This compiles the host REPL and launches an interactive session.

## Architecture Overview

AOS is split into a small kernel and a set of host tools. The kernel boots via
GRUB and initialises a handful of subsystems (memory, filesystem and branch
manager). The host REPL links against the same subsystems to provide a
lightweight command line environment. Each subsystem resides under
`subsystems/` and exposes a header-only API.

For deeper details see [docs/system_architecture.md](docs/system_architecture.md).
An overview of command dispatch can be found in
[docs/command_flow.md](docs/command_flow.md).

## What's inside?

- `bare_metal_os/` – kernel sources and boot files
- `src/` – shared utilities and host code
- `subsystems/` – memory, fs, branch and other core modules
- `include/` – public headers used across the project
- `apps_src/` – sample applications
- `docs/` – project documentation

## Build

```bash
make host         # build host-side REPL
make bootloader   # build bootloader
make kernel       # build kernel
make bare         # create aos.bin for QEMU
```

AOS uses BIOS INT 13h to load the 1 MiB kernel image.

## Running AOS in QEMU

**Prerequisite**: install QEMU—e.g., on Debian/Ubuntu:

```bash
sudo apt install qemu-system-x86
```

First build the components:

```bash
make host
make bootloader
make kernel
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
Uses OpenAI ChatCompletion via a helper script.

The interactive REPL also supports an `ai` command. Run `ai setup`
once to configure your provider and store the API key securely in
`~/.aos/ai.conf`. Environment variables still override the stored
value when present.

## Branch Manager Demo

```bash
make branch
./build/branch_demo
```
Creates a few branches, stops one, then deletes it, showing persistence via `~/.aos/branches.json`.

## Checkpoint HAL Demo

```bash
make aicell
./examples/aicell_demo.sh
```
Demonstrates the new aicell daemon and checkpoint API.

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

## Web Branch UI

Launch a lightweight web visualizer:

```bash
python3 scripts/branch_ui.py
```

Then open `http://localhost:8000` in your browser. Drag nodes to rearrange the
graph and double-click a branch to open a new tab pane. Right-click a tab to
close it. The demo reads from `examples/graph_sample.json` and serves files from
the new `ui/` directory. A `/metrics` endpoint exposes runtime metrics for the
dashboard, while `/export` and `/import` allow workspace sync via JSON.
## Graphical Desktop & AI Copilot

Launch the desktop UI with live branch data and AI chat:

```bash
make desktop-ui
```

Then open `http://localhost:8000` to view the graph and chat with the embedded copilot.


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

## Networking

```bash
make net
./build/net_echo --server --port 12345 &
./build/net_echo --host 127.0.0.1 --port 12345
make net-http
./examples/net_http_demo.sh
```

Demonstrates TCP echo and a simple HTTP server.

## Updating

Run `scripts/aos_updater.sh` to fetch the latest changes and regenerate
`CHANGELOG.md` automatically.

## Troubleshooting

If a build fails, check `AOS-CHECKLIST.log` for recorded errors. When running
`make run` ensure QEMU is installed and accessible in your `PATH`. Missing
dependencies for optional features such as the AI backend will result in
warning messages but do not stop the REPL from launching.

## Checklist Log

The build process writes warnings and errors to `AOS-CHECKLIST.log`.
Most `make` targets will fail if this log is non-empty.
Check the file whenever a build fails:

```bash
cat AOS-CHECKLIST.log
```

## Contributing
See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.
See [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md) for community standards.
Additional developer documentation is available under the [docs/](docs/) directory.
See [docs/profiler_usage.md](docs/profiler_usage.md) for the profiler stub.
See [AGENT.md](AGENT.md) and [PATCHLOG.md](PATCHLOG.md) for development logs. The roadmap is maintained in [ROADMAP.md](ROADMAP.md).

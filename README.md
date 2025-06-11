[![Coverage](https://codecov.io/gh/RedactedCoder23/AOS/branch/main/graph/badge.svg)](https://codecov.io/gh/RedactedCoder23/AOS)
# AOS

[![Build Status](https://img.shields.io/github/actions/workflow/status/RedactedCoder23/AOS/ci.yml?branch=main)](https://github.com/RedactedCoder23/AOS/actions)
[![License](https://img.shields.io/github/license/RedactedCoder23/AOS)](LICENSE)

Current Version: **v0.3.0**

Minimal experimental OS used for interpreter tests. The latest release adds a
task orchestrator with plugin-based AI providers, coverage tracking and live UI
previews.

## v0.3.0 Preview

Milestone&nbsp;1 introduces:
- persistent branches with federation support
- plugin hot-swap with basic sandboxing
- AI provider integration and web branch UI
- a `verify_all.sh` script for one-shot build and tests

## Quickstart

```bash
git clone https://github.com/RedactedCoder23/AOS.git
cd AOS
./verify_all.sh

# Or run the demo container
docker-compose -f demo/docker-compose.yml up --build
```

## Setup

Run the bootstrap script to install all build dependencies:

```bash
./scripts/bootstrap.sh
```

Then build the host REPL with `make host` and launch an interactive session.

## Prerequisites

- GCC **10** or newer ([install](https://gcc.gnu.org/))
- Python **3.8** or newer ([install](https://www.python.org/downloads/))
- QEMU **6.0** or newer for bare metal tests ([install](https://www.qemu.org/download/))
- Node.js **18** or newer ([install](https://nodejs.org/en/download/))
- `make`, `pkg-config`, `libcurl-dev`, `libncurses-dev`

### Prerequisites for `make bare`

- GNU make, gcc, binutils
- nasm (for assembling stage2.S)
- grub-mkrescue, xorriso (to build ISOs)
- qemu-system-x86_64 (for testing in QEMU)

Install Python requirements and pre-commit hooks:

```bash
pip install -r requirements.txt
pre-commit install
```

Pinned versions live in `requirements*.txt`. To update them:

```bash
pip install -r requirements.txt -U
pip freeze | grep -E 'openai|cbor2|pytest-cov|cryptography|keyring|flask|PyYAML|psutil|fastapi' > requirements.txt
pip freeze | grep -E 'black|flake8|pytest|pre-commit|pre-commit-hooks|playwright' > requirements-dev.txt
```

See [ARCHITECTURE.md](ARCHITECTURE.md) for a detailed overview of the system design.

## Build

```bash
make host         # build host-side REPL
make bootloader   # build bootloader
make kernel       # build kernel
make bare         # create aos.bin for QEMU
```

AOS uses BIOS INT 13h to load the 1 MiB kernel image.

## Testing

Run the linters and both test suites before sending patches:

```bash
pre-commit run --all-files
make test-unit
make test-integration
```

Generate `compile_commands.json` for clang-tidy with:

```bash
make compdb
```

## CLI Options

The helper `agent_runner.py` accepts a `--provider` flag to override the AI
provider used by `merge_ai.py`.

### Usage

```bash
python3 scripts/agent_runner.py --provider echo "<cmd>" hb.json result.json
```

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

Set `AOS_AI_OFFLINE=1` to use the bundled mock backend instead of contacting the
OpenAI service.

The interactive REPL also supports an `ai` command. Run `ai setup`
once to configure your provider and store the API key securely in
`~/.aos/ai.conf`. Environment variables still override the stored
value when present.

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
dashboard, while `/export` and `/import` allow workspace sync via JSON. Each
branch panel now shows coverage trends and task metrics.
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

## Releases

Stable snapshots are tagged in git. Check the
[releases page](https://github.com/RedactedCoder23/AOS/releases)
for downloadable archives and changelogs.

## Contributing
See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.
See [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md) for community standards.
Additional developer documentation is available under the [docs/](docs/) directory.
See [docs/REMAINING.md](docs/REMAINING.md) for tasks pending implementation.
See [docs/profiler_usage.md](docs/profiler_usage.md) for the profiler stub.
See [AGENT.md](AGENT.md) and [PATCHLOG.md](PATCHLOG.md) for development logs. The roadmap is maintained in [ROADMAP.md](ROADMAP.md).

## License

AOS is released under the [MIT License](LICENSE). This permits reuse, modification and distribution
as long as the license notice is included. The project is provided **as is** without warranty.


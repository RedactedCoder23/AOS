# AOS Roadmap

## Short-term
- Improve test coverage across subsystems.
- Flesh out device driver and security subsystems.
- Stabilise logging and error handling APIs.

## Long-term
- Full networking stack with TLS.
- Graphical desktop enhancements.
- Package manager and plugin marketplace.

Major milestones are tracked by version tags in git.

| Phase | Description | Status |
|-------|-------------|-------|
| 0 | Repo inventory, docs, coverage baseline | ✔ |
| 1 | Kernel–host boundary and IPC | ⚙ |
| 2 | Memory management and snapshotting | ⚪ |
| 3 | Branch & process table | ⚪ |
| 4 | Filesystem & devices | ⚪ |
| 5 | Security & policy engine | ⚪ |
| 6 | REPL and scripting | ⚪ |
| 7 | Plugin system & WASM | ⚪ |
| 8 | AI integration | ⚪ |
| 9 | UI (web/native/cli) | ⚪ |
| 10 | Federation & distributed | ⚪ |

## Current Sprint Goals
- Extend ext2 tests for persistence.
- Hook the security subsystem into the WASM runtime for capability checks.
- Integrate checkpoint delta handling in branch management.
- Implement IPC skeleton:
  - `include/ipc.h` with ring buffer structure.
  - `syscall_dispatch` stub in kernel.
  - `ipc_host.c` daemon logging syscalls.
- Add test stubs for branch, fs, ai, dev, plugin and wasm runtime.
- Update CI to collect coverage and publish badge.
- [2025-06-10] Phase 1: IPC protocol, mapping, and tests underway.

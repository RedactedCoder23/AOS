# System Architecture

AOS is composed of modular subsystems that interact through simple C APIs.
This document provides an overview of the current design.

## Components
- **Kernel**: boots via GRUB and initialises memory, filesystem and branch manager.
- **Host Tools**: a REPL allowing branch management, filesystem access and AI commands.
- **Subsystems**:
  - `memory` – simple free list allocator.
  - `fs` – in-memory and persistent backends.
  - `branch` – hierarchical branch graph with network sync.
  - `net` – minimal TCP/UDP helpers.

## Data Flow
1. The host REPL issues commands.
2. Commands invoke subsystem APIs.
3. Subsystems log operations to `AOS-CHECKLIST.log`.
4. Persistent components write data under the user's home directory.

```
Kernel -> Subsystems -> Host REPL
                   \-> Plugins
```

Diagrams describing the kernel boot sequence, subsystem interactions and data
flow are referenced here but omitted from the repository. See the project wiki
for sketches and further design notes.

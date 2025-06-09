# Agent Log

## Context
Initial repository had no AGENT log. PATCHLOG.md notes prior work on branch CLI and adjacency matrix for bare metal branch graph. This session implements new persistence, REPL, graph, and AI stub features.

## Decisions
- Using `src/branch_manager.c` for persistent branch storage with JSON file `~/.aos/branches.json`.
- Extending `Branch` with `parent` field and maintaining adjacency matrix `adj` as global branch graph.
- New REPL commands will be parsed in `src/main.c`.
- AI syscall stub implemented in `src/ai_syscall.c` with buffer-based API.

## Unresolved



### Step 1
- Added branch_save_all() and branch_load_all() wrappers in branch_manager.c
- Declared APIs in branch.h

### Step 2
- Replaced main.c with interactive REPL supporting branch create/clone/list/tree/save/load. Errors append to AGENT.md.

### Step 3
- Replaced branch array with BranchGraph storing adjacency matrix and parent field. Added bm_graph and updated delete/clone/save/load. CLI tree command now shows hierarchy.

### Step 4
- Added buffer-based ai_infer stub and integrated `ai` command in REPL.

### Session 2
- Fixed branch graph orientation bug. Clone now links parent->child for tree output.
- Updated load_state to rebuild symmetric adjacency matrix.
- Verified `branch tree` prints hierarchy correctly.
 codex/implement-udp-based-peer-discovery-and-branch-sync
RESOLVED: unknown command handled in REPL
=======
 main
## [2025-06-09 06:06 UTC] — meta sweep: repo agent-memory update
by: codex
- Introduced time-stamped log format and baton-pass comments across core subsystems.
- Added header comments noting open issues for memory, fs, ai, branches, plugins, and REPL.
- No functional changes.

Next agent must:
- Follow the same log and comment style for all future updates.
 codex/implement-udp-based-peer-discovery-and-branch-sync
- Refine peer discovery protocol and implement graceful shutdown of network thread.

### Step 5 — [2025-06-09 06:17 UTC] by codex
- Implemented UDP peer discovery and sync in `branch_net.c` with `br_start_service`, `br_discover`, and `br_sync_peer` APIs.
- Added REPL commands `peer ls` and `branch sync <peer>`.
- Network server replies to `DISCOVER` and `PING` messages; simple broadcast used for discovery.
- Removed error log for unknown commands, resolving previous UNRESOLVED entry.
- Limitations: no authentication, blocking server thread, and fixed port 9999.

=======
 codex/implement-plugin-loader-and-hot-swap-framework
- Review plugin sandboxing and security.

## [2025-06-09 06:18 UTC] — plugin hot-swap MVP
by: codex
- Implemented plugin_unload and extended plugin interface with init/exec/cleanup.
- Added REPL `plugin` commands supporting load/unload/list.
- Updated sample plugin and demo to exercise unload logic.
- Resolved UNRESOLVED entry by recognizing plugin commands in REPL.
- Open issue: plugin path validation and sandboxing remain.
=======
- Resolve the earlier UNRESOLVED entry about "unknown command" in REPL.
## [2025-06-09 06:16 UTC] — ai backend hook [agent-mem]
by: codex
- Added Python script `scripts/ai_backend.py` using OpenAI API.
- ai_syscall.c now executes the script for `ai` REPL command.
- Requires `OPENAI_API_KEY` and network access.
- Limited quoting, prompts with quotes may fail.
main
 main

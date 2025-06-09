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
## [2025-06-09 06:06 UTC] — meta sweep: repo agent-memory update
by: codex
- Introduced time-stamped log format and baton-pass comments across core subsystems.
- Added header comments noting open issues for memory, fs, ai, branches, plugins, and REPL.
- No functional changes.
### Step 5 — [2025-06-09 06:17 UTC] by codex
- Implemented UDP peer discovery and sync in `branch_net.c` with `br_start_service`, `br_discover`, and `br_sync_peer` APIs.
- Added REPL commands `peer ls` and `branch sync <peer>`.
- Network server replies to `DISCOVER` and `PING` messages; simple broadcast used for discovery.
- Removed error log for unknown commands, resolving previous UNRESOLVED entry.
- Limitations: no authentication, blocking server thread, and fixed port 9999.

- Review plugin sandboxing and security.

## [2025-06-09 06:18 UTC] — plugin hot-swap MVP
by: codex
- Implemented plugin_unload and extended plugin interface with init/exec/cleanup.
- Added REPL `plugin` commands supporting load/unload/list.
- Updated sample plugin and demo to exercise unload logic.
- Resolved UNRESOLVED entry by recognizing plugin commands in REPL.
- Open issue: plugin path validation and sandboxing remain.
- Resolve the earlier UNRESOLVED entry about "unknown command" in REPL.
## [2025-06-09 06:16 UTC] — ai backend hook [agent-mem]
by: codex
- Added Python script `scripts/ai_backend.py` using OpenAI API.
- ai_syscall.c now executes the script for `ai` REPL command.
- Requires `OPENAI_API_KEY` and network access.
- Limited quoting, prompts with quotes may fail.

## [2025-06-09 06:31 UTC] — post-merge integration sweep [agent-mem]
by: codex
- Cleaned merge artifacts in meta logs.
- Standardized baton-pass comments across subsystems.
- Smoke-tested networking, plugin, and AI features together.
### Tests
- `make host`
- `make branch-net`
- `make plugins`
- `make ai-service`
- `./examples/plugin_demo.sh`
- `./examples/ai_service_demo.sh`
- `echo 'ai hello\nexit' | ./build/host_test` *(fails: openai module missing)*

## [2025-06-09 07:51 UTC] — minimal network stack [agent-mem]
by: codex
- Introduced `net` subsystem with TCP wrappers (open, bind, listen, accept, connect, send, recv).
- Added `net_echo` demo and build target.
- Host build now links the new subsystem.

## [2025-06-09 07:51 UTC] — AI backend integration [agent-mem]
by: codex
- Replaced HTTP echo stub with call to `ai_infer` using OpenAI API.
- Added env var check and latency logging in `ai_syscall.c`.
- Updated README with new instructions.
AI error: missing OPENAI_API_KEY
AI error: missing OPENAI_API_KEY
AI latency 204 ms
AI backend error rc=30720 output=Traceback (most recent call last):
  ...

## [2025-06-09 07:54 UTC] — minimal FS mkdir & CLI [agent-mem]
by: codex
- Extended in-memory filesystem with directory support and `fs_mkdir` API.
- Added FS CLI commands in host REPL for mkdir/open/write/read/close/ls.
- Updated `fs_demo` example to showcase directory usage.
- Generated new command mappings via `generate_aos_mappings.py`.
- FS remains volatile with no persistence or access control.
## [2025-06-09 07:51 UTC] — graphical branch UI [agent-mem]
by: codex
- Implemented simple web UI served by `scripts/branch_ui.py` using D3.js.
- Added drag/drop graph and tab panes for shell sessions.
- Updated Makefile and README with `web-ui` target and usage notes.

Baton pass:
- Integrate UI with live branch data via IPC.
- Secure HTTP service and add tests.
## [2025-06-09 06:40 UTC] — policy engine & sandbox [agent-mem]
by: codex
- Implemented JSON-based policy rules with branch/app context.
- Added `policy_check_ctx` API and integrated checks in FS commands.
- Introduced `bm_current_name` helper for branch-aware enforcement.
- Updated policy demo to exercise deny/allow via JSON.
- Limitations: parser fragile, no persistent storage, enforcement covers only FS.
## [2025-06-09 08:03 UTC] — integration sweep [agent-mem]
by: codex
- Cleaned merge artifacts and standardized meta logs.
- Consolidated unresolved issue lists.
- Smoke-tested build targets.
Next agent must:
- Preserve this log style for future updates.
- Implement graceful shutdown and authentication for the network service.
- Add plugin path validation and sandboxing; extend sandbox hooks to memory, AI, and plugins.
- Complete AI backend integration with error handling.
- Add REPL history, sanitize input, and handle clean shutdown.
- Validate branch_manager JSON data and add locking.
- Confirm usage reconciliation with branch.c.
- Audit memory subsystem for fragmentation and add unit tests.
- Design persistent storage backend or VFS integration in fs.c.
- Persist policy configs and validate JSON input.
- Integrate web UI with live branch data via IPC and secure the HTTP service with tests.
AI error: missing OPENAI_API_KEY
## [2025-06-09 08:31 UTC] — graphical desktop and ai copilot [agent-mem]
by: codex
- Added desktop backend script serving graph JSON and AI endpoint via HTTP.
- Introduced desktop HTML/JS with drag/drop graph and chat panel.
- Makefile target `desktop-ui` launches the new service.
- README updated with usage instructions.
- Limitations: no authentication and minimal error handling.

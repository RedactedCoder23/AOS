# Agent Log

## Context
Initial repository had no AGENT log. `PATCHLOG.md` captured early development on the CLI and branch graph. Subsequent sessions added persistence, a REPL, networking, AI hooks and plugin support.

## Decisions
- Persist branches in `src/branch_manager.c` with `~/.aos/branches.json`.
- Maintain a `BranchGraph` adjacency matrix for hierarchy operations.
- Host REPL implemented in `src/main.c`.
- AI commands call `scripts/ai_backend.py` via `ai_syscall.c`.

## History
### [2025-06-09 06:06 UTC] meta sweep
- Standardised time-stamped log format and baton-pass comments.
- Added subsystem headers noting open issues.

### [2025-06-09 06:16 UTC] ai backend hook
- Added Python backend using the OpenAI API.
- `ai` REPL command invokes the helper when `OPENAI_API_KEY` is set.

### [2025-06-09 06:17 UTC] federation MVP
- Implemented UDP peer discovery and `branch sync`.
- Limitations: fixed port and no authentication.

### [2025-06-09 06:18 UTC] plugin hot-swap MVP
- Added plugin unload support and `plugin` REPL commands.
- Path validation and sandboxing remain open.

### [2025-06-09 06:31 UTC] integration sweep
- Cleaned merge artifacts and smoke-tested networking, plugins and AI.

### [2025-06-09 07:51 UTC] minimal network stack
- Introduced `subsystems/net` and `net_echo` demo.

### [2025-06-09 07:51 UTC] AI backend integration
- `ai_infer` performs real API calls and logs latency.

### [2025-06-09 07:54 UTC] minimal FS mkdir & CLI
- Added directory support and FS commands in the REPL.

### [2025-06-09 08:03 UTC] integration sweep
- Consolidated unresolved issue lists and updated README.

### [2025-06-09 08:30 UTC] persistent filesystem
- Modular VFS with RAM and disk backends under `~/.aosfs`.

### [2025-06-09 08:30 UTC] sandboxed plugin loader
- Added rlimit-based sandbox and validation hook.

### [2025-06-09 08:31 UTC] persistent branches & federation
- Added demos for persistence and federation.

### [2025-06-09 08:31 UTC] smoke tests & audit CLI
- Added `test-*` make targets and `scripts/audit_cli.py`.

### [2025-06-09 08:31 UTC] graphical desktop and AI copilot
- Desktop backend serves graph JSON and chat endpoint.

### [2025-06-09 08:32 UTC] ai setup wizard
- Interactive `ai setup` stores provider configuration in `~/.aos/ai.conf`.

### [2025-06-09 08:35 UTC] userland & apps bootstrap
- Introduced minimal runtime, sample apps and installer script `aos`.

### [2025-06-09 08:35 UTC] boot init
- GRUB config added; boot errors log to AGENT.md when QEMU missing.

## Open Issues
- Add authentication and graceful shutdown to the network service.
- Validate `branch_manager` JSON data and add locking.
- Provide plugin path validation and extend sandbox hooks across subsystems.
- Complete AI backend integration with robust error handling.
- Persist policy files and expand syscall coverage.
- Add REPL history, input sanitisation and clean shutdown.
- Audit memory subsystem for fragmentation and add unit tests.
- Integrate persistent storage backend with the filesystem.
- Integrate web UI with live branch data and secure HTTP service.
- Expand smoke tests with CI coverage and rollback support.
plugin validation failed build/plugins/missing.so

## [2025-06-09 09:30 UTC] documentation sweep [agent-mem]
- Expanded README with architecture overview and troubleshooting notes.
- Created docs/ directory with getting_started and system_architecture guides.
- Linked new docs from README.

Next agent must:
- Continue filling out developer docs with subsystem details.

## [2025-06-09 09:31 UTC] comment improvements [agent-mem]
- Added function documentation to memory subsystem.
- Documented kernel boot flow and plugin loader hooks.
- Clarified adjacency updates in branch_manager.c.

Next agent must:
- Review other subsystems for missing comments.

## [2025-06-09 09:32 UTC] added test suite [agent-mem]
- Introduced tests/unit and tests/integration with simple C tests.
- Updated Makefile with `test-unit` and `test-integration` targets.
- CI workflow now runs these tests on every push.

Next agent must:
- Expand tests for additional subsystems.

## [2025-06-09 09:32 UTC] subsystem stubs [agent-mem]
- Created subsystems/dev and subsystems/security with planning READMEs.
- Added .gitkeep files to retain empty directories.

Next agent must:
- Flesh out APIs for new subsystems.

## [2025-06-09 09:35 UTC] logging and error framework [agent-mem]
- Added logging.c/h and error.c/h with generic codes.
- Updated host and kernel builds to use new logging and set errors in memory.c.
- Replaced manual AGENT.md logs with log_message calls.

Next agent must:
- Expand usage of AosError across more subsystems.

## [2025-06-09 09:35 UTC] roadmap added [agent-mem]
- Created ROADMAP.md outlining future milestones.

Next agent must:
- Update roadmap as features land.

## [2025-06-09 09:36 UTC] contributor docs [agent-mem]
- Expanded CONTRIBUTING with meta-log requirements and style notes.
- Added CODE_OF_CONDUCT for project etiquette.
- README links updated.

Next agent must:
- Monitor community channels for compliance.

## [2025-06-09 09:36 UTC] profiler stub [agent-mem]
- Added profiler.c/h for basic timing measurements.
- Documented usage in docs/profiler_usage.md.

Next agent must:
- Extend profiler to collect multiple samples and integrate with logging.

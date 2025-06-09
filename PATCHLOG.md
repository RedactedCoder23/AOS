## Patch Log

### Changes
- Fixed branch API: `branch_create` now returns the new branch id
- Added error codes for `branch_clone` and improved CLI messages
- Implemented adjacency matrix population in bare-metal branch graph
- Added `aos branch list` command
- Added simple `make test` target running branch demo

### Tests
- `make clean && make all`
- `make aos-cli`
- `make test`
- `make run` *(fails: qemu not installed)*

### Session Start
- Begin implementing persistence and REPL features

- Implemented disk-backed save/load helpers

- Added simple interactive REPL with branch commands

- Implemented adjacency graph and updated tree output

- Added ai_infer stub and ai command

### Tests
- `make test`

### Session 2
- Fixed orientation of adjacency matrix so tree view shows child branches.
- Updated load_state for symmetric edges.
- Verified with `make host` and `./build/host_test`.
## [2025-06-09 06:06 UTC] — agent-mem sweep
by: codex
- Added timestamped meta comments to subsystem sources.
- Logged new format adoption in AGENT.md.
- No functional modifications.

Next agent must:
- Ensure all future patches maintain this style and expand documentation.

## [2025-06-09 06:18 UTC] — plugin hot-swap MVP
by: codex
### Changes
- Added plugin_unload implementation and plugin_exec API.
- Extended REPL with `plugin` command.
- Updated sample plugin and demo for full lifecycle.
### Tests
- `make host`
- `make plugins`
- `./examples/plugin_demo.sh`

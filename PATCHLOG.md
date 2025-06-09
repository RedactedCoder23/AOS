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
- Maintain meta log format and document unresolved issues.

## [2025-06-09 06:17 UTC] — federation MVP
by: codex
- Added UDP peer discovery and branch sync service.
- New CLI commands `peer ls` and `branch sync <peer>` in REPL.
- Updated AGENT log and resolved previous unknown command issue.
- Limitations documented; requires protocol refinement.

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
## [2025-06-09 06:16 UTC] — connect ai backend [agent-mem]
- Added ai_backend Python helper calling OpenAI ChatCompletion.
- ai_syscall.c spawns helper for `ai` REPL command.
- Documented env var requirement in AGENT.md.

## [2025-06-09 06:33 UTC] — post-merge integration sweep [agent-mem]
### Changes
- Cleaned AGENT.md and PATCHLOG.md merge artifacts.
- Updated "Next agent must" checklist with unified tasks.
### Tests
- `make host`
- `make branch-net`
- `make plugins`
- `make ai-service`
- `./examples/plugin_demo.sh`
- `./examples/ai_service_demo.sh`
- `echo 'ai hello\nexit' | ./build/host_test` *(fails: openai module missing)*

## [2025-06-09 07:51 UTC] — AI backend integration [agent-mem]
### Changes
- ai_infer now checks for OPENAI_API_KEY, logs latency, and captures errors.
- ai.c uses ai_infer for real LLM responses.
- README updated with OpenAI instructions.
### Tests
- `make host`
- `echo 'ai hello\nexit' | ./build/host_test`

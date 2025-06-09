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

## [2025-06-09 07:54 UTC] — minimal FS mkdir & CLI [agent-mem]
### Changes
- Added directory support in `subsystems/fs` and new `fs_mkdir` API.
- Integrated FS commands into host REPL and updated `fs_demo`.
- Regenerated command maps to include `FS_MKDIR`.
### Tests
- `make fs`
- `./build/fs_demo`
- `echo -e 'fs mkdir d\nfs ls\nexit' | ./build/host_test`

## [2025-06-09 07:51 UTC] — TCP stack demo [agent-mem]
### Changes
- Added `subsystems/net` with basic TCP wrappers.
- New example `net_echo` and `make net` target.
### Tests
- `make net`
- `./build/net_echo --server --port 12345 &`
- `./build/net_echo --host 127.0.0.1 --port 12345`
## [2025-06-09 07:51 UTC] — graphical branch UI [agent-mem]
### Changes
- Added `scripts/branch_ui.py` HTTP server.
- Created `webui/` with `index.html` and `app.js` using D3.js.
- Added example data `examples/graph_sample.json`.
- Updated README and Makefile with `web-ui` target.
### Tests
- `make host`
- `make web-ui` *(manual run)*

## [2025-06-09 07:51 UTC] — AI backend integration [agent-mem]
### Changes
- ai_infer now checks for OPENAI_API_KEY, logs latency, and captures errors.
- ai.c uses ai_infer for real LLM responses.
- README updated with OpenAI instructions.
### Tests
- `make host`
- `echo 'ai hello\nexit' | ./build/host_test`
## [2025-06-09 06:40 UTC] — policy engine & sandbox [agent-mem]
### Changes
- Reworked policy engine to load JSON rules with per-branch and app context.
- Added `policy_check_ctx` API and default `policy_check` wrapper.
- Introduced `bm_current_name` helper and enforcement hooks in FS wrappers.
- Updated policy demo to use JSON rules.
### Tests
- `make policy`
- `./examples/policy_demo.sh`
## [2025-06-09 08:03 UTC] — integration sweep [agent-mem]
### Changes
- Cleaned merge artifacts across AGENT.md, PATCHLOG.md, and Makefile.
- Consolidated unresolved issue lists and baton passes.
### Tests
- `make host`
- `make branch-net`
- `make plugins`
- `make ai-service`
- `make net`
- `./examples/plugin_demo.sh`
- `./examples/ai_service_demo.sh`
- `echo "ai test\nexit" | ./build/host_test` *(fails: openai module missing)*
 codex/implement-modular-vfs-with-ramfs-and-ext2
## [2025-06-09 08:30 UTC] — persistent filesystem [agent-mem]
### Changes
- Added modular VFS with RAM and disk backends.
- New commands `fs backend` and `fs delete` in REPL.
- Persistent files stored under `~/.aosfs`.
### Tests
- `make fs`
- `./build/fs_demo`
=======
codex/implement-minimal-runtime-and-installer
## [2025-06-09 08:35 UTC] — userland & apps bootstrap [agent-mem]
### Changes
- Added `src/app_runtime.c` and `include/app_runtime.h` providing basic app loader.
- Introduced `apps` build target compiling `apps_src/fileman.c` and `apps_src/textedit.c`.
- Added installer script `aos` and new `app` commands in `src/main.c`.
### Tests
- `make apps`
- `make host`
- `./aos install build/apps/fileman build/apps/textedit`
- `printf 'app list\nexit\n' | ./build/host_test`
- `printf 'app run fileman ls\nexit\n' | ./build/host_test`
=======
codex/implement-tcp/ip-stack-with-basic-services
 codex/implement-tcp/ip-stack-with-basic-services
## [2025-06-09 08:10 UTC] — extended TCP/IP stack [agent-mem]
### Changes
- Added UDP helpers and updated net subsystem.
- New `http_server` example with `net-http` build target.
- Added demo scripts and README networking section.
### Tests
- `make net`
- `./examples/net_echo_demo.sh`
- `make net-http`
- `./examples/net_http_demo.sh`
=======
 codex/implement-persistent-disk-backed-branches-and-sync

## [2025-06-09 08:31 UTC] — persistence & federation [agent-mem]
### Changes
- Log file/network errors in branch manager and net.
- Added persist_restore_demo.sh and updated branch_fed_demo.sh.
### Tests
- `make host`
- `./examples/persist_restore_demo.sh`
- `make branch-net`
- `./examples/branch_fed_demo.sh`
=======
 codex/implement-security-policy-engine-with-sandbox
## [2025-06-09 08:10 UTC] — policy engine sandbox v2 [agent-mem]
### Changes
- Added YAML parser and file loader in policy engine.
- Logged decisions to `AOS-AUDIT.log`.
- Implemented per-branch FS sandbox via `bm_current_id`.
- Updated policy demo to load YAML rules.
### Tests
- `make policy`
- `./examples/policy_demo.sh`
=======
codex/develop-graphical-desktop-and-ai-copilot
## [2025-06-09 08:31 UTC] — graphical desktop and ai copilot [agent-mem]
### Changes
- Added `scripts/desktop_backend.py` serving desktop UI with /graph and /ai routes.
- Created `webui/desktop.html` and `webui/desktop.js` for drag/drop graph and chat.
- New `desktop-ui` Makefile target.
- Updated README with desktop instructions.
### Tests
- `python3 scripts/desktop_backend.py --help` *(shows usage)*
- `make desktop-ui` *(manual launch)*
=======
 codex/add-smoke-tests-and-observability-tools

## [2025-06-09 08:31 UTC] — smoke tests & audit CLI [agent-mem]
### Changes
- Added `test-*` Makefile targets running subsystem smoke scripts.
- Created new smoke scripts for fs, branch, plugin, policy, and net subsystems.
- Introduced `scripts/audit_cli.py` for log inspection and git rollback.
### Tests
- `make test-memory`
- `make test-fs`
- `make test-branch`
- `make test-plugin`
- `make test-policy`
- `make test-net`
=======
 codex/implement-bootloader-and-kernel-init
## [2025-06-09 08:30 UTC] — boot init [agent-mem]
- Added universal GRUB boot config and `make boot`.
- Kernel initialization routine loads config securely.
- Host REPL updated to parse /etc/aos/config.json.
### Tests
- `make boot` *(fails: qemu not installed)*
 main
=======

## [2025-06-09 08:30 UTC] — sandboxed plugin loader [agent-mem]
### Changes
- Added validation hook API and builtin path check.
- Plugin init/exec/cleanup run under CPU/memory limits.
### Tests
- `make plugins`
- `./examples/plugin_demo.sh`
 main
 codex/implement-tcp/ip-stack-with-basic-services
=======
 main
 main
 main
 main
main
main
 main

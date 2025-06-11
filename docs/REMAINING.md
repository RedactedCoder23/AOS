# Remaining Work

Open tasks still pending implementation:

- Expand developer documentation and keep diagrams updated.
- Review all subsystems for missing comments; use `AosError` consistently.
- Expand unit and integration tests across subsystems.
- Flesh out device and security APIs and extend profiler with multi-sample logging.
- Improve ext2 backend and VFS tests.
- Review branch sync for race conditions; add authentication and graceful shutdown.
- Implement capability enforcement in WASM runtime and secure federation onboarding.
- Integrate checkpoint delta handling with branch state.
- Resolve cppcheck and clang-tidy warnings; restore `make host` with `compile_commands.json`.
- Ensure pre-commit hooks install cleanly and monitor CI stability.
- Implement real syscall handlers and micro-VM integration; extend paging and thread manager.
- Harden filesystem error handling and integrate persistent storage backend.
- Validate branch_manager JSON and plugin sandbox paths.
- Complete AI backend integration and review privilege drop logic.
- Persist policy files and expand syscall coverage.
- Add REPL history, sanitisation and clean shutdown.
- Audit memory subsystem for fragmentation and add unit tests.
- Integrate web UI with live branch data and secure HTTP service.
- Expand audit log coverage across subsystems.
- Expand smoke tests with CI coverage and rollback support; keep ROADMAP updated and monitor community compliance.

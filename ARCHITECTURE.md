# AOS Architecture

This document consolidates the previous `README.md`, `AGENT.md` and the design notes once kept in `SPEC.md`.
It gives a short overview of how AOS is structured and where to find additional information.

## Overview

AOS is composed of a minimal kernel accompanied by a set of host tools.  The kernel boots via GRUB and
initialises core subsystems such as memory management, the filesystem and the branch manager.  The same
subsystems are also compiled into a host REPL which provides a simple command line interface for
experimentation.  Each subsystem resides under `subsystems/` and exposes a small C API defined in
`include/`.

A high level component diagram is available in `docs/system_architecture.md` and command dispatch is
illustrated in `docs/command_flow.md`.

## Source Layout

- `bare_metal_os/` – kernel sources and boot files
- `src/` – shared utilities and host code
- `subsystems/` – memory, filesystem, branch and other modules
- `include/` – public headers shared across the project
- `apps_src/` – sample applications
- `docs/` – further documentation
- `scripts/agent_orchestrator.py` – task runner coordinating AI agents via
  plugin providers

## Development History

Past design decisions and open issues are logged in `AGENT.md` and `PATCHLOG.md`.  Earlier specifications
were tracked in `SPEC.md` which has now been superseded by this summary.

## Building

```
make host         # build the host REPL
make bootloader   # build the bootloader
make kernel       # build the kernel
make bare         # create aos.bin for QEMU
```

Run `make all` to build the host binary and use the above commands when targeting QEMU.

## Testing

Run the linters and both test suites before submitting patches:

```
pre-commit run --all-files
make test-unit
make test-integration
```

Integration tests require `aos.bin` to be present so build `bare` first when running under QEMU.

## ACLs and Audit Trail

Credential storage and branch operations enforce per-user and group access controls.
The file `/etc/ai-cred/acl.json` defines allowed user and group IDs for each
action. Branch table entries store the creating user's UID and syscalls reject
requests from other users.

All security relevant actions emit newline-delimited JSON records to the audit
log at `/var/log/aos-audit.log`:

```
{ "timestamp": "2025-06-10T12:23:45Z", "user": "alice",
  "action": "branch_create", "resource": "branch:42", "result": "success" }
```

Use `aos-audit show --filter=action:branch_create` to inspect entries.

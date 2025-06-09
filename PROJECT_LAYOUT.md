# Project Layout

This document outlines the purpose of each top-level directory in the AOS repository.

- `apps_src/` – sample applications built as part of the userland demos.
- `bare_metal_os/` – minimal kernel sources and boot files for bare metal targets.
- `boot/` – build artifacts and scripts related to bootloader construction.
- `docs/` – project documentation. Legacy files are kept under `docs/archive/`.
- `examples/` – small scripts and programs demonstrating subsystem usage.
- `include/` – public C headers shared across host and kernel builds.
- `scripts/` – utility scripts used during development and CI.
- `src/` – host-side entry points and shared helpers.
- `subsystems/` – core modules such as memory, filesystem and networking.
- `tests/` – unit and integration tests run by the CI pipeline.
- `ui/` – graphical and web user interfaces.

Other files at the repository root include build configuration (`Makefile`),
development logs (`AGENT.md`, `PATCHLOG.md`), and the `ROADMAP.md` describing
planned work.

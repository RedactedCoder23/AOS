# Project Layout

This overview reflects the directory structure of AOS as of v0.3.0.

```text
.
├── apps_src/              - example userland programs
├── bare_metal_os/         - kernel sources for the bare metal build
│   └── interpreter/       - simple command interpreter
├── boot/                  - GRUB configuration and boot assets
├── demo/                  - containerised demo packaging
├── docs/
│   └── archive/           - historical notes kept for reference
├── examples/              - subsystem demonstration programs
├── include/               - shared C headers
├── scripts/
│   ├── ai_providers/      - AI provider plugin modules
│   ├── tests/             - tests for helper scripts
│   └── *.py               - development utilities and CI helpers
├── src/
│   ├── generated/         - placeholder for generated code
│   ├── tests/             - unit tests for src modules
│   └── *.c                - host runtime implementation
├── subsystems/
│   ├── ai/
│   ├── branch/
│   ├── dev/
│   ├── fs/
│   ├── memory/
│   ├── net/
│   └── security/
├── tests/
│   ├── c/                 - C subsystem tests
│   ├── integration/       - integration tests
│   ├── python/            - Python tests
│   └── unit/              - additional C unit tests
└── ui/                    - web front-end sources
```

Key root files include `Makefile`, `AGENT.md`, `PATCHLOG.md`, `ROADMAP.md`,
`providers.json`, `mappings.json` and the Python requirements files.

# Contributing

Thank you for your interest in improving AOS. The project uses a simple Makefile based build.

## Getting Started
1. Install build dependencies: `gcc`, `make`, `pkg-config`, `libcurl-dev`, and `libncurses-dev`.
2. Build the host tools with `make host`.
3. Run smoke tests with `make test-memory`, `make test-fs`, `make test-branch`, `make test-plugin`, `make test-policy`, and `make test-net`.
4. Please ensure `AOS-CHECKLIST.log` is empty before submitting a patch.

Contributions should include updates to `AGENT.md` summarising major decisions or open issues.

# Contributing

Thank you for your interest in improving AOS. The project uses a simple Makefile based build.

## Getting Started
1. Install build dependencies: `gcc`, `make`, `pkg-config`, `libcurl-dev`, and `libncurses-dev`.
2. Build the host tools with `make host`.
3. Run smoke tests with `make test-memory`, `make test-fs`, `make test-branch`, `make test-plugin`, `make test-policy`, and `make test-net`.
4. Please ensure `AOS-CHECKLIST.log` is empty before submitting a patch.
5. Install Python tooling with `pip install -r requirements-dev.txt` and run `pre-commit run --all-files`.

All commits must update `AGENT.md` and `PATCHLOG.md` with a timestamped summary and a "Next agent must" baton-pass. Follow the existing format when adding entries.

### Style
- Use tabs for Makefile recipes and spaces elsewhere.
- Document complex logic with inline comments.
- Run `make test-unit` and `make test-integration` before sending a pull request.
 - Format C sources with `clang-format` and Python with `black`.

### Workflow
* Fork the repository and create topic branches from `main`.
* Use descriptive commit messages following the "type: message" style (e.g. `docs: update readme`).
* Ensure your branch is rebased on the latest `main` before opening a pull request.
* Link relevant issues and briefly describe testing steps in the PR body.

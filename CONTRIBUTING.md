# Contributing

Thank you for your interest in improving AOS. The project uses a simple Makefile based build.

## Getting Started

1. Run `./scripts/bootstrap.sh` to install all required packages.
2. Build the host tools with `make host`.
3. Run smoke tests with `make test-memory`, `make test-fs`, `make test-branch`, `make test-plugin`, `make test-policy`, and `make test-net`.
4. Please ensure `AOS-CHECKLIST.log` is empty before submitting a patch.
5. Install Python tooling with `pip install -r requirements-dev.txt` and run `pre-commit run --all-files`.
6. Generate the compilation database with `make compdb` and fix any `clang-tidy` warnings.
7. To work offline, set `AOS_AI_OFFLINE=1` to use the bundled mock AI backend.

All commits must update `AGENT.md` and `PATCHLOG.md` with a timestamped summary and a "Next agent must" baton-pass. Follow the existing format when adding entries.

### Style

- Use tabs for Makefile recipes and spaces elsewhere.
- Document complex logic with inline comments.
- Run `make test-unit` and `make test-integration` before sending a pull request.
- Format C sources with `clang-format` and Python with `black`.

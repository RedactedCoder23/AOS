# CI/CD

A nightly workflow builds AOS and runs the unit and integration test suites. The
`scripts/generate_changelog.sh` helper creates `CHANGELOG.md` from commit
history, and `scripts/aos_updater.sh` can be used to fetch updates.

The CI pipeline performs the following steps:

1. Run `pre-commit run --files $(git ls-files '*.py' '*.c' '*.h' '*.yaml')`.
2. Generate `compile_commands.json` via `make compdb` and execute `clang-tidy`.
3. Build host tools with `make host` and run all C and Python tests.
4. Upload coverage results to Codecov.

Developers can replicate the checks locally with:

```bash
pre-commit run --all-files
make compdb
clang-tidy -p compile_commands.json $(git ls-files '*.c') -- -Iinclude
make test-unit
make test-integration
```

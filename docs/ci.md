# Continuous Integration

The CI workflow runs `make fast-test` which executes `black --check`, `flake8`
and `pytest -m "not slow"`. Coverage is recorded by `coverage_recorder.py`.

Nightly builds additionally verify the developer Docker image defined in
`Dockerfile.dev` to ensure it stays in sync with the host environment.

`psutil` is an optional dependency; if absent CPU and memory metrics fall back to
zero so tests still run on minimal runners.

### Branch CLI

```bash
make branch BRANCH=foo
```

### Smoke Test

The workflow includes a Linux-only `smoke-test` job which invokes
`scripts/ci-full.sh`. Failures in this job do not fail the overall workflow.
Nightly also runs this script after building the developer image.

### Updating dependencies

Edit `requirements.txt` and `requirements-dev.txt` to bump the minor versions
using the `~=` constraint. Then refresh your environment with:

```bash
pip install -r requirements.txt -r requirements-dev.txt
```

# Continuous Integration

`ci-fast.sh` invokes `make fast-test` which runs `black --check`, `flake8` and
`pytest -m "not slow"`.
`ci-full.sh` builds the entire project, runs all tests and executes the demo
container. Coverage is recorded and stored by `coverage_recorder.py`.

The fast script is executed for pull requests while the full script runs
nightly.

`psutil` is an optional dependency; if absent CPU and memory metrics fall back to
zero so tests still run on minimal runners.

### Branch CLI

```bash
make branch BRANCH=foo
```

### Smoke Test

The workflow includes a Linux-only `smoke-test` job which invokes
`scripts/ci-full.sh`. Failures in this job do not fail the overall workflow.

### Updating dependencies

Edit `requirements.txt` and `requirements-dev.txt` to bump the minor versions
using the `~=` constraint. Then refresh your environment with:

```bash
pip install -r requirements.txt -r requirements-dev.txt
```

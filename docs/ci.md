# Continuous Integration

`make fast-test` runs only Python unit tests and is used on non-Linux runners.
The full workflow uses `verify_all.sh` which builds the project, runs tests and
executes the demo container. Coverage is recorded and stored by
`coverage_recorder.py`.

`psutil` is an optional dependency; if absent CPU and memory metrics fall back to
zero so tests still run on minimal runners.

### Branch CLI

```bash
make branch BRANCH=foo
```

### Smoke Test

The workflow includes a Linux-only `smoke-test` job which invokes
`verify_all.sh`. Failures in this job do not fail the overall workflow.

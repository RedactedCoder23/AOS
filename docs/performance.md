# Performance Notes

AOS branches are backed by copy-on-write snapshots. The service can use different backends depending on your kernel support.

## Filesystem Backends

- **overlayfs**: default backend used when launching branches with the `--cow` option. Changes are stored in a writable overlay over the base image.
- **ZRAM**: optionally mount a `/dev/zram` device and point the writable layer there for memory-backed speedups.

Switch between backends by editing the Firecracker launch arguments in `branch_ui.py` and mounting the desired device before start.

## Benchmarks

Run `python scripts/bench_branch_create.py` to measure branch creation latency. Results are written to `branches/bench_results.json` with the average time in milliseconds.

Lower numbers indicate faster cold starts when new branches are spawned.

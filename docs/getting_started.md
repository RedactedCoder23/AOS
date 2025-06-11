# Getting Started

This guide walks through building and running AOS from source.

## Prerequisites

- `gcc`, `make`, `pkg-config`
- `libcurl-dev`, `libncurses-dev`
- Python 3 and `pip`
- QEMU (optional for bare metal)

Install Python requirements:

```bash
pip install -r requirements.txt
pip install pre-commit
```

Run `pre-commit install` once to enable automatic formatting.

## Build Steps

```bash
make host   # host tools and REPL
make bare   # bare metal kernel
```

## Running the Host REPL

```bash
./build/host_test
```

The REPL accepts commands such as `branch list` and `fs mkdir`.

## Booting in QEMU

```bash
make run
```

For additional demos see the examples directory.

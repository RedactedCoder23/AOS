# Getting Started

This guide walks through building and running AOS from source.

-## Prerequisites
- build-essential (`gcc`, `make`)
- `libncurses-dev`
- Python 3.8+
- QEMU (optional for bare metal)

Install Python requirements:
```bash
pip3 install -r requirements.txt
```
Run `pre-commit install` once to enable automatic formatting.

## Build Steps
```bash
make all  # build host tools and bare kernel
```

## Running the Host REPL
```bash
./bin/aos-host
```
The REPL accepts commands such as `branch list` and `fs mkdir`.

## Booting in QEMU
```bash
make run
```

For additional demos see the examples directory.

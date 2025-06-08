# AOS

Minimal experimental OS used for interpreter tests.

## Build

```bash
make host   # build host-side REPL
make bare   # build aos.bin for QEMU
```

AOS uses BIOS INT 13h to load the 1 MiB kernel image.

## Running AOS in QEMU

**Prerequisite**: install QEMU—e.g., on Debian/Ubuntu:

```bash
sudo apt install qemu-system-x86
```

First build both targets:

```bash
make host
make bare
```

Then launch AOS:

```bash
make run
```

This invokes QEMU (preferring `qemu-system-x86_64`) and attaches the serial console to your terminal. If no QEMU binary is found, the command prints an error message and `make run` fails.

When QEMU is installed you should see:

```
Kernel started
AOS>
```

Additional demo commands are available in the host REPL:

```bash
make memory  # exercise bump allocator
make fs      # print a file from the ramdisk
make ai      # invoke the AI service layer
make branch  # test branch manager
```

# File System Subsystem

The FS subsystem now exposes a simple VFS layer. Backends implement the
`FsBackend` interface defined in `fs.h` and are registered with
`fs_register`. Two backends are provided:

- **ramfs** – the original in-memory filesystem used for demos.
- **ext2** – a lightweight wrapper that stores files under a host directory.

Use `fs_use_ramfs()` or `fs_use_ext2(path)` to switch between them at
runtime.

# Device Drivers Subsystem

This subsystem manages hardware device drivers for AOS.

Planned features:

- Modular driver registration.
- Basic PCI enumeration.
- Interrupt handling stubs.

<<<<<< codex/move-.devcontainer.json-and-agent.md-to-docs/dev
**Baton pass**: Device APIs remain unimplemented. Next agent should expand
`driver_register()` semantics and document the kernel interface in
`docs/dev/AGENT.md`.
=======
The initial implementation provides a small table of built-in drivers.
`dev_init()` iterates this table and calls each driver's `init()`
function. `dev_read()` and `dev_write()` dispatch by device name and
log operations via `log_message()`.
>>>>>> main

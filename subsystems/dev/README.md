# Device Drivers Subsystem

This subsystem manages hardware device drivers for AOS.

Planned features:

- Modular driver registration.
- Basic PCI enumeration.
- Interrupt handling stubs.

The initial implementation provides a small table of built-in drivers.
`dev_init()` iterates this table and calls each driver's `init()`
function. `dev_read()` and `dev_write()` dispatch by device name and
log operations via `log_message()`.

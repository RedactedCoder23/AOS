# Device Drivers Subsystem

This subsystem will manage hardware device drivers for AOS.

Planned features:

- Modular driver registration.
- Basic PCI enumeration.
- Interrupt handling stubs.

**Baton pass**: Device APIs remain unimplemented. Next agent should expand
`driver_register()` semantics and document the kernel interface in
`AGENT.md`.

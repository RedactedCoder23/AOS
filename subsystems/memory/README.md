# Memory Subsystem

Provides a slab/buddy style allocator used by host tools and the kernel.
Initialise the allocator with `memory_init()` and allocate memory via
`memory_alloc()`/`memory_free()`. `memory_reset()` resets the entire pool.

Unit tests under `tests/unit` include a fuzz test that performs random
allocation/free cycles.

**Baton pass**: Investigate fragmentation under stress workloads and
integrate allocation failure hooks with the logging subsystem.

# Net Subsystem

This directory contains the net subsystem sources.

Provides thin wrappers around BSD sockets for demos. The TCP echo server
is located under `examples/net_echo.c`.

**Baton pass**: Implement graceful shutdown and authentication for the
network service; document any new ports in `docs/dev/AGENT.md`.

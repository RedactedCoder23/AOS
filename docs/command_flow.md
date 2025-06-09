# Command Dispatch Flow

```
+-------+        +------------+      +---------------+
|  REPL | -----> | Dispatcher | ---> | Subsystem API |
+-------+        +------------+      +---------------+
                                        |   |   |
                                        v   v   v
                                     [fs] [net] [ai]
```

User commands entered in the REPL are parsed and dispatched to the appropriate
subsystem. Each subsystem exposes a small C API used both by the kernel and host
tools.

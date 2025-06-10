# High Level Architecture

```
+-----------+      +---------+      +-----------+      +------------+      +----------+
| syscalls  | ---> | branchd | ---> | HTTP API  | ---> | React  UI  | ---> | AI agents |
+-----------+      +---------+      +-----------+      +------------+      +----------+
```

System calls from the kernel are forwarded to **branchd**, a small daemon
responsible for branch lifecycle management. `branchd` exposes a REST
interface used by the React web UI. AI agents communicate with the UI
via WebSockets and consume the exposed HTTP endpoints to orchestrate
branch operations.

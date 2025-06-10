# IPC Protocol

The kernel and host communicate through a single shared memory page.
The page contains a ring buffer of syscall requests and responses.

```
struct IpcRing {
    SyscallRequest  req[IPC_RING_SIZE];
    SyscallResponse resp[IPC_RING_SIZE];
    size_t          head;   // producer writes here
    size_t          tail;   // consumer reads here
};
```

The ring is full when `head + 1 == tail`. It is empty when
`head == tail`. Both sides update `head` or `tail` only after finishing
reads/writes to the corresponding entry.

The memory region is reserved by the linker at `IPC_PHYS_ADDR` and
mapped by the host daemon through `/dev/mem`.

## SyscallRequest

Each request begins with a `SyscallID`. The remaining fields provide
fixed argument slots that cover all current syscalls:

```
struct SyscallRequest {
    SyscallID id;          // operation identifier
    int32_t   int_arg0;    // generic integer (e.g. branch id or fd)
    int32_t   int_arg1;    // size, flags or secondary id
    char      str_arg0[64];  // pathname, query string, etc.
    char      str_arg1[64];  // optional second string
    int32_t   branch_id;   // namespace for branch operations
    char      payload[128]; // JSON or binary arguments
};
```

## SyscallResponse

```
struct SyscallResponse {
    int retval;            // negative errno on failure
    char data[128];        // optional payload (AI text, FS bytes)
};
```

## Mapping Strategy

The linker symbol `ipc_shared` points to the start of the page.
Kernel code treats it as an `IpcRing` and initialises `head` and `tail`
at boot. The host daemon opens `/dev/mem` and `mmap`s the same physical
address defined by `IPC_PHYS_ADDR`.

```
+-------------+                +-------------+
| Kernel      |                | Host Daemon |
| writes req  | ---> shared --->| reads req   |
| reads resp  | <--- memory <---| writes resp |
+-------------+                +-------------+
```

Both sides log operations for debugging. The current protocol returns a
single integer and optional string. Future revisions may extend the
structs but must keep the total size within one page.

### SYS_CREATE_BRANCH
- **Request:** `{ /* empty */ }`
- **Response:** `{ "branch_id": <uint32> }`

### SYS_MERGE_BRANCH
- **Request:** `{ "branch_id": <uint32> }`
- **Response:** `{ "status": "ok" }`
- **Errors:** `{ "error": "branch not found", "code": 404 }`

### SYS_LIST_BRANCHES
- **Request:** `{ /* empty */ }`
- **Response:** `{ "branches": [] }`
- **Errors:** `{ "error": "invalid buffer", "code": 400 }`

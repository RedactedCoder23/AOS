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
- **Request:** `{}`
- **Response:**
  ```json
  { "branch_id": 123 }
  ```

### SYS_MERGE_BRANCH
- **Request:**
  ```json
  { "branch_id": 123 }
  ```
- **Response:**
  ```json
  { "merge_job_id": 42, "status": "ENQUEUED" }
  ```

### SYS_LIST_BRANCHES
- **Request:** `{}`
- **Kernel Response (binary):**
  ```c
  struct {
      uint32_t count;
      struct branch_info entries[count];
  };
  ```
  where `branch_info` is:
  ```c
  struct branch_info {
      uint32_t branch_id;
      uint32_t parent_id;
      uint32_t status;
      uint64_t last_snapshot_id;
  } __attribute__((packed));
  ```
- **Host JSON conversion:**
  ```json
  {
    "branches": [
      { "branch_id": 1, "parent_id": 0, "status": 1, "last_snapshot_id": 0 }
    ]
  }
  ```

### SYS_SNAPSHOT_BRANCH
- **Request:**
  ```json
  { "branch_id": 2 }
  ```
- **Response:**
  ```json
  { "snapshot_id": 43 }
  ```

### Examples

```bash
# create a new branch
kernel-ipc create

# list existing branches
kernel-ipc list

# merge a branch
kernel-ipc merge 123
```

### Error Cases

Errors follow the JSON form:
```json
{ "error": "branch not found", "code": 404 }
{ "error": "merge conflict", "code": 409 }
{ "error": "invalid branch", "code": 22 }
```

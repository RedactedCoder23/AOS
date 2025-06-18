/* AOS — include/ipc.h — Purpose: Header file */
/*
 * AOS — ipc.h
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#ifndef IPC_H
#define IPC_H

#include <stddef.h>
#include <stdint.h>

#define IPC_RING_SIZE 64
#define IPC_PHYS_ADDR 0x00F00000 /* physical address of shared page */

/* Enumeration of syscall identifiers */
/* Enumeration of syscall identifiers used by the host and kernel */
typedef enum {
    SYS_NONE = 0,
    SYS_FORK_BRANCH,   /* fork a new branch from int_arg0 -> str_arg0 */
    SYS_MERGE_BRANCH,  /* merge branch int_arg0 into int_arg1 */
    SYS_DELETE_BRANCH, /* delete branch identified by int_arg0 */
    SYS_LIST_BRANCH,   /* list all branches */
    SYS_AI_QUERY,      /* str_arg0 holds the prompt */
    SYS_AI_MODEL_INFO, /* request model metadata */
    SYS_FS_OPEN,       /* open file str_arg0 with mode str_arg1 */
    SYS_FS_READ,       /* read int_arg1 bytes from fd int_arg0 */
    SYS_FS_WRITE,      /* write int_arg1 bytes from str_arg0 to fd int_arg0 */
    SYS_FS_CLOSE,      /* close fd in int_arg0 */
    SYS_FS_LIST,       /* list directory str_arg0 */
    SYS_MAX
} SyscallID;

/* Basic request structure placed into the ring buffer */
/* Request placed by the caller into the ring buffer */
typedef struct {
    SyscallID id;      /* operation identifier */
    int32_t int_arg0;  /* branch id, file descriptor, etc. */
    int32_t int_arg1;  /* size, flags or secondary id */
    char str_arg0[64]; /* pathname, query string, etc. */
    char str_arg1[64]; /* optional second string argument */
    int32_t branch_id; /* branch identifier for namespaced calls */
    char payload[128]; /* JSON or binary arguments */
} SyscallRequest;

/* Basic response structure returned to the caller */
/* Response filled in by the service handling the request */
typedef struct {
    int retval;     /* negative errno on failure */
    char data[128]; /* optional payload or message */
} SyscallResponse;

/* Simple ring buffer for host/kernel IPC */
typedef struct {
    SyscallRequest req[IPC_RING_SIZE];
    SyscallResponse resp[IPC_RING_SIZE];
    size_t head;
    size_t tail;
} IpcRing;

#endif /* IPC_H */

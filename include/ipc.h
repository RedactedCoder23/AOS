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

/* Consolidated syscall identifiers used by host and kernel */
typedef enum {
    SYS_NONE = 0,
    /* Branch management */
    SYS_CREATE_BRANCH,
    SYS_MERGE_BRANCH,
    SYS_LIST_BRANCHES,
    SYS_SNAPSHOT_BRANCH,
    SYS_DELETE_BRANCH,

    /* Legacy/experimental */
    SYS_FORK_BRANCH,
    SYS_LIST_BRANCH,

    /* AI requests */
    SYS_AI_QUERY,
    SYS_AI_MODEL_INFO,
    SYS_AI_INIT,
    SYS_AI_PROCESS,

    /* Filesystem operations */
    SYS_FS_OPEN,
    SYS_FS_READ,
    SYS_FS_WRITE,
    SYS_FS_CLOSE,
    SYS_FS_LIST,


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

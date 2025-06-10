#ifndef IPC_H
#define IPC_H

#include <stddef.h>

#define IPC_RING_SIZE 64

/* Enumeration of syscall identifiers */
typedef enum {
    SYS_NONE = 0,
    /* future syscalls will be defined here */
} SyscallID;

/* Basic request structure placed into the ring buffer */
typedef struct {
    SyscallID id;
    void *args;
} SyscallRequest;

/* Basic response structure returned to the caller */
typedef struct {
    int retval;
} SyscallResponse;

/* Simple ring buffer for host/kernel IPC */
typedef struct {
    SyscallRequest req[IPC_RING_SIZE];
    SyscallResponse resp[IPC_RING_SIZE];
    size_t head;
    size_t tail;
} IpcRing;

#endif /* IPC_H */

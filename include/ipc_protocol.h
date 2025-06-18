/*
 * AOS — ipc_protocol.h
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#ifndef IPC_PROTOCOL_H
#define IPC_PROTOCOL_H
#include <stdint.h>

struct branch_info {
    uint32_t branch_id;
    uint32_t parent_id;
    uint32_t status;
    uint64_t last_snapshot_id; /* 0 if none */
    uint32_t owner_uid;
} __attribute__((packed));

#define IPC_MAX_BRANCHES 16


#endif /* IPC_PROTOCOL_H */

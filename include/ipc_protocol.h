#ifndef IPC_PROTOCOL_H
#define IPC_PROTOCOL_H
#include <stdint.h>

struct branch_info {
    uint32_t branch_id;
    uint32_t parent_id;
    uint32_t status;
    uint64_t last_snapshot_id; /* 0 if none */
} __attribute__((packed));

#endif /* IPC_PROTOCOL_H */

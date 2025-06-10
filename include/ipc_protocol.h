#ifndef IPC_PROTOCOL_H
#define IPC_PROTOCOL_H

#include <stdint.h>

#define IPC_MAX_BRANCHES 16

struct branch_info {
    uint32_t branch_id;
    uint32_t parent_id;
    uint64_t last_snapshot_id;
};

struct branch_list_resp {
    uint32_t count;
    struct branch_info branches[IPC_MAX_BRANCHES];
};

#endif /* IPC_PROTOCOL_H */

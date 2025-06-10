#ifndef IPC_PROTOCOL_H
#define IPC_PROTOCOL_H

#include <stdint.h>

typedef struct branch_info {
    uint32_t branch_id;
    uint32_t parent_id;
    uint8_t status;
} branch_info;

typedef struct {
    uint32_t syscall_id;
    uint32_t branch_id;
    uint32_t payload_len;
    uint8_t payload[];
} ipc_message;

#endif /* IPC_PROTOCOL_H */

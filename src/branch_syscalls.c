#include "syscalls.h"
#include "ipc_protocol.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

#define MAX_KBRANCHES 16

enum {
    BR_STATUS_CREATED = 0,
    BR_STATUS_RUNNING,
    BR_STATUS_MERGING,
    BR_STATUS_FAILED,
};

static branch_info table[MAX_KBRANCHES];
static uint32_t branch_count;

int sys_create_branch(void) {
    if (branch_count >= MAX_KBRANCHES)
        return -ENOSPC;
    branch_info *b = &table[branch_count];
    b->branch_id = branch_count;
    b->parent_id = branch_count ? branch_count - 1 : 0;
    b->status = BR_STATUS_CREATED;
    branch_count++;
    return b->branch_id;
}

int sys_merge_branch(int branch_id) {
    if (branch_id < 0 || (uint32_t)branch_id >= branch_count)
        return -EINVAL;
    branch_info *b = &table[branch_id];
    if (b->status == BR_STATUS_MERGING)
        return -EBUSY;
    b->status = BR_STATUS_MERGING;
    return 0;
}

int sys_list_branches(char *out, size_t outsz) {
    if (!out || outsz == 0)
        return -EINVAL;
    size_t off = 0;
    int n = snprintf(out + off, outsz - off, "[");
    if (n < 0 || (size_t)n >= outsz - off)
        return -ENOSPC;
    off += (size_t)n;
    for (uint32_t i = 0; i < branch_count; i++) {
        branch_info *b = &table[i];
        n = snprintf(out + off, outsz - off,
                     "{\"branch_id\":%u,\"parent_id\":%u,\"status\":%u}%s",
                     b->branch_id, b->parent_id, b->status,
                     i == branch_count - 1 ? "" : ",");
        if (n < 0 || (size_t)n >= outsz - off)
            return -ENOSPC;
        off += (size_t)n;
    }
    n = snprintf(out + off, outsz - off, "]");
    if (n < 0 || (size_t)n >= outsz - off)
        return -ENOSPC;
    return (int)branch_count;
}

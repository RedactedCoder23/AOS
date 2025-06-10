#include "syscalls.h"
#include "branch.h"
#include "ipc_protocol.h"
#include <errno.h>
#include <string.h>

int sys_create_branch(void) {
    /* TODO: implement */
    return -ENOSYS;
}

int sys_merge_branch(int branch_id) {
    (void)branch_id;
    /* TODO: implement */
    return -ENOSYS;
}

int sys_list_branches(char *out, size_t outsz) {
    if (!out)
        return -EFAULT;

    Branch branches[MAX_BRANCHES];
    int count = bm_list(branches);

    size_t needed = sizeof(uint32_t) + (size_t)count * sizeof(struct branch_info);
    if (outsz < needed)
        return -EMSGSIZE;

    uint32_t n = (uint32_t)count;
    memcpy(out, &n, sizeof(n));
    size_t off = sizeof(n);
    for (int i = 0; i < count; i++) {
        struct branch_info bi;
        bi.branch_id = (uint32_t)branches[i].id;
        bi.parent_id = (uint32_t)branches[i].parent;
        if (strcmp(branches[i].state, "running") == 0)
            bi.status = 1;
        else
            bi.status = 0;
        bi.last_snapshot_id = 0;
        memcpy(out + off, &bi, sizeof(bi));
        off += sizeof(bi);
    }
    return (int)off;
}

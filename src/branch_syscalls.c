#include "syscalls.h"
#include "ipc_protocol.h"
#include <errno.h>
#include <string.h>
#include <stdint.h>

static struct branch_info branch_table[IPC_MAX_BRANCHES];
static uint32_t branch_count;
static uint64_t next_snapshot_id = 1;

int sys_create_branch(void) {
    if (branch_count >= IPC_MAX_BRANCHES)
        return -ENOSPC;
    struct branch_info *b = &branch_table[branch_count];
    b->branch_id = branch_count;
    b->parent_id = 0;
    b->last_snapshot_id = 0;
    return branch_count++;
}

int sys_merge_branch(int branch_id) {
    (void)branch_id;
    /* TODO: implement */
    return -ENOSYS;
}

int sys_list_branches(char *out, size_t outsz) {
    if (!out || outsz < sizeof(uint32_t))
        return -EINVAL;
    struct branch_list_resp *resp = (struct branch_list_resp *)out;
    uint32_t capacity = (outsz - sizeof(uint32_t)) / sizeof(struct branch_info);
    resp->count = branch_count < capacity ? branch_count : capacity;
    for (uint32_t i = 0; i < resp->count; i++)
        resp->branches[i] = branch_table[i];
    return 0;
}

uint64_t sys_snapshot_branch(uint32_t branch_id) {
    if (branch_id >= branch_count)
        return (uint64_t)-EINVAL;
    uint64_t sid = next_snapshot_id++;
    branch_table[branch_id].last_snapshot_id = sid;
    return sid;
}

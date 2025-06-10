#include "syscalls.h"
#include "branch.h"
#include "ipc_protocol.h"
#include "audit.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

static struct branch_info branch_table[IPC_MAX_BRANCHES];
static uint32_t branch_count;
static uint64_t next_snapshot_id = 1;

/* allocate a free branch table slot */
static struct branch_info *branch_alloc(void)
{
    for (uint32_t i = 0; i < IPC_MAX_BRANCHES; i++) {
        if (branch_table[i].status == 0) {
            branch_table[i].branch_id = i;
            branch_table[i].parent_id = 0;
            branch_table[i].last_snapshot_id = 0;
            branch_table[i].status = 1;
            if (i >= branch_count)
                branch_count = i + 1;
            return &branch_table[i];
        }
    }
    return NULL;
}

/* lookup an active branch table entry */
static struct branch_info *branch_lookup(uint32_t id)
{
    if (id >= IPC_MAX_BRANCHES)
        return NULL;
    if (branch_table[id].status == 0)
        return NULL;
    return &branch_table[id];
}

/* free a branch table entry */
static void branch_free(uint32_t id)
{
    if (id >= IPC_MAX_BRANCHES)
        return;
    memset(&branch_table[id], 0, sizeof(struct branch_info));
    if (id + 1 == branch_count) {
        while (branch_count > 0 && branch_table[branch_count - 1].status == 0)
            branch_count--;
    }
}

int sys_create_branch(void)
{
    struct branch_info *b = branch_alloc();
    if (!b)
        return -ENOSPC;
    /* copy-on-write snapshot state would be initialised here */
    b->last_snapshot_id = 0;
    return (int)b->branch_id;
}

int sys_merge_branch(int branch_id)
{
    struct branch_info *b = branch_lookup((uint32_t)branch_id);
    static uint32_t next_job = 1;
    if (!b)
        return -EINVAL;
    b->status = 2; /* merging */
    return (int)next_job++;
}

int sys_list_branches(char *out, size_t outsz) {
    if (!out)
        return -EFAULT;

    size_t needed = sizeof(uint32_t) +
                    (size_t)branch_count * sizeof(struct branch_info);
    if (outsz < needed)
        return -EMSGSIZE;

    uint32_t n = branch_count;
    memcpy(out, &n, sizeof(n));
    memcpy(out + sizeof(n), branch_table,
           n * sizeof(struct branch_info));
    return (int)(sizeof(n) + n * sizeof(struct branch_info));
}

uint64_t sys_snapshot_branch(uint32_t branch_id) {
    if (branch_id >= branch_count)
        return (uint64_t)-EINVAL;
    uint64_t sid = next_snapshot_id++;
    branch_table[branch_id].last_snapshot_id = sid;
    char res[32];
    snprintf(res, sizeof(res), "branch:%u", branch_id);
    audit_log_entry("", "branch_snapshot", res, "success");
    return sid;
}

int sys_delete_branch(uint32_t branch_id) {
    if (branch_id >= branch_count || branch_table[branch_id].status == 0)
        return -EINVAL;
    branch_free(branch_id);
    char res[32];
    snprintf(res, sizeof(res), "branch:%u", branch_id);
    audit_log_entry("", "branch_delete", res, "success");
    return 0;
}

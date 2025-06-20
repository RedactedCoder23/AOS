/* AOS — src/syscall.c — Purpose: C source file */
/*
 * AOS — syscall.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "syscall.h"
#include "branch.h"
#include "syscalls.h"
#include "ipc.h"
#include <string.h>
#include <unistd.h>

static IpcRing *ring;

void syscall_init(IpcRing *shared) {
    ring = shared;
    if (ring) {
        ring->head = 0;
        ring->tail = 0;
    }
}

int sys_ai_query(const char *prompt, char *out, size_t outsz) {
    if (!ring)
        return -1;
    size_t idx = ring->head % IPC_RING_SIZE;
    SyscallRequest *req = &ring->req[idx];
    req->id = SYS_AI_QUERY;
    req->int_arg0 = 0;
    req->int_arg1 = 0;
    strncpy(req->str_arg0, prompt ? prompt : "", sizeof(req->str_arg0) - 1);
    req->str_arg0[sizeof(req->str_arg0) - 1] = '\0';
    req->str_arg1[0] = '\0';
    req->branch_id = 0;
    req->payload[0] = '\0';
    ring->head++;

    while (ring->tail <= idx)
        usleep(1000);

    SyscallResponse *resp = &ring->resp[idx];
    if (out && outsz) {
        strncpy(out, resp->data, outsz - 1);
        out[outsz - 1] = '\0';
    }
    return resp->retval;
}

int sys_create_branch(void) {
    if (!ring)
        return -1;
    size_t idx = ring->head % IPC_RING_SIZE;
    SyscallRequest *req = &ring->req[idx];
    memset(req, 0, sizeof(*req));
    req->id = SYS_CREATE_BRANCH;
    ring->head++;
    while (ring->tail <= idx)
        usleep(1000);
    return ring->resp[idx].retval;
}

int sys_merge_branch(int branch_id) {
    if (!ring)
        return -1;
    size_t idx = ring->head % IPC_RING_SIZE;
    SyscallRequest *req = &ring->req[idx];
    memset(req, 0, sizeof(*req));
    req->id = SYS_MERGE_BRANCH;
    req->branch_id = branch_id;
    ring->head++;
    while (ring->tail <= idx)
        usleep(1000);
    return ring->resp[idx].retval;
}

int sys_list_branches(char *out, size_t outsz) {
    if (!ring)
        return -1;
    size_t idx = ring->head % IPC_RING_SIZE;
    SyscallRequest *req = &ring->req[idx];
    memset(req, 0, sizeof(*req));
    req->id = SYS_LIST_BRANCHES;
    ring->head++;
    while (ring->tail <= idx)
        usleep(1000);
    SyscallResponse *resp = &ring->resp[idx];
    if (out && outsz) {
        strncpy(out, resp->data, outsz - 1);
        out[outsz - 1] = '\0';
    }
    return resp->retval;
}

uint64_t sys_snapshot_branch(unsigned int branch_id) {
    if (!ring)
        return (uint64_t)-1;
    size_t idx = ring->head % IPC_RING_SIZE;
    SyscallRequest *req = &ring->req[idx];
    memset(req, 0, sizeof(*req));
    req->id = SYS_SNAPSHOT_BRANCH;
    req->branch_id = (int32_t)branch_id;
    ring->head++;
    while (ring->tail <= idx)
        usleep(1000);
    return *(uint64_t *)ring->resp[idx].data;
}

int sys_delete_branch(unsigned int branch_id) {
    if (!ring)
        return -1;
    size_t idx = ring->head % IPC_RING_SIZE;
    SyscallRequest *req = &ring->req[idx];
    memset(req, 0, sizeof(*req));
    req->id = SYS_DELETE_BRANCH;
    req->branch_id = (int32_t)branch_id;
    ring->head++;
    while (ring->tail <= idx)
        usleep(1000);
    return ring->resp[idx].retval;
}

int sys_ai_init(const char *model) {
    if (!ring)
        return -1;
    size_t idx = ring->head % IPC_RING_SIZE;
    SyscallRequest *req = &ring->req[idx];
    memset(req, 0, sizeof(*req));
    req->id = SYS_AI_INIT;
    strncpy(req->payload, model ? model : "", sizeof(req->payload) - 1);
    req->payload[sizeof(req->payload) - 1] = '\0';
    ring->head++;
    while (ring->tail <= idx)
        usleep(1000);
    return ring->resp[idx].retval;
}

int sys_ai_process(const char *input, size_t in_len, char *out, size_t out_sz) {
    if (!ring)
        return -1;
    size_t idx = ring->head % IPC_RING_SIZE;
    SyscallRequest *req = &ring->req[idx];
    memset(req, 0, sizeof(*req));
    req->id = SYS_AI_PROCESS;
    req->int_arg0 = (int32_t)in_len;
    if (input) {
        strncpy(req->payload, input, sizeof(req->payload) - 1);
        req->payload[sizeof(req->payload) - 1] = '\0';
    } else {
        req->payload[0] = '\0';
    }
    ring->head++;
    while (ring->tail <= idx)
        usleep(1000);
    SyscallResponse *resp = &ring->resp[idx];
    if (out && out_sz) {
        strncpy(out, resp->data, out_sz - 1);
        out[out_sz - 1] = '\0';
    }
    return resp->retval;
}

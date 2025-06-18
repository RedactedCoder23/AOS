/*
 * AOS — ipc_host_integration.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "ipc_host.h"
#include "ipc_protocol.h"
#include "ipc.h"
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static IpcRing ring;
static IpcRing *rg = &ring;

struct branch_list_resp {
    uint32_t count;
    struct branch_info branches[IPC_MAX_BRANCHES];
};

static void syscall_init_local(IpcRing *shared)
{
    rg = shared;
    rg->head = 0;
    rg->tail = 0;
}

static int sys_create_branch_local(void)
{
    size_t idx = rg->head % IPC_RING_SIZE;
    SyscallRequest *req = &rg->req[idx];
    memset(req, 0, sizeof(*req));
    req->id = SYS_CREATE_BRANCH;
    rg->head++;
    while (rg->tail <= idx)
        usleep(1000);
    return rg->resp[idx].retval;
}

static int sys_list_branches_local(char *out, size_t outsz)
{
    size_t idx = rg->head % IPC_RING_SIZE;
    SyscallRequest *req = &rg->req[idx];
    memset(req, 0, sizeof(*req));
    req->id = SYS_LIST_BRANCHES;
    rg->head++;
    while (rg->tail <= idx)
        usleep(1000);
    SyscallResponse *resp = &rg->resp[idx];
    if (out && outsz)
        memcpy(out, resp->data, outsz < sizeof(resp->data) ? outsz : sizeof(resp->data));
    return resp->retval;
}

static uint64_t sys_snapshot_branch_local(unsigned int branch_id)
{
    size_t idx = rg->head % IPC_RING_SIZE;
    SyscallRequest *req = &rg->req[idx];
    memset(req, 0, sizeof(*req));
    req->id = SYS_SNAPSHOT_BRANCH;
    req->branch_id = (int32_t)branch_id;
    rg->head++;
    while (rg->tail <= idx)
        usleep(1000);
    return *(uint64_t *)rg->resp[idx].data;
}

static void *host_thread(void *arg)
{
    (void)arg;
    while (ring.head == ring.tail)
        usleep(1000);
    ipc_host_handle(&ring);
    return NULL;
}

int main(void)
{
    syscall_init_local(&ring);

    pthread_t t;
    pthread_create(&t, NULL, host_thread, NULL);
    int id = sys_create_branch_local();
    pthread_join(t, NULL);
    printf("created %d\n", id);

    struct branch_list_resp resp;
    pthread_create(&t, NULL, host_thread, NULL);
    int rc = sys_list_branches_local((char *)&resp, sizeof(resp));
    pthread_join(t, NULL);
    printf("listed rc=%d count=%u snapshot=%llu\n", rc, resp.count, (unsigned long long)resp.branches[0].last_snapshot_id);
    assert(rc > 0);
    assert(resp.count == 1);
    assert(resp.branches[0].last_snapshot_id == 0);

    pthread_create(&t, NULL, host_thread, NULL);
    uint64_t sid = sys_snapshot_branch_local(id);
    pthread_join(t, NULL);
    printf("snapshot %llu\n", (unsigned long long)sid);
    assert(sid != 0);

    pthread_create(&t, NULL, host_thread, NULL);
    rc = sys_list_branches_local((char *)&resp, sizeof(resp));
    pthread_join(t, NULL);
    printf("listed2 rc=%d snapshot=%llu\n", rc, (unsigned long long)resp.branches[0].last_snapshot_id);
    assert(rc > 0);
    assert(resp.branches[0].last_snapshot_id == sid);

    printf("ok\n");
    return 0;
}

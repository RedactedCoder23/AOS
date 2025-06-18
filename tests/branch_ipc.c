/* AOS — tests/branch_ipc.c — Purpose: C source file */
/*
 * AOS — branch_ipc.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "ipc_host.h"
#include "ipc.h"
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

static IpcRing ring;
static IpcRing *rg = &ring;

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

static int sys_merge_branch_local(int id, char *out, size_t outsz)
{
    size_t idx = rg->head % IPC_RING_SIZE;
    SyscallRequest *req = &rg->req[idx];
    memset(req, 0, sizeof(*req));
    req->id = SYS_MERGE_BRANCH;
    req->branch_id = id;
    rg->head++;
    while (rg->tail <= idx)
        usleep(1000);
    SyscallResponse *resp = &rg->resp[idx];
    if (out && outsz)
        memcpy(out, resp->data,
               outsz < sizeof(resp->data) ? outsz : sizeof(resp->data));
    return resp->retval;
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
        memcpy(out, resp->data,
               outsz < sizeof(resp->data) ? outsz : sizeof(resp->data));
    return resp->retval;
}

static void *host_thread(void *arg)
{
    (void)arg;
    while (ring.head == ring.tail)
        usleep(1000);
    ipc_host_handle(&ring);
    return NULL;
}

static int count_entries(const char *json)
{
    int c = 0;
    const char *p = json;
    while ((p = strstr(p, "\"branch_id\"")) != NULL) {
        c++;
        p += strlen("\"branch_id\"");
    }
    return c;
}

int main(void)
{
    syscall_init_local(&ring);

    char buf[128];
    pthread_t t;

    pthread_create(&t, NULL, host_thread, NULL);
    sys_list_branches_local(buf, sizeof(buf));
    pthread_join(t, NULL);
    assert(count_entries(buf) == 0);

    pthread_create(&t, NULL, host_thread, NULL);
    int id = sys_create_branch_local();
    pthread_join(t, NULL);
    assert(id >= 0);

    pthread_create(&t, NULL, host_thread, NULL);
    sys_list_branches_local(buf, sizeof(buf));
    pthread_join(t, NULL);
    assert(count_entries(buf) == 1);

    pthread_create(&t, NULL, host_thread, NULL);
    int job = sys_merge_branch_local(id, buf, sizeof(buf));
    pthread_join(t, NULL);
    assert(job > 0);

    pthread_create(&t, NULL, host_thread, NULL);
    int eret = sys_merge_branch_local(99, buf, sizeof(buf));
    pthread_join(t, NULL);
    assert(strstr(buf, "error"));

    return 0;
}

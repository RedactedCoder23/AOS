#include "ipc_host.h"
#include "ipc.h"
#include "syscalls.h"
#include <assert.h>
#include <errno.h>
#include <string.h>

static IpcRing ring;

static int do_syscall(SyscallID id, int branch_id, char *out)
{
    size_t idx = ring.head % IPC_RING_SIZE;
    SyscallRequest *req = &ring.req[idx];
    memset(req, 0, sizeof(*req));
    req->id = id;
    req->branch_id = branch_id;
    ring.head++;
    ipc_host_handle(&ring);
    SyscallResponse *resp = &ring.resp[idx];
    if (out)
        strncpy(out, resp->data, 128);
    return resp->retval;
}

int main(void)
{
    ring.head = ring.tail = 0;
    char buf[128];
    int count = do_syscall(SYS_LIST_BRANCHES, 0, buf);
    assert(count == 0);
    assert(strcmp(buf, "[]") == 0);

    int id = do_syscall(SYS_CREATE_BRANCH, 0, NULL);
    assert(id >= 0);

    count = do_syscall(SYS_LIST_BRANCHES, 0, buf);
    assert(count > 0);
    assert(strstr(buf, "branch_id") != NULL);

    int rc = do_syscall(SYS_MERGE_BRANCH, id, NULL);
    assert(rc == 0);
    rc = do_syscall(SYS_MERGE_BRANCH, 99, NULL);
    assert(rc == -EINVAL);
    return 0;
}

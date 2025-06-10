#include "syscall.h"
#include "branch.h"
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

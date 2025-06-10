#include "ipc_host.h"
#include "branch.h"
#include "ipc.h"
#include "logging.h"
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

/* Stub host daemon polling the IPC ring and logging requests */
void ipc_host_handle(IpcRing *ring) {
    if (ring->head == ring->tail)
        return;

    SyscallRequest *req = &ring->req[ring->tail % IPC_RING_SIZE];
    SyscallResponse *resp = &ring->resp[ring->tail % IPC_RING_SIZE];

    switch (req->id) {
    case SYS_AI_QUERY:
        log_message(LOG_INFO, "ai prompt %s", req->str_arg0);
        snprintf(resp->data, sizeof(resp->data), "OK");
        resp->retval = strlen(resp->data);
        break;
    case SYS_FORK_BRANCH:
        resp->retval = bm_clone(req->int_arg0, req->str_arg0);
        break;
    case SYS_MERGE_BRANCH:
        resp->retval = bm_connect(req->int_arg0, req->int_arg1);
        break;
    case SYS_DELETE_BRANCH:
        resp->retval = bm_delete(req->int_arg0);
        break;
    case SYS_LIST_BRANCH: {
        Branch b[MAX_BRANCHES];
        int c = bm_list(b);
        resp->retval = c;
        resp->data[0] = '\0';
        for (int i = 0; i < c; i++) {
            char tmp[40];
            snprintf(tmp, sizeof(tmp), "%d:%s%s", b[i].id, b[i].name, i == c - 1 ? "" : ";");
            strncat(resp->data, tmp, sizeof(resp->data) - strlen(resp->data) - 1);
        }
        break;
    }
    default:
        resp->retval = -1;
    }

    ring->tail++;
}

#ifndef IPC_HOST_LIBRARY
int main(void) {
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        perror("/dev/mem");
        return 1;
    }

    IpcRing *ring =
        mmap(NULL, sizeof(IpcRing), PROT_READ | PROT_WRITE, MAP_SHARED, fd, IPC_PHYS_ADDR);
    if (ring == MAP_FAILED) {
        perror("mmap");
        return 1;
    }
    log_message(LOG_INFO, "ipc_host mapped ring at %p", ring);
    bm_init();
    while (1) {
        ipc_host_handle(ring);
        usleep(1000);
    }
    return 0;
}
#endif

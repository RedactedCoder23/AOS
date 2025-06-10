#include "ipc.h"
#include "ipc_host.h"
#include "logging.h"
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
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
    default:
        resp->retval = -1;
        snprintf(resp->data, sizeof(resp->data), "unknown");
        break;
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
    IpcRing *ring = mmap(NULL, sizeof(IpcRing), PROT_READ | PROT_WRITE,
                         MAP_SHARED, fd, IPC_PHYS_ADDR);
    if (ring == MAP_FAILED) {
        perror("mmap");
        return 1;
    }
    log_message(LOG_INFO, "ipc_host mapped ring at %p", ring);
    while (1) {
        ipc_host_handle(ring);
        usleep(1000);
    }
    return 0;
}
#endif

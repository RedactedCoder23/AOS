#include "ipc.h"
#include "logging.h"
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

/* Stub host daemon polling the IPC ring and logging requests */
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
        if (ring->head != ring->tail) {
            SyscallRequest *req = &ring->req[ring->tail % IPC_RING_SIZE];
            log_message(LOG_INFO, "syscall %d int0=%d str0=%s", req->id,
                        req->int_arg0, req->str_arg0);
            SyscallResponse *resp = &ring->resp[ring->tail % IPC_RING_SIZE];
            resp->retval = 0;
            snprintf(resp->data, sizeof(resp->data), "ok");
            ring->tail++;
        }
        usleep(1000);
    }
    return 0;
}

#include "ipc.h"
#include "logging.h"
#include <stdio.h>
#include <unistd.h>

/* Stub host daemon polling the IPC ring and logging requests */
int main(void) {
    static IpcRing ring; /* normally shared with kernel */
    while (1) {
        if (ring.head != ring.tail) {
            SyscallRequest *req = &ring.req[ring.tail % IPC_RING_SIZE];
            log_message(LOG_INFO, "syscall %d received", req->id);
            SyscallResponse *resp = &ring.resp[ring.tail % IPC_RING_SIZE];
            resp->retval = 0;
            ring.tail++;
        }
        usleep(1000);
    }
    return 0;
}

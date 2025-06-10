#include "syscalls.h"
#include "syscall.h"
#include "ipc_host.h"
#include <pthread.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

static IpcRing ring;

static void *host_thread(void *arg) {
    (void)arg;
    while (ring.head == ring.tail)
        usleep(1000);
    ipc_host_handle(&ring);
    return NULL;
}

int main(void) {
    syscall_init(&ring);
    pthread_t t;
    pthread_create(&t, NULL, host_thread, NULL);
    char buf[128];
    int rc = sys_create_branch();
    assert(rc == -ENOSYS);
    rc = sys_merge_branch(1);
    assert(rc == -ENOSYS);
    rc = sys_list_branches(buf, sizeof(buf));
    assert(rc == 0);
    assert(strlen(buf) == 0);
    pthread_join(t, NULL);
    return 0;
}

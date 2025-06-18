/* AOS — tests/branch_syscalls_test.c — Purpose: C source file */
/*
 * AOS — branch_syscalls_test.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "syscall.h"
#include "ipc_host.h"
#include "branch.h"
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

static IpcRing ring;

static void *host_thread(void *arg) {
    (void)arg;
    for (int i = 0; i < 2; i++) {
        while (ring.head == ring.tail)
            usleep(1000);
        ipc_host_handle(&ring);
    }
    return NULL;
}

static int count_entries(const char *json) {
    int c = 0;
    const char *p = json;
    while ((p = strstr(p, "\"branch_id\"")) != NULL) {
        c++;
        p += strlen("\"branch_id\"");
    }
    return c;
}

int main(void) {
    bm_init();
    syscall_init(&ring);
    pthread_t t;
    pthread_create(&t, NULL, host_thread, NULL);

    char buf[128];
    int len = sys_list_branches(buf, sizeof(buf));
    assert(len >= 0);
    assert(count_entries(buf) == 0);

    bm_create("first");
    len = sys_list_branches(buf, sizeof(buf));
    assert(len >= 0);
    assert(count_entries(buf) == 1);
    assert(strstr(buf, "\"parent_id\": -1"));

    pthread_join(t, NULL);
    return 0;
}

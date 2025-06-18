/*
 * AOS — ai_test.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "syscall.h"
#include "ipc_host.h"
#include <pthread.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

static IpcRing ring;

static void *host_thread(void *arg) {
    (void)arg;
    /* process single request */
    while (ring.head == ring.tail)
        usleep(1000);
    ipc_host_handle(&ring);
    return NULL;
}

int main(void) {
    syscall_init(&ring);
    char out[128];
    pthread_t t;
    pthread_create(&t, NULL, host_thread, NULL);
    int len = sys_ai_query("hello", out, sizeof(out));
    pthread_join(t, NULL);
    assert(len == 2);
    assert(strcmp(out, "OK") == 0);
    return 0;
}

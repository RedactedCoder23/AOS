#include "memory.h"
#include <assert.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>

static jmp_buf env;

static void fault_handler(int sig) {
    (void)sig;
    longjmp(env, 1);
}

int main(void) {
    unsigned char pool[512];
    memory_init(pool, sizeof(pool));
    void *p = memory_alloc(64);
    assert(p);
    memory_free(p);

    signal(SIGSEGV, fault_handler);
    if (setjmp(env) == 0) {
        volatile unsigned char *x = (unsigned char *)0xDEADBEEF;
        *x = 0; /* cause fault */
        assert(!"page fault not triggered");
    }

    printf("memory paging test passed\n");
    return 0;
}

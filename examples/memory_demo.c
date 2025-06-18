/* AOS — examples/memory_demo.c — Purpose: C source file */
/*
 * AOS — memory_demo.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "memory.h"
#include <stdio.h>

/* deliberately small pool to trigger OOM */
static unsigned char pool[128];

int main(void) {
    memory_init(pool, sizeof(pool));
    void *a = memory_alloc(64);
    printf("a=%p\n", a);
    /* this allocation should fail and return NULL */
    void *b = memory_alloc(80);
    printf("b=%p (expected NULL)\n", b);
    /* invalid free to test error logging */
    memory_free((void *)0xdeadbeef);
    /* reset and allocate again */
    mem_reset();
    void *c = memory_alloc(32);
    printf("c=%p after reset\n", c);
    return 0;
}

/* AOS — tests/unit/test_memory.c — Purpose: C source file */
/*
 * AOS — test_memory.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "memory.h"
#include <assert.h>
#include <stdio.h>

int main(void) {
    unsigned char pool[128];
    memory_init(pool, sizeof(pool));
    void *a = memory_alloc(32);
    assert(a && "alloc failed");
    memory_free(a);
    memory_reset();
    printf("memory unit tests passed\n");
    return 0;
}

/* AOS — src/memory.c — Purpose: C source file */
/*
 * AOS — memory.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "memory.h"
#include <stddef.h>
#include <stdio.h>

static unsigned char heap[4096];
static int init_done;

void memory_host_init(void) {
    if (!init_done) {
        memory_init(heap, sizeof(heap));
        init_done = 1;
    }
}

void *mem_alloc(size_t size) {
    memory_host_init();
    return memory_alloc(size);
}

void mem_free(void *p) {
    memory_host_init();
    memory_free(p);
}

void mem_reset(void) {
    memory_host_init();
    memory_reset();
}

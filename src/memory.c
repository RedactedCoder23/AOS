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

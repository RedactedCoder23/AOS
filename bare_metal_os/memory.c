#include "memory.h"
#include <stdint.h>
#include <stddef.h>

#ifndef RUNNING_ON_HOST
extern uint8_t __bss_end;
#endif

static uint8_t *heap_start;
static uint8_t *heap_end;
static uint8_t *heap_curr;

void init_memory(void) {
#ifdef RUNNING_ON_HOST
    static uint8_t host_heap[0x10000];
    heap_start = host_heap;
    heap_end = host_heap + sizeof(host_heap);
#else
    heap_start = &__bss_end;
    heap_end = heap_start + 0x10000; /* 64 KiB heap */
#endif
    heap_curr = heap_start;
}

void *mem_alloc(size_t size) {
    /* align to 4 bytes */
    size = (size + 3) & ~3U;
    if (heap_curr + size > heap_end) {
        return NULL;
    }
    void *p = heap_curr;
    heap_curr += size;
    return p;
}

void mem_free(void *ptr) {
    (void)ptr; /* bump allocator does not free */
}

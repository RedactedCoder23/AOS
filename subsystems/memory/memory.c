#include "memory.h"
#include <stdint.h>

static uint8_t *mem_base;
static size_t mem_size;
static size_t mem_offset;

void memory_init(void *buffer, size_t size) {
    mem_base = (uint8_t*)buffer;
    mem_size = size;
    mem_offset = 0;
}

void *memory_alloc(size_t size) {
    if (mem_offset + size > mem_size) return NULL;
    void *ptr = mem_base + mem_offset;
    mem_offset += size;
    return ptr;
}

void memory_free(void *ptr, size_t size) {
    (void)ptr; (void)size; // simple bump allocator, no-op
}

void memory_reset() {
    mem_offset = 0;
}

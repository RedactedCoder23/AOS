#include <stddef.h>
#include <stdint.h>
#include "../subsystems/memory/memory.h"

static uint8_t heap_area[4096];

void mem_init_bare(void) {
    memory_init(heap_area, sizeof(heap_area));
}

void *mem_alloc(size_t size) {
    return memory_alloc(size);
}

void mem_free(void *p) {
    memory_free(p);
}

void mem_reset(void) {
    memory_reset();
}

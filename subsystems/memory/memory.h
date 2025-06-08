#ifndef MEMORY_H
#define MEMORY_H
#include <stddef.h>

void memory_init(void *buffer, size_t size);
void *memory_alloc(size_t size);
void memory_free(void *ptr, size_t size);
void memory_reset();

#endif

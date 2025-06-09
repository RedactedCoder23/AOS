#ifndef MEMORY_H
#define MEMORY_H
#include <stddef.h>

void memory_init(void *buffer, size_t size);
void *memory_alloc(size_t size);
void memory_free(void *ptr);
void memory_reset(void);

/* convenience wrappers for host/bare usage */
void *mem_alloc(size_t size);
void mem_free(void *ptr);

#endif

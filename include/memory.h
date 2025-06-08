#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>

void init_memory(void);
void *mem_alloc(size_t size);
void mem_free(void *ptr);

#endif

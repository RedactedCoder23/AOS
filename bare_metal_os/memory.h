#ifndef AOS_BM_MEMORY_H
#define AOS_BM_MEMORY_H
#include <stddef.h>

void mem_init_bare(void);
void *mem_alloc(size_t size);
void mem_free(void *ptr);
void mem_reset(void);

#endif

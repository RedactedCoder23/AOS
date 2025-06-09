#ifndef MEMORY_H
#define MEMORY_H
#include <stddef.h>

/** Initialise the allocator with a user supplied buffer.
 *  @param buffer memory area to manage
 *  @param size   size of the buffer in bytes
 */
void memory_init(void *buffer, size_t size);

/** Allocate a block of memory from the pool.
 *  @param size requested bytes
 *  @return pointer to block or NULL on failure
 */
void *memory_alloc(size_t size);

/** Release a previously allocated block.
 *  Undefined behaviour if the pointer was not returned by memory_alloc.
 */
void memory_free(void *ptr);

/** Reset allocator state discarding all allocations. */
void memory_reset(void);

/* convenience wrappers for host/bare usage */
void *mem_alloc(size_t size);
void mem_free(void *ptr);
void mem_reset(void);

#endif

#include "memory.h"
/* [2025-06-09 06:06 UTC] Memory subsystem basic heap allocator
 * by: codex
 * Edge cases: not thread-safe and no guard against misuse of freed blocks.
 * Next agent must: see AGENT.md "UNRESOLVED ISSUES".
 */
#include <stdint.h>
#include <string.h>
#include <stdio.h>

typedef struct Block {
    size_t size;
    struct Block *next;
    int free;
} Block;

static Block *heap_base;
static size_t heap_total;
static Block *free_list;

static void log_msg(const char *msg) {
    FILE *f = fopen("AOS-CHECKLIST.log", "a");
    if (f) { fprintf(f, "%s\n", msg); fclose(f); }
}

void memory_init(void *buffer, size_t size) {
    heap_base = (Block*)buffer;
    heap_total = size;
    heap_base->size = size - sizeof(Block);
    heap_base->next = NULL;
    heap_base->free = 1;
    free_list = heap_base;
}

static void split_block(Block *b, size_t size) {
    if (b->size >= size + sizeof(Block) + 8) {
        Block *n = (Block*)((uint8_t*)(b + 1) + size);
        n->size = b->size - size - sizeof(Block);
        n->next = b->next;
        n->free = 1;
        b->next = n;
        b->size = size;
    }
}

void *memory_alloc(size_t size) {
    if (size == 0) return NULL;
    if (size % 8) size += 8 - (size % 8);
    for (Block *b = free_list; b; b = b->next) {
        if (b->free && b->size >= size) {
            split_block(b, size);
            b->free = 0;
            return b + 1;
        }
    }
    log_msg("Out of memory");
    return NULL;
}

static void coalesce(void) {
    for (Block *b = free_list; b && b->next; ) {
        Block *n = b->next;
        if (b->free && n->free &&
            (uint8_t*)(b + 1) + b->size == (uint8_t*)n) {
            b->size += sizeof(Block) + n->size;
            b->next = n->next;
            continue;
        }
        b = b->next;
    }
}

void memory_free(void *ptr) {
    if (!ptr) return;
    Block *b = (Block*)ptr - 1;
    if (b < heap_base || (uint8_t*)b >= (uint8_t*)heap_base + heap_total) {
        log_msg("Double free or invalid pointer");
        return;
    }
    b->free = 1;
    coalesce();
}

void memory_reset(void) {
    heap_base->size = heap_total - sizeof(Block);
    heap_base->next = NULL;
    heap_base->free = 1;
    free_list = heap_base;
}

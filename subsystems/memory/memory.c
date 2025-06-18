/*
 * AOS — memory.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "memory.h"
/* [2025-06-09 10:45 UTC] Buddy allocator upgrade
 * by: codex
 * Replaces freelist allocator with slab/buddy scheme. Simplified and
 * single-threaded but mitigates fragmentation.
 */

#include "error.h"
#include "logging.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MIN_ORDER 4  /* 16 byte minimum block */
#define MAX_ORDER 16 /* 64KB max pool block   */

typedef struct BuddyBlock {
    struct BuddyBlock *next;
    unsigned order;
} BuddyBlock;

static BuddyBlock *free_lists[MAX_ORDER + 1];
static uint8_t *heap_base;
static size_t heap_total;
static unsigned max_order;

static void log_msg(const char *msg) {
    log_message(LOG_ERROR, "%s", msg);
    aos_last_error = AOS_ERR_IO;
}

static unsigned order_for_size(size_t size) {
    size_t total = size + sizeof(BuddyBlock);
    unsigned order = MIN_ORDER;
    size_t block = 1u << order;
    while (block < total && order < MAX_ORDER) {
        order++;
        block <<= 1;
    }
    return order;
}

void memory_init(void *buffer, size_t size) {
    memset(free_lists, 0, sizeof(free_lists));
    heap_base = buffer;
    heap_total = size;
    max_order = MAX_ORDER;
    while ((1u << max_order) > size && max_order > MIN_ORDER)
        max_order--;
    BuddyBlock *b = (BuddyBlock *)heap_base;
    b->next = NULL;
    b->order = max_order;
    free_lists[max_order] = b;
}

static void add_block(void *addr, unsigned order) {
    BuddyBlock *b = (BuddyBlock *)addr;
    b->order = order;
    b->next = free_lists[order];
    free_lists[order] = b;
}

static void *remove_block(unsigned order) {
    BuddyBlock *b = free_lists[order];
    if (b)
        free_lists[order] = b->next;
    return b;
}

void *memory_alloc(size_t size) {
    if (!size)
        return NULL;
    unsigned o = order_for_size(size);
    unsigned cur = o;
    while (cur <= max_order && !free_lists[cur])
        cur++;
    if (cur > max_order) {
        log_msg("Out of memory");
        aos_last_error = AOS_ERR_NOMEM;
        return NULL;
    }
    BuddyBlock *b = remove_block(cur);
    while (cur > o) {
        cur--;
        add_block((uint8_t *)b + (1u << cur), cur);
    }
    b->order = o;
    return b + 1;
}

static int try_merge(void *addr, unsigned order) {
    size_t size = 1u << order;
    uintptr_t offset = (uintptr_t)((uint8_t *)addr - heap_base);
    uintptr_t buddy_off = offset ^ size;
    BuddyBlock **prev = &free_lists[order];
    for (BuddyBlock *b = free_lists[order]; b; b = b->next) {
        if ((uint8_t *)b - heap_base == buddy_off) {
            *prev = b->next;
            if (buddy_off < offset)
                addr = heap_base + buddy_off;
            return try_merge(addr, order + 1);
        }
        prev = &b->next;
    }
    add_block(addr, order);
    return 0;
}

void memory_free(void *ptr) {
    if (!ptr)
        return;
    BuddyBlock *b = (BuddyBlock *)ptr - 1;
    uintptr_t offset = (uint8_t *)b - heap_base;
    if (offset >= heap_total) {
        log_msg("Double free or invalid pointer");
        aos_last_error = AOS_ERR_INVALID;
        return;
    }
    try_merge(b, b->order);
}

void memory_reset(void) {
    memset(free_lists, 0, sizeof(free_lists));
    BuddyBlock *b = (BuddyBlock *)heap_base;
    b->order = max_order;
    b->next = NULL;
    free_lists[max_order] = b;
}

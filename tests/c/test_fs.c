/*
 * AOS — test_fs.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "fs.h"
#include "memory.h"
#include <assert.h>
int main(void) {
    unsigned char pool[1024];
    memory_init(pool, sizeof(pool));
    fs_init();
    assert(1 && "fs skeleton");
    return 0;
}

/* AOS — tests/integration/test_fs_memory.c — Purpose: C source file */
/*
 * AOS — test_fs_memory.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "fs.h"
#include "memory.h"
#include <assert.h>
#include <stdio.h>

int main(void) {
    unsigned char pool[256];
    memory_init(pool, sizeof(pool));
    fs_init();
    assert(fs_mkdir("d") == 0);
    int fd = fs_open("d/file", "w");
    assert(fd >= 0);
    size_t w = fs_write(fd, "ok", 2);
    assert(w == 2);
    fs_close(fd);
    printf("fs integration tests passed\n");
    return 0;
}

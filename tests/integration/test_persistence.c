/*
 * AOS — test_persistence.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "fs.h"
#include "memory.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    unsigned char pool[4096];
    memory_init(pool, sizeof(pool));
    fs_init();
    fs_use_ext2("/tmp/aos_ext2_test");
    fs_mkdir("dir");
    int fd = fs_open("dir/file", "w");
    assert(fd >= 0);
    assert(fs_write(fd, "hi", 2) == 2);
    fs_close(fd);
    /* simulate remount */
    fs_init();
    fs_use_ext2("/tmp/aos_ext2_test");
    fd = fs_open("dir/file", "r");
    assert(fd >= 0);
    char buf[3] = {0};
    assert(fs_read(fd, buf, 2) == 2);
    fs_close(fd);
    assert(buf[0] == 'h' && buf[1] == 'i');
    printf("persistence test passed\n");
    return 0;
}

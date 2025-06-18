/*
 * AOS — fs_demo.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "fs.h"
#include <stdio.h>

int main(void) {
    fs_init();
    fs_use_ext2("/tmp/aos_ext2_demo");
    fs_mkdir("docs");
    int fd = fs_open("docs/foo.txt", "w");
    fs_write(fd, "hello", 5);
    fs_close(fd);

    fd = fs_open("docs/foo.txt", "r");
    char buf[16];
    size_t r = fs_read(fd, buf, sizeof(buf) - 1);
    buf[r] = '\0';
    printf("read: %s\n", buf);
    fs_close(fd);

    fs_ls();
    return 0;
}

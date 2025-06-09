#include "fs.h"
#include <stdio.h>

int main(void) {
    fs_init();
    int fd = fs_open("foo.txt", "w");
    fs_write(fd, "hello", 5);
    fs_close(fd);

    fd = fs_open("foo.txt", "r");
    char buf[16];
    size_t r = fs_read(fd, buf, sizeof(buf) - 1);
    buf[r] = '\0';
    printf("read: %s\n", buf);
    fs_close(fd);

    fs_ls();
    return 0;
}

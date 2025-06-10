#include "fs.h"
#include "memory.h"
#include <assert.h>
#include <stdio.h>

int main(void) {
    unsigned char pool[4096];
    memory_init(pool, sizeof(pool));
    fs_init();

    fs_mkdir("d");
    int fd = fs_open("d/file", "w");
    assert(fd >= 0);
    assert(fs_write(fd, "hi", 2) == 2);
    fs_close(fd);

    assert(fs_checkpoint_save("/tmp/fs.chk") == 0);
    fs_init();
    assert(fs_checkpoint_load("/tmp/fs.chk") == 0);

    fd = fs_open("d/file", "r");
    assert(fd >= 0);
    char buf[3] = {0};
    assert(fs_read(fd, buf, 2) == 2);
    fs_close(fd);
    assert(buf[0] == 'h' && buf[1] == 'i');
    printf("fs checkpoint test passed\n");
    return 0;
}

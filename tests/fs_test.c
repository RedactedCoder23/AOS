#include "fs.h"
#include "memory.h"
#include <assert.h>
#include <stdio.h>
#include <sys/stat.h>

int main(void) {
    unsigned char pool[4096];
    memory_init(pool, sizeof(pool));
    fs_init();

    fs_mkdir("d");
    int fd = fs_open("d/file", "w");
    assert(fd >= 0);
    assert(fs_write(fd, "hi", 2) == 2);
    fs_close(fd);

    const char *chk = "tests/tmp/fs.chk";
    mkdir("tests/tmp", 0755);
    assert(fs_checkpoint_save(chk) == 0);
    struct stat st;
    assert(stat(chk, &st) == 0 && st.st_size > 0);
    fs_init();
    assert(fs_checkpoint_load(chk) == 0);

    fd = fs_open("d/file", "r");
    assert(fd >= 0);
    char buf[3] = {0};
    assert(fs_read(fd, buf, 2) == 2);
    fs_close(fd);
    assert(buf[0] == 'h' && buf[1] == 'i');
    printf("fs checkpoint test passed\n");
    return 0;
}

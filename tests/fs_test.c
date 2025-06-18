/* AOS — tests/fs_test.c — Purpose: C source file */
/*
 * AOS — fs_test.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "fs.h"
#include "memory.h"
#include <assert.h>
#include <stdio.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

static void timeout(int _) {
    fprintf(stderr, "[fs_test] timed out\n");
    exit(1);
}

int main(void) {
    signal(SIGALRM, timeout);
    alarm(5);
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
    printf("[fs_test] starting checkpoint_save()\n");
    fflush(stdout);
    int rc = fs_checkpoint_save(chk);
    printf("[fs_test] returned from checkpoint_save(), rc=%d\n", rc);
    fflush(stdout);
    alarm(0);
    assert(rc == 0);
    printf("[fs_test] checking file exists\n");
    fflush(stdout);
    struct stat st;
    assert(stat(chk, &st) == 0 && st.st_size > 0);
    printf("[fs_test] reload fs\n");
    fflush(stdout);
    fs_init();
    printf("[fs_test] loading checkpoint\n");
    fflush(stdout);
    int rc2 = fs_checkpoint_load(chk);
    printf("[fs_test] load rc=%d\n", rc2);
    fflush(stdout);
    assert(rc2 == 0);

    fd = fs_open("d/file", "r");
    assert(fd >= 0);
    char buf[3] = {0};
    assert(fs_read(fd, buf, 2) == 2);
    fs_close(fd);
    assert(buf[0] == 'h' && buf[1] == 'i');
    printf("fs checkpoint test passed\n");
    return 0;
}

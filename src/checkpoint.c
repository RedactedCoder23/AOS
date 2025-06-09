#include "checkpoint.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef __linux__
#include <unistd.h>
#endif

/* Stub implementation. Linux attempts to call CRIU while other
 * platforms fallback to copying files. */

static int write_stub(const char *path, const char *msg) {
    FILE *f = fopen(path, "w");
    if (!f) return -1;
    fputs(msg, f);
    fclose(f);
    return 0;
}

int cp_snapshot(const char *branch, const char *dest) {
#ifdef __linux__
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "criu dump -t %d -D %s/%s --shell-job >/dev/null 2>&1", getpid(), dest, branch);
    int rc = system(cmd);
    return rc;
#else
    char file[256];
    snprintf(file, sizeof(file), "%s/%s.chk", dest, branch);
    return write_stub(file, "snapshot");
#endif
}

int cp_diff(const char *branch, const char *dest) {
    char file[256];
    snprintf(file, sizeof(file), "%s/%s.diff", dest, branch);
    return write_stub(file, "diff");
}

int cp_restore(const char *branch, const char *src) {
#ifdef __linux__
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "criu restore -D %s/%s >/dev/null 2>&1", src, branch);
    return system(cmd);
#else
    char file[256];
    snprintf(file, sizeof(file), "%s/%s.chk", src, branch);
    FILE *f = fopen(file, "r");
    if (!f) return -1;
    fclose(f);
    return 0;
#endif
}

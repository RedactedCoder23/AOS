/* AOS — src/app_runtime.c — Purpose: C source file */
/*
 * AOS — app_runtime.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "app_runtime.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define APP_DIR "packages"

int app_run(const char *name, int argc, char **argv) {
    char path[128];
    snprintf(path, sizeof(path), "%s/%s", APP_DIR, name);
    if (access(path, X_OK) != 0) {
        printf("app %s not found\n", name);
        return -1;
    }
    char **args = calloc(argc + 2, sizeof(char *));
    args[0] = path;
    for (int i = 0; i < argc; i++)
        args[i + 1] = argv[i];
    args[argc + 1] = NULL;
    pid_t pid = fork();
    if (pid == 0) {
        execv(path, args);
        perror("execv");
        _exit(EXIT_FAILURE);
    }
    int status = 0;
    if (pid > 0)
        waitpid(pid, &status, 0);
    free(args);
    return status;
}

void app_list(void) {
    DIR *d = opendir(APP_DIR);
    if (!d) {
        printf("no apps installed\n");
        return;
    }
    struct dirent *de;
    while ((de = readdir(d))) {
        if (de->d_name[0] != '.')
            printf("%s\n", de->d_name);
    }
    closedir(d);
}

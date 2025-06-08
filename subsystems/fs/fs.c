#include "fs.h"
#include <string.h>
#include <stdio.h>

#define MAX_FILES 16
#define MAX_NAME 32
#define MAX_CONTENT 256

static struct { char name[MAX_NAME]; char content[MAX_CONTENT]; } files[MAX_FILES];
static int file_count;

void fs_init() { file_count = 0; }

int fs_write(const char *name, const char *data) {
    if (file_count >= MAX_FILES) return -1;
    strncpy(files[file_count].name, name, MAX_NAME-1);
    strncpy(files[file_count].content, data, MAX_CONTENT-1);
    file_count++;
    return 0;
}

const char *fs_read(const char *name) {
    for (int i=0;i<file_count;i++) if (strcmp(files[i].name,name)==0) return files[i].content;
    return NULL;
}

void fs_list() {
    for (int i=0;i<file_count;i++) printf("%s\n", files[i].name);
}

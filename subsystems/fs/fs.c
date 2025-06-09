#include "fs.h"
#include "branch.h"
/* [2025-06-09 06:06 UTC] In-memory FS subsystem
 * by: codex
 * Edge cases: lacks persistence and access control.
 * Next agent must: see AGENT.md "UNRESOLVED ISSUES".
 */
#include <string.h>
#include <stdio.h>

__attribute__((weak)) int bm_current_id(void) { return 0; }

#define MAX_FILES 16
#define MAX_NAME 32
#define MAX_CONTENT 256

typedef struct File {
    char name[MAX_NAME];
    char content[MAX_CONTENT];
    size_t size;
    size_t pos;
    int used;
    int open;
    int dir; /* 1 if directory */
} File;

static File files[MAX_BRANCHES][MAX_FILES];

static File *cur_files(void) {
    int id = bm_current_id();
    if (id < 0 || id >= MAX_BRANCHES) id = 0;
    return files[id];
}

void fs_init(void) {
    for (int b=0; b<MAX_BRANCHES; b++) {
        memset(files[b], 0, sizeof(files[b]));
        strncpy(files[b][0].name, "/", MAX_NAME-1);
        files[b][0].used = 1;
        files[b][0].dir = 1;
    }
}

static File *find_file(const char *name) {
    File *fs = cur_files();
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs[i].used && strcmp(fs[i].name, name) == 0)
            return &fs[i];
    }
    return NULL;
}

int fs_open(const char *name, const char *mode) {
    File *fs = cur_files();
    File *f = find_file(name);
    if (f && f->dir)
        return -1; /* cannot open directory */
    if (!f) {
        for (int i = 0; i < MAX_FILES; i++) {
            if (!fs[i].used) { f = &fs[i]; break; }
        }
        if (!f) return -1;
        memset(f, 0, sizeof(File));
        strncpy(f->name, name, MAX_NAME-1);
        f->used = 1;
    }
    f->pos = 0;
    f->open = 1;
    if (mode && mode[0] == 'w') f->size = 0;
    return (int)(f - fs);
}

size_t fs_read(int fd, char *buf, size_t size) {
    File *fs = cur_files();
    if (fd < 0 || fd >= MAX_FILES) return 0;
    File *f = &fs[fd];
    if (!f->open) return 0;
    size_t remain = f->size - f->pos;
    if (size > remain) size = remain;
    memcpy(buf, f->content + f->pos, size);
    f->pos += size;
    return size;
}

size_t fs_write(int fd, const char *buf, size_t size) {
    File *fs = cur_files();
    if (fd < 0 || fd >= MAX_FILES) return 0;
    File *f = &fs[fd];
    if (!f->open) return 0;
    if (f->pos + size > MAX_CONTENT) size = MAX_CONTENT - f->pos;
    memcpy(f->content + f->pos, buf, size);
    f->pos += size;
    if (f->pos > f->size) f->size = f->pos;
    return size;
}

void fs_close(int fd) {
    File *fs = cur_files();
    if (fd < 0 || fd >= MAX_FILES) return;
    fs[fd].open = 0;
}

int fs_mkdir(const char *name) {
    File *fs = cur_files();
    if (find_file(name)) return -1;
    for (int i = 0; i < MAX_FILES; i++) {
        if (!fs[i].used) {
            memset(&fs[i], 0, sizeof(File));
            strncpy(fs[i].name, name, MAX_NAME-1);
            fs[i].used = 1;
            fs[i].dir = 1;
            return 0;
        }
    }
    return -1;
}

void fs_ls(void) {
    File *fs = cur_files();
    for (int i = 1; i < MAX_FILES; i++)
        if (fs[i].used)
            printf("%s%s\n", fs[i].name, fs[i].dir ? "/" : "");
}

#include "../subsystems/fs/fs.h"
#include <string.h>
#include <stddef.h>

#define MAX_FILES 16
#define MAX_NAME 32
#define MAX_CONTENT 256

typedef struct {
    char name[MAX_NAME];
    char content[MAX_CONTENT];
    size_t size;
    size_t pos;
    int used;
    int open;
} File;

static File files[MAX_FILES];

void fs_init(void) {
    memset(files, 0, sizeof(files));
}

static File *find_file(const char *name) {
    for (int i=0;i<MAX_FILES;i++)
        if (files[i].used && strcmp(files[i].name, name)==0)
            return &files[i];
    return NULL;
}

int fs_open(const char *name, const char *mode) {
    File *f = find_file(name);
    if (!f) {
        for (int i=0;i<MAX_FILES;i++)
            if (!files[i].used) { f=&files[i]; break; }
        if (!f) return -1;
        memset(f,0,sizeof(File));
        strncpy(f->name,name,MAX_NAME-1);
        f->used=1;
    }
    f->pos=0;
    f->open=1;
    if (mode && mode[0]=='w') f->size=0;
    return (int)(f - files);
}

size_t fs_read(int fd, char *buf, size_t n) {
    if (fd<0 || fd>=MAX_FILES) return 0;
    File *f=&files[fd];
    if (!f->open) return 0;
    size_t remain=f->size - f->pos;
    if (n>remain) n=remain;
    memcpy(buf, f->content + f->pos, n);
    f->pos += n;
    return n;
}

size_t fs_write(int fd, const char *buf, size_t n) {
    if (fd<0 || fd>=MAX_FILES) return 0;
    File *f=&files[fd];
    if (!f->open) return 0;
    if (f->pos + n > MAX_CONTENT) n = MAX_CONTENT - f->pos;
    memcpy(f->content + f->pos, buf, n);
    f->pos += n;
    if (f->pos > f->size) f->size = f->pos;
    return n;
}

void fs_close(int fd) {
    if (fd<0 || fd>=MAX_FILES) return;
    files[fd].open=0;
}

void fs_ls(void) {
    (void)0; /* No console on bare metal */
}

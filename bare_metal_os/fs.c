/*
 * AOS — fs.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "fs_bare.h"
#include <stddef.h>

static void mem_set(void *d, int v, size_t n) {
    unsigned char *p = d;
    for (size_t i = 0; i < n; i++)
        p[i] = v;
}
static void mem_cpy(void *d, const void *s, size_t n) {
    unsigned char *dd = d;
    const unsigned char *ss = s;
    for (size_t i = 0; i < n; i++)
        dd[i] = ss[i];
}
static int str_cmp(const char *a, const char *b) {
    while (*a && *a == *b) {
        a++;
        b++;
    }
    return (unsigned char)*a - (unsigned char)*b;
}
static void str_ncpy(char *d, const char *s, size_t n) {
    size_t i = 0;
    for (; i < n - 1 && s[i]; i++)
        d[i] = s[i];
    if (n)
        d[i] = 0;
}

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
    int dir;
} File;

static File files[MAX_FILES];

void fs_init(void) {
    mem_set(files, 0, sizeof(files));
    str_ncpy(files[0].name, "/", MAX_NAME);
    files[0].used = 1;
    files[0].dir = 1;
}

static File *find_file(const char *name) {
    for (int i = 0; i < MAX_FILES; i++)
        if (files[i].used && str_cmp(files[i].name, name) == 0)
            return &files[i];
    return NULL;
}

int fs_open(const char *name, const char *mode) {
    File *f = find_file(name);
    if (f && f->dir)
        return -1;
    if (!f) {
        for (int i = 0; i < MAX_FILES; i++)
            if (!files[i].used) {
                f = &files[i];
                break;
            }
        if (!f)
            return -1;
        mem_set(f, 0, sizeof(File));
        str_ncpy(f->name, name, MAX_NAME);
        f->used = 1;
    }
    f->pos = 0;
    f->open = 1;
    if (mode && mode[0] == 'w')
        f->size = 0;
    return (int)(f - files);
}

size_t fs_read(int fd, char *buf, size_t n) {
    if (fd < 0 || fd >= MAX_FILES)
        return 0;
    File *f = &files[fd];
    if (!f->open)
        return 0;
    size_t remain = f->size - f->pos;
    if (n > remain)
        n = remain;
    mem_cpy(buf, f->content + f->pos, n);
    f->pos += n;
    return n;
}

size_t fs_write(int fd, const char *buf, size_t n) {
    if (fd < 0 || fd >= MAX_FILES)
        return 0;
    File *f = &files[fd];
    if (!f->open)
        return 0;
    if (f->pos + n > MAX_CONTENT)
        n = MAX_CONTENT - f->pos;
    mem_cpy(f->content + f->pos, buf, n);
    f->pos += n;
    if (f->pos > f->size)
        f->size = f->pos;
    return n;
}

void fs_close(int fd) {
    if (fd < 0 || fd >= MAX_FILES)
        return;
    files[fd].open = 0;
}

int fs_mkdir(const char *name) {
    if (find_file(name))
        return -1;
    for (int i = 0; i < MAX_FILES; i++) {
        if (!files[i].used) {
            mem_set(&files[i], 0, sizeof(File));
            str_ncpy(files[i].name, name, MAX_NAME);
            files[i].used = 1;
            files[i].dir = 1;
            return 0;
        }
    }
    return -1;
}

void fs_ls(void) { (void)0; /* No console on bare metal */ }

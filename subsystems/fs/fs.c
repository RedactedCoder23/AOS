#include "fs.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/* simple VFS layer with pluggable backends */
static FsBackend *current;

void fs_register(FsBackend *backend) {
    current = backend;
    if (current && current->init)
        current->init();
}

void fs_init(void) {
    if (!current)
        fs_use_ramfs();
    if (current && current->init)
        current->init();
}

int fs_open(const char *name, const char *mode) {
    return current->open ? current->open(name, mode) : -1;
}

size_t fs_read(int fd, char *buf, size_t n) {
    return current->read ? current->read(fd, buf, n) : 0;
}

size_t fs_write(int fd, const char *buf, size_t n) {
    return current->write ? current->write(fd, buf, n) : 0;
}

void fs_close(int fd) {
    if (current->close)
        current->close(fd);
}

void fs_ls(void) {
    if (current->ls)
        current->ls();
}

int fs_mkdir(const char *name) { return current->mkdir ? current->mkdir(name) : -1; }

int fs_delete(const char *name) { return current->delete ? current->delete (name) : -1; }

/* --- ramfs backend (existing in-memory demo) --- */
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
} RamFile;

static RamFile rfiles[MAX_FILES];

static void ramfs_init(void) {
    memset(rfiles, 0, sizeof(rfiles));
    strncpy(rfiles[0].name, "/", MAX_NAME - 1);
    rfiles[0].used = 1;
    rfiles[0].dir = 1;
}

static RamFile *ram_find(const char *name) {
    for (int i = 0; i < MAX_FILES; i++)
        if (rfiles[i].used && strcmp(rfiles[i].name, name) == 0)
            return &rfiles[i];
    return NULL;
}

static int ram_open(const char *name, const char *mode) {
    RamFile *f = ram_find(name);
    if (f && f->dir)
        return -1;
    if (!f) {
        for (int i = 0; i < MAX_FILES; i++)
            if (!rfiles[i].used) {
                f = &rfiles[i];
                break;
            }
        if (!f)
            return -1;
        memset(f, 0, sizeof(RamFile));
        strncpy(f->name, name, MAX_NAME - 1);
        f->used = 1;
    }
    f->pos = 0;
    f->open = 1;
    if (mode && mode[0] == 'w')
        f->size = 0;
    return (int)(f - rfiles);
}

static size_t ram_read(int fd, char *buf, size_t n) {
    if (fd < 0 || fd >= MAX_FILES)
        return 0;
    RamFile *f = &rfiles[fd];
    if (!f->open)
        return 0;
    size_t rem = f->size - f->pos;
    if (n > rem)
        n = rem;
    memcpy(buf, f->content + f->pos, n);
    f->pos += n;
    return n;
}

static size_t ram_write(int fd, const char *buf, size_t n) {
    if (fd < 0 || fd >= MAX_FILES)
        return 0;
    RamFile *f = &rfiles[fd];
    if (!f->open)
        return 0;
    if (f->pos + n > MAX_CONTENT)
        n = MAX_CONTENT - f->pos;
    memcpy(f->content + f->pos, buf, n);
    f->pos += n;
    if (f->pos > f->size)
        f->size = f->pos;
    return n;
}

static void ram_close(int fd) {
    if (fd < 0 || fd >= MAX_FILES)
        return;
    rfiles[fd].open = 0;
}

static int ram_mkdir(const char *name) {
    if (ram_find(name))
        return -1;
    for (int i = 0; i < MAX_FILES; i++) {
        if (!rfiles[i].used) {
            memset(&rfiles[i], 0, sizeof(RamFile));
            strncpy(rfiles[i].name, name, MAX_NAME - 1);
            rfiles[i].used = 1;
            rfiles[i].dir = 1;
            return 0;
        }
    }
    return -1;
}

static void ram_ls(void) {
    for (int i = 1; i < MAX_FILES; i++)
        if (rfiles[i].used)
            printf("%s%s\n", rfiles[i].name, rfiles[i].dir ? "/" : "");
}

static int ram_delete(const char *name) {
    RamFile *f = ram_find(name);
    if (!f || f->dir)
        return -1;
    f->used = 0;
    return 0;
}

static FsBackend ram_backend = {ramfs_init, ram_open, ram_read,  ram_write,
                                ram_close,  ram_ls,   ram_mkdir, ram_delete};

void fs_use_ramfs(void) { fs_register(&ram_backend); }

/* --- ext2 backend (simplified host directory) --- */
#define MAX_OPEN 16
static FILE *efiles[MAX_OPEN];
static char root_path[256];

static void ext2_init(void) {
    for (int i = 0; i < MAX_OPEN; i++)
        efiles[i] = NULL;
    mkdir(root_path, 0755);
}

static int ext2_open(const char *name, const char *mode) {
    char p[512];
    snprintf(p, sizeof(p), "%s/%s", root_path, name);
    FILE *f = fopen(p, mode);
    if (!f)
        return -1;
    for (int i = 0; i < MAX_OPEN; i++)
        if (!efiles[i]) {
            efiles[i] = f;
            return i;
        }
    fclose(f);
    return -1;
}

static size_t ext2_read(int fd, char *buf, size_t n) {
    if (fd < 0 || fd >= MAX_OPEN || !efiles[fd])
        return 0;
    return fread(buf, 1, n, efiles[fd]);
}

static size_t ext2_write(int fd, const char *buf, size_t n) {
    if (fd < 0 || fd >= MAX_OPEN || !efiles[fd])
        return 0;
    return fwrite(buf, 1, n, efiles[fd]);
}

static void ext2_close(int fd) {
    if (fd < 0 || fd >= MAX_OPEN || !efiles[fd])
        return;
    fclose(efiles[fd]);
    efiles[fd] = NULL;
}

static int ext2_mkdir(const char *name) {
    char p[512];
    snprintf(p, sizeof(p), "%s/%s", root_path, name);
    return mkdir(p, 0755);
}

static void ext2_ls(void) {
    DIR *d = opendir(root_path);
    if (!d)
        return;
    struct dirent *e;
    while ((e = readdir(d)))
        if (strcmp(e->d_name, ".") && strcmp(e->d_name, ".."))
            printf("%s\n", e->d_name);
    closedir(d);
}

static int ext2_delete(const char *name) {
    char p[512];
    snprintf(p, sizeof(p), "%s/%s", root_path, name);
    return remove(p);
}

static FsBackend ext2_backend = {ext2_init,  ext2_open, ext2_read,  ext2_write,
                                 ext2_close, ext2_ls,   ext2_mkdir, ext2_delete};

void fs_use_ext2(const char *root) {
    strncpy(root_path, root, sizeof(root_path) - 1);
    root_path[sizeof(root_path) - 1] = '\0';
    fs_register(&ext2_backend);
}

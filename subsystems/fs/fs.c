#include "fs.h"
#include "branch.h"
/* [2025-06-09 06:06 UTC] In-memory FS subsystem
 * by: codex
 * Edge cases: lacks persistence and access control.
 * Next agent must: see AGENT.md "UNRESOLVED ISSUES".
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>

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

typedef enum { FS_RAM, FS_DISK } FsBackend;

typedef struct {
    int used;
    FsBackend backend;
    union {
        File *mem;
        FILE *disk;
    } h;
} VHandle;

static VHandle handles[MAX_FILES];
static FsBackend active_backend = FS_RAM;
static char disk_root[PATH_MAX];

static void log_fs_error(const char *msg) {
    FILE *f = fopen("AGENT.md", "a");
    if (f) { fprintf(f, "FS ERROR: %s\n", msg); fclose(f); }
}

void fs_init(void) {
 codex/implement-modular-vfs-with-ramfs-and-ext2
    memset(files, 0, sizeof(files));
    memset(handles, 0, sizeof(handles));
    strncpy(files[0].name, "/", MAX_NAME-1);
    files[0].used = 1;
    files[0].dir = 1;
    const char *home = getenv("HOME");
    if (home) {
        snprintf(disk_root, sizeof(disk_root), "%s/.aosfs", home);
        if (mkdir(disk_root, 0755) < 0 && errno != EEXIST) {
            log_fs_error("cannot create persistent fs root");
            perror("fs_init");
            exit(1);
        }
    } else {
        disk_root[0] = '\0';
=======
    for (int b=0; b<MAX_BRANCHES; b++) {
        memset(files[b], 0, sizeof(files[b]));
        strncpy(files[b][0].name, "/", MAX_NAME-1);
        files[b][0].used = 1;
        files[b][0].dir = 1;
 main
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

codex/implement-modular-vfs-with-ramfs-and-ext2
static int alloc_handle(void) {
    for (int i = 0; i < MAX_FILES; i++)
        if (!handles[i].used) {
            handles[i].used = 1;
            return i;
        }
    return -1;
}

/*************** RAM BACKEND **************/
static int ram_open(const char *name, const char *mode) {
=======
int fs_open(const char *name, const char *mode) {
    File *fs = cur_files();
 main
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
 codex/implement-modular-vfs-with-ramfs-and-ext2
    int h = alloc_handle();
    if (h < 0) return -1;
    handles[h].backend = FS_RAM;
    handles[h].h.mem = f;
    return h;
}

static size_t ram_read(VHandle *vh, char *buf, size_t size) {
    File *f = vh->h.mem;
=======
    return (int)(f - fs);
}

size_t fs_read(int fd, char *buf, size_t size) {
    File *fs = cur_files();
    if (fd < 0 || fd >= MAX_FILES) return 0;
    File *f = &fs[fd];
 main
    if (!f->open) return 0;
    size_t remain = f->size - f->pos;
    if (size > remain) size = remain;
    memcpy(buf, f->content + f->pos, size);
    f->pos += size;
    return size;
}

codex/implement-modular-vfs-with-ramfs-and-ext2
static size_t ram_write(VHandle *vh, const char *buf, size_t size) {
    File *f = vh->h.mem;
=======
size_t fs_write(int fd, const char *buf, size_t size) {
    File *fs = cur_files();
    if (fd < 0 || fd >= MAX_FILES) return 0;
    File *f = &fs[fd];
 main
    if (!f->open) return 0;
    if (f->pos + size > MAX_CONTENT) size = MAX_CONTENT - f->pos;
    memcpy(f->content + f->pos, buf, size);
    f->pos += size;
    if (f->pos > f->size) f->size = f->pos;
    return size;
}

 codex/implement-modular-vfs-with-ramfs-and-ext2
static void ram_close(VHandle *vh) {
    if (vh->h.mem) vh->h.mem->open = 0;
}

static int ram_mkdir(const char *name) {
=======
void fs_close(int fd) {
    File *fs = cur_files();
    if (fd < 0 || fd >= MAX_FILES) return;
    fs[fd].open = 0;
}

int fs_mkdir(const char *name) {
    File *fs = cur_files();
 main
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

 codex/implement-modular-vfs-with-ramfs-and-ext2
static void ram_ls(void) {
=======
void fs_ls(void) {
    File *fs = cur_files();
 main
    for (int i = 1; i < MAX_FILES; i++)
        if (fs[i].used)
            printf("%s%s\n", fs[i].name, fs[i].dir ? "/" : "");
}

static int ram_delete(const char *name) {
    File *f = find_file(name);
    if (!f || f->dir) return -1;
    f->used = 0;
    return 0;
}

/*************** DISK BACKEND **************/
static int disk_open(const char *name, const char *mode) {
    if (!disk_root[0]) return -1;
    char path[PATH_MAX];
    snprintf(path, sizeof(path), "%s/%s", disk_root, name);
    FILE *fp = fopen(path, mode);
    if (!fp) return -1;
    int h = alloc_handle();
    if (h < 0) { fclose(fp); return -1; }
    handles[h].backend = FS_DISK;
    handles[h].h.disk = fp;
    return h;
}

static size_t disk_read(VHandle *vh, char *buf, size_t size) {
    return fread(buf, 1, size, vh->h.disk);
}

static size_t disk_write(VHandle *vh, const char *buf, size_t size) {
    return fwrite(buf, 1, size, vh->h.disk);
}

static void disk_close(VHandle *vh) {
    fclose(vh->h.disk);
}

static int disk_mkdir(const char *name) {
    if (!disk_root[0]) return -1;
    char path[PATH_MAX];
    snprintf(path, sizeof(path), "%s/%s", disk_root, name);
    return mkdir(path, 0755);
}

static void disk_ls(void) {
    if (!disk_root[0]) return;
    DIR *d = opendir(disk_root);
    if (!d) return;
    struct dirent *e;
    while ((e = readdir(d))) {
        if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0) continue;
        if (e->d_type == DT_DIR)
            printf("%s/\n", e->d_name);
        else
            printf("%s\n", e->d_name);
    }
    closedir(d);
}

static int disk_delete(const char *name) {
    if (!disk_root[0]) return -1;
    char path[PATH_MAX];
    snprintf(path, sizeof(path), "%s/%s", disk_root, name);
    return remove(path);
}

/*************** PUBLIC API **************/

int fs_use_persistent(void) {
    if (!disk_root[0]) {
        log_fs_error("HOME not set for persistent fs");
        return -1;
    }
    active_backend = FS_DISK;
    return 0;
}

void fs_use_ramfs(void) {
    active_backend = FS_RAM;
}

int fs_open(const char *name, const char *mode) {
    if (active_backend == FS_DISK) return disk_open(name, mode);
    return ram_open(name, mode);
}

size_t fs_read(int fd, char *buf, size_t size) {
    if (fd < 0 || fd >= MAX_FILES || !handles[fd].used) return 0;
    VHandle *vh = &handles[fd];
    if (vh->backend == FS_DISK) return disk_read(vh, buf, size);
    return ram_read(vh, buf, size);
}

size_t fs_write(int fd, const char *buf, size_t size) {
    if (fd < 0 || fd >= MAX_FILES || !handles[fd].used) return 0;
    VHandle *vh = &handles[fd];
    if (vh->backend == FS_DISK) return disk_write(vh, buf, size);
    return ram_write(vh, buf, size);
}

void fs_close(int fd) {
    if (fd < 0 || fd >= MAX_FILES || !handles[fd].used) return;
    VHandle *vh = &handles[fd];
    if (vh->backend == FS_DISK) disk_close(vh);
    else ram_close(vh);
    handles[fd].used = 0;
}

int fs_mkdir(const char *name) {
    if (active_backend == FS_DISK) return disk_mkdir(name);
    return ram_mkdir(name);
}

void fs_ls(void) {
    if (active_backend == FS_DISK) disk_ls();
    else ram_ls();
}

int fs_delete(const char *name) {
    if (active_backend == FS_DISK) return disk_delete(name);
    return ram_delete(name);
}

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

/* --- ramfs backend rewritten to support directories and dynamic data --- */
#include "memory.h"
#include <time.h>

typedef struct RamNode {
    char *name;
    int dir;
    struct RamNode *parent;
    struct RamNode *sibling;
    struct RamNode *child;
    unsigned char *data;
    size_t size;
    int open;
} RamNode;

typedef struct {
    RamNode *node;
    size_t pos;
    int used;
} RamHandle;

#define MAX_HANDLES 16

static RamNode root_node;
static RamHandle handles[MAX_HANDLES];

static void free_node(RamNode *n) {
    if (!n)
        return;
    while (n->child)
        free_node(n->child);
    if (n->sibling)
        free_node(n->sibling);
    if (n->data)
        mem_free(n->data);
    if (n->name && n != &root_node)
        mem_free(n->name);
    if (n != &root_node)
        mem_free(n);
}

static void ramfs_init(void) {
    free_node(root_node.child);
    memset(&root_node, 0, sizeof(root_node));
    root_node.name = "/";
}

static RamNode *find_child(RamNode *dir, const char *name) {
    for (RamNode *c = dir->child; c; c = c->sibling)
        if (strcmp(c->name, name) == 0)
            return c;
    return NULL;
}

static RamNode *create_child(RamNode *dir, const char *name, int is_dir) {
    RamNode *n = mem_alloc(sizeof(RamNode));
    if (!n)
        return NULL;
    memset(n, 0, sizeof(RamNode));
    n->name = mem_alloc(strlen(name) + 1);
    if (!n->name) {
        mem_free(n);
        return NULL;
    }
    strcpy(n->name, name);
    n->dir = is_dir;
    n->parent = dir;
    n->sibling = dir->child;
    dir->child = n;
    return n;
}

static int next_segment(const char **path, char *seg, size_t max) {
    const char *p = *path;
    if (!*p)
        return 0;
    while (*p == '/')
        p++;
    size_t len = 0;
    while (p[len] && p[len] != '/')
        len++;
    if (len >= max)
        return -1;
    memcpy(seg, p, len);
    seg[len] = '\0';
    *path = p[len] ? p + len + 1 : p + len;
    return 1;
}

static RamNode *resolve_path(const char *path, int create, int dir) {
    char seg[64];
    RamNode *cur = &root_node;
    while (next_segment(&path, seg, sizeof(seg)) > 0) {
        RamNode *child = find_child(cur, seg);
        if (!child) {
            if (!create)
                return NULL;
            child = create_child(cur, seg, *path ? 1 : dir);
            if (!child)
                return NULL;
        }
        cur = child;
        if (*path && !cur->dir)
            return NULL;
    }
    return cur;
}

static int alloc_handle(RamNode *n) {
    for (int i = 0; i < MAX_HANDLES; i++)
        if (!handles[i].used) {
            handles[i].used = 1;
            handles[i].node = n;
            handles[i].pos = 0;
            n->open++;
            return i;
        }
    return -1;
}

static void release_handle(int fd) {
    if (fd < 0 || fd >= MAX_HANDLES || !handles[fd].used)
        return;
    handles[fd].node->open--;
    handles[fd].used = 0;
}

static int ram_open(const char *name, const char *mode) {
    int write_mode = mode && mode[0] == 'w';
    RamNode *n = resolve_path(name, write_mode, 0);
    if (!n || n->dir)
        return -1;
    if (write_mode) {
        if (n->data)
            mem_free(n->data);
        n->data = NULL;
        n->size = 0;
    }
    return alloc_handle(n);
}

static size_t ram_read(int fd, char *buf, size_t n) {
    if (fd < 0 || fd >= MAX_HANDLES || !handles[fd].used)
        return 0;
    RamNode *node = handles[fd].node;
    if (node->dir)
        return 0;
    size_t rem = node->size - handles[fd].pos;
    if (n > rem)
        n = rem;
    memcpy(buf, node->data + handles[fd].pos, n);
    handles[fd].pos += n;
    return n;
}

static size_t ram_write(int fd, const char *buf, size_t n) {
    if (fd < 0 || fd >= MAX_HANDLES || !handles[fd].used)
        return 0;
    RamNode *node = handles[fd].node;
    if (node->dir)
        return 0;
    size_t need = handles[fd].pos + n;
    if (need > node->size) {
        unsigned char *d = mem_alloc(need);
        if (!d)
            return 0;
        if (node->data) {
            memcpy(d, node->data, node->size);
            mem_free(node->data);
        }
        node->data = d;
        node->size = need;
    }
    memcpy(node->data + handles[fd].pos, buf, n);
    handles[fd].pos += n;
    if (handles[fd].pos > node->size)
        node->size = handles[fd].pos;
    return n;
}

static void ram_close(int fd) { release_handle(fd); }

static int ram_mkdir(const char *name) {
    RamNode *n = resolve_path(name, 1, 1);
    return n ? 0 : -1;
}

static void list_dir(RamNode *d, const char *prefix) {
    for (RamNode *c = d->child; c; c = c->sibling) {
        printf("%s%s%s\n", prefix, c->name, c->dir ? "/" : "");
        if (c->dir) {
            char next[256];
            snprintf(next, sizeof(next), "%s%s/", prefix, c->name);
            list_dir(c, next);
        }
    }
}

static void ram_ls(void) { list_dir(&root_node, ""); }

static int remove_child(RamNode *n) {
    if (!n || n == &root_node)
        return -1;
    if (n->open || n->child)
        return -1;
    RamNode *p = n->parent;
    RamNode **link = &p->child;
    while (*link && *link != n)
        link = &(*link)->sibling;
    if (*link)
        *link = n->sibling;
    if (n->data)
        mem_free(n->data);
    mem_free(n->name);
    mem_free(n);
    return 0;
}

static int ram_delete(const char *name) {
    RamNode *n = resolve_path(name, 0, 0);
    if (!n || n->dir)
        return -1;
    return remove_child(n);
}

static void dump_node(FILE *f, RamNode *n, const char *prefix) {
    char cur[256];
    if (n != &root_node) {
        snprintf(cur, sizeof(cur), "%s%s", prefix, n->name);
        if (n->dir)
            fprintf(f, "D %s\n", cur);
        else {
            fprintf(f, "F %s ", cur);
            for (size_t i = 0; i < n->size; i++)
                fprintf(f, "%02x", n->data[i]);
            fprintf(f, "\n");
        }
        snprintf(cur, sizeof(cur), "%s%s/", prefix, n->name);
    } else {
        strncpy(cur, prefix, sizeof(cur));
    }
    for (RamNode *c = n->child; c; c = c->sibling)
        dump_node(f, c, cur);
}

static int ram_cp_save(const char *file) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "python3 scripts/aos_checkpoint.py write %s", file);
    FILE *p = popen(cmd, "w");
    if (!p)
        return -1;
    dump_node(p, &root_node, "");
    int rc = pclose(p);
    return rc == 0 ? 0 : -1;
}

static int ram_cp_load(const char *file) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "python3 scripts/aos_checkpoint.py read %s", file);
    FILE *p = popen(cmd, "r");
    if (!p)
        return -1;
    ramfs_init();
    char line[1024];
    while (fgets(line, sizeof(line), p)) {
        if (line[0] == 'D') {
            char *path = line + 2;
            path[strcspn(path, "\n")] = '\0';
            fs_mkdir(path);
        } else if (line[0] == 'F') {
            char *path = line + 2;
            char *sp = strchr(path, ' ');
            if (!sp)
                continue;
            *sp = '\0';
            char *hex = sp + 1;
            hex[strcspn(hex, "\n")] = '\0';
            size_t len = strlen(hex) / 2;
            unsigned char *data = mem_alloc(len);
            if (!data)
                continue;
            for (size_t i = 0; i < len; i++) {
                unsigned int v;
                sscanf(hex + 2 * i, "%02x", &v);
                data[i] = v;
            }
            int fd = fs_open(path, "w");
            if (fd >= 0) {
                fs_write(fd, (char *)data, len);
                fs_close(fd);
            }
            mem_free(data);
        }
    }
    int rc = pclose(p);
    return rc == 0 ? 0 : -1;
}

static FsBackend ram_backend = {ramfs_init,  ram_open, ram_read,  ram_write,
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

static void mkdir_p_path(const char *path) {
    char tmp[512];
    strncpy(tmp, path, sizeof(tmp));
    for (char *p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            mkdir(tmp, 0755);
            *p = '/';
        }
    }
}

int fs_checkpoint_save(const char *file) {
    mkdir_p_path(file);
    return ram_cp_save(file);
}

int fs_checkpoint_load(const char *file) { return ram_cp_load(file); }

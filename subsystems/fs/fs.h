/* AOS — subsystems/fs/fs.h — Purpose: Header file */
/*
 * AOS — fs.h
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#ifndef FS_H
#define FS_H
#include <stddef.h>

typedef struct FsBackend {
    void (*init)(void);
    int (*open)(const char *name, const char *mode);
    size_t (*read)(int fd, char *buf, size_t size);
    size_t (*write)(int fd, const char *buf, size_t size);
    void (*close)(int fd);
    void (*ls)(void);
    int (*mkdir)(const char *name);
    int (*delete)(const char *name);
} FsBackend;

void fs_register(FsBackend *backend);
void fs_init(void);
int fs_open(const char *name, const char *mode);
size_t fs_read(int fd, char *buf, size_t size);
size_t fs_write(int fd, const char *buf, size_t size);
void fs_close(int fd);
void fs_ls(void);
int fs_mkdir(const char *name);
int fs_delete(const char *name);
void fs_use_ramfs(void);
void fs_use_ext2(const char *root);
int fs_checkpoint_save(const char *file);
int fs_checkpoint_load(const char *file);

#endif

#ifndef AOS_BM_FS_H
#define AOS_BM_FS_H
#include <stddef.h>

void fs_init(void);
int fs_open(const char *name, const char *mode);
size_t fs_read(int fd, char *buf, size_t n);
size_t fs_write(int fd, const char *buf, size_t n);
void fs_close(int fd);
void fs_ls(void);
int fs_mkdir(const char *name);

#endif

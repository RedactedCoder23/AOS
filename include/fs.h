#ifndef FS_H
#define FS_H

#include <stddef.h>

typedef struct FSFile {
    const char *name;
    const char *data;
    size_t size;
} FSFile;

void fs_init(void);
const FSFile *fs_open(const char *name);
size_t fs_read(const FSFile *file, char *buf, size_t maxlen);

#endif

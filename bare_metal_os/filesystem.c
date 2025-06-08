#include "fs.h"
#include <string.h>

static FSFile ramdisk[] = {
    { "hello.txt", "Hello from FS\n", 15 },
    { NULL, NULL, 0 }
};

void fs_init(void) {}

const FSFile *fs_open(const char *name) {
    for (FSFile *f = ramdisk; f->name; f++) {
        if (strcmp(f->name, name) == 0)
            return f;
    }
    return NULL;
}

size_t fs_read(const FSFile *file, char *buf, size_t maxlen) {
    if (!file) return 0;
    if (file->size < maxlen)
        maxlen = file->size;
    memcpy(buf, file->data, maxlen);
    return maxlen;
}

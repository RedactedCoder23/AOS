#ifndef FS_H
#define FS_H

void fs_init();
int fs_write(const char *name, const char *data);
const char *fs_read(const char *name);
void fs_list();

#endif

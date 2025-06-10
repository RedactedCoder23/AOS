#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stddef.h>

#define SYS_CREATE_BRANCH  0x30
#define SYS_MERGE_BRANCH   0x31
#define SYS_LIST_BRANCHES  0x32

int sys_create_branch(void);
int sys_merge_branch(int branch_id);
int sys_list_branches(char *out, size_t outsz);

#endif /* SYSCALLS_H */

#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stddef.h>
#include <stdint.h>

#define SYS_CREATE_BRANCH  0x30
#define SYS_MERGE_BRANCH   0x31
#define SYS_LIST_BRANCHES  0x32
#define SYS_SNAPSHOT_BRANCH 0x33

int sys_create_branch(void);
int sys_merge_branch(int branch_id);
int sys_list_branches(char *out, size_t outsz);
uint64_t sys_snapshot_branch(unsigned int branch_id);

#endif /* SYSCALLS_H */

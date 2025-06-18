/*
 * AOS — syscalls.h
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stddef.h>
#include <stdint.h>



int sys_create_branch(void);
int sys_merge_branch(int branch_id);
int sys_list_branches(char *out, size_t outsz);
uint64_t sys_snapshot_branch(unsigned int branch_id);
int sys_delete_branch(unsigned int branch_id);

#endif /* SYSCALLS_H */

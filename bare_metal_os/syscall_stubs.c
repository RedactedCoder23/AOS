/* AOS — bare_metal_os/syscall_stubs.c — Purpose: minimal syscall stubs */
#include "../include/syscalls.h"

int sys_create_branch(void) { return -1; }
int sys_merge_branch(int branch_id) { (void)branch_id; return -1; }
int sys_list_branches(char *out, size_t outsz) { (void)out;(void)outsz; return -1; }
uint64_t sys_snapshot_branch(unsigned int branch_id) { (void)branch_id; return (uint64_t)-1; }
int sys_delete_branch(unsigned int branch_id) { (void)branch_id; return -1; }

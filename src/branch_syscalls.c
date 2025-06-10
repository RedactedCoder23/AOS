#include "syscalls.h"
#include <errno.h>
#include <string.h>

int sys_create_branch(void) {
    /* TODO: implement */
    return -ENOSYS;
}

int sys_merge_branch(int branch_id) {
    (void)branch_id;
    /* TODO: implement */
    return -ENOSYS;
}

int sys_list_branches(char *out, size_t outsz) {
    if (out && outsz)
        out[0] = '\0';
    /* TODO: implement */
    return 0;
}

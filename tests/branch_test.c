#include "branch_manager.h"
#include <stdio.h>
#include <stdlib.h>

static void dummy_func(void *arg) {
    (void)arg;
    printf("dummy_func\n");
}

int main(void) {
    printf("starting branch tests\n");
    branch_t *b = branch_create(dummy_func, NULL);
    if (!b) {
        fprintf(stderr, "branch_create failed\n");
        abort();
    }
    branch_t *child = branch_fork(b);
    if (!child) {
        fprintf(stderr, "branch_fork failed\n");
        abort();
    }
    branch_join(child);
    branch_join(b);
    printf("branch tests done\n");
    return 0;
}

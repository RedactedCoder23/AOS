#include "branch.h"
#include <stdio.h>

void branch_manager_init(void) {}

void branch_force_open(const char *name) {
    printf("Force opening branch: %s\n", name);
}

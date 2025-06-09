#include "policy.h"
#include <stdio.h>

int main(void) {
    policy_load_file("examples/policy_rules.yaml");
    if (policy_check_ctx("default", "repl", "FS_OPEN"))
        printf("allowed\n");
    if (!policy_check_ctx("default", "repl", "FS_WRITE"))
        printf("denied\n");
    return 0;
}

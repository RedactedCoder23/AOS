#include "policy.h"
#include <assert.h>
#include <stdio.h>

int main(void) {
    const char *yaml = "- branch: main\n  app: repl\n  syscall: FS_WRITE\n  deny: true\n";
    policy_load(yaml);
    assert(policy_check_ctx("main", "repl", "FS_WRITE") == 0);
    assert(policy_check_ctx("main", "repl", "FS_READ") == 1);
    printf("policy tests passed\n");
    return 0;
}

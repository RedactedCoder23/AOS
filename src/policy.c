#include "policy.h"
#include <stdio.h>

static const char *policy;

void policy_load(const char *script) {
    policy = script;
    printf("policy loaded: %s\n", script ? script : "");
}

int policy_check(const char *action) {
    (void)action;
    return 1;
}

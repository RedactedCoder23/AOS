#include "policy.h"
#include <stdio.h>
#include <string.h>

static char deny_action[32];

void policy_load(const char *script) {
    deny_action[0] = '\0';
    if (script && strncmp(script, "deny ", 5) == 0) {
        strncpy(deny_action, script + 5, sizeof(deny_action) - 1);
    }
    printf("policy loaded: %s\n", script ? script : "");
}

int policy_check(const char *action) {
    if (deny_action[0] && action && strcmp(action, deny_action) == 0) {
        FILE *f = fopen("AOS-CHECKLIST.log", "a");
        if (f) { fprintf(f, "policy deny %s\n", action); fclose(f); }
        return 0;
    }
    return 1;
}

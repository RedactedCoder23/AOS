#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CMD_LEN 128

void handle_command(const char *in) {
    char buf[MAX_CMD_LEN];
    if (strlen(in) >= MAX_CMD_LEN) {
        fprintf(stderr, "cmd too long\n");
        return;
    }
    strcpy(buf, in);

    uint64_t mask = strtoull(buf, NULL, 2);
    (void)mask;
    // dispatch logic goes here
}

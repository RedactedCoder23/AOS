#include "ai.h"
#include <stdio.h>

void ai_init(void) {}

void ai_request(const char *prompt, int remote) {
    if (remote) {
        printf("AI remote: %s\n", prompt);
    } else {
        printf("AI local: %s\n", prompt);
    }
}

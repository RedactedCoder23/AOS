#include "ai.h"
#include <stdio.h>
#include <string.h>

static char buffer[256];

const char *ai_reply(const char *prompt) {
    snprintf(buffer, sizeof(buffer), "You said: %s", prompt);
    return buffer;
}

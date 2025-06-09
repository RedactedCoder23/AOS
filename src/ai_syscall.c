#include "ai.h"
#include <stdio.h>

static int service_id;

int ai_service_create(const char *type, const char *params) {
    printf("created AI service %d type=%s params=%s\n", service_id, type ? type : "", params ? params : "");
    return service_id++;
}

void ai_service_monitor(int id) {
    printf("AI service %d running\n", id);
}

int ai_infer(const char *prompt, char *out, unsigned long outsz) {
    (void)prompt;
    if (!out || outsz == 0) return -1;
    snprintf(out, outsz, "NOT IMPLEMENTED");
    return 0;
}

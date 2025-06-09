#include "ai.h"
/* [2025-06-09 06:06 UTC] AI syscall stub bridging to userland
 * by: codex
 * Edge cases: buffer size fixed; service IDs not persisted.
 * Next agent must: implement real inference backend and error reporting.
 */
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
    if (!prompt || !out || outsz == 0)
        return -1;

    char cmd[512];
    /* call external Python backend with escaped prompt */
    snprintf(cmd, sizeof(cmd), "scripts/ai_backend.py \"%s\"", prompt);

    FILE *fp = popen(cmd, "r");
    if (!fp) {
        snprintf(out, outsz, "error: popen failed");
        return -1;
    }
    size_t n = fread(out, 1, outsz - 1, fp);
    out[n] = '\0';
    int rc = pclose(fp);
    if (rc != 0)
        return -1;
    return 0;
}

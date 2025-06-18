/*
 * AOS — ai_syscall.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "ai.h"
/* [2025-06-09 06:06 UTC] AI syscall stub bridging to userland
 * by: codex
 * Edge cases: buffer size fixed; service IDs not persisted.
 * See AGENT.md "Open Issues" for pending work.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int service_id;

int ai_service_create(const char *type, const char *params) {
    printf("created AI service %d type=%s params=%s\n", service_id, type ? type : "",
           params ? params : "");
    return service_id++;
}

void ai_service_monitor(int id) { printf("AI service %d running\n", id); }

int ai_infer(const char *prompt, char *out, unsigned long outsz) {
    if (!prompt || !out || outsz == 0)
        return -1;

    ai_config_load();
    const char *prov = ai_get_provider();
    const char *key = ai_get_param();
    if (!prov[0])
        prov = "openai"; /* default */
    if (strcmp(prov, "openai") == 0) {
        if (!key) {
            if (outsz > 0)
                snprintf(out, outsz, "missing OPENAI_API_KEY");
            FILE *log = fopen("AGENT.md", "a");
            if (log) {
                fprintf(log, "AI error: missing OPENAI_API_KEY\n");
                fclose(log);
            }
            return -1;
        }
        setenv("OPENAI_API_KEY", key, 1);
    } else {
        if (outsz > 0)
            snprintf(out, outsz, "provider %s not supported", prov);
        return -1;
    }

    char cmd[512];
    /* call external Python backend with escaped prompt */
    snprintf(cmd, sizeof(cmd), "scripts/ai_backend.py \"%s\" 2>&1", prompt);

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    FILE *fp = popen(cmd, "r");
    if (!fp) {
        snprintf(out, outsz, "error: popen failed");
        FILE *log = fopen("AGENT.md", "a");
        if (log) {
            fprintf(log, "AI error: popen failed\n");
            fclose(log);
        }
        return -1;
    }
    size_t n = fread(out, 1, outsz - 1, fp);
    out[n] = '\0';
    int rc = pclose(fp);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    long ms = (t1.tv_sec - t0.tv_sec) * 1000 + (t1.tv_nsec - t0.tv_nsec) / 1000000;
    FILE *log = fopen("AGENT.md", "a");
    if (log) {
        fprintf(log, "AI latency %ld ms\n", ms);
        if (rc != 0)
            fprintf(log, "AI backend error rc=%d output=%s\n", rc, out);
        fclose(log);
    }
    if (rc != 0)
        return -1;
    return 0;
}

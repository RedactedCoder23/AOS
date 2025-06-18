/*
 * AOS — audit.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "audit.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static void write_line(const char *path, const char *line) {
    FILE *f = fopen(path, "a");
    if (!f)
        return;
    fputs(line, f);
    fputc('\n', f);
    fclose(f);
}

void audit_log_entry(const char *user, const char *action, const char *resource,
                     const char *result) {
    char ts[32];
    time_t now = time(NULL);
    strftime(ts, sizeof(ts), "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));
    char buf[256];
    snprintf(buf, sizeof(buf),
             "{\"timestamp\":\"%s\",\"user\":\"%s\",\"action\":\"%s\","
             "\"resource\":\"%s\",\"result\":\"%s\"}",
             ts, user ? user : "", action ? action : "", resource ? resource : "",
             result ? result : "");
    write_line("/var/log/aos-audit.log", buf);
}

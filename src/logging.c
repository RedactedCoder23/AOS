/*
 * AOS — logging.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "logging.h"
#include <stdarg.h>

static FILE *log_fp = NULL;

void log_init(FILE *dest) { log_fp = dest ? dest : stderr; }

void log_message(LogLevel level, const char *fmt, ...) {
    if (!log_fp)
        log_init(NULL);
    const char *pfx = level == LOG_ERROR ? "ERR" : (level == LOG_WARN ? "WARN" : "INFO");
    fprintf(log_fp, "%s: ", pfx);
    va_list ap;
    va_start(ap, fmt);
    vfprintf(log_fp, fmt, ap);
    va_end(ap);
    fputc('\n', log_fp);
    fflush(log_fp);
}

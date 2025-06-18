/* AOS — src/profiler.c — Purpose: C source file */
/*
 * AOS — profiler.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
/* Profiler implementation stub. Measures elapsed time between start and report.
 * Integration with the logging subsystem is planned. */

#include "profiler.h"
#include "logging.h"
#include <stdio.h>
#include <time.h>

static clock_t start_time;

void profiler_start(void) { start_time = clock(); }

void profiler_stop(void) { /* noop for now */ }

void profiler_report(const char *label) {
    clock_t end = clock();
    double secs = (double)(end - start_time) / CLOCKS_PER_SEC;
    log_message(LOG_INFO, "[profiler] %s: %.3fs", label, secs);
}

/* Future work: integrate with logging and store multiple samples. */

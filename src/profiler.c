#include "profiler.h"
#include <time.h>
#include <stdio.h>

static clock_t start_time;

void profiler_start(void) {
    start_time = clock();
}

void profiler_stop(void) {
    /* noop for now */
}

void profiler_report(const char *label) {
    clock_t end = clock();
    double secs = (double)(end - start_time) / CLOCKS_PER_SEC;
    printf("[profiler] %s: %.3fs\n", label, secs);
}

/* Future work: integrate with logging and store multiple samples. */

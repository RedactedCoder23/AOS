/* AOS — tests/unit/test_logging.c — Purpose: C source file */
/*
 * AOS — test_logging.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "logging.h"
#include <stdio.h>
int main(void) {
    log_init(stdout);
    log_message(LOG_INFO, "log ok");
    log_message(LOG_WARN, "warn ok");
    log_message(LOG_ERROR, "error ok");
    return 0;
}

/*
 * AOS — security.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "security.h"
#include "logging.h"

void security_init(void) { log_message(LOG_INFO, "security subsystem init (stub)"); }

int check_capability(const char *cap) {
    log_message(LOG_INFO, "checking cap %s (stub)", cap);
    return 0;
}

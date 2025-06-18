/*
 * AOS — test_security.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "security.h"
#include <assert.h>
int main(void) {
    security_init();
    assert(check_capability("none") == 0);
    return 0;
}

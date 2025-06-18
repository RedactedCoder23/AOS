/*
 * AOS — test_dev.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "dev.h"
#include <assert.h>
int main(void) {
    driver_register("dummy");
    driver_list();
    assert(1 && "dev skeleton");
    return 0;
}

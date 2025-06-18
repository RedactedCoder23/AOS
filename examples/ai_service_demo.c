/* AOS — examples/ai_service_demo.c — Purpose: C source file */
/*
 * AOS — ai_service_demo.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "ai.h"
int main(void) {
    int id = ai_service_create("test", "params");
    ai_service_monitor(id);
    return 0;
}

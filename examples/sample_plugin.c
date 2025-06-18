/* AOS — examples/sample_plugin.c — Purpose: C source file */
/*
 * AOS — sample_plugin.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include <stdio.h>

int plugin_init(void) {
    printf("sample plugin init\n");
    return 0;
}

void plugin_exec(void) { printf("sample plugin executed\n"); }

void plugin_cleanup(void) { printf("sample plugin cleanup\n"); }

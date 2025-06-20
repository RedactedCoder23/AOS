/* AOS — examples/plugin_demo.c — Purpose: C source file */
/*
 * AOS — plugin_demo.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "plugin.h"
#include <stdio.h>
int main(void) {
    plugin_install("local");
    plugin_load("sample");
    /* attempt to load a missing plugin to trigger error */
    if (plugin_load("missing") != 0)
        printf("missing plugin handled\n");
    plugin_list();
    plugin_unload("sample");
    plugin_list();
    return 0;
}

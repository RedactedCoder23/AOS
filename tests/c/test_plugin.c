/* AOS — tests/c/test_plugin.c — Purpose: C source file */
/*
 * AOS — test_plugin.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "plugin.h"
#include <assert.h>
#include <stdio.h>

int main(void) {
    assert(plugin_load("sample") == 0);
    assert(plugin_exec("sample") == 0);
    assert(plugin_unload("sample") == 0);
    printf("plugin tests passed\n");
    return 0;
}

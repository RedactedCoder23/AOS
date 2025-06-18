/*
 * AOS — plugin_supervisor.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "plugin_supervisor.h"
#include "wasm_runtime.h"
#include <stdio.h>

int ps_load(const char *path) {
    wasm_load_module(path);
    printf("supervisor loaded %s\n", path);
    return 0;
}

int ps_unload(const char *name) {
    printf("supervisor unloaded %s\n", name);
    return 0;
}

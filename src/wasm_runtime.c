#include "wasm_runtime.h"
#include <stdio.h>

static int initialised;

int wasm_runtime_init(void) {
    initialised = 1;
    printf("wasm runtime initialised\n");
    return 0;
}

int wasm_load_module(const char *path) {
    if (!initialised) wasm_runtime_init();
    printf("loading wasm module %s\n", path);
    return 0; /* stub */
}

int wasm_invoke(const char *func) {
    if (!initialised) return -1;
    printf("invoke %s\n", func);
    return 0;
}

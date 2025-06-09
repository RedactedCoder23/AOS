#ifndef WASM_RUNTIME_H
#define WASM_RUNTIME_H

/* Minimal stub of a WASM runtime interface. */
int wasm_runtime_init(void);
int wasm_load_module(const char *path);
int wasm_invoke(const char *func);

#endif

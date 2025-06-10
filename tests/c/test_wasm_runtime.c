#include "wasm_runtime.h"
#include <assert.h>
int main(void) {
    wasm_runtime_init();
    assert(1 && "wasm runtime skeleton");
    return 0;
}

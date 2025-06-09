#include <assert.h>
#include <stdio.h>
#include "memory.h"

int main(void) {
    unsigned char pool[128];
    memory_init(pool, sizeof(pool));
    void *a = memory_alloc(32);
    assert(a && "alloc failed");
    memory_free(a);
    memory_reset();
    printf("memory unit tests passed\n");
    return 0;
}

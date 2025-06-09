#include <stdio.h>
#include "memory.h"

static unsigned char pool[1024];

int main(void) {
    memory_init(pool, sizeof(pool));
    void *a = memory_alloc(64);
    void *b = memory_alloc(128);
    printf("a=%p b=%p\n", a, b);
    memory_free(b);
    void *c = memory_alloc(32);
    printf("c=%p (reused)\n", c);
    return 0;
}

#include <stdio.h>
#include "memory.h"

static unsigned char pool[1024];

int main(void) {
    memory_init(pool, sizeof(pool));
    void *a = memory_alloc(128);
    void *b = memory_alloc(256);
    printf("alloc a=%p b=%p\n", a, b);
    memory_reset();
    void *c = memory_alloc(512);
    printf("after reset c=%p\n", c);
    return 0;
}

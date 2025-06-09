#include "memory.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
    unsigned char pool[4096];
    memory_init(pool, sizeof(pool));
    void *ptrs[128] = {0};
    srand(0);
    for (int i = 0; i < 1000; i++) {
        int idx = rand() % 128;
        if (ptrs[idx]) {
            memory_free(ptrs[idx]);
            ptrs[idx] = NULL;
        } else {
            size_t s = (rand() % 64) + 1;
            ptrs[idx] = memory_alloc(s);
        }
    }
    for (int i = 0; i < 128; i++)
        if (ptrs[i])
            memory_free(ptrs[i]);
    printf("memory fuzz test passed\n");
    return 0;
}

/* AOS — examples/aicell_client.c — Purpose: C source file */
/*
 * AOS — aicell_client.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "aicell.h"
#include <stdio.h>

int main(void) {
    struct ai_ring *ring;
    if (aicell_open(&ring, 0) != 0) {
        perror("aicell_open");
        return 1;
    }
    aicell_send(ring, "hello");
    char resp[256];
    if (aicell_recv(ring, resp, sizeof(resp)) == 0)
        printf("resp: %s\n", resp);
    aicell_close(ring);
    return 0;
}

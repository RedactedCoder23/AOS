#include "aicell.h"
#include <stdio.h>
#include <unistd.h>

int main(void) {
    struct ai_ring *ring;
    if (aicell_open(&ring, 1) != 0) {
        perror("aicell_open");
        return 1;
    }
    char msg[256];
    while (1) {
        if (aicell_recv(ring, msg, sizeof(msg)) == 0) {
            printf("AI cell got: %s\n", msg);
            aicell_send(ring, "ok");
        }
        usleep(100000);
    }
    aicell_close(ring);
    return 0;
}

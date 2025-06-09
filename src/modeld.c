#include <stdio.h>
#include <unistd.h>
#include "aicell.h"

/* aos-modeld daemon: tracks models via simple IPC. */
int main(void) {
    struct ai_ring *ring;
    if (aicell_open(&ring, 0) != 0) {
        perror("aicell_open");
        return 1;
    }
    char buf[256];
    while (1) {
        if (aicell_recv(ring, buf, sizeof(buf)) == 0)
            printf("modeld req: %s\n", buf);
        usleep(100000);
    }
    aicell_close(ring);
    return 0;
}

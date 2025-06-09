#ifndef AICELL_H
#define AICELL_H
#include <stddef.h>

/* Shared-memory ring buffer for AI tasks. */

struct ai_ring {
    char buffer[1024];
    size_t head;
    size_t tail;
};

int aicell_open(struct ai_ring **ring, int create);
void aicell_close(struct ai_ring *ring);
int aicell_send(struct ai_ring *ring, const char *msg);
int aicell_recv(struct ai_ring *ring, char *out, size_t outsz);

#endif

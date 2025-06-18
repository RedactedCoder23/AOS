/* AOS — src/aicell.c — Purpose: C source file */
/*
 * AOS — aicell.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "aicell.h"
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

/* Minimal shared-memory ring buffer with busy wait polling. */

int aicell_open(struct ai_ring **ring, int create) {
    int flags = O_RDWR;
    if (create)
        flags |= O_CREAT;
    int fd = shm_open("/aos_aicell", flags, 0666);
    if (fd < 0)
        return -1;
    if (create)
        ftruncate(fd, sizeof(struct ai_ring));
    *ring = mmap(NULL, sizeof(struct ai_ring), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (*ring == MAP_FAILED)
        return -1;
    if (create) {
        (*ring)->head = (*ring)->tail = 0;
        memset((*ring)->buffer, 0, sizeof((*ring)->buffer));
    }
    return 0;
}

void aicell_close(struct ai_ring *ring) { munmap(ring, sizeof(struct ai_ring)); }

static size_t next_pos(size_t pos) { return (pos + 1) % sizeof(((struct ai_ring *)0)->buffer); }

int aicell_send(struct ai_ring *ring, const char *msg) {
    size_t len = strlen(msg) + 1;
    for (size_t i = 0; i < len; i++) {
        size_t next = next_pos(ring->tail);
        if (next == ring->head)
            return -1; /* full */
        ring->buffer[ring->tail] = msg[i];
        ring->tail = next;
    }
    return 0;
}

int aicell_recv(struct ai_ring *ring, char *out, size_t outsz) {
    if (ring->head == ring->tail)
        return -1; /* empty */
    size_t i = 0;
    while (ring->head != ring->tail && i + 1 < outsz) {
        out[i++] = ring->buffer[ring->head];
        ring->head = next_pos(ring->head);
        if (out[i - 1] == '\0')
            break;
    }
    if (i == outsz)
        out[outsz - 1] = '\0';
    return 0;
}

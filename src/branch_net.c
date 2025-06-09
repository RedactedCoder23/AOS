#include "branch.h"
#include <stdio.h>
#include <string.h>

#define MAX_PEERS 4
static char peers[MAX_PEERS][64];
static int peer_count;

int br_peer_add(const char *addr) {
    if (peer_count >= MAX_PEERS) return -1;
    strncpy(peers[peer_count], addr ? addr : "", 63);
    peers[peer_count][63] = '\0';
    printf("added peer %s\n", peers[peer_count]);
    peer_count++;
    return 0;
}

int br_sync(void) {
    printf("Syncing with %d peers\n", peer_count);
    return 0;
}

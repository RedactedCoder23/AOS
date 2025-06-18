/* AOS — examples/branch_fed_demo.c — Purpose: C source file */
/*
 * AOS — branch_fed_demo.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "branch.h"
#include <arpa/inet.h>
#include <getopt.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static void *server(void *arg) {
    int port = *(int *)arg;
    int s = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(s, (struct sockaddr *)&addr, sizeof(addr));
    char buf[2048];
    struct sockaddr_in cli;
    socklen_t slen = sizeof(cli);
    if (recvfrom(s, buf, sizeof(buf) - 1, 0, (struct sockaddr *)&cli, &slen) > 0) {
        sendto(s, "PONG", 4, 0, (struct sockaddr *)&cli, slen);
        char *json = bm_graph_export_json();
        sendto(s, json, strlen(json), 0, (struct sockaddr *)&cli, slen);
        free(json);
        int n = recvfrom(s, buf, sizeof(buf) - 1, 0, (struct sockaddr *)&cli, &slen);
        if (n > 0) {
            buf[n] = '\0';
            bm_graph_merge_json(buf);
        }
    }
    close(s);
    return NULL;
}

int main(int argc, char **argv) {
    int port = 9999;
    char *peers[8];
    int pc = 0;
    int do_sync = 0;
    static struct option opts[] = {{"port", required_argument, 0, 'p'},
                                   {"peer", required_argument, 0, 'r'},
                                   {"sync", no_argument, 0, 's'},
                                   {0, 0, 0, 0}};
    int opt;
    while ((opt = getopt_long(argc, argv, "", opts, NULL)) != -1) {
        switch (opt) {
        case 'p':
            port = atoi(optarg);
            break;
        case 'r':
            if (pc < 8)
                peers[pc++] = optarg;
            break;
        case 's':
            do_sync = 1;
            break;
        }
    }
    br_set_port(port);
    pthread_t th;
    pthread_create(&th, NULL, server, &port);
    sleep(1);
    for (int i = 0; i < pc; i++)
        br_peer_add(peers[i]);
    if (do_sync) {
        br_sync();
        char *json = bm_graph_export_json();
        FILE *f = fopen("federation_output.log", "w");
        if (f) {
            fputs(json, f);
            fclose(f);
        }
        free(json);
    }
    pthread_join(th, NULL);
    return 0;
}

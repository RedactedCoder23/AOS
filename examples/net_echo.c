/*
 * AOS — net_echo.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "net.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
    int server = 0;
    const char *host = "127.0.0.1";
    int port = 12345;

    static struct option opts[] = {{"server", no_argument, 0, 's'},
                                   {"host", required_argument, 0, 'h'},
                                   {"port", required_argument, 0, 'p'},
                                   {0, 0, 0, 0}};
    int opt;
    while ((opt = getopt_long(argc, argv, "sh:p:", opts, NULL)) != -1) {
        switch (opt) {
        case 's':
            server = 1;
            break;
        case 'h':
            host = optarg;
            break;
        case 'p':
            port = atoi(optarg);
            break;
        }
    }

    if (server) {
        int s = net_socket();
        net_bind(s, host, port);
        net_listen(s, 1);
        int c = net_accept(s);
        char buf[256];
        int n = net_recv(c, buf, sizeof(buf));
        if (n > 0)
            net_send(c, buf, n);
        net_close(c);
        net_close(s);
    } else {
        int s = net_socket();
        if (net_connect(s, host, port) < 0) {
            perror("connect");
            return 1;
        }
        net_send(s, "ping", 4);
        char buf[256];
        int n = net_recv(s, buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';
            printf("%s\n", buf);
        }
        net_close(s);
    }
    return 0;
}

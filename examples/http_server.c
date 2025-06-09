#include "net.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

int main(int argc, char **argv) {
    int port = 8080;
    static struct option opts[] = {{"port", required_argument, 0, 'p'}, {0, 0, 0, 0}};
    int opt;
    while ((opt = getopt_long(argc, argv, "p:", opts, NULL)) != -1) {
        if (opt == 'p')
            port = atoi(optarg);
    }

    int s = net_socket();
    if (s < 0) {
        perror("socket");
        return 1;
    }
    int yes = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    if (net_bind(s, NULL, port) < 0) {
        perror("bind");
        return 1;
    }
    net_listen(s, 1);
    printf("HTTP server on port %d\n", port);
    int c = net_accept(s);
    if (c < 0) {
        perror("accept");
        return 1;
    }
    char buf[1024];
    int n = net_recv(c, buf, sizeof(buf) - 1);
    if (n > 0) {
        const char *resp = "HTTP/1.0 200 OK\r\nContent-Type: text/plain\r\n\r\nHello from AOS\n";
        net_send(c, resp, strlen(resp));
    }
    net_close(c);
    net_close(s);
    return 0;
}

#include "net.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int net_socket(void) { return socket(AF_INET, SOCK_STREAM, 0); }

int net_bind(int sock, const char *ip, int port) {
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = ip ? inet_addr(ip) : INADDR_ANY;
    return bind(sock, (struct sockaddr *)&addr, sizeof(addr));
}

int net_listen(int sock, int backlog) { return listen(sock, backlog); }

int net_accept(int sock) { return accept(sock, NULL, NULL); }

int net_connect(int sock, const char *ip, int port) {
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip ? ip : "127.0.0.1");
    return connect(sock, (struct sockaddr *)&addr, sizeof(addr));
}

int net_send(int sock, const void *buf, int len) { return send(sock, buf, len, 0); }

int net_recv(int sock, void *buf, int len) { return recv(sock, buf, len, 0); }

void net_close(int sock) { close(sock); }

int net_udp_socket(void) { return socket(AF_INET, SOCK_DGRAM, 0); }

int net_sendto(int sock, const char *ip, int port, const void *buf, int len) {
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = ip ? inet_addr(ip) : INADDR_BROADCAST;
    return sendto(sock, buf, len, 0, (struct sockaddr *)&addr, sizeof(addr));
}

int net_recvfrom(int sock, char *out_ip, int *out_port, void *buf, int len) {
    struct sockaddr_in addr;
    socklen_t slen = sizeof(addr);
    int n = recvfrom(sock, buf, len, 0, (struct sockaddr *)&addr, &slen);
    if (n >= 0 && out_ip)
        strcpy(out_ip, inet_ntoa(addr.sin_addr));
    if (n >= 0 && out_port)
        *out_port = ntohs(addr.sin_port);
    return n;
}

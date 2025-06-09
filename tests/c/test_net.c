#include "net.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    int a = net_udp_socket();
    int b = net_udp_socket();
    assert(a >= 0 && b >= 0);
    assert(net_bind(a, "127.0.0.1", 12345) == 0);
    assert(net_bind(b, "127.0.0.1", 12346) == 0);
    const char msg[] = "hi";
    assert(net_sendto(b, "127.0.0.1", 12345, msg, 2) == 2);
    char ip[16] = {0};
    int port = 0;
    char buf[4];
    int n = net_recvfrom(a, ip, &port, buf, sizeof(buf));
    assert(n == 2);
    assert(strcmp(ip, "127.0.0.1") == 0);
    assert(port == 12346);
    net_close(a);
    net_close(b);
    printf("net tests passed\n");
    return 0;
}

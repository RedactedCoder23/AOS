#ifndef NET_H
#define NET_H

int net_socket(void);
int net_bind(int sock, const char *ip, int port);
int net_listen(int sock, int backlog);
int net_accept(int sock);
int net_connect(int sock, const char *ip, int port);
int net_send(int sock, const void *buf, int len);
int net_recv(int sock, void *buf, int len);
void net_close(int sock);

#endif

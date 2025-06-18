/* AOS — subsystems/net/net.h — Purpose: Header file */
/*
 * AOS — net.h
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
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

/* UDP helpers */
int net_udp_socket(void);
int net_sendto(int sock, const char *ip, int port, const void *buf, int len);
int net_recvfrom(int sock, char *out_ip, int *out_port, void *buf, int len);

#endif

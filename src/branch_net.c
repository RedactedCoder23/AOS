#include "branch.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>

#define MAX_PEERS 4
static char peers[MAX_PEERS][64];
static int peer_count;
static int local_port = 9999;

static char *build_graph_json(void) {
    Branch b[MAX_BRANCHES];
    int n = bm_list(b);
    char *out = malloc(4096);
    char *p = out;
    p += sprintf(p, "{\"branches\":[");
    for (int i=0;i<n;i++) {
        p += sprintf(p, "{\"id\":%d,\"name\":\"%s\"}%s", b[i].id, b[i].name,
                       i==n-1?"":",");
    }
    p += sprintf(p, "],\"edges\":[");
    int first=1;
    for (int i=0;i<n;i++) {
        for (int j=0;j<b[i].conn_count;j++) {
            int to=b[i].connections[j];
            if (i<to) {
                p += sprintf(p, "%s[%d,%d]", first?"":",", b[i].id, to);
                first=0;
            }
        }
    }
    sprintf(p, "]}");
    return out;
}

static void merge_graph_json(const char *json) {
    if (!json) return;
    Branch local[MAX_BRANCHES];
    int lc = bm_list(local);
    int id_map[256];
    for (int i=0;i<256;i++) id_map[i]=-1;

    const char *p = strstr(json, "\"branches\"");
    if (p) {
        p = strchr(p,'['); if (p) p++;
        while (p && *p && *p!=']') {
            int id; char name[32];
            if (sscanf(p, "{\"id\":%d,\"name\":\"%31[^\"]\"}", &id, name) == 2) {
                int local_id=-1;
                for(int j=0;j<lc;j++) if(strcmp(local[j].name,name)==0){local_id=local[j].id;break;}
                if(local_id<0) local_id=bm_create_remote(name);
                id_map[id]=local_id;
            }
            p=strchr(p,'}'); if(!p) break; p++; if(*p==',') p++;
        }
    }
    p = strstr(json,"\"edges\"");
    if (p) {
        p=strchr(p,'['); if(p) p++;
        while(p && *p && *p!=']') {
            int a,bid; if(sscanf(p,"[%d,%d]",&a,&bid)==2){
                int la=id_map[a]; int lb=id_map[bid];
                if(la>=0 && lb>=0) bm_connect(la,lb);
            }
            p=strchr(p,']'); if(!p) break; p++; if(*p==',') p++;
        }
    }
}

char *bm_graph_export_json(void) { return build_graph_json(); }
void bm_graph_merge_json(const char *json) { merge_graph_json(json); }

int bm_sync_all(void) { return br_sync(); }

void br_set_port(int p) { local_port = p; }

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
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        FILE *f = fopen("AOS-CHECKLIST.log", "a");
        if (f) { fprintf(f, "socket error\n"); fclose(f); }
        return -1;
    }
    struct sockaddr_in laddr = {0};
    laddr.sin_family = AF_INET;
    laddr.sin_port = htons(local_port);
    laddr.sin_addr.s_addr = INADDR_ANY;
    bind(sock, (struct sockaddr*)&laddr, sizeof(laddr));

    struct timeval tv = {1, 0};
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    for (int i = 0; i < peer_count; i++) {
        char host[64];
        int port = 9999;
        strncpy(host, peers[i], sizeof(host));
        char *colon = strchr(host, ':');
        if (colon) { *colon = '\0'; port = atoi(colon + 1); }
        struct sockaddr_in addr = {0};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        if (inet_aton(host, &addr.sin_addr) == 0) {
            printf("invalid addr %s\n", peers[i]);
            continue;
        }
        sendto(sock, "PING", 4, 0, (struct sockaddr*)&addr, sizeof(addr));
        char buf[2048];
        int n = recvfrom(sock, buf, sizeof(buf)-1, 0, NULL, NULL);
        if (n <= 0) {
            FILE *f = fopen("AOS-CHECKLIST.log", "a");
            if (f) { fprintf(f, "timeout to %s\n", peers[i]); fclose(f); }
            printf("no response from %s\n", peers[i]);
            continue;
        }
        buf[n] = '\0';
        if (strncmp(buf, "PONG", 4) == 0) {
            char *json = build_graph_json();
            sendto(sock, json, strlen(json), 0, (struct sockaddr*)&addr, sizeof(addr));
            free(json);
            n = recvfrom(sock, buf, sizeof(buf)-1, 0, NULL, NULL);
            if (n > 0) { buf[n]='\0'; merge_graph_json(buf); }
        } else if (buf[0]=='{') {
            merge_graph_json(buf);
        }
    }
    close(sock);
    return 0;
}

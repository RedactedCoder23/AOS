#include "config.h"
#include <stdio.h>
#include <string.h>

#define MAX_CFG 16

static struct { char key[32]; char val[64]; } cfg[MAX_CFG];
static int cfg_count;

static void add_pair(const char *k, const char *v) {
    if (cfg_count >= MAX_CFG) return;
    strncpy(cfg[cfg_count].key, k, 31); cfg[cfg_count].key[31] = '\0';
    strncpy(cfg[cfg_count].val, v, 63); cfg[cfg_count].val[63] = '\0';
    cfg_count++;
}

void config_load_default(void) {
    cfg_count = 0;
    FILE *f = fopen("/etc/aos/config.json", "r");
    if (!f) { fprintf(stderr, "config missing: /etc/aos/config.json\n"); return; }
    char buf[512];
    size_t n = fread(buf, 1, sizeof(buf)-1, f);
    fclose(f);
    buf[n] = '\0';
    char *p = buf;
    while ((p = strchr(p, '"'))) {
        char *k = p + 1;
        char *ke = strchr(k, '"');
        if (!ke) break;
        *ke = '\0';
        char *colon = strchr(ke + 1, ':');
        if (!colon) break;
        colon++;
        while (*colon==' '||*colon=='\"') colon++;
        char *ve = strpbrk(colon, ",}" );
        if (!ve) break;
        char val[64];
        size_t vl = ve - colon;
        if (vl > 63) vl = 63;
        strncpy(val, colon, vl); val[vl] = '\0';
        add_pair(k, val);
        p = ve + 1;
    }
}

const char *config_get(const char *key) {
    for (int i=0;i<cfg_count;i++)
        if (strcmp(cfg[i].key, key) == 0) return cfg[i].val;
    return NULL;
}

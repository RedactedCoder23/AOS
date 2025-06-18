/* AOS — bare_metal_os/config.c — Purpose: C source file */
/*
 * AOS — config.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "config.h"
#include "../subsystems/fs/fs.h"
#include <stddef.h>

#define MAX_CFG 8

struct config {
    char key[32];
    char value[64];
};

static struct config cfg[MAX_CFG];
static int cfg_count;

static void str_ncpy(char *d, const char *s, size_t n) {
    size_t i = 0;
    for (; i < n - 1 && s[i]; i++)
        d[i] = s[i];
    if (n)
        d[i] = 0;
}

static int str_cmp(const char *a, const char *b) {
    while (*a && *a == *b) {
        a++;
        b++;
    }
    return (unsigned char)*a - (unsigned char)*b;
}

static void add_pair(const char *k, const char *v) {
    if (cfg_count >= MAX_CFG)
        return;
    str_ncpy(cfg[cfg_count].key, k, sizeof(cfg[cfg_count].key));
    str_ncpy(cfg[cfg_count].value, v, sizeof(cfg[cfg_count].value));
    cfg_count++;
}

void config_load_default(void) {
    cfg_count = 0;
    int fd = fs_open("/etc/aos.conf", "r");
    if (fd >= 0) {
        char buf[256];
        size_t n = fs_read(fd, buf, sizeof(buf) - 1);
        fs_close(fd);
        buf[n] = 0;
        char *p = buf;
        while (*p) {
            char *line = p;
            while (*p && *p != '\n' && *p != '\r')
                p++;
            if (*p) {
                *p = 0;
                p++;
            }
            if (line[0] == 0)
                continue;
            char *eq = line;
            while (*eq && *eq != '=')
                eq++;
            if (*eq != '=') {
                cfg_count = 0;
                break;
            }
            *eq = 0;
            add_pair(line, eq + 1);
        }
    }
    if (cfg_count == 0) {
        add_pair("mode", "production");
        add_pair("log_level", "info");
    }
}

const char *config_get(const char *key) {
    for (int i = 0; i < cfg_count; i++)
        if (str_cmp(cfg[i].key, key) == 0)
            return cfg[i].value;
    (void)key;
    return 0;
}

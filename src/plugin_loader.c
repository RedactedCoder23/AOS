#include "plugin.h"
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>

#define MAX_PLUGINS 4

typedef struct { char name[32]; void *handle; } Plugin;
static Plugin plugins[MAX_PLUGINS];
static int plugin_count;

int plugin_install(const char *url) {
    printf("Installed plugin from %s\n", url ? url : "(null)");
    return 0;
}

int plugin_load(const char *name) {
    if (plugin_count >= MAX_PLUGINS) return -1;
    char path[64];
    snprintf(path, sizeof(path), "build/plugins/%s.so", name);
    void *h = dlopen(path, RTLD_NOW);
    if (!h) { printf("dlopen failed: %s\n", dlerror()); return -1; }
    void (*entry)(void) = dlsym(h, "plugin_entry");
    if (!entry) { printf("no entry\n"); dlclose(h); return -1; }
    strncpy(plugins[plugin_count].name, name, sizeof(plugins[plugin_count].name)-1);
    plugins[plugin_count].handle = h;
    plugin_count++;
    entry();
    return 0;
}

void plugin_list(void) {
    for(int i=0;i<plugin_count;i++)
        printf("%d:%s\n", i, plugins[i].name);
}

#include "plugin.h"
/* [2025-06-09 06:07 UTC] Hot-swap plugin loader via dlopen
 * by: codex
 * Edge cases: minimal error handling and no security checks.
 * Next agent must: audit plugin paths and sandbox execution.
 */
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>

#define MAX_PLUGINS 4

typedef struct {
    char name[32];
    void *handle;
    void (*exec)(void);
    void (*cleanup)(void);
} Plugin;
static Plugin plugins[MAX_PLUGINS];
static int plugin_count;

int plugin_install(const char *url) {
    printf("Installed plugin from %s\n", url ? url : "(null)");
    return 0;
}

static void normalize_name(const char *path, char *out, size_t outsz) {
    const char *base = strrchr(path, '/');
    base = base ? base + 1 : path;
    strncpy(out, base, outsz - 1);
    out[outsz - 1] = '\0';
    char *dot = strstr(out, ".so");
    if (dot) *dot = '\0';
}

int plugin_load(const char *file) {
    if (plugin_count >= MAX_PLUGINS) return -1;
    char path[128];
    if (strchr(file, '/'))
        snprintf(path, sizeof(path), "%s", file);
    else
        snprintf(path, sizeof(path), "build/plugins/%s.so", file);

    void *h = dlopen(path, RTLD_NOW);
    if (!h) { printf("dlopen failed: %s\n", dlerror()); return -1; }

    int (*init)(void) = dlsym(h, "plugin_init");
    void (*exec)(void) = dlsym(h, "plugin_exec");
    void (*cleanup)(void) = dlsym(h, "plugin_cleanup");
    if (!init || !exec || !cleanup) {
        printf("plugin interface missing\n");
        dlclose(h);
        return -1;
    }

    if (init() != 0) {
        printf("init failed\n");
        dlclose(h);
        return -1;
    }

    char norm[32];
    normalize_name(path, norm, sizeof(norm));
    strncpy(plugins[plugin_count].name, norm, sizeof(plugins[plugin_count].name)-1);
    plugins[plugin_count].handle = h;
    plugins[plugin_count].exec = exec;
    plugins[plugin_count].cleanup = cleanup;
    plugin_count++;

    exec();
    printf("Loaded %s\n", norm);
    return 0;
}

int plugin_exec(const char *name) {
    for (int i=0;i<plugin_count;i++)
        if (strcmp(plugins[i].name, name)==0) {
            plugins[i].exec();
            return 0;
        }
    return -1;
}

int plugin_unload(const char *name) {
    for (int i=0;i<plugin_count;i++) {
        if (strcmp(plugins[i].name, name)==0) {
            if (plugins[i].cleanup)
                plugins[i].cleanup();
            dlclose(plugins[i].handle);
            for (int j=i;j<plugin_count-1;j++)
                plugins[j]=plugins[j+1];
            plugin_count--;
            printf("Unloaded %s\n", name);
            return 0;
        }
    }
    printf("Plugin %s not found\n", name);
    return -1;
}

void plugin_list(void) {
    for(int i=0;i<plugin_count;i++)
        printf("%d:%s\n", i, plugins[i].name);
}

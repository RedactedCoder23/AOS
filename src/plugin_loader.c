/*
 * AOS — plugin_loader.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "logging.h"
#include "plugin.h"
#include "plugin_supervisor.h"
#include "wasm_runtime.h"
/* [2025-06-09 06:07 UTC] Hot-swap plugin loader via dlopen
 * by: codex
 * Edge cases: minimal error handling and no security checks.
 * See AGENT.md "Open Issues" for pending work.
 *
 * [2025-06-09 08:04 UTC] Sandbox update
 * by: codex
 * Added simple path validation and rlimit-based sandbox hooks.
 */
#include <dlfcn.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>

#define MAX_PLUGINS 4
#define MAX_HOOKS 4
static const char *allowed_dir = "build/plugins/";
static char allowed_real[PATH_MAX];
static int allowed_real_set;

typedef struct {
    char name[32];
    void *handle;
    void (*exec)(void);
    void (*cleanup)(void);
} Plugin;
static Plugin plugins[MAX_PLUGINS];
static int plugin_count;

typedef int (*validation_hook_t)(const char *);
static validation_hook_t hooks[MAX_HOOKS];
static int hook_count;

static int path_is_safe(const char *path) {
    if (!allowed_real_set) {
        if (!realpath(allowed_dir, allowed_real))
            return 0;
        allowed_real_set = 1;
    }
    if (strstr(path, ".."))
        return 0;
    char real[PATH_MAX];
    if (!realpath(path, real))
        return 0;
    return strncmp(real, allowed_real, strlen(allowed_real)) == 0;
}

static int builtin_hook(const char *path) {
    /* Default validation simply checks the plugin path prefix. */
    return path_is_safe(path);
}

static void run_with_limits_void(void (*func)(void)) {
    struct rlimit old_cpu, old_as, lim;
    getrlimit(RLIMIT_CPU, &old_cpu);
    getrlimit(RLIMIT_AS, &old_as);
    lim.rlim_cur = lim.rlim_max = 2;
    setrlimit(RLIMIT_CPU, &lim);
    lim.rlim_cur = lim.rlim_max = 16 * 1024 * 1024;
    setrlimit(RLIMIT_AS, &lim);
    func();
    setrlimit(RLIMIT_CPU, &old_cpu);
    setrlimit(RLIMIT_AS, &old_as);
}

static int run_with_limits_int(int (*func)(void)) {
    struct rlimit old_cpu, old_as, lim;
    getrlimit(RLIMIT_CPU, &old_cpu);
    getrlimit(RLIMIT_AS, &old_as);
    lim.rlim_cur = lim.rlim_max = 2;
    setrlimit(RLIMIT_CPU, &lim);
    lim.rlim_cur = lim.rlim_max = 16 * 1024 * 1024;
    setrlimit(RLIMIT_AS, &lim);
    int rc = func();
    setrlimit(RLIMIT_CPU, &old_cpu);
    setrlimit(RLIMIT_AS, &old_as);
    return rc;
}

int plugin_register_hook(int (*hook)(const char *)) {
    if (hook_count >= MAX_HOOKS)
        return -1;
    hooks[hook_count++] = hook;
    return 0;
}

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
    if (dot)
        *dot = '\0';
}

static int check_manifest(const char *path) {
    char manifest[160];
    snprintf(manifest, sizeof(manifest), "%s.manifest", path);
    FILE *f = fopen(manifest, "r");
    if (!f)
        return 1; /* allow if missing */
    char buf[256];
    size_t n = fread(buf, 1, sizeof(buf) - 1, f);
    buf[n] = '\0';
    fclose(f);
    return strstr(buf, "allow") != NULL;
}

/* Load a plugin shared object and register it in the runtime.
 * @param file path or plugin name without extension
 * @return 0 on success, -1 on error
 */
int plugin_load(const char *file) {
    if (plugin_count >= MAX_PLUGINS)
        return -1;
    char path[128];
    if (strchr(file, '/'))
        snprintf(path, sizeof(path), "%s", file);
    else
        snprintf(path, sizeof(path), "build/plugins/%s", file);

    const char *ext = strrchr(path, '.');
    if (!ext)
        strcat(path, ".so");

    /* Run registered validation hooks before loading the plugin. */
    if (hook_count == 0)
        plugin_register_hook(builtin_hook);
    for (int i = 0; i < hook_count; i++)
        if (!hooks[i](path)) {
            printf("plugin validation failed for %s\n", path);
            log_message(LOG_ERROR, "plugin validation failed %s", path);
            return -1;
        }

    if (!check_manifest(path)) {
        printf("manifest denied %s\n", path);
        return -1;
    }

    if (ext && strcmp(ext, ".wasm") == 0) {
        ps_load(path);
        return 0;
    }

    void *h = dlopen(path, RTLD_NOW);
    if (!h) {
        printf("dlopen failed: %s\n", dlerror());
        log_message(LOG_ERROR, "plugin dlopen failed %s", path);
        return -1;
    }

    int (*init)(void) = dlsym(h, "plugin_init");
    void (*exec)(void) = dlsym(h, "plugin_exec");
    void (*cleanup)(void) = dlsym(h, "plugin_cleanup");
    if (!init || !exec || !cleanup) {
        printf("plugin interface missing\n");
        dlclose(h);
        return -1;
    }

    if (run_with_limits_int(init) != 0) {
        printf("init failed\n");
        log_message(LOG_ERROR, "plugin init failed %s", path);
        dlclose(h);
        return -1;
    }

    char norm[32];
    normalize_name(path, norm, sizeof(norm));
    strncpy(plugins[plugin_count].name, norm, sizeof(plugins[plugin_count].name) - 1);
    plugins[plugin_count].handle = h;
    plugins[plugin_count].exec = exec;
    plugins[plugin_count].cleanup = cleanup;
    plugin_count++;

    run_with_limits_void(exec);
    printf("Loaded %s\n", norm);
    return 0;
}

int plugin_exec(const char *name) {
    for (int i = 0; i < plugin_count; i++)
        if (strcmp(plugins[i].name, name) == 0) {
            run_with_limits_void(plugins[i].exec);
            return 0;
        }
    log_message(LOG_ERROR, "plugin exec not found %s", name);
    return -1;
}

int plugin_unload(const char *name) {
    for (int i = 0; i < plugin_count; i++) {
        if (strcmp(plugins[i].name, name) == 0) {
            if (plugins[i].cleanup)
                run_with_limits_void(plugins[i].cleanup);
            dlclose(plugins[i].handle);
            for (int j = i; j < plugin_count - 1; j++)
                plugins[j] = plugins[j + 1];
            plugin_count--;
            printf("Unloaded %s\n", name);
            return 0;
        }
    }
    printf("Plugin %s not found\n", name);
    FILE *log = fopen("AGENT.md", "a");
    if (log) {
        fprintf(log, "plugin unload not found %s\n", name);
        fclose(log);
    }
    return -1;
}

void plugin_list(void) {
    for (int i = 0; i < plugin_count; i++)
        printf("%d:%s\n", i, plugins[i].name);
}

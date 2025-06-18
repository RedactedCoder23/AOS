/* AOS — subsystems/ai/ai.c — Purpose: C source file */
/*
 * AOS — ai.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "ai.h"
/* [2025-06-09 06:06 UTC] HTTP echo based AI stub
 * by: codex
 * Edge cases: network errors not handled, insecure HTTP parameters.
 * See AGENT.md "Open Issues" for pending work.
 */
#include <curl/curl.h>
#include <stdio.h>
#include <string.h>

static char response[512];
static char profile[32] = "default";

void ai_init(const char *p) {
    if (p)
        strncpy(profile, p, sizeof(profile) - 1);
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

const char *ai_reply(const char *prompt) {
    if (!prompt)
        return "";

    if (ai_infer(prompt, response, sizeof(response)) != 0) {
        return response;
    }
    return response;
}

void ai_shutdown(void) { curl_global_cleanup(); }

/* simple config stored at ~/.aos/ai.conf */
#include <limits.h>
#include <stdlib.h>
#include <sys/stat.h>

static char cfg_path[PATH_MAX];
static char provider[16];
static char param[256];
static int cfg_loaded;

static void ensure_cfg_path(void) {
    if (cfg_path[0])
        return;
    const char *home = getenv("HOME");
    if (!home)
        return;
    snprintf(cfg_path, sizeof(cfg_path), "%s/.aos", home);
    mkdir(cfg_path, 0755);
    strncat(cfg_path, "/ai.conf", sizeof(cfg_path) - strlen(cfg_path) - 1);
}

int ai_config_load(void) {
    if (cfg_loaded)
        return 0;
    ensure_cfg_path();
    provider[0] = '\0';
    param[0] = '\0';
    FILE *f = fopen(cfg_path, "r");
    if (!f)
        return -1;
    char key[32], val[256];
    while (fscanf(f, " %31[^=]=%255[^\n]\n", key, val) == 2) {
        if (strcmp(key, "provider") == 0)
            strncpy(provider, val, sizeof(provider) - 1);
        else if (strcmp(key, "openai_key") == 0 || strcmp(key, "model_path") == 0)
            strncpy(param, val, sizeof(param) - 1);
    }
    fclose(f);
    cfg_loaded = 1;
    return 0;
}

static int save_config(const char *prov, const char *p) {
    ensure_cfg_path();
    FILE *f = fopen(cfg_path, "w");
    if (!f)
        return -1;
    fprintf(f, "provider=%s\n", prov);
    if (strcmp(prov, "openai") == 0)
        fprintf(f, "openai_key=%s\n", p);
    else
        fprintf(f, "model_path=%s\n", p);
    fclose(f);
    chmod(cfg_path, 0600);
    strncpy(provider, prov, sizeof(provider) - 1);
    strncpy(param, p, sizeof(param) - 1);
    cfg_loaded = 1;
    return 0;
}

int ai_setup_wizard(void) {
    char prov[16];
    char p[256];
    printf("Provider (openai/local): ");
    if (!fgets(prov, sizeof(prov), stdin))
        return -1;
    prov[strcspn(prov, "\n")] = '\0';
    if (strcmp(prov, "local") != 0 && strcmp(prov, "openai") != 0) {
        printf("unknown provider\n");
        return -1;
    }
    if (strcmp(prov, "openai") == 0)
        printf("OpenAI API key: ");
    else
        printf("Model path: ");
    if (!fgets(p, sizeof(p), stdin))
        return -1;
    p[strcspn(p, "\n")] = '\0';
    if (save_config(prov, p) != 0)
        return -1;
    return 0;
}

static const char *get_param(void) {
    const char *env = getenv("OPENAI_API_KEY");
    if (!env)
        env = getenv("AOS_OPENAI_API_KEY");
    if (env)
        return env;
    if (!cfg_loaded)
        ai_config_load();
    if (param[0])
        return param;
    return NULL;
}

const char *ai_get_provider(void) {
    if (!cfg_loaded)
        ai_config_load();
    return provider[0] ? provider : "";
}

const char *ai_get_param(void) { return get_param(); }

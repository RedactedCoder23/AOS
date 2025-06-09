#include "ai.h"
/* [2025-06-09 06:06 UTC] HTTP echo based AI stub
 * by: codex
 * Edge cases: network errors not handled, insecure HTTP parameters.
 * Next agent must: replace with actual backend and add retries.
 */
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

static char response[512];
static char profile[32] = "default";

void ai_init(const char *p) {
    if (p) strncpy(profile, p, sizeof(profile)-1);
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

static size_t write_cb(char *ptr, size_t size, size_t nmemb, void *userdata) {
    size_t len = size * nmemb;
    if (len >= sizeof(response)) len = sizeof(response) - 1;
    memcpy(response, ptr, len);
    response[len] = '\0';
    return len;
}

const char *ai_reply(const char *prompt) {
    CURL *c = curl_easy_init();
    if (!c) {
        snprintf(response, sizeof(response), "init error");
        return response;
    }
    char url[256];
    snprintf(url, sizeof(url), "https://postman-echo.com/get?profile=%s&prompt=%s", profile, prompt);
    curl_easy_setopt(c, CURLOPT_URL, url);
    curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_perform(c);
    curl_easy_cleanup(c);
    return response;
}

void ai_shutdown(void) {
    curl_global_cleanup();
}

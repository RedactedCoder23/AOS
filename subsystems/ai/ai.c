#include "ai.h"
/* [2025-06-09 06:06 UTC] HTTP echo based AI stub
 * by: codex
 * Edge cases: network errors not handled, insecure HTTP parameters.
 * Next agent must: see AGENT.md "UNRESOLVED ISSUES".
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

const char *ai_reply(const char *prompt) {
    if (!prompt) return "";

    if (ai_infer(prompt, response, sizeof(response)) != 0) {
        return response;
    }
    return response;
}

void ai_shutdown(void) {
    curl_global_cleanup();
}

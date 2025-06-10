#include "policy.h"
#include "audit.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* [2025-06-09 06:06 UTC] Simple policy engine
 * by: codex
 * Edge cases: naive JSON parser, no persistence or integrity checks.
 */

#define MAX_RULES 32

typedef struct {
    char branch[32];
    char app[32];
    char action[32];
    int allow; /* 1 allow, 0 deny */
} PolicyRule;

static PolicyRule rules[MAX_RULES];
static int rule_count;

static void reset_rules(void) {
    rule_count = 0;
    memset(rules, 0, sizeof(rules));
}

static void parse_json(const char *json);
static void parse_yaml(const char *yaml);

void policy_load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f)
        return;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = malloc(sz + 1);
    if (!buf) {
        fclose(f);
        return;
    }
    fread(buf, 1, sz, f);
    buf[sz] = '\0';
    fclose(f);
    policy_load(buf);
    free(buf);
}

void policy_load(const char *text) {
    reset_rules();
    if (!text)
        return;
    if (strchr(text, '{') || strchr(text, '['))
        parse_json(text);
    else
        parse_yaml(text);
    printf("policy loaded: %d rules\n", rule_count);
}

static void parse_json(const char *json) {
    const char *p = json;
    while ((p = strchr(p, '{')) && rule_count < MAX_RULES) {
        PolicyRule *r = &rules[rule_count];
        strcpy(r->branch, "*");
        strcpy(r->app, "*");
        r->action[0] = '\0';
        r->allow = 1;
        const char *f;
        if ((f = strstr(p, "\"branch\"")))
            sscanf(strchr(f, ':') + 1, "\"%31[^\"]\"", r->branch);
        if ((f = strstr(p, "\"app\"")))
            sscanf(strchr(f, ':') + 1, "\"%31[^\"]\"", r->app);
        if ((f = strstr(p, "\"syscall\"")))
            sscanf(strchr(f, ':') + 1, "\"%31[^\"]\"", r->action);
        if ((f = strstr(p, "\"allow\""))) {
            char val[6];
            sscanf(strchr(f, ':') + 1, " %5[^,}\n]", val);
            r->allow = (val[0] == 't' || val[0] == '1');
        }
        if ((f = strstr(p, "\"deny\""))) {
            char val[6];
            sscanf(strchr(f, ':') + 1, " %5[^,}\n]", val);
            r->allow = !(val[0] == 't' || val[0] == '1');
        }
        if (r->action[0])
            rule_count++;
        p = strchr(p, '}');
        if (!p)
            break;
        p++;
    }
}

static void parse_yaml(const char *yaml) {
    const char *p = yaml;
    PolicyRule cur;
    char key[32];
    strcpy(cur.branch, "*");
    strcpy(cur.app, "*");
    cur.action[0] = '\0';
    cur.allow = 1;
    while (*p && rule_count < MAX_RULES) {
        if (*p == '-') {
            if (cur.action[0]) {
                rules[rule_count++] = cur;
                if (rule_count >= MAX_RULES)
                    break;
            }
            strcpy(cur.branch, "*");
            strcpy(cur.app, "*");
            cur.action[0] = '\0';
            cur.allow = 1;
            p++; /* skip '-' */
        }
        if (sscanf(p, " branch: %31s", cur.branch) == 1) {         /* keep */
        } else if (sscanf(p, " app: %31s", cur.app) == 1) {        /* keep */
        } else if (sscanf(p, " syscall: %31s", cur.action) == 1) { /* keep */
        } else if (sscanf(p, " allow: %31s", key) == 1) {
            cur.allow = (key[0] == 't' || key[0] == '1');
        } else if (sscanf(p, " deny: %31s", key) == 1) {
            cur.allow = !(key[0] == 't' || key[0] == '1');
        }
        p = strchr(p, '\n');
        if (!p)
            break;
        p++;
    }
    if (cur.action[0] && rule_count < MAX_RULES)
        rules[rule_count++] = cur;
}

static int match(const char *rule_val, const char *ctx_val) {
    return strcmp(rule_val, "*") == 0 || !ctx_val || strcmp(rule_val, ctx_val) == 0;
}

int policy_check_ctx(const char *branch, const char *app, const char *action) {
    for (int i = 0; i < rule_count; i++) {
        PolicyRule *r = &rules[i];
        if (match(r->branch, branch) && match(r->app, app) && strcmp(r->action, action) == 0) {
            audit_log_entry(app ? app : "", action, branch ? branch : "",
                            r->allow ? "ALLOW" : "DENY");
            if (!r->allow) {
                FILE *f = fopen("AOS-CHECKLIST.log", "a");
                if (f) {
                    fprintf(f, "policy deny %s\n", action);
                    fclose(f);
                }
            }
            return r->allow;
        }
    }
    audit_log_entry(app ? app : "", action, branch ? branch : "", "ALLOW");
    return 1; /* default allow */
}

int policy_check(const char *action) { return policy_check_ctx("*", "*", action); }

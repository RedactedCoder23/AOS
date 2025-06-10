#include "lang_vm.h"
#include "ai.h"
#include "branch.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *skip_ws(const char *p) {
    while (p && *p && isspace((unsigned char)*p))
        p++;
    return p;
}

static const char *parse_ident(const char *p, char *out, size_t n) {
    size_t i = 0;
    while (*p && !isspace((unsigned char)*p) && *p != ')' && i + 1 < n)
        out[i++] = *p++;
    out[i] = '\0';
    return p;
}

static const char *parse_string(const char *p, char *out, size_t n) {
    if (*p != '"')
        return NULL;
    p++;
    size_t i = 0;
    while (*p && *p != '"' && i + 1 < n)
        out[i++] = *p++;
    out[i] = '\0';
    if (*p != '"')
        return NULL;
    return p + 1;
}

static int exec_expr(const char *op, const char *arg) {
    if (strcmp(op, "print") == 0) {
        printf("%s\n", arg);
    } else if (strcmp(op, "branch-create") == 0 || strcmp(op, "branch_create") == 0) {
        bm_create(arg);
    } else if (strcmp(op, "ai-query") == 0 || strcmp(op, "ai_query") == 0) {
        const char *resp = ai_reply(arg);
        printf("%s\n", resp);
    } else if (strcmp(op, "exit") == 0) {
        return 1;
    }
    return 0;
}

static int vm_run(const char *src) {
    const char *p = src;
    char op[32];
    char arg[256];
    while ((p = strchr(p, '('))) {
        p++;
        p = skip_ws(p);
        p = parse_ident(p, op, sizeof(op));
        p = skip_ws(p);
        arg[0] = '\0';
        if (*p == '"') {
            const char *np = parse_string(p, arg, sizeof(arg));
            if (!np)
                return -1;
            p = np;
        } else if (*p && *p != ')') {
            p = parse_ident(p, arg, sizeof(arg));
        }
        const char *close = strchr(p, ')');
        if (!close)
            break;
        p = close + 1;
        if (exec_expr(op, arg))
            break;
    }
    return 0;
}

int lang_vm_run_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f)
        return -1;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = malloc(sz + 1);
    if (!buf) {
        fclose(f);
        return -1;
    }
    fread(buf, 1, sz, f);
    buf[sz] = '\0';
    fclose(f);
    int rc = vm_run(buf);
    free(buf);
    return rc;
}


#include "branch.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <limits.h>
#include <time.h>

static Branch branches[MAX_BRANCHES];
static int branch_count = 0;
static int current_branch = -1;
static char save_path[PATH_MAX];

static void save_state(void) {
    if (!save_path[0]) return;
    FILE *f = fopen(save_path, "w");
    if (!f) return;
    fprintf(f, "[\n");
    for (int i = 0; i < branch_count; i++) {
        Branch *b = &branches[i];
        if (b->origin) continue; /* don't persist remote */
        fprintf(f,
            " {\"id\":%d,\"name\":\"%s\",\"created\":%ld,\"state\":\"%s\",\"connections\":[",
            b->id, b->name, b->created_ts, b->state);
        for (int j = 0; j < b->conn_count; j++) {
            if (j) fprintf(f, ",");
            fprintf(f, "%d", b->connections[j]);
        }
        fprintf(f, "]}%s\n", i == branch_count - 1 ? "" : ",");
    }
    fprintf(f, "]\n");
    fclose(f);
}

static void load_state(void) {
    if (!save_path[0]) return;
    FILE *f = fopen(save_path, "r");
    if (!f) return;
    branch_count = 0;
    char line[256];
    while (fgets(line, sizeof(line), f) && branch_count < MAX_BRANCHES) {
        Branch *b = &branches[branch_count];
        if (sscanf(line,
                   " {\"id\":%d,\"name\":\"%31[^\"]\",\"created\":%ld,\"state\":\"%15[^\"]\"",
                   &b->id, b->name, &b->created_ts, b->state) == 4) {
            char *conn = strchr(line, '[');
            b->conn_count = 0;
            if (conn) {
                conn++;
                while (*conn && *conn != ']') {
                    int id;
                    if (sscanf(conn, "%d", &id) == 1 && b->conn_count < MAX_BRANCHES)
                        b->connections[b->conn_count++] = id;
                    conn = strchr(conn, ',');
                    if (!conn) break;
                    conn++;
                }
            }
            b->origin = 0;
            branch_count++;
        }
    }
    fclose(f);
}

void bm_init(void) {
    const char *home = getenv("HOME");
    branch_count = 0;
    current_branch = -1;
    if (home) {
        snprintf(save_path, sizeof(save_path), "%s/.aos", home);
        mkdir(save_path, 0755);
        strncat(save_path, "/branches.json", sizeof(save_path)-strlen(save_path)-1);
        load_state();
    } else {
        save_path[0] = '\0';
    }
}

int bm_create(const char *name) {
    if (branch_count >= MAX_BRANCHES || !name)
        return BM_ERR_FULL;
    for (int i = 0; i < branch_count; i++) {
        if (strcmp(branches[i].name, name) == 0)
            return BM_ERR_DUPLICATE;
    }
    Branch *b = &branches[branch_count];
    b->id = branch_count;
    strncpy(b->name, name, sizeof(b->name) - 1);
    b->name[sizeof(b->name) - 1] = '\0';
    b->conn_count = 0;
    b->created_ts = time(NULL);
    strncpy(b->state, "running", sizeof(b->state) - 1);
    b->origin = 0;
    int id = branch_count++;
    save_state();
    return id;
}

int bm_create_remote(const char *name) {
    int id = bm_create(name);
    if (id >= 0) branches[id].origin = 1;
    return id;
}

int bm_switch(int id) {
    if (id < 0 || id >= branch_count)
        return BM_ERR_INVALID;
    current_branch = id;
    save_state();
    return BM_SUCCESS;
}

int bm_connect(int from, int to) {
    if (from < 0 || from >= branch_count || to < 0 || to >= branch_count)
        return BM_ERR_INVALID;
    Branch *bf = &branches[from];
    Branch *bt = &branches[to];
    // Avoid duplicates
    for (int i=0;i<bf->conn_count;i++) if (bf->connections[i]==to) return BM_ERR_DUPLICATE;
    if (bf->conn_count < MAX_BRANCHES)
        bf->connections[bf->conn_count++] = to;
    for (int i=0;i<bt->conn_count;i++) if (bt->connections[i]==from) return BM_ERR_DUPLICATE;
    if (bt->conn_count < MAX_BRANCHES)
        bt->connections[bt->conn_count++] = from;
    save_state();
    return BM_SUCCESS;
}

int bm_list(Branch *out) {
    if (!out) return 0;
    for (int i=0; i<branch_count; ++i)
        out[i] = branches[i];
    return branch_count;
}

int bm_stop(int id) {
    if (id < 0 || id >= branch_count)
        return BM_ERR_INVALID;
    strncpy(branches[id].state, "stopped", sizeof(branches[id].state)-1);
    save_state();
    return BM_SUCCESS;
}

int bm_delete(int id) {
    if (id < 0 || id >= branch_count)
        return BM_ERR_INVALID;
    for (int i = id; i < branch_count - 1; i++) {
        branches[i] = branches[i + 1];
        branches[i].id = i;
    }
    branch_count--;
    save_state();
    return BM_SUCCESS;
}


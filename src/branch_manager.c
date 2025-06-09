#include "branch.h"
/* [2025-06-09 06:06 UTC] Branch manager with JSON persistence
 * by: codex
 * Edge cases: load_state assumes valid file; no locking for concurrent access.
 * Next agent must: see AGENT.md "UNRESOLVED ISSUES".
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <limits.h>
#include <time.h>

static BranchGraph graph;
static int current_branch = -1;
static char save_path[PATH_MAX];

static void load_state(void);

static void log_io_error(const char *msg) {
    FILE *log = fopen("AGENT.md", "a");
    if (log) { fprintf(log, "IO error: %s\n", msg); fclose(log); }
}

static void save_state(void) {
    if (!save_path[0]) return;
    FILE *f = fopen(save_path, "w");
    if (!f) { log_io_error("save_state fopen failed"); return; }
    fprintf(f, "[\n");
    for (int i = 0; i < graph.count; i++) {
        Branch *b = &graph.branches[i];
        if (b->origin) continue; /* don't persist remote */
        fprintf(f,
            " {\"id\":%d,\"name\":\"%s\",\"created\":%ld,\"state\":\"%s\",\"connections\":[",
            b->id, b->name, b->created_ts, b->state);
        for (int j = 0; j < b->conn_count; j++) {
            if (j) fprintf(f, ",");
            fprintf(f, "%d", b->connections[j]);
        }
        fprintf(f, "]}%s\n", i == graph.count - 1 ? "" : ",");
    }
    fprintf(f, "]\n");
    fclose(f);
}

void branch_save_all(void) {
    save_state();
}

void branch_load_all(void) {
    load_state();
}

static void load_state(void) {
    if (!save_path[0]) return;
    FILE *f = fopen(save_path, "r");
    if (!f) { log_io_error("load_state fopen failed"); return; }
    graph.count = 0;
    memset(graph.adj, 0, sizeof(graph.adj));
    char line[256];
    while (fgets(line, sizeof(line), f) && graph.count < MAX_BRANCHES) {
        Branch *b = &graph.branches[graph.count];
        if (sscanf(line,
                   " {\"id\":%d,\"name\":\"%31[^\"]\",\"created\":%ld,\"state\":\"%15[^\"]\"",
                   &b->id, b->name, &b->created_ts, b->state) == 4) {
            char *conn = strchr(line, '[');
            b->conn_count = 0;
            if (conn) {
                conn++;
                while (*conn && *conn != ']') {
                    int idv;
                    if (sscanf(conn, "%d", &idv) == 1 && b->conn_count < MAX_BRANCHES)
                        b->connections[b->conn_count++] = idv;
                    conn = strchr(conn, ',');
                    if (!conn) break;
                    conn++;
                }
            }
            b->origin = 0;
            b->parent = b->conn_count ? b->connections[0] : -1;
            for (int j=0;j<b->conn_count;j++) {
                int cid = b->connections[j];
                graph.adj[b->id][cid] = 1;
                graph.adj[cid][b->id] = 1;
            }
            graph.count++;
        } else {
            log_io_error("load_state parse error");
        }
    }
    fclose(f);
}

void bm_init(void) {
    const char *home = getenv("HOME");
    graph.count = 0;
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
    if (graph.count >= MAX_BRANCHES || !name)
        return BM_ERR_FULL;
    for (int i = 0; i < graph.count; i++) {
        if (strcmp(graph.branches[i].name, name) == 0)
            return BM_ERR_DUPLICATE;
    }
    Branch *b = &graph.branches[graph.count];
    b->id = graph.count;
    strncpy(b->name, name, sizeof(b->name) - 1);
    b->name[sizeof(b->name) - 1] = '\0';
    b->conn_count = 0;
    b->created_ts = time(NULL);
    strncpy(b->state, "running", sizeof(b->state) - 1);
    b->origin = 0;
    b->parent = -1;
    int id = graph.count++;
    save_state();
    return id;
}

int bm_clone(int id, const char *name) {
    if (id < 0 || id >= graph.count) return BM_ERR_INVALID;
    char newname[32];
    if (name) {
        strncpy(newname, name, sizeof(newname)-1);
        newname[sizeof(newname)-1] = '\0';
    } else {
        snprintf(newname, sizeof(newname), "%s_clone", graph.branches[id].name);
    }
    int nid = bm_create(newname);
    if (nid >= 0) {
        Branch *b = &graph.branches[nid];
        b->conn_count = 1;
        b->connections[0] = id;
        b->parent = id;
        /* link parent->child and child->parent for graph traversal */
        graph.adj[id][nid] = 1;
        graph.adj[nid][id] = 1;
    }
    return nid;
}

int bm_create_remote(const char *name) {
    int id = bm_create(name);
    if (id >= 0) graph.branches[id].origin = 1;
    return id;
}

int bm_switch(int id) {
    if (id < 0 || id >= graph.count)
        return BM_ERR_INVALID;
    current_branch = id;
    save_state();
    return BM_SUCCESS;
}

int bm_connect(int from, int to) {
    if (from < 0 || from >= graph.count || to < 0 || to >= graph.count)
        return BM_ERR_INVALID;
    Branch *bf = &graph.branches[from];
    Branch *bt = &graph.branches[to];
    // Avoid duplicates
    for (int i=0;i<bf->conn_count;i++) if (bf->connections[i]==to) return BM_ERR_DUPLICATE;
    if (bf->conn_count < MAX_BRANCHES)
        bf->connections[bf->conn_count++] = to;
    graph.adj[from][to] = 1;
    for (int i=0;i<bt->conn_count;i++) if (bt->connections[i]==from) return BM_ERR_DUPLICATE;
    if (bt->conn_count < MAX_BRANCHES)
        bt->connections[bt->conn_count++] = from;
    graph.adj[to][from] = 1;
    save_state();
    return BM_SUCCESS;
}

int bm_list(Branch *out) {
    if (!out) return 0;
    for (int i=0; i<graph.count; ++i)
        out[i] = graph.branches[i];
    return graph.count;
}

int bm_stop(int id) {
    if (id < 0 || id >= graph.count)
        return BM_ERR_INVALID;
    strncpy(graph.branches[id].state, "stopped", sizeof(graph.branches[id].state)-1);
    save_state();
    return BM_SUCCESS;
}

int bm_delete(int id) {
    if (id < 0 || id >= graph.count)
        return BM_ERR_INVALID;
    for (int i = id; i < graph.count - 1; i++) {
        graph.branches[i] = graph.branches[i + 1];
        graph.branches[i].id = i;
        for (int j=0;j<graph.count;j++)
            graph.adj[i][j] = graph.adj[i+1][j];
    }
    for (int j = id; j < graph.count - 1; j++)
        for (int i=0;i<graph.count-1;i++)
            graph.adj[i][j] = graph.adj[i][j+1];
    graph.count--;
    save_state();
    return BM_SUCCESS;
}

int bm_graph(BranchGraph *out) {
    if (!out) return BM_ERR_INVALID;
    *out = graph;
    return graph.count;
}

const char *bm_current_name(void) {
    if (current_branch >= 0 && current_branch < graph.count)
        return graph.branches[current_branch].name;
    return "default";
}

int bm_current_id(void) {
    return current_branch;
}


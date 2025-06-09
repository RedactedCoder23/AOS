#include "branch.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <limits.h>

static Branch branches[MAX_BRANCHES];
static int branch_count = 0;
static int current_branch = -1;
static char save_path[PATH_MAX];

static void save_state(void) {
    if (!save_path[0]) return;
    FILE *f = fopen(save_path, "wb");
    if (!f) return;
    fwrite(&branch_count, sizeof(branch_count), 1, f);
    fwrite(&current_branch, sizeof(current_branch), 1, f);
    fwrite(branches, sizeof(Branch), MAX_BRANCHES, f);
    fclose(f);
}

static void load_state(void) {
    if (!save_path[0]) return;
    FILE *f = fopen(save_path, "rb");
    if (!f) return;
    fread(&branch_count, sizeof(branch_count), 1, f);
    fread(&current_branch, sizeof(current_branch), 1, f);
    fread(branches, sizeof(Branch), MAX_BRANCHES, f);
    fclose(f);
}

void bm_init(void) {
    const char *home = getenv("HOME");
    branch_count = 0;
    current_branch = -1;
    if (home) {
        snprintf(save_path, sizeof(save_path), "%s/.aos", home);
        mkdir(save_path, 0755);
        strncat(save_path, "/branches.bin", sizeof(save_path)-strlen(save_path)-1);
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
    strncpy(b->name, name, sizeof(b->name)-1);
    b->name[sizeof(b->name)-1] = '\0';
    b->conn_count = 0;
    int id = branch_count++;
    save_state();
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


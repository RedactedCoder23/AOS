#include "branch.h"
/* [2025-06-09 06:06 UTC] Legacy branch subsystem
 * by: codex
 * Edge cases: differs from BranchGraph manager and may be unused.
 * Next agent must: see AGENT.md "UNRESOLVED ISSUES".
 */
#include <stdio.h>
#include <string.h>

#define MAX_BRANCHES 8
static branch branches[MAX_BRANCHES];
static int count;

void branch_init(void) { count = 0; }

int branch_create(const char *name) {
    if (count >= MAX_BRANCHES)
        return -1;
    branches[count].id = count;
    strncpy(branches[count].name, name, 31);
    branches[count].name[31] = '\0';
    strcpy(branches[count].state, "running");
    branches[count].parent = NULL;
    count++;
    return branches[count - 1].id;
}

int branch_clone(int id, const char *name) {
    if (id < 0 || id >= count)
        return -1; /* invalid source */
    if (count >= MAX_BRANCHES)
        return -2; /* overflow */
    branches[count] = branches[id];
    branches[count].id = count;
    if (name) {
        strncpy(branches[count].name, name, 31);
        branches[count].name[31] = '\0';
    }
    branches[count].parent = &branches[id];
    strcpy(branches[count].state, "running");
    count++;
    return branches[count - 1].id;
}

int branch_list(branch_graph *out) {
    if (!out)
        return -1;
    out->count = count;
    for (int i = 0; i < count; i++) {
        out->branches[i] = branches[i];
        for (int j = 0; j < count; j++)
            out->adj[i][j] = 0;
        if (branches[i].parent)
            out->adj[i][branches[i].parent->id] = 1;
    }
    return count;
}

int branch_stop(int id) {
    if (id < 0 || id >= count)
        return -1;
    strcpy(branches[id].state, "stopped");
    return 0;
}

int branch_delete(int id) {
    if (id < 0 || id >= count)
        return -1;
    for (int i = id; i < count - 1; i++)
        branches[i] = branches[i + 1];
    count--;
    return 0;
}

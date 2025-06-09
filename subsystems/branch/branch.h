#ifndef BRANCH_H
#define BRANCH_H

typedef struct branch {
    int id;
    char name[32];
    char state[16];
    struct branch *parent;
} branch;

typedef struct branch_graph {
    branch branches[8];
    int adj[8][8];
    int count;
} branch_graph;

void branch_init(void);
int branch_create(const char *name);
int branch_clone(int id, const char *name);
int branch_list(branch_graph *out);
int branch_stop(int id);
int branch_delete(int id);

#endif

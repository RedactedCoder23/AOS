#ifndef BRANCH_H
#define BRANCH_H

#define MAX_BRANCHES 16

// Return codes
#define BM_SUCCESS 0
#define BM_ERR_FULL -1
#define BM_ERR_DUPLICATE -2
#define BM_ERR_INVALID -3

typedef struct Branch {
    int id;
    char name[32];
    int connections[MAX_BRANCHES];
    int conn_count;
} Branch;

// Initialize branch manager
void bm_init(void);
// Create a branch with given name. Returns new id or negative error code.
int bm_create(const char *name);
// Switch to branch id. Returns BM_SUCCESS or error.
int bm_switch(int id);
// Connect two branches bidirectionally. Returns BM_SUCCESS or error.
int bm_connect(int from, int to);
// Copy branch list into out array, returns count
int bm_list(Branch *out);

#endif // BRANCH_H

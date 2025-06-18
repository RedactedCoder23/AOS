/*
 * AOS — branch.h
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
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
    long created_ts;
    char state[16];
    int origin; /* 0=local,1=remote */
    int parent; /* -1 for root */
} Branch;

typedef struct BranchGraph {
    Branch branches[MAX_BRANCHES];
    int adj[MAX_BRANCHES][MAX_BRANCHES];
    int count;
} BranchGraph;

// Initialize branch manager
void bm_init(void);
// Create a branch with given name. Returns new id or negative error code.
int bm_create(const char *name);
int bm_clone(int id, const char *name);
int bm_create_remote(const char *name);
// Switch to branch id. Returns BM_SUCCESS or error.
int bm_switch(int id);
// Connect two branches bidirectionally. Returns BM_SUCCESS or error.
int bm_connect(int from, int to);
int bm_stop(int id);
int bm_delete(int id);
// Copy branch list into out array, returns count
int bm_list(Branch *out);
int bm_graph(BranchGraph *out);
void branch_save_all(void);
void branch_load_all(void);

// VM management
int bm_vm_create(const char *name, const char *image, int mem, int cpu);
void bm_vm_list(void);
int bm_vm_switch(int id);
int bm_vm_stop(int id);

/* Return name of current branch or "default" if none */
const char *bm_current_name(void);
/* Return id of current branch or -1 if none */
int bm_current_id(void);

// Networking
int br_peer_add(const char *addr);
int br_sync(void);
void br_set_port(int port);
void br_start_service(void);
int br_discover(char out[][64], int max);
int br_sync_peer(const char *addr);
int bm_sync_all(void);
/*
Graph JSON format:
{ "branches": [ { "id":1, "name":"shell" }, ... ],
  "edges": [ [1,2], [2,3], ... ] }
*/
char *bm_graph_export_json(void);
void bm_graph_merge_json(const char *json);

#endif // BRANCH_H

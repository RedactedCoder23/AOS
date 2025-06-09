#include "../include/branch.h"

static Branch branches[MAX_BRANCHES];
static int branch_count;

void bm_init(void) {
    branch_count = 0;
}

int bm_create(const char *name) {
    if (branch_count >= MAX_BRANCHES)
        return BM_ERR_FULL;
    Branch *b = &branches[branch_count];
    b->id = branch_count;
    int i=0;
    for (; name[i] && i < (int)sizeof(b->name)-1; i++)
        b->name[i] = name[i];
    b->name[i] = '\0';
    b->conn_count = 0;
    const char *st = "running";
    i=0;
    while (st[i] && i < (int)sizeof(b->state)-1) { b->state[i]=st[i]; i++; }
    b->state[i]='\0';
    b->origin = 0;
    branch_count++;
    return b->id;
}

int bm_clone(int id, const char *name){
    if(id < 0 || id >= branch_count)
        return BM_ERR_INVALID;
    if(branch_count >= MAX_BRANCHES)
        return BM_ERR_FULL;
    Branch *src = &branches[id];
    Branch *b = &branches[branch_count];
    *b = *src;
    b->id = branch_count;
    if(name){
        int i=0; for(; name[i] && i < (int)sizeof(b->name)-1; i++) b->name[i]=name[i];
        b->name[i]='\0';
    }
    b->conn_count = 1;
    b->connections[0] = id;
    branch_count++;
    return b->id;
}

int bm_list(Branch *out) {
    if (!out) return 0;
    for (int i=0; i<branch_count; i++) out[i] = branches[i];
    return branch_count;
}

int bm_graph(BranchGraph *out){
    if(!out) return BM_ERR_INVALID;
    out->count = branch_count;
    for(int i=0;i<branch_count;i++){
        out->branches[i]=branches[i];
        for(int j=0;j<branch_count;j++) out->adj[i][j]=0;
        for(int k=0;k<branches[i].conn_count;k++){
            int t = branches[i].connections[k];
            if(t >=0 && t < branch_count)
                out->adj[i][t] = 1;
        }
    }
    return branch_count;
}

int bm_stop(int id) { (void)id; return BM_ERR_INVALID; }
int bm_delete(int id) { (void)id; return BM_ERR_INVALID; }
int bm_create_remote(const char *name) { (void)name; return BM_ERR_INVALID; }
int bm_switch(int id) { (void)id; return BM_ERR_INVALID; }
int bm_connect(int from, int to) { (void)from;(void)to; return BM_ERR_INVALID; }
int bm_vm_create(const char *n,const char *i,int m,int c){(void)n;(void)i;(void)m;(void)c;return BM_ERR_INVALID;}
void bm_vm_list(void) {}
int bm_vm_switch(int id){(void)id;return BM_ERR_INVALID;}
int bm_vm_stop(int id){(void)id;return BM_ERR_INVALID;}
int br_peer_add(const char *a){(void)a;return BM_ERR_INVALID;}
int br_sync(void){return BM_ERR_INVALID;}
void br_set_port(int p){(void)p;}
int bm_sync_all(void){return BM_ERR_INVALID;}
char *bm_graph_export_json(void){return 0;}
void bm_graph_merge_json(const char *j){(void)j;}

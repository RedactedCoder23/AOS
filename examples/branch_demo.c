#include "branch.h"
#include <stdio.h>
int main(void){
    branch_init();
    branch_create("main");
    branch_create("ui");
    branch_stop(1);
    branch_graph g; branch_list(&g);
    for(int i=0;i<g.count;i++)
        printf("%d:%s (%s)\n", g.branches[i].id, g.branches[i].name, g.branches[i].state);
    branch_delete(1);
    branch_list(&g);
    for(int i=0;i<g.count;i++)
        printf("%d:%s (%s)\n", g.branches[i].id, g.branches[i].name, g.branches[i].state);
    return 0;
}

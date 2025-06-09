#include "branch.h"
#include <assert.h>
#include <stdio.h>

int main(void) {
    branch_graph g;
    branch_init();
    int main_id = branch_create("main");
    assert(main_id >= 0);
    assert(branch_clone(main_id, "dev") >= 0);
    assert(branch_list(&g) == 2);
    assert(g.count == 2);
    assert(branch_stop(main_id) == 0);
    assert(branch_delete(main_id) == 0);
    printf("branch tests passed\n");
    return 0;
}

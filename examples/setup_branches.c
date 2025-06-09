#include "branch.h"
int main(void){
    bm_init();
    bm_create("tree");
    bm_create("shell");
    bm_create("ai");
    bm_connect(0,1);
    bm_connect(1,2);
    return 0;
}

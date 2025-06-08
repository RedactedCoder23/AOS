#include "branch.h"
#include <stdio.h>
int main(void){
    branch_init();
    branch_create("main");
    branch_create("ui");
    branch_list();
    return 0;
}

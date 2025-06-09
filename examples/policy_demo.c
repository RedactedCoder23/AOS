#include "policy.h"
#include <stdio.h>
int main(void){
    policy_load("allow-all");
    if(policy_check("test")) printf("allowed\n");
    return 0;
}

#include "policy.h"
#include <stdio.h>
int main(void){
    policy_load("allow-all");
    if(policy_check("test")) printf("allowed\n");
    /* load deny rule and verify blocking */
    policy_load("deny test");
    if(!policy_check("test")) printf("denied\n");
    return 0;
}

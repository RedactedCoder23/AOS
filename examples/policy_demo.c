#include "policy.h"
#include <stdio.h>

int main(void){
    const char *allow_json = "[{\"syscall\":\"TEST\",\"allow\":true}]";
    const char *deny_json  = "[{\"syscall\":\"TEST\",\"allow\":false}]";
    policy_load(allow_json);
    if(policy_check("TEST")) printf("allowed\n");
    policy_load(deny_json);
    if(!policy_check("TEST")) printf("denied\n");
    return 0;
}

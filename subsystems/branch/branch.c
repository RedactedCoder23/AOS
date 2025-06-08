#include "branch.h"
#include <stdio.h>
#include <string.h>

#define MAX_BRANCHES 8
static char names[MAX_BRANCHES][32];
static int count;

void branch_init(){count=0;}

int branch_create(const char *name){
    if(count>=MAX_BRANCHES) return -1;
    strncpy(names[count],name,31);
    count++; return 0;
}

void branch_list(){
    for(int i=0;i<count;i++) printf("%s\n", names[i]);
}

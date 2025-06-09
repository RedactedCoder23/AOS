#include "branch.h"
#include <stdio.h>
#include <string.h>

#define MAX_BRANCHES 8
typedef struct { char name[32]; char state[16]; } B;
static B branches[MAX_BRANCHES];
static int count;

void branch_init(){count=0;}

int branch_create(const char *name){
    if(count>=MAX_BRANCHES) return -1;
    strncpy(branches[count].name,name,31);
    strcpy(branches[count].state,"running");
    count++; return 0;
}

void branch_list(){
    for(int i=0;i<count;i++) printf("%d:%s (%s)\n", i, branches[i].name, branches[i].state);
}

int branch_stop(int id){
    if(id<0||id>=count) return -1;
    strcpy(branches[id].state,"stopped");
    return 0;
}

int branch_delete(int id){
    if(id<0||id>=count) return -1;
    for(int i=id;i<count-1;i++) branches[i]=branches[i+1];
    count--; return 0;
}

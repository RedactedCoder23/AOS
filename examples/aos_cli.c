#include "branch.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv){
    if(argc < 3){
        printf("usage: %s branch <create|clone|list> [args]\n", argv[0]);
        return 1;
    }
    branch_init();
    if(strcmp(argv[1], "branch") != 0){
        printf("unknown command %s\n", argv[1]);
        return 1;
    }
    if(strcmp(argv[2], "create") == 0){
        if(argc < 4){
            printf("missing branch name\n");
            return 1;
        }
        int id = branch_create(argv[3]);
        if(id < 0){
            printf("error creating branch\n");
            return 1;
        }
        printf("created %d\n", id);
    } else if(strcmp(argv[2], "clone") == 0){
        if(argc < 5){
            printf("usage: %s branch clone <id> <name>\n", argv[0]);
            return 1;
        }
        int src = atoi(argv[3]);
        int id = branch_clone(src, argv[4]);
        if(id == -1){
            printf("invalid branch id %d\n", src);
            return 1;
        } else if(id == -2){
            printf("branch limit reached\n");
            return 1;
        }
        printf("cloned %d\n", id);
    } else if(strcmp(argv[2], "list") == 0){
        branch_graph g;
        branch_list(&g);
        for(int i=0;i<g.count;i++)
            printf("%d:%s (%s)\n", g.branches[i].id, g.branches[i].name, g.branches[i].state);
    } else {
        printf("unknown subcommand %s\n", argv[2]);
        return 1;
    }
    return 0;
}

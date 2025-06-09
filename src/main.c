#include <stdio.h>
/* [2025-06-09 06:06 UTC] Host-side REPL entry
 * by: codex
 * Edge cases: input parsing brittle; no history or completion.
 * Next agent must: add command history and sanitize input.
 */
#include <string.h>
#include <stdlib.h>
#include "branch.h"
#include "ai.h"

static void log_agent_error(const char *msg) {
    FILE *f = fopen("AGENT.md", "a");
    if (f) { fprintf(f, "UNRESOLVED: %s\n", msg); fclose(f); }
}

static void print_list(void) {
    Branch b[MAX_BRANCHES];
    int n = bm_list(b);
    for (int i=0;i<n;i++)
        printf("%d:%s (%s)\n", b[i].id, b[i].name, b[i].state);
}

static void tree_rec(BranchGraph *g, int id, int d) {
    for (int i=0;i<d;i++) printf("  ");
    printf("%s\n", g->branches[id].name);
    for (int j=0;j<g->count;j++)
        if (g->adj[id][j] && g->branches[j].parent==id)
            tree_rec(g, j, d+1);
}

static void print_tree(void) {
    BranchGraph g;
    bm_graph(&g);
    for (int i=0;i<g.count;i++)
        if (g.branches[i].parent==-1)
            tree_rec(&g, i, 0);
}

int main(void) {
    char line[256];

    bm_init();
    branch_load_all();

    printf("AOS> ");
    while (fgets(line, sizeof(line), stdin)) {
        char *nl = strchr(line, '\n');
        if (nl) *nl = '\0';
        if (strcmp(line, "exit") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) { printf("AOS> "); continue; }
        if (strcmp(cmd, "branch") == 0) {
            char *sub = strtok(NULL, " ");
            if (!sub) {
                printf("usage: branch <create|clone|list|tree|save|load>\n");
                log_agent_error("branch missing subcommand");
            } else if (strcmp(sub, "create") == 0) {
                char *name = strtok(NULL, " ");
                if (!name) { printf("missing name\n"); log_agent_error("branch create missing name"); }
                else { int id = bm_create(name); if (id<0) printf("error\n"); else printf("created %d\n", id); }
            } else if (strcmp(sub, "clone") == 0) {
                char *sid = strtok(NULL, " ");
                char *name = strtok(NULL, " ");
                if (!sid || !name) { printf("usage: branch clone <id> <name>\n"); log_agent_error("branch clone args"); }
                else { int id = bm_clone(atoi(sid), name); if (id<0) printf("clone error\n"); else printf("cloned %d\n", id); }
            } else if (strcmp(sub, "list") == 0) {
                print_list();
            } else if (strcmp(sub, "tree") == 0) {
                print_tree();
            } else if (strcmp(sub, "save") == 0) {
                branch_save_all();
                printf("saved\n");
            } else if (strcmp(sub, "load") == 0) {
                branch_load_all();
                printf("loaded\n");
            } else {
                printf("unknown subcommand %s\n", sub);
                log_agent_error("branch unknown subcommand");
            }
        } else if (strcmp(cmd, "ai") == 0) {
            char *question = strtok(NULL, "");
            if (!question) {
                printf("usage: ai <question>\n");
                log_agent_error("ai missing question");
            } else {
                char out[64];
                ai_infer(question, out, sizeof(out));
                printf("%s\n", out);
            }
        } else {
            printf("Unknown command\n");
            log_agent_error("unknown command");
        }
        printf("AOS> ");
    }
    return 0;
}

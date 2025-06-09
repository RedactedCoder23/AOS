#include <stdio.h>
/* [2025-06-09 06:06 UTC] Host-side REPL entry
 * by: codex
 * Edge cases: input parsing brittle; no history or completion. Networking thread not joined.
 * Next agent must: see AGENT.md "UNRESOLVED ISSUES".
 */
#include <string.h>
#include <stdlib.h>
#include "branch.h"
#include "ai.h"
#include "plugin.h"
#include "fs.h"
codex/implement-minimal-runtime-and-installer
#include "app_runtime.h"
=======
#include "config.h"
 main

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
    config_load_default();
    fs_init();
    if (ai_config_load() != 0)
        printf("Run 'ai setup' to configure provider\n");
    const char *p = getenv("AOS_PORT");
    if (p) br_set_port(atoi(p));
    br_start_service();
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
            } else if (strcmp(sub, "sync") == 0) {
                char *addr = strtok(NULL, " ");
                if (!addr) { printf("usage: branch sync <peer>\n"); }
                else br_sync_peer(addr);
            } else {
                printf("unknown subcommand %s\n", sub);
            }
        } else if (strcmp(cmd, "peer") == 0) {
            char *sub = strtok(NULL, " ");
            if (sub && strcmp(sub, "ls") == 0) {
                char found[8][64];
                int n = br_discover(found, 8);
                for (int i=0;i<n;i++) printf("%s\n", found[i]);
            } else {
                printf("usage: peer ls\n");
            }
        } else if (strcmp(cmd, "fs") == 0) {
            char *sub = strtok(NULL, " ");
            if (!sub) {
                printf("usage: fs <mkdir|open|write|read|close|delete|backend|ls> ...\n");
            } else if (strcmp(sub, "mkdir") == 0) {
                char *name = strtok(NULL, " ");
                if (!name) printf("missing name\n");
                else if (fs_mkdir(name) < 0) printf("mkdir error\n");
            } else if (strcmp(sub, "open") == 0) {
                char *name = strtok(NULL, " ");
                char *mode = strtok(NULL, " ");
                if (!name || !mode) {
                    printf("usage: fs open <name> <mode>\n");
                } else {
                    int fd = fs_open(name, mode);
                    if (fd < 0) printf("open error\n");
                    else printf("%d\n", fd);
                }
            } else if (strcmp(sub, "write") == 0) {
                char *sfd = strtok(NULL, " ");
                char *text = strtok(NULL, "");
                if (!sfd || !text) {
                    printf("usage: fs write <fd> <text>\n");
                } else {
                    int fd = atoi(sfd);
                    size_t w = fs_write(fd, text, strlen(text));
                    printf("%zu\n", w);
                }
            } else if (strcmp(sub, "read") == 0) {
                char *sfd = strtok(NULL, " ");
                char *bytes = strtok(NULL, " ");
                if (!sfd || !bytes) {
                    printf("usage: fs read <fd> <bytes>\n");
                } else {
                    int fd = atoi(sfd);
                    int n = atoi(bytes);
                    char buf[256]; if (n > 255) n = 255;
                    size_t r = fs_read(fd, buf, n);
                    buf[r] = '\0';
                    printf("%s\n", buf);
                }
            } else if (strcmp(sub, "close") == 0) {
                char *sfd = strtok(NULL, " ");
                if (!sfd) printf("usage: fs close <fd>\n");
                else fs_close(atoi(sfd));
            } else if (strcmp(sub, "delete") == 0) {
                char *name = strtok(NULL, " ");
                if (!name) printf("usage: fs delete <name>\n");
                else if (fs_delete(name) < 0) printf("delete error\n");
            } else if (strcmp(sub, "backend") == 0) {
                char *type = strtok(NULL, " ");
                if (!type) {
                    printf("usage: fs backend <ram|disk>\n");
                } else if (strcmp(type, "ram") == 0) {
                    fs_use_ramfs();
                } else if (strcmp(type, "disk") == 0) {
                    if (fs_use_persistent() != 0) printf("mount error\n");
                } else {
                    printf("unknown backend %s\n", type);
                }
            } else if (strcmp(sub, "ls") == 0) {
                fs_ls();
            } else {
                printf("unknown subcommand %s\n", sub);
            }
        } else if (strcmp(cmd, "ai") == 0) {
            char *arg = strtok(NULL, "");
            if (!arg) {
                printf("usage: ai <question|setup>\n");
                log_agent_error("ai missing arg");
            } else if (strcmp(arg, "setup") == 0) {
                if (ai_setup_wizard() == 0) printf("config saved\n");
                else printf("setup failed\n");
            } else {
                char out[64];
                ai_infer(arg, out, sizeof(out));
                printf("%s\n", out);
            }
        } else if (strcmp(cmd, "plugin") == 0) {
            char *sub = strtok(NULL, " ");
            if (!sub) {
                printf("usage: plugin <load|unload|list> <file|name>\n");
                log_agent_error("plugin missing subcommand");
            } else if (strcmp(sub, "load") == 0) {
                char *file = strtok(NULL, " ");
                if (!file) { printf("missing file\n"); }
                else if (plugin_load(file) != 0) printf("load failed\n");
            } else if (strcmp(sub, "unload") == 0) {
                char *name = strtok(NULL, " ");
                if (!name) { printf("missing name\n"); }
                else if (plugin_unload(name) != 0) printf("unload failed\n");
            } else if (strcmp(sub, "list") == 0) {
                plugin_list();
            } else {
                printf("unknown subcommand %s\n", sub);
                log_agent_error("plugin unknown subcommand");
            }
        } else if (strcmp(cmd, "app") == 0) {
            char *sub = strtok(NULL, " ");
            if (!sub) {
                printf("usage: app <run|list> [name args...]\n");
            } else if (strcmp(sub, "list") == 0) {
                app_list();
            } else if (strcmp(sub, "run") == 0) {
                char *name = strtok(NULL, " ");
                if (!name) { printf("usage: app run <name> [args]\n"); }
                else {
                    char *pargs[8];
                    int n=0; char *a;
                    while ((a = strtok(NULL, " ")) && n<8) pargs[n++]=a;
                    pargs[n]=NULL;
                    app_run(name, n, pargs);
                }
            } else {
                printf("unknown subcommand %s\n", sub);
            }
        } else {
            printf("Unknown command\n");
        }
        printf("AOS> ");
    }
    return 0;
}

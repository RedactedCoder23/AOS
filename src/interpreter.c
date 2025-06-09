#include <stdio.h>
#include "interpreter.h"
#include "ui_graph.h"
#include "memory.h"
#include "fs.h"
#include "ai.h"
#include "branch.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

void add_command(const char *name, command_handler handler) {
    (void)handler;
    printf("Registered command: %s\n", name);
}

void cmd_hello_wrapper(int argc, char **argv) {
    (void)argc; (void)argv;
    printf("Hello, AOS!\n");
}

void cmd_bye_wrapper(int argc, char **argv) {
    (void)argc; (void)argv;
    printf("Goodbye from AOS!\n");
}

void ui_graph_wrapper(int argc, char **argv) {
    (void)argc; (void)argv;
    ui_graph_run();
}

void cmd_ui_graph_wrapper(int argc, char **argv) {
    ui_graph_wrapper(argc, argv);
}

static unsigned char mem_pool[4096];
static int subsys_init = 0;
static void ensure_init(void) {
    if (subsys_init) return;
    memory_init(mem_pool, sizeof(mem_pool));
    fs_init();
    ai_init("default");
    bm_init();
    subsys_init = 1;
}

static void log_checklist(const char *msg) {
    FILE *f = fopen("AOS-CHECKLIST.log", "a");
    if (f) {
        fprintf(f, "%s\n", msg);
        fclose(f);
    }
}

void cmd_mem_alloc_wrapper(int argc, char **argv) {
    ensure_init();
    if (argc < 2) {
        printf("Error: usage: MEM_ALLOC <bytes>\n");
        log_checklist("MEM_ALLOC missing arg");
        return;
    }
    char *end;
    errno = 0;
    size_t sz = strtoul(argv[1], &end, 10);
    if (*end || errno != 0) {
        printf("Error: invalid size '%s'\n", argv[1]);
        log_checklist("MEM_ALLOC invalid size");
        return;
    }
    void *p = memory_alloc(sz);
    if (!p) {
        printf("Error: out of memory\n");
        log_checklist("MEM_ALLOC out of memory");
        return;
    }
    printf("Allocated %zu at %p\n", sz, p);
}

void cmd_mem_free_wrapper(int argc, char **argv) {
    ensure_init();
    if (argc < 2) {
        printf("Error: usage: MEM_FREE <ptr>\n");
        log_checklist("MEM_FREE missing arg");
        return;
    }
    char *end;
    errno = 0;
    unsigned long long addr = strtoull(argv[1], &end, 0);
    if (*end || errno != 0) {
        printf("Error: invalid pointer '%s'\n", argv[1]);
        log_checklist("MEM_FREE invalid pointer");
        return;
    }
    void *p = (void*)addr;
    memory_free(p);
    printf("Freed %p\n", p);
}

void cmd_fs_open_wrapper(int argc, char **argv) {
    ensure_init();
    if (argc < 3) {
        printf("Error: usage: FS_OPEN <name> <mode>\n");
        log_checklist("FS_OPEN missing arg");
        return;
    }
    int fd = fs_open(argv[1], argv[2]);
    if (fd < 0) {
        printf("Error: open %s failed\n", argv[1]);
        log_checklist("FS_OPEN failed");
        return;
    }
    printf("fd=%d\n", fd);
}

void cmd_fs_write_wrapper(int argc, char **argv) {
    ensure_init();
    if (argc < 3) {
        printf("Error: usage: FS_WRITE <fd> <text>\n");
        log_checklist("FS_WRITE missing arg");
        return;
    }
    char *end;
    errno = 0;
    int fd = strtol(argv[1], &end, 10);
    if (*end || errno != 0) {
        printf("Error: invalid fd '%s'\n", argv[1]);
        log_checklist("FS_WRITE invalid fd");
        return;
    }
    size_t w = fs_write(fd, argv[2], strlen(argv[2]));
    if (w == 0) {
        printf("Error: write failed\n");
        log_checklist("FS_WRITE failed");
    } else {
        printf("Wrote %zu bytes\n", w);
    }
}

void cmd_fs_read_wrapper(int argc, char **argv) {
    ensure_init();
    if (argc < 3) {
        printf("Error: usage: FS_READ <fd> <bytes>\n");
        log_checklist("FS_READ missing arg");
        return;
    }
    char *end;
    errno = 0;
    int fd = strtol(argv[1], &end, 10);
    if (*end || errno != 0) {
        printf("Error: invalid fd '%s'\n", argv[1]);
        log_checklist("FS_READ invalid fd");
        return;
    }
    errno = 0;
    size_t n = strtoul(argv[2], &end, 10);
    if (*end || errno != 0) {
        printf("Error: invalid byte count '%s'\n", argv[2]);
        log_checklist("FS_READ invalid count");
        return;
    }
    char buf[256];
    if (n >= sizeof(buf)) n = sizeof(buf)-1;
    size_t r = fs_read(fd, buf, n);
    buf[r] = '\0';
    if (r == 0) {
        printf("Error: read failed or EOF\n");
        log_checklist("FS_READ failed");
    } else {
        printf("%s\n", buf);
    }
}

void cmd_fs_close_wrapper(int argc, char **argv) {
    ensure_init();
    if (argc < 2) {
        printf("Error: usage: FS_CLOSE <fd>\n");
        log_checklist("FS_CLOSE missing arg");
        return;
    }
    char *end;
    errno = 0;
    int fd = strtol(argv[1], &end, 10);
    if (*end || errno != 0) {
        printf("Error: invalid fd '%s'\n", argv[1]);
        log_checklist("FS_CLOSE invalid fd");
        return;
    }
    fs_close(fd);
    printf("Closed %d\n", fd);
}

void cmd_fs_ls_wrapper(int argc, char **argv) {
    (void)argc; (void)argv; ensure_init();
    fs_ls();
}

void cmd_ai_prompt_wrapper(int argc, char **argv) {
    ensure_init();
    if (argc < 2) {
        printf("Error: usage: AI_PROMPT <text>\n");
        log_checklist("AI_PROMPT missing arg");
        return;
    }
    const char *resp = ai_reply(argv[1]);
    if (strcmp(resp, "init error") == 0) {
        printf("Error: AI service unavailable\n");
        log_checklist("AI_PROMPT failure");
    } else {
        printf("%s\n", resp);
    }
}

void cmd_ai_profile_wrapper(int argc, char **argv) {
    ensure_init();
    if (argc < 2) {
        printf("Error: usage: AI_PROFILE <name>\n");
        log_checklist("AI_PROFILE missing arg");
        return;
    }
    ai_shutdown();
    ai_init(argv[1]);
    printf("Profile set to %s\n", argv[1]);
}

void cmd_br_create_wrapper(int argc, char **argv) {
    ensure_init();
    if (argc < 2) {
        printf("Error: usage: BR_CREATE <name>\n");
        log_checklist("BR_CREATE missing arg");
        return;
    }
    int id = bm_create(argv[1]);
    if (id < 0) {
        printf("Error: failed to create branch %s\n", argv[1]);
        log_checklist("BR_CREATE failed");
        return;
    }
    printf("Created %d\n", id);
}

void cmd_br_list_wrapper(int argc, char **argv) {
    ensure_init();
    Branch b[MAX_BRANCHES];
    int n = bm_list(b);
    for (int i=0;i<n;i++)
        printf("%d:%s (%s)\n", b[i].id, b[i].name, b[i].state);
}

void cmd_br_stop_wrapper(int argc, char **argv) {
    ensure_init();
    if (argc < 2) {
        printf("Error: usage: BR_STOP <id>\n");
        log_checklist("BR_STOP missing arg");
        return;
    }
    int rc = bm_stop(atoi(argv[1]));
    if (rc != BM_SUCCESS) {
        printf("Error: invalid branch id\n");
        log_checklist("BR_STOP invalid id");
    } else {
        printf("Stopped branch %s\n", argv[1]);
    }
}

void cmd_br_delete_wrapper(int argc, char **argv) {
    ensure_init();
    if (argc < 2) {
        printf("Error: usage: BR_DELETE <id>\n");
        log_checklist("BR_DELETE missing arg");
        return;
    }
    int rc = bm_delete(atoi(argv[1]));
    if (rc != BM_SUCCESS) {
        printf("Error: invalid branch id\n");
        log_checklist("BR_DELETE invalid id");
    } else {
        printf("Deleted branch %s\n", argv[1]);
    }
}

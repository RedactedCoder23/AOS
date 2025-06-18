/* AOS — subsystems/security/security.c — Purpose: C source file */
/*
 * AOS — security.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "security.h"
#include "logging.h"
#include "fs.h"
#include "ipc.h"
#include <string.h>

/* simple ACL table */
#define MAX_ACL 32

typedef struct {
    int id;
    int allow;
} AclEntry;

static AclEntry acl_table[MAX_ACL];
static int acl_count;

/* syscall name mapping matching SyscallID enum order */
static const char *sys_names[] = {
    "SYS_NONE",        "SYS_FORK_BRANCH", "SYS_MERGE_BRANCH",
    "SYS_DELETE_BRANCH", "SYS_LIST_BRANCH", "SYS_AI_QUERY",
    "SYS_AI_MODEL_INFO", "SYS_FS_OPEN",     "SYS_FS_READ",
    "SYS_FS_WRITE",     "SYS_FS_CLOSE",    "SYS_FS_LIST",
};

static int name_to_id(const char *name) {
    for (int i = 0; i < (int)SYS_MAX && i < (int)(sizeof(sys_names) / sizeof(sys_names[0])); i++) {
        if (strcmp(name, sys_names[i]) == 0)
            return i;
    }
    return -1;
}

static void parse_line(const char *line) {
    char name[32];
    char action[16];
    if (sscanf(line, "%31s %15s", name, action) != 2)
        return;
    int id = name_to_id(name);
    if (id < 0 || acl_count >= MAX_ACL)
        return;
    acl_table[acl_count].id = id;
    acl_table[acl_count].allow =
        (strncmp(action, "allow", 5) == 0 || strncmp(action, "ALLOW", 5) == 0);
    acl_count++;
}

void security_init(void) {
    acl_count = 0;
    int fd = fs_open("/etc/aos.acl", "r");
    if (fd < 0) {
        log_message(LOG_WARN, "security: no ACL file found");
        return;
    }

    char buf[128];
    char line[128];
    size_t line_len = 0;
    size_t n;
    while ((n = fs_read(fd, buf, sizeof(buf))) > 0) {
        for (size_t i = 0; i < n; i++) {
            char c = buf[i];
            if (c == '\n' || line_len >= sizeof(line) - 1) {
                line[line_len] = '\0';
                if (line_len)
                    parse_line(line);
                line_len = 0;
            } else if (c != '\r') {
                line[line_len++] = c;
            }
        }
    }
    if (line_len) {
        line[line_len] = '\0';
        parse_line(line);
    }
    fs_close(fd);

    log_message(LOG_INFO, "security acl loaded %d entries", acl_count);
}

int check_capability(const char *cap) {
    log_message(LOG_INFO, "checking cap %s", cap);
    return 0;
}

int security_check(int syscall_id) {
    for (int i = 0; i < acl_count; i++) {
        if (acl_table[i].id == syscall_id) {
            if (!acl_table[i].allow) {
                const char *name = (syscall_id >= 0 && syscall_id < (int)(sizeof(sys_names)/sizeof(sys_names[0]))) ? sys_names[syscall_id] : "?";
                log_message(LOG_WARN, "security deny %s", name);
                return -1;
            }
            return 0;
        }
    }
    return 0; /* default allow */
}


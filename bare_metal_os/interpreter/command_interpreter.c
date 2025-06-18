/* AOS — bare_metal_os/interpreter/command_interpreter.c — Purpose: C source file */
/*
 * AOS — command_interpreter.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "command_interpreter.h"
#include "../include/branch.h"
#include "../include/syscalls.h"
#include <stddef.h>
#include <stdint.h>
#include "../memory.h"
#include "../fs_bare.h"

/* bare-metal subsystem prototypes */

/* basic serial helpers */
static inline void outb(uint16_t p, uint8_t v) { asm volatile("outb %0,%1" ::"a"(v), "Nd"(p)); }
static inline uint8_t inb(uint16_t p) {
    uint8_t r;
    asm volatile("inb %1,%0" : "=a"(r) : "Nd"(p));
    return r;
}
static void serial_init(void) {
    outb(0x3F8 + 1, 0x00);
    outb(0x3F8 + 3, 0x80);
    outb(0x3F8 + 0, 0x03);
    outb(0x3F8 + 1, 0x00);
    outb(0x3F8 + 3, 0x03);
    outb(0x3F8 + 2, 0xC7);
    outb(0x3F8 + 4, 0x0B);
}
static int serial_received(void) { return inb(0x3F8 + 5) & 1; }
static char serial_read(void) {
    while (!serial_received())
        ;
    return inb(0x3F8);
}
static int serial_empty(void) { return inb(0x3F8 + 5) & 0x20; }
static void serial_write(char a) {
    while (!serial_empty())
        ;
    outb(0x3F8, a);
}
static void serial_print(const char *s) {
    while (*s)
        serial_write(*s++);
}

static size_t str_len(const char *s) {
    size_t n = 0;
    while (s[n])
        n++;
    return n;
}
static int str_cmp(const char *a, const char *b) {
    while (*a && *a == *b) {
        a++;
        b++;
    }
    return (unsigned char)*a - (unsigned char)*b;
}

static unsigned int str_to_uint(const char *s) {
    unsigned int v = 0;
    int base = 10;
    if (s[0] == '0' && s[1] == 'x') {
        base = 16;
        s += 2;
    }
    while (*s) {
        char c = *s++;
        if (c >= '0' && c <= '9')
            v = v * base + (c - '0');
        else if (base == 16 && c >= 'a' && c <= 'f')
            v = v * 16 + (c - 'a' + 10);
        else if (base == 16 && c >= 'A' && c <= 'F')
            v = v * 16 + (c - 'A' + 10);
        else
            break;
    }
    return v;
}

static void print_uint(unsigned int v) {
    char buf[16];
    int i = 0;
    if (v == 0) {
        serial_write('0');
        return;
    }
    while (v > 0 && i < 16) {
        buf[i++] = '0' + (v % 10);
        v /= 10;
    }
    while (i--)
        serial_write(buf[i]);
}

static void print_hex(unsigned int v) {
    char hex[8];
    int i = 0;
    serial_print("0x");
    for (int j = 0; j < 8; j++) {
        int d = (v >> ((7 - j) * 4)) & 0xF;
        hex[j] = d < 10 ? '0' + d : 'a' + d - 10;
    }
    for (i = 0; i < 8; i++)
        serial_write(hex[i]);
}

static void print_hex64(uint64_t v) {
    char hex[16];
    serial_print("0x");
    for (int j = 0; j < 16; j++) {
        int d = (v >> ((15 - j) * 4)) & 0xF;
        hex[j] = d < 10 ? '0' + d : 'a' + d - 10;
    }
    for (int i = 0; i < 16; i++)
        serial_write(hex[i]);
}

/* command registration and stubs so generated tables link */
void add_command(const char *name, command_handler handler) {
    (void)name;
    (void)handler;
}
#define STUB(fn)                                                                                   \
    void fn(int a, char **b) {                                                                     \
        (void)a;                                                                                   \
        (void)b;                                                                                   \
    }
STUB(cmd_hello_wrapper)
STUB(cmd_bye_wrapper)
STUB(cmd_ui_graph_wrapper)
STUB(cmd_mem_alloc_wrapper)
STUB(cmd_mem_free_wrapper)
STUB(cmd_fs_open_wrapper)
STUB(cmd_fs_write_wrapper)
STUB(cmd_fs_read_wrapper)
STUB(cmd_fs_close_wrapper)
STUB(cmd_fs_mkdir_wrapper)
STUB(cmd_fs_ls_wrapper)
STUB(cmd_ai_prompt_wrapper)
STUB(cmd_ai_profile_wrapper)
STUB(cmd_br_create_wrapper)
STUB(cmd_br_list_wrapper)
STUB(cmd_br_stop_wrapper)
STUB(cmd_br_delete_wrapper)
STUB(cmd_br_vm_create_wrapper)
STUB(cmd_br_vm_list_wrapper)
STUB(cmd_br_vm_switch_wrapper)
STUB(cmd_br_vm_stop_wrapper)
STUB(cmd_plugin_install_wrapper)
STUB(cmd_plugin_list_wrapper)
STUB(cmd_plugin_load_wrapper)
STUB(cmd_br_peer_add_wrapper)
STUB(cmd_br_sync_wrapper)
STUB(cmd_br_sync_all_wrapper)
STUB(cmd_ai_service_create_wrapper)
STUB(cmd_ai_service_monitor_wrapper)
STUB(cmd_policy_load_wrapper)
STUB(cmd_policy_check_wrapper)

static void readline(char *buf, int max) {
    int i = 0;
    char c;
    while (i < max - 1) {
        c = serial_read();
        if (c == '\r')
            continue;
        if (c == '\n') {
            serial_write('\n');
            break;
        }
        serial_write(c);
        buf[i++] = c;
    }
    buf[i] = 0;
}

static void print_help(void) {
    serial_print("MEM_ALLOC <bytes>\n");
    serial_print("MEM_FREE <addr>\n");
    serial_print("FS_OPEN <name> <mode>\n");
    serial_print("FS_WRITE <fd> <text>\n");
    serial_print("FS_READ <fd> <bytes>\n");
    serial_print("FS_MKDIR <name>\n");
    serial_print("FS_LS\n");
    serial_print("BR_CREATE <name>\n");
    serial_print("BR_CLONE <id> <name>\n");
    serial_print("BR_LIST\n");
    serial_print("help\n");
    serial_print("exit\n");
}

static int command_interpreter(int argc, char **argv) {
    if (argc == 0)
        return 0;

    if (str_cmp(argv[0], "create") == 0) {
        int id = sys_create_branch();
        if (id < 0)
            serial_print("error\n");
        else {
            serial_print("created ");
            print_uint((unsigned int)id);
            serial_print("\n");
        }
        return 1;
    } else if (str_cmp(argv[0], "merge") == 0) {
        if (argc < 2) {
            serial_print("error\n");
            return 1;
        }
        int job = sys_merge_branch((int)str_to_uint(argv[1]));
        if (job < 0)
            serial_print("error\n");
        else {
            serial_print("merge job ");
            print_uint((unsigned int)job);
            serial_print("\n");
        }
        return 1;
    } else if (str_cmp(argv[0], "list") == 0) {
        char buf[128];
        int rc = sys_list_branches(buf, sizeof(buf));
        if (rc < 0)
            serial_print("error\n");
        else {
            serial_print(buf);
            serial_print("\n");
        }
        return 1;
    } else if (str_cmp(argv[0], "snapshot") == 0) {
        if (argc < 2) {
            serial_print("error\n");
            return 1;
        }
        uint64_t sid = sys_snapshot_branch((unsigned int)str_to_uint(argv[1]));
        if ((int64_t)sid < 0)
            serial_print("error\n");
        else {
            serial_print("snapshot ");
            print_hex64(sid);
            serial_print("\n");
        }
        return 1;
    }

    return 0;
}

void repl(void) {
    serial_init();
    serial_print("AOS v0.3.0 booting...\n");
    mem_init_bare();
    fs_init();
    bm_init();
    char line[128];
    char *argv[4];
    while (1) {
        serial_print("bare> ");
        readline(line, sizeof(line));
        int argc = 0;
        char *p = line;
        while (*p && argc < 4) {
            while (*p == ' ')
                p++;
            if (!*p)
                break;
            argv[argc++] = p;
            while (*p && *p != ' ')
                p++;
            if (*p) {
                *p = 0;
                p++;
            }
        }
        if (argc == 0)
            continue;
        if (str_cmp(argv[0], "exit") == 0)
            break;
        if (command_interpreter(argc, argv))
            continue;
        else if (str_cmp(argv[0], "help") == 0) {
            print_help();
            continue;
        } else if (str_cmp(argv[0], "MEM_ALLOC") == 0) {
            if (argc < 2) {
                serial_print("Error\n");
                continue;
            }
            unsigned int sz = str_to_uint(argv[1]);
            void *p = mem_alloc(sz);
            if (!p) {
                serial_print("Error\n");
                continue;
            }
            serial_print("Allocated ");
            print_uint(sz);
            serial_print(" at ");
            print_hex((unsigned int)(uintptr_t)p);
            serial_print("\n");
        } else if (str_cmp(argv[0], "MEM_FREE") == 0) {
            if (argc < 2) {
                serial_print("Error\n");
                continue;
            }
            void *addr = (void *)(uintptr_t)str_to_uint(argv[1]);
            mem_free(addr);
            serial_print("Freed ");
            print_hex((unsigned int)(uintptr_t)addr);
            serial_print("\n");
        } else if (str_cmp(argv[0], "FS_OPEN") == 0) {
            if (argc < 3) {
                serial_print("Error\n");
                continue;
            }
            int fd = fs_open(argv[1], argv[2]);
            if (fd < 0)
                serial_print("Error\n");
            else {
                serial_print("fd=");
                print_uint(fd);
                serial_print("\n");
            }
        } else if (str_cmp(argv[0], "FS_WRITE") == 0) {
            if (argc < 3) {
                serial_print("Error\n");
                continue;
            }
            int fd = str_to_uint(argv[1]);
            size_t len = str_len(argv[2]);
            size_t w = fs_write(fd, argv[2], len);
            if (w == 0)
                serial_print("Error\n");
            else {
                serial_print("Wrote ");
                print_uint(w);
                serial_print(" bytes\n");
            }
        } else if (str_cmp(argv[0], "FS_READ") == 0) {
            if (argc < 3) {
                serial_print("Error\n");
                continue;
            }
            int fd = str_to_uint(argv[1]);
            unsigned int n = str_to_uint(argv[2]);
            char buf[256];
            if (n > 255)
                n = 255;
            size_t r = fs_read(fd, buf, n);
            buf[r] = 0;
            if (r == 0)
                serial_print("Error\n");
            else {
                serial_print(buf);
                serial_print("\n");
            }
        } else if (str_cmp(argv[0], "FS_MKDIR") == 0) {
            if (argc < 2) {
                serial_print("Error\n");
                continue;
            }
            int rc = fs_mkdir(argv[1]);
            if (rc < 0)
                serial_print("Error\n");
        } else if (str_cmp(argv[0], "FS_LS") == 0) {
            fs_ls();
        } else if (str_cmp(argv[0], "BR_CREATE") == 0) {
            if (argc < 2) {
                serial_print("Error\n");
                continue;
            }
            int id = bm_create(argv[1]);
            if (id < 0)
                serial_print("Error\n");
            else {
                serial_print("Created ");
                print_uint(id);
                serial_print("\n");
            }
        } else if (str_cmp(argv[0], "BR_CLONE") == 0) {
            if (argc < 3) {
                serial_print("Error\n");
                continue;
            }
            int id = bm_clone(str_to_uint(argv[1]), argv[2]);
            if (id < 0)
                serial_print("Error\n");
            else {
                serial_print("Cloned ");
                print_uint(id);
                serial_print("\n");
            }
        } else if (str_cmp(argv[0], "BR_LIST") == 0) {
            BranchGraph g;
            bm_graph(&g);
            for (int i = 0; i < g.count; i++) {
                serial_print("ID ");
                print_uint(g.branches[i].id);
                serial_print(": ");
                serial_print(g.branches[i].name);
                serial_print("\n");
            }
        } else {
            serial_print("Unknown command\n");
        }
    }
}

/* main for possible standalone testing; kernel.c provides its own entry */
#ifdef BARE_METAL_STANDALONE
int main(void) {
    repl();
    return 0;
}
#endif

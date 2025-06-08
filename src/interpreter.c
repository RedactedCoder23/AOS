#include <stdio.h>
#include <string.h>
#include "interpreter.h"
#include "memory.h"
#include "fs.h"
#include "ai.h"
#include "branch.h"

void add_command(const char *name, void (*handler)()) {
    (void)handler;
    printf("Registered command: %s\n", name);
}

void say_hello() {
    printf("Hello from handler\n");
}

void say_bye() {
    printf("Bye from handler\n");
}

void test_alloc() {
    char *p = mem_alloc(16);
    if (p) {
        strcpy(p, "allocated!");
        printf("%s at %p\n", p, (void*)p);
    } else {
        printf("alloc failed\n");
    }
}

void do_cat() {
    const FSFile *f = fs_open("hello.txt");
    if (!f) { printf("not found\n"); return; }
    char buf[64];
    size_t n = fs_read(f, buf, sizeof(buf)-1);
    buf[n] = '\0';
    printf("%s", buf);
}

void ai_demo() {
    ai_request("Test", 0);
}

void open_branch() {
    branch_force_open("demo");
}

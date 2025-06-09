#include <stdio.h>
#include <string.h>
#include "interpreter.h"
#include "command_interpreter.h"

extern CommandDictionary commands[];

typedef command_handler handler_fn;

static handler_fn find_handler(const char *name) {
    for (CommandDictionary *cd = commands; cd->name != NULL; cd++) {
        if (strcmp(cd->name, name) == 0) return cd->handler;
    }
    return NULL;
}

int main(void) {
    char line[256];
    char *argv[8];
    int argc;
    init_command_map();
    printf("AOS> ");
    while (fgets(line, sizeof(line), stdin)) {
        char *nl = strchr(line, '\n');
        if (nl) *nl = '\0';
        if (strcmp(line, "exit") == 0) break;
        argc = 0;
        char *tok = strtok(line, " ");
        while (tok && argc < 8) { argv[argc++] = tok; tok = strtok(NULL, " "); }
        if (argc == 0) { printf("AOS> "); continue; }
        handler_fn h = find_handler(argv[0]);
        if (h) {
            h(argc, argv);
        } else {
            printf("Unknown command: %s\n", argv[0]);
        }
        printf("AOS> ");
    }
    return 0;
}

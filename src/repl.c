/*
 * AOS — repl.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "repl.h"
#include "command_interpreter.h"
#include <stdio.h>
#include <string.h>

extern CommandDictionary commands[];

static CommandDictionary *lookup(const char *name) {
    for (int i = 0; commands[i].name; i++)
        if (strcmp(commands[i].name, name) == 0)
            return &commands[i];
    return NULL;
}

int dispatch_command(int argc, char **argv) {
    if (argc == 0)
        return -1;
    CommandDictionary *c = lookup(argv[0]);
    if (!c) {
        printf("Unknown command\n");
        return -1;
    }
    if (c->handler)
        c->handler(argc, argv);
    return 0;
}

void repl_run(void) {
    char line[256];
    printf("AOS> ");
    while (fgets(line, sizeof(line), stdin)) {
        char *nl = strchr(line, '\n');
        if (nl)
            *nl = '\0';
        char *argv[8];
        int argc = 0;
        char *tok = strtok(line, " ");
        while (tok && argc < 8) {
            argv[argc++] = tok;
            tok = strtok(NULL, " ");
        }
        if (argc == 0) {
            printf("AOS> ");
            continue;
        }
        if (strcmp(argv[0], "exit") == 0)
            break;
        dispatch_command(argc, argv);
        printf("AOS> ");
    }
}

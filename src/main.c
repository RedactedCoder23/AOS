#include <stdio.h>
#include <string.h>
#include "interpreter.h"
#include "command_interpreter.h"

extern CommandDictionary commands[];

typedef void (*handler_fn)();

static handler_fn find_handler(const char *name) {
    for (CommandDictionary *cd = commands; cd->name != NULL; cd++) {
        if (strcmp(cd->name, name) == 0) return cd->handler;
    }
    return NULL;
}

int main(void) {
    char line[256];
    init_command_map();
    printf("AOS> ");
    while (fgets(line, sizeof(line), stdin)) {
        char *nl = strchr(line, '\n');
        if (nl) *nl = '\0';
        if (strcmp(line, "exit") == 0) break;
        handler_fn h = find_handler(line);
        if (h) {
            h();
        } else {
            printf("Unknown command: %s\n", line);
        }
        printf("AOS> ");
    }
    return 0;
}

#include <stdio.h>
#include "interpreter.h"

void add_command(const char *name, void (*handler)()) {
    (void)handler;
    printf("Registered command: %s\n", name);
}

void cmd_hello_wrapper(void) {
    // TODO: implement
    printf("Executed HELLO\n");
}

void cmd_bye_wrapper(void) {
    // TODO: implement
    printf("Executed BYE\n");
}

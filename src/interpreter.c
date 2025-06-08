#include <stdio.h>
#include "interpreter.h"

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

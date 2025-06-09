#include <stdio.h>
#include "interpreter.h"
#include "ui_graph.h"

void add_command(const char *name, void (*handler)()) {
    (void)handler;
    printf("Registered command: %s\n", name);
}

void cmd_hello_wrapper(void) {
    printf("Hello, AOS!\n");
}

void cmd_bye_wrapper(void) {
    printf("Goodbye from AOS!\n");
}

void ui_graph_wrapper(void) {
    ui_graph_run();
}

void cmd_ui_graph_wrapper(void) {
    ui_graph_wrapper();
}

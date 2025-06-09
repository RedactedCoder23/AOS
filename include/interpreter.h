#ifndef INTERPRETER_H
#define INTERPRETER_H

void add_command(const char *name, void (*handler)());
void cmd_hello_wrapper(void);
void cmd_bye_wrapper(void);
void ui_graph_wrapper(void);
void cmd_ui_graph_wrapper(void);
void init_command_map();

#endif

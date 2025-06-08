#ifndef INTERPRETER_H
#define INTERPRETER_H

void add_command(const char *name, void (*handler)());
void say_hello();
void say_bye();
void init_command_map();

#endif

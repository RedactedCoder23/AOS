#ifndef INTERPRETER_H
#define INTERPRETER_H

void add_command(const char *name, void (*handler)());
void say_hello();
void say_bye();
void init_command_map();
void test_alloc();
void do_cat();
void ai_demo();
void open_branch();

#endif

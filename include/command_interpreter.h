#ifndef COMMAND_INTERPRETER_H
#define COMMAND_INTERPRETER_H

#include <stddef.h>
#include "interpreter.h"
typedef struct {
    const char *name;
    const char *binary;
    void (*handler)();
} CommandDictionary;

#endif

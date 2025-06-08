#ifndef COMMAND_INTERPRETER_H
#define COMMAND_INTERPRETER_H

#include <stddef.h>
#include <stdint.h>
#include "interpreter.h"
#include "category_defs.h"
typedef struct {
    const char *name;
    const char *binary;
    void (*handler)();
    uint8_t category;
} CommandDictionary;

#endif

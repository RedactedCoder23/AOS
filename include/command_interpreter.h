#ifndef COMMAND_INTERPRETER_H
#define COMMAND_INTERPRETER_H

#include "category_defs.h"
#include "interpreter.h"
#include <stddef.h>
#include <stdint.h>
typedef struct {
    const char *name;
    const char *binary;
    command_handler handler;
    uint8_t category;
} CommandDictionary;

extern const size_t COMMAND_COUNT;

#endif

/* AOS — include/command_interpreter.h — Purpose: Header file */
/*
 * AOS — command_interpreter.h
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
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

void repl(void);

#endif

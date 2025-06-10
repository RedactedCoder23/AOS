#ifndef AOS_TRAPS_H
#define AOS_TRAPS_H

#include <stdint.h>

typedef struct registers {
    uint32_t rip;
    uint32_t cs;
    uint32_t rflags;
    uint32_t rsp;
    uint32_t ss;
} registers_t;

void isr_default(registers_t *r) __attribute__((interrupt));

#endif

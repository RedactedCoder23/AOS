/*
 * AOS — kernel.c
 * (c) 2025 RedactedCoder23
 * Purpose: Kernel entry point
 * Dependencies: requires bootloader.bin stage1
 */
#include "../include/ipc.h"
#include "command_interpreter.h"
#include "config.h"
#include "error.h"
#include "idt.h"
#include "serial.h"
#include "traps.h"
#include <errno.h>
#include <stdint.h>

#define AOS_VERSION "0.3.0"

/* Boot entry points provided by assembly stub. */
extern void repl(void);
void mem_init_bare(void);
void fs_init(void);
void bm_init(void);
void idt_init(void);

/* Dispatch syscalls coming from the host interface */

/* Initialise subsystems before entering the REPL. */
static void kernel_init(void) {
    idt_init();
    mem_init_bare();
    fs_init();
    bm_init();
    config_load_default();
}

static void kernel_main(void) {
    serial_init();
    serial_print("AOS v" AOS_VERSION " booting...\n");
    kernel_init();
    repl();
}

void main(void) { kernel_main(); }

void _start(void) {
    /* Minimal bootstrap that calls main and halts when it returns. */
    main();
    for (;;)
        __asm__("hlt");
}

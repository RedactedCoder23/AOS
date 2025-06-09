#include <stdint.h>
#include "command_interpreter.h"
#include "config.h"

extern void repl(void);
void mem_init_bare(void);
void fs_init(void);
void bm_init(void);

static void kernel_init(void) {
    mem_init_bare();
    fs_init();
    bm_init();
    config_load_default();
}

void main(void) {
    kernel_init();
    repl();
}

void _start(void) {
    main();
    for(;;) __asm__("hlt");
}


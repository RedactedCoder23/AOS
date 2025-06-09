#include <stdint.h>
#include "command_interpreter.h"
#include "config.h"
#include "logging.h"
#include "error.h"

/* Boot entry points provided by assembly stub. */
extern void repl(void);
void mem_init_bare(void);
void fs_init(void);
void bm_init(void);

/* Initialise subsystems before entering the REPL. */
static void kernel_init(void) {
    log_init(NULL);
    log_message(LOG_INFO, "Hello from AOS");
    mem_init_bare();
    fs_init();
    bm_init();
    config_load_default();
}

void main(void) {
    /* Entry called by bootloader. Start subsystems and drop into REPL. */
    kernel_init();
    repl();
}

void _start(void) {
    /* Minimal bootstrap that calls main and halts when it returns. */
    main();
    for(;;) __asm__("hlt");
}


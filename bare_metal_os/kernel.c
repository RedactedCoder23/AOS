#include <stdint.h>
#include "command_interpreter.h"

extern void repl(void);

void main(void) {
    repl();
}

void _start(void) {
    main();
    for(;;) __asm__("hlt");
}

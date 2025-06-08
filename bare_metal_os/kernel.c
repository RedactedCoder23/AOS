// kernel.c
#include <stdint.h>

static volatile uint16_t *const VGA = (uint16_t *)0xB8000;

void main(void) {
    const char *msg = "Kernel started";
    for (int i = 0; msg[i]; i++) {
        VGA[i] = 0x1F00 | msg[i];
    }
}

void _start(void) {
    main();
    for (;;)
        __asm__("hlt");
}
